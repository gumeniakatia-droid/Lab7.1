#include "RootFindingTask.h"
#include <cmath>
#include <limits>
#include <algorithm>

void RootFindingTask::validateInterval(double a, double b)
{
    if (!std::isfinite(a))
        throw InvalidIntervalException("a", a);
    if (!std::isfinite(b))
        throw InvalidIntervalException("b", b);
    if (a >= b)
        throw std::invalid_argument("Left bound must be strictly less than right bound");
}

void RootFindingTask::validateTolerance(double eps)
{
    if (!std::isfinite(eps) || eps <= 0.0)
        throw std::invalid_argument("Tolerance must be a positive finite number");
}

void RootFindingTask::validateMaxIterations(std::size_t max_iterations)
{
    if (max_iterations == 0)
        throw std::invalid_argument("Maximum number of iterations must be positive");
}

RootFindingTask::RootFindingTask(double a, double b, double eps, std::size_t max_iterations)
    : a_(a)
    , b_(b)
    , eps_(eps)
    , max_iterations_(max_iterations)
{
    validateInterval(a, b);
    validateTolerance(eps);
    validateMaxIterations(max_iterations);
}

void RootFindingTask::setInterval(double a, double b)
{
    validateInterval(a, b);
    a_ = a;
    b_ = b;
}

void RootFindingTask::setTolerance(double eps)
{
    validateTolerance(eps);
    eps_ = eps;
}

void RootFindingTask::setMaxIterations(std::size_t max_iterations)
{
    validateMaxIterations(max_iterations);
    max_iterations_ = max_iterations;
}

double RootFindingTask::bisection(const Function& f) const
{
    double left = a_;
    double right = b_;

    double f_left = f(left);
    double f_right = f(right);

    if (f_left * f_right > 0.0)
        throw NoSignChangeException();

    if (std::abs(f_left) <= eps_)
        return left;
    if (std::abs(f_right) <= eps_)
        return right;

    for (std::size_t i = 0; i < max_iterations_; ++i)
    {
        const double mid = left + (right - left) / 2.0;
        const double f_mid = f(mid);

        if (std::abs(f_mid) <= eps_ || (right - left) / 2.0 <= eps_)
            return mid;

        if (f_left * f_mid < 0.0)
        {
            right = mid;
            f_right = f_mid;
        }
        else
        {
            left = mid;
            f_left = f_mid;
        }
    }

    throw NoConvergenceException(max_iterations_);
}

double RootFindingTask::newton(const Function& f, const Derivative& df, double x0) const
{
    if (!std::isfinite(x0))
        throw std::invalid_argument("Initial guess for Newton method must be finite");

    double x = x0;

    for (std::size_t i = 0; i < max_iterations_; ++i)
    {
        const double fx = f(x);
        if (std::abs(fx) <= eps_)
            return x;

        const double dfx = df(x);
        if (std::abs(dfx) < 1e-15)  
            throw ZeroDerivativeException(x);

        const double x_next = x - fx / dfx;

        if (!std::isfinite(x_next))
            throw std::overflow_error("Newton iteration produced non-finite value");

        if (std::abs(x_next - x) <= eps_)
            return x_next;

        x = x_next;
    }

    throw NoConvergenceException(max_iterations_);
}

double RootFindingTask::secant(const Function& f, double x0, double x1) const
{
    if (!std::isfinite(x0) || !std::isfinite(x1))
        throw std::invalid_argument("Initial points for secant method must be finite");

    double prev = x0;
    double curr = x1;

    for (std::size_t i = 0; i < max_iterations_; ++i)
    {
        const double f_prev = f(prev);
        const double f_curr = f(curr);

        if (std::abs(f_curr) <= eps_)
            return curr;

        const double denom = f_curr - f_prev;
        if (std::abs(denom) < 1e-15)
            throw ZeroDenominatorException();

        const double next = curr - f_curr * (curr - prev) / denom;

        if (!std::isfinite(next))
            throw std::overflow_error("Secant iteration produced non-finite value");

        if (std::abs(next - curr) <= eps_)
            return next;

        prev = curr;
        curr = next;
    }

    throw NoConvergenceException(max_iterations_);
}

void RootFindingTask::computeAndStoreBoth(const Function& f, const Derivative& df, double newton_x0)
{

    const double bis_root = bisection(f);               
    const double newt_root = newton(f, df, newton_x0);  

    last_bisection_root_ = bis_root;
    last_newton_root_ = newt_root;
}
