#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <optional>

#include "Expression.h"

namespace Calculator
{
    enum class AngleMode
    {
        Radians,
        Degrees
    };

    struct EvaluationContext
    {
        AngleMode angleMode = AngleMode::Radians;
        std::optional<double> answer;
    };

    class Evaluator
    {
    public:
        Evaluator();
        explicit Evaluator(AngleMode angleMode);
        explicit Evaluator(EvaluationContext context);

        double evaluate(const Expression& expression) const;

    private:
        double angleInRadians(double angle) const;

        EvaluationContext context;
    };
}

#endif
