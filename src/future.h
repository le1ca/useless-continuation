#ifndef future_h_
#define future_h_

#include <memory>
#include <functional>
#include <type_traits>
#include <condition_variable>
#include <mutex>

/** Recursive form:
 *  This future promises a value of type T, and has a continuation chain
 *  in which the next future produces type U.
 */
template <typename T, typename U=void, typename ...Continuation>
class future {
public:

    future() = default;

    future(future<T, U, Continuation...> const&) = delete;

    /** wait(): Blocks until the value is filled and returns a pointer to it.
     */
    std::unique_ptr<T> wait() {
        std::unique_lock<std::mutex> lock(m_lock);
        m_cond.wait(lock, [this]{ return m_value != nullptr; });
        return std::move(m_value);
    }

    /** then(): Adds a continuation to this future. The callback will be called when
     *  *this is populated. Returns a future to the return value of the callback.
     */
    future<U, Continuation...>& then(std::function<U(T const&)> callback) {
         std::unique_lock<std::mutex> lock(m_lock);
         m_callback = callback;

         // This check overcomes the race between a continuation being registered and this
         // future being posted.
         if(m_value != nullptr) {
            m_result.post(m_callback(*m_value));
         }

         return m_result;
    }

    /** post(): Sets the value of this future, activates a thread waiting on the value (if any),
     *  and fills the "next" future with the result of the callback
     */
    void post(T const& val) {
        std::unique_lock<std::mutex> lock(m_lock);
        m_value = std::make_unique<T>(val);
        m_cond.notify_all();
        if(m_callback){
            m_result.post(m_callback(val));
        }
    }

private:

    std::function<U(T const&)> m_callback;
    std::unique_ptr<T> m_value;
    future<U, Continuation...> m_result;

    std::mutex m_lock;
    std::condition_variable m_cond;

};

/** Base case:
 *  This represents the end of a continuation chain and simply allows us to block
 *  until the chain has been completely evaluated.
 */
template <>
class future<void, void> {
public:

    future() = default;
    future(future<void, void> const&) = delete;

    void wait() {
        std::unique_lock<std::mutex> lock(m_lock);
        m_cond.wait(lock, [this]{ return m_done; });
    }

    void post() {
        std::unique_lock<std::mutex> lock(m_lock);
        m_done = true;
        m_cond.notify_all();
    }

private:

    volatile bool m_done;
    std::mutex m_lock;
    std::condition_variable m_cond;

};

/** Base case:
 *  This represents an element immediately prior to the end of the chain. It has a value, but
 *  any callback registered with it does not.
 */
template <typename T>
class future<T, void> {
public:

    future() = default;
    future(future<T, void> const&) = delete;

    std::unique_ptr<T> wait() {
        std::unique_lock<std::mutex> lock(m_lock);
        m_cond.wait(lock, [this]{ return m_value != nullptr; });
        return std::move(m_value);
    }

    future<void, void>& then(std::function<void(T const&)> callback) {
        std::unique_lock<std::mutex> lock(m_lock);
        m_callback = callback;
        if(m_value != nullptr) {
            m_callback(*m_value);
            m_result.post();
        }
        return m_result;
    }

    void post(T const& val) {
        std::unique_lock<std::mutex> lock(m_lock);
        m_value = std::make_unique<T>(val);
        m_cond.notify_all();
        m_result.post();
    }

private:

    std::function<void(T const&)> m_callback;
    std::unique_ptr<T> m_value;
    future<void, void> m_result;
    std::mutex m_lock;
    std::condition_variable m_cond;

};

#endif
