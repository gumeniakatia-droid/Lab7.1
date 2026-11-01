#include <gtest/gtest.h>
#include "RootFindingTask.h"
#include "RootExceptions.h"
#include <cmath>
#include <limits>

// Test function: x^3 - x - 2, root ≈ 1.5213797068
double test_f(double x)
{
    return x * x * x - x - 2.0;
}

double test_df(double x)
{
    return 3.0 * x * x - 1.0;
}

// ---------- Construction & invariants ----------

TEST(RootFindingTaskTest, CreatesValidTask)
{
    RootFindingTask task(1.0, 2.0, 1e-6, 50);
    EXPECT_DOUBLE_EQ(task.a(), 1.0);
    EXPECT_DOUBLE_EQ(task.b(), 2.0);
    EXPECT_DOUBLE_EQ(task.eps(), 1e-6);
    EXPECT_EQ(task.max_iterations(), 50u);
}

TEST(RootFindingTaskTest, RejectsInvalidInterval)
{
    EXPECT_THROW(RootFindingTask(2.0, 1.0, 1e-6, 50), std::invalid_argument);
    EXPECT_THROW(RootFindingTask(1.0, 1.0, 1e-6, 50), std::invalid_argument);
}

TEST(RootFindingTaskTest, RejectsNonFiniteBounds)
{
    const double inf = std::numeric_limits<double>::infinity();
    const double nan = std::numeric_limits<double>::quiet_NaN();

    EXPECT_THROW(RootFindingTask(1.0, inf, 1e-6, 50), InvalidIntervalException);
    EXPECT_THROW(RootFindingTask(nan, 2.0, 1e-6, 50), InvalidIntervalException);
}

TEST(RootFindingTaskTest, RejectsInvalidTolerance)
{
    EXPECT_THROW(RootFindingTask(1.0, 2.0, 0.0, 50), std::invalid_argument);
    EXPECT_THROW(RootFindingTask(1.0, 2.0, -1e-6, 50), std::invalid_argument);
}

TEST(RootFindingTaskTest, RejectsZeroMaxIterations)
{
    EXPECT_THROW(RootFindingTask(1.0, 2.0, 1e-6, 0), std::invalid_argument);
}

// ---------- Correctness of methods ----------

TEST(RootFindingTaskTest, BisectionFindsRoot)
{
    RootFindingTask task(1.0, 2.0, 1e-10, 100);
    const double root = task.bisection(test_f);
    EXPECT_NEAR(root, 1.5213797068, 1e-8);
    EXPECT_NEAR(test_f(root), 0.0, 1e-9);
}

TEST(RootFindingTaskTest, NewtonFindsRoot)
{
    RootFindingTask task(1.0, 2.0, 1e-12, 50);
    const double root = task.newton(test_f, test_df, 1.5);
    EXPECT_NEAR(root, 1.5213797068, 1e-10);
}

TEST(RootFindingTaskTest, SecantFindsRoot)
{
    RootFindingTask task(1.0, 2.0, 1e-10, 50);
    const double root = task.secant(test_f, 1.0, 2.0);
    EXPECT_NEAR(root, 1.5213797068, 1e-8);
}

// ---------- Exception scenarios ----------

TEST(RootFindingTaskTest, BisectionThrowsNoSignChange)
{
    RootFindingTask task(0.0, 1.0, 1e-6, 50);
    auto positive = [](double x) { return x * x + 1.0; };
    EXPECT_THROW(task.bisection(positive), NoSignChangeException);
}

TEST(RootFindingTaskTest, NewtonThrowsZeroDerivative)
{
    RootFindingTask task(-1.0, 1.0, 1e-6, 50);
    auto flat = [](double) { return 1.0; };
    auto zero_df = [](double) { return 0.0; };
    EXPECT_THROW(task.newton(flat, zero_df, 0.0), ZeroDerivativeException);
}

TEST(RootFindingTaskTest, SecantThrowsZeroDenominator)
{
    RootFindingTask task(0.0, 1.0, 1e-6, 50);
    auto const_f = [](double) { return 5.0; };
    EXPECT_THROW(task.secant(const_f, 0.0, 1.0), ZeroDenominatorException);
}

TEST(RootFindingTaskTest, ThrowsNoConvergence)
{
    RootFindingTask task(1.0, 2.0, 1e-15, 2); // too few iterations
    EXPECT_THROW(task.bisection(test_f), NoConvergenceException);
}

// ---------- Strong guarantee ----------

TEST(RootFindingTaskTest, SetIntervalPreservesStateOnException)
{
    RootFindingTask task(1.0, 2.0, 1e-6, 50);
    const double old_a = task.a();
    const double old_b = task.b();

    EXPECT_THROW(task.setInterval(5.0, 3.0), std::invalid_argument);

    EXPECT_DOUBLE_EQ(task.a(), old_a);
    EXPECT_DOUBLE_EQ(task.b(), old_b);
}

TEST(RootFindingTaskTest, ComputeAndStoreBothStrongGuarantee)
{
    RootFindingTask task(1.0, 2.0, 1e-8, 100);

    // First successful call
    task.computeAndStoreBoth(test_f, test_df, 1.5);
    ASSERT_TRUE(task.last_bisection_root().has_value());
    ASSERT_TRUE(task.last_newton_root().has_value());
    const double saved_bis = *task.last_bisection_root();
    const double saved_newt = *task.last_newton_root();

    // Now force failure on the second method by using a bad derivative
    auto bad_df = [](double) { return 0.0; };
    EXPECT_THROW(
        task.computeAndStoreBoth(test_f, bad_df, 1.5),
        ZeroDerivativeException
    );

    // Cached values must remain unchanged
    ASSERT_TRUE(task.last_bisection_root().has_value());
    ASSERT_TRUE(task.last_newton_root().has_value());
    EXPECT_DOUBLE_EQ(*task.last_bisection_root(), saved_bis);
    EXPECT_DOUBLE_EQ(*task.last_newton_root(), saved_newt);
}

// ---------- Boundary / tolerance ----------

TEST(RootFindingTaskTest, LengthWithTolerance)
{
    RootFindingTask task(0.1, 0.4, 1e-12, 50);
    // Just a sanity check that observers work with floating-point values
    EXPECT_NEAR(task.b() - task.a(), 0.3, 1e-12);
}
