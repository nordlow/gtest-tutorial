#include <limits.h>
#include <vector>
#include "gtest/gtest.h"

namespace {

TEST(StdVectorTest, DiffersInTheBeginning) {
    const size_t n = 1000000;

    std::vector<int> x(n);
    std::vector<int> y(n);
    y[0] = 20;

    EXPECT_EQ(x, y);
}

TEST(StdVectorTest, DiffersInTheMiddle) {
    const size_t n = 1000000;

    std::vector<int> x(n);
    std::vector<int> y(n);
    y[n/2] = 20;

    EXPECT_EQ(x, y);
}

TEST(StdVectorTest, DiffersInTheEnd) {
    const size_t n = 1000000;

    std::vector<int> x(n);
    std::vector<int> y(n);
    y[n - 1] = 20;

    EXPECT_EQ(x, y);
}

}
