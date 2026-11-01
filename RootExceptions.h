#pragma once

#include <stdexcept>
#include <string>
#include <cstddef>

class RootException : public std::runtime_error
{
public:
    explicit RootException(const std::string& message)
        : std::runtime_error(message)
    {
    }
};

class InvalidIntervalException : public RootException
{
private:
    std::string parameter_;
    double value_;

public:
    InvalidIntervalException(const std::string& parameter, double value)
        : RootException("Invalid interval boundary: " + parameter)
        , parameter_(parameter)
        , value_(value)
    {
    }

    const std::string& parameter() const noexcept { return parameter_; }
    double value() const noexcept { return value_; }
};

class NoSignChangeException : public RootException
{
public:
    NoSignChangeException()
        : RootException("Function does not change sign on the given interval")
    {
    }
};

class ZeroDerivativeException : public RootException
{
private:
    double x_;

public:
    explicit ZeroDerivativeException(double x)
        : RootException("Zero derivative at point")
        , x_(x)
    {
    }

    double x() const noexcept { return x_; }
};

class ZeroDenominatorException : public RootException
{
public:
    ZeroDenominatorException()
        : RootException("Zero denominator in secant method (f(x1) == f(x0))")
    {
    }
};

class NoConvergenceException : public RootException
{
private:
    std::size_t max_iterations_;

public:
    explicit NoConvergenceException(std::size_t max_iterations)
        : RootException("Failed to converge within the maximum number of iterations")
        , max_iterations_(max_iterations)
    {
    }

    std::size_t max_iterations() const noexcept { return max_iterations_; }
};
