#pragma once

#include <cmath>
#include <limits>
#include <format>


//  Purpose:
//
//    LocalMinReverseCommunication() seeks a minimizer of a scalar function of a scalar variable.
//
//  Discussion:
//
//    This routine seeks an approximation to the point where a function
//    F attains a minimum on the interval (A,B).
//
//    The method used is a combination of golden section search and
//    successive parabolic interpolation.  Convergence is never much
//    slower than that for a Fibonacci search.  If F has a continuous
//    second derivative which is positive at the minimum (which is not
//    at A or B), then convergence is superlinear, and usually of the
//    order of about 1.324...
//
//    The routine is a revised version of the Brent local minimization
//    algorithm, using reverse communication.
//
//    It is worth stating explicitly that this routine will NOT be
//    able to detect a minimizer that occurs at either initial endpoint
//    A or B.  If this is a concern to the user, then the user must
//    either ensure that the initial interval is larger, or to check
//    the function value at the returned minimizer against the values
//    at either endpoint.
//
//  Licensing:
//
//    This code is distributed under the MIT license.
//
//  Modified:
//
//    29 May 2021
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Richard Brent,
//    Algorithms for Minimization Without Derivatives,
//    Dover, 2002,
//    ISBN: 0-486-41998-3,
//    LC: QA402.5.B74.
//
//    David Kahaner, Cleve Moler, Steven Nash,
//    Numerical Methods and Software,
//    Prentice Hall, 1989,
//    ISBN: 0-13-627258-4,
//    LC: TA345.K34.
//
//  Parameters
//
//    Input/output, double &A, &B.  On input, the left and right
//    endpoints of the initial interval.  On output, the lower and upper
//    bounds for an interval containing the minimizer.  It is required
//    that A < B.
//
//    Input/output, int &STATUS, used to communicate between
//    the user and the routine.  The user only sets STATUS to zero on the first
//    call, to indicate that this is a startup call.  The routine returns STATUS
//    positive to request that the function be evaluated at ARG, or returns
//    STATUS as 0, to indicate that the iteration is complete and that
//    ARG is the estimated minimizer.
//
//    Input, double VALUE, the function value at ARG, as requested
//    by the routine on the previous call.
//
//    Output, double LocalMinReverseCommunication, the currently considered point.
//    On return with STATUS positive, the user is requested to evaluate the
//    function at this point, and return the value in VALUE.  On return with
//    STATUS zero, this is the routine's estimate for the function minimizer.
//
//  Local:
//
//    double C: the squared inverse of the golden ratio.
//
//    double EPS: the square root of the relative machine precision.
class LocalMinReverseCommunication {
public:
    LocalMinReverseCommunication(const double from, const double to)
        : a(from)
        , b(to)
    {
        if (b <= a)
        {
            throw std::runtime_error(std::format("LocalMinReverseCommunication: A < B is required, but A = {:f}; B = {:f}", a, b));
        }
    }

    auto IsReady() const -> bool {
        return iteration == 0;
    }

    auto operator()(const double value) -> double {
        static const double tol = std::numeric_limits<double>::epsilon();
        static const double eps = std::sqrt(tol);

        // First iteration
        if (iteration == 0)
        {
            c = 0.5 * (3.0 - std::sqrt(5.0));

            v = a + c * (b - a);
            w = v;
            x = v;
            e = 0.0;

            iteration = 1;
            arg = x;

            return arg;
        }
        // Second iteration
        else if (iteration == 1)
        {
            fx = value;
            fv = fx;
            fw = fx;
        }
        // Subsequent iterations
        else if (2 <= iteration)
        {
            fu = value;

            if (fu <= fx)
            {
                if (x <= u)
                {
                    a = x;
                }
                else
                {
                    b = x;
                }
                v = w;
                fv = fw;
                w = x;
                fw = fx;
                x = u;
                fx = fu;
            }
            else
            {
                if (u < x)
                {
                    a = u;
                }
                else
                {
                    b = u;
                }

                if (fu <= fw || w == x)
                {
                    v = w;
                    fv = fw;
                    w = u;
                    fw = fu;
                }
                else if (fu <= fv || v == x || v == w)
                {
                    v = u;
                    fv = fu;
                }
            }
        }

        // Take the next step.
        double midpoint = 0.5 * (a + b);
        double tol1 = eps * std::fabs(x) + tol / 3.0;
        double tol2 = 2.0 * tol1;

        // If the stopping criterion is satisfied, we can exit.
        if (std::fabs(x - midpoint) <= (tol2 - 0.5 * (b - a)))
        {
            iteration = 0;
            return arg;
        }

        // Is golden-section necessary?
        if (std::fabs(e) <= tol1)
        {
            if (midpoint <= x)
            {
                e = a - x;
            }
            else
            {
                e = b - x;
            }
            d = c * e;
        }
        // Consider fitting a parabola.
        else
        {
            r = (x - w) * (fx - fv);
            q = (x - v) * (fx - fw);
            p = (x - v) * q - (x - w) * r;
            q = 2.0 * (q - r);
            if (0.0 < q)
            {
                p = - p;
            }
            q = std::fabs(q);
            r = e;
            e = d;

            // Choose a golden-section step if the parabola is not advised.
            if (
                (std::fabs(0.5 * q * r) <= std::fabs(p)) ||
                (p <= q * (a - x)) ||
                (q * (b - x) <= p))
            {
                if (midpoint <= x)
                {
                    e = a - x;
                }
                else
                {
                    e = b - x;
                }
                d = c * e;
            }
            // Choose a parabolic interpolation step.
            else
            {
                d = p / q;
                u = x + d;

                if ((u - a) < tol2)
                {
                    d = std::copysign(tol1, midpoint - x);
                }

                if ((b - u) < tol2)
                {
                    d = std::copysign(tol1, midpoint - x);
                }
            }
        }

        // F must not be evaluated too close to X.
        if (tol1 <= std::fabs(d))
        {
            u = x + d;
        }
        else
        {
            u = x + std::copysign(tol1, d);
        }

        // Request value of F(U).
        arg = u;
        iteration = iteration + 1;

        return arg;
    }

private:
    double a;
    double b;
    int iteration = 0;
    double arg = 0.0;
    double c = 0.0;
    double d = 0.0;
    double e = 0.0;
    double fu = 0.0;
    double fv = 0.0;
    double fw = 0.0;
    double fx = 0.0;
    double p = 0.0;
    double q = 0.0;
    double r = 0.0;
    double u = 0.0;
    double v = 0.0;
    double w = 0.0;
    double x = 0.0;
};
