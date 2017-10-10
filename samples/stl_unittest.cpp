#include <limits.h>
#include <vector>
#include "gtest/gtest.h"
#include "gmock/gmock.h"

/** Test STL-container comparison
    See also: https://github.com/google/googletest/blob/master/googlemock/docs/CheatSheet.md#container-matchers
*/

namespace {

TEST(StdVectorTest, DiffersInTheBeginning) {
    const size_t n = 1000000;

    std::vector<int> x(n);

    std::vector<int> y(n);
    y[0] = 20;
    y[1] = 30;
    y[2] = 40;

    EXPECT_THAT(x, ::testing::ContainerEq(y));
}

TEST(StdVectorTest, DiffersInTheMiddle) {
    const size_t n = 1000000;

    std::vector<int> x(n);

    std::vector<int> y(n);
    y[n/2 + 0] = 20;
    y[n/2 + 1] = 30;
    y[n/2 + 3] = 40;

    EXPECT_THAT(x, ::testing::ContainerEq(y));
}

TEST(StdVectorTest, DiffersInTheEnd) {
    const size_t n = 1000000;

    std::vector<int> x(n);

    std::vector<int> y(n);
    y[n - 1] = 20;

    EXPECT_THAT(x, ::testing::ContainerEq(y));
}

}
