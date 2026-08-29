#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "Expression.h"

namespace Calculator
{
    enum class AngleMode
    {
        Radians,
        Degrees
    };

    class Evaluator
    {
    public:
        explicit Evaluator(AngleMode angleMode = AngleMode::Radians);

        double evaluate(const Expression& expression) const;

    private:
        double angleInRadians(double angle) const;

        AngleMode angleMode;
    };
}

#endif
