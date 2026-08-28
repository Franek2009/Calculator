#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "Expression.h"

namespace Calculator
{
    class Evaluator
    {
    public:
        double evaluate(const Expression& expression) const;
    };
}

#endif
