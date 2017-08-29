#include <gtest/gtest.h>

#include "future.h"

#include <thread>
#include <string>
#include <sstream>
#include <iostream>

TEST(test_future, simple_wait) {

    future<int> f;
    int r;

    std::thread client ([&f, &r]{
        std::unique_ptr<int> rp = f.wait();
        r = *rp;
    });

    f.post(12);
    client.join();

    ASSERT_EQ(r, 12);

}

TEST(test_future, continuations) {

    future<int, int, std::string> f;
    std::string r;

    std::thread client ([&f, &r]{
        std::unique_ptr<std::string> rp = f.then(
            [](int x) -> int {
                return x + 5;
            }
        ).then(
            [](int y) -> std::string {
                std::stringstream ss;
                ss << y;
                return ss.str();
            }
        ).wait();
        r = *rp;
    });

    f.post(10);
    client.join();

    ASSERT_EQ(r, "15");

}

TEST(test_future, continuations_chain) {

    future<int, int, int, int, std::string> f;
    std::string r;

    std::thread client ([&f, &r]{
        std::unique_ptr<std::string> rp = f.then(
            [](int w) -> int {
                return w + 5;
            }
        ).then(
            [](int x) -> int {
                return x * 2;
            }
        ).then(
            [](int y) -> int {
                return 50 - y;
            }
        ).then(
            [](int z) -> std::string {
                std::stringstream ss;
                ss << z;
                return ss.str();
            }
        ).wait();
        r = *rp;
    });

    f.post(10);
    client.join();

    ASSERT_EQ(r, "20");

}

TEST(test_future, continuations_chain_void_terminated) {

    future<int, int, int, int, std::string, void> f;
    std::string r;

    std::thread client ([&f, &r]{
        f.then(
            [](int w) -> int {
                return w + 5;
            }
        ).then(
            [](int x) -> int {
                return x * 2;
            }
        ).then(
            [](int y) -> int {
                return 50 - y;
            }
        ).then(
            [](int z) -> std::string {
                std::stringstream ss;
                ss << z;
                return ss.str();
            }
        ).then(
            [&r](std::string const& s) -> void {
                r = s;
            }
        ).wait();
    });

    f.post(10);
    client.join();

    ASSERT_EQ(r, "20");

}
