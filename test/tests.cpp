#include <gtest/gtest.h>
#include <cmath>
#include "LocalMinReverseCommunication.hpp"

TEST(LocalMinRCTest, MinimizesQuadraticFunction) {
    double a = 0.0;
    double b = 5.0;
    double value = 0.0;
    double arg = 0.0;

    // Iteratively call local_min_rc until status == 0.
    LocalMinReverseCommunication local_min_rc{a, b};
    while (true) {
        arg = local_min_rc(value);
        if (local_min_rc.IsReady()) {
            break;
        }
        value = (arg - 2.0) * (arg - 2.0);
    }

    // We expect the minimizer to be near x = 2.0.
    EXPECT_NEAR(arg, 2.0, 1e-4);
}

TEST(LocalMinRCTest, MinimizesCosFunction) {
    double a = 0.0;
    double b = 6.28; // 2*pi
    double value = 0.0;
    double arg = 0.0;

    LocalMinReverseCommunication local_min_rc{a, b};
    while (true) {
        arg = local_min_rc(value);
        if (local_min_rc.IsReady()) {
            break;
        }
        value = std::cos(arg);
    }

    // We expect the minimizer to be near x = pi (cos derivative = 0, maximum or minimum).
    // Actually, cos x has a minimum near x = pi, so check close to pi.
    EXPECT_NEAR(arg, 3.14159, 1e-3);
}
