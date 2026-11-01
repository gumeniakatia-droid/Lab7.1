#include <iostream>
#include <cmath>
#include "RootFindingTask.h"
#include "RootExceptions.h"
#include "ScopeLogger.h"

// Test function: f(x) = x^3 - x - 2  
double f(double x)
{
    return x * x * x - x - 2.0;
}

double df(double x)
{
    return 3.0 * x * x - 1.0;
}

void level3()
{
    ScopeLogger logger("level3");
    std::cout << "Creating task with invalid interval...\n";
    RootFindingTask task(5.0, 1.0, 1e-6, 100); 
}

void level2()
{
    ScopeLogger logger("level2");
    try
    {
        level3();
    }
    catch (const std::exception& ex)
    {
        std::cout << "[LOG] level2 caught: " << ex.what() << '\n';
        throw; 
    }
}

void level1()
{
    ScopeLogger logger("level1");
    level2();
}

int main()
{
    std::cout << "=== RootFindingLab Demo ===\n\n";

    try
    {
        RootFindingTask task(1.0, 2.0, 1e-8, 100);

        std::cout << "Interval: [" << task.a() << ", " << task.b() << "]\n";
        std::cout << "Tolerance: " << task.eps() << "\n\n";

        const double root_bis = task.bisection(f);
        std::cout << "Bisection root: " << root_bis << "\n";

        const double root_newt = task.newton(f, df, 1.5);
        std::cout << "Newton root:    " << root_newt << "\n";

        const double root_sec = task.secant(f, 1.0, 2.0);
        std::cout << "Secant root:    " << root_sec << "\n\n";

        task.computeAndStoreBoth(f, df, 1.5);
        std::cout << "Stored bisection: " << *task.last_bisection_root() << "\n";
        std::cout << "Stored Newton:    " << *task.last_newton_root() << "\n\n";
    }
    catch (const RootException& ex)
    {
        std::cout << "RootException: " << ex.what() << "\n";
    }
    catch (const std::exception& ex)
    {
        std::cout << "std::exception: " << ex.what() << "\n";
    }

    std::cout << "\n=== Stack unwinding demonstration ===\n";
    try
    {
        level1();
    }
    catch (const std::exception& ex)
    {
        std::cout << "[MAIN] Exception: " << ex.what() << "\n";
    }
    std::cout << "Program continues normally.\n";

    std::cout << "\n=== Error scenarios ===\n";

    try
    {
        RootFindingTask task(1.0, 2.0, 1e-8, 5);
        auto no_sign = [](double x) { return x * x + 1.0; };
        task.setInterval(0.0, 0.5);
        task.bisection(no_sign);
    }
    catch (const NoSignChangeException& ex)
    {
        std::cout << "Caught NoSignChangeException: " << ex.what() << "\n";
    }

    try
    {
        RootFindingTask task(-1.0, 1.0, 1e-8, 100);
        auto flat = [](double) { return 1.0; };
        auto zero_df = [](double) { return 0.0; };
        task.newton(flat, zero_df, 0.0);
    }
    catch (const ZeroDerivativeException& ex)
    {
        std::cout << "Caught ZeroDerivativeException at x = " << ex.x() << "\n";
    }

    try
    {
        RootFindingTask task(0.0, 1.0, 1e-8, 100);
        auto const_f = [](double) { return 5.0; };
        task.secant(const_f, 0.0, 1.0);
    }
    catch (const ZeroDenominatorException& ex)
    {
        std::cout << "Caught ZeroDenominatorException: " << ex.what() << "\n";
    }

    try
    {
        RootFindingTask task(1.0, 2.0, 1e-12, 3); 
        task.bisection(f);
    }
    catch (const NoConvergenceException& ex)
    {
        std::cout << "Caught NoConvergenceException (max = "
                  << ex.max_iterations() << ")\n";
    }

    return 0;
}
