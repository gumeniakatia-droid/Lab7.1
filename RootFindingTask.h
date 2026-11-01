#pragma once

#include "RootExceptions.h"
#include <functional>
#include <optional>
#include <cmath>
#include <utility>

class RootFindingTask
{
public:
    using Function = std::function<double(double)>;
    using Derivative = std::function<double(double)>;

private:
    double a_;                  
    double b_;                  
    double eps_;                
    std::size_t max_iterations_;

    std::optional<double> last_bisection_root_;
    std::optional<double> last_newton_root_;

    static void validateInterval(double a, double b);
    static void validateTolerance(double eps);
    static void validateMaxIterations(std::size_t max_iterations);

public:
    RootFindingTask(double a, double b, double eps, std::size_t max_iterations);

    double a() const noexcept { return a_; }
    double b() const noexcept { return b_; }
    double eps() const noexcept { return eps_; }
    std::size_t max_iterations() const noexcept { return max_iterations_; }

    std::optional<double> last_bisection_root() const noexcept { return last_bisection_root_; }
    std::optional<double> last_newton_root() const noexcept { return last_newton_root_; }

    void setInterval(double a, double b);

    void setTolerance(double eps);

    void setMaxIterations(std::size_t max_iterations);

    double bisection(const Function& f) const;

    double newton(const Function& f, const Derivative& df, double x0) const;

    double secant(const Function& f, double x0, double x1) const;

    void computeAndStoreBoth(const Function& f, const Derivative& df, double newton_x0);
};
