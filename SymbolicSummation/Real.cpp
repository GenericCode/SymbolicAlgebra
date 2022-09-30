//
//  Real.cpp
//  SymbolicSummation
//
//  Created by Laina Stahulak on 1/28/22.
//
#include "Real.hpp"
#include "Expression.hpp"
#include "Container.hpp"
#include "AlgebraicHelpers.hpp"
#include "AbstractHelpers.hpp"


Real::Real( const Real& target) {
    value = target.value;
};

Real::Real(float newVal)  {
    if(ceil(newVal) == newVal)
        value = (int)newVal;
    else
        value = newVal;
};

const Real& Real::operator=(const Real& target) {
    value = target.value;
    return *this;
};
Expression Real::add(Expression other) const {
    if(isSubtypeOf(other, REALTYPE)) {
        const Real& thisObj = *this;
        bool sign = other->getTypeHash() == SIGNTYPE;
        float otherVal;
        if(sign) {
            const Real& otherObj = dynamic_cast<const Real&>(*-other);
            otherVal = -otherObj.value;
        } else {
            const Real& otherObj = dynamic_cast<const Real&>(*other);
            otherVal = otherObj.value;
        }
        float newVal = thisObj.value + otherVal;
        Expression result = declareReal(newVal);
        return result;
    } else if(other->getTypeHash() == ADDTYPE) {
        Expression negativeOf;
        return combineSums(*new Expression(this),other);
    }
    else {
        Expression result = *new Expression(new Add(*new Expression(this),other));
        return result;
    }
};
Expression Real::subtract(Expression other) const {
    if(isSubtypeOf(other, REALTYPE)) {
        const Real& otherReal = dynamic_cast<const Real&>(*other);
        float newVal = value-otherReal.value;
        Expression result = declareReal(newVal);
        return result;
    } else if(other->getTypeHash() == ADDTYPE) {
        Expression negativeOf = -*other;
        return combineSums(*new Expression(this),negativeOf);
    }
    else {
        Expression result = *new Expression(new Add(*new Expression(this),other));
        return result;
    }
};
Expression Real::negate() const {
    Expression result = declareReal(-value);
    return result;
};
Expression Real::multiply(Expression other) const {
    if(isSubtypeOf(other, REALTYPE)) {
        if(other.getTypeHash() != SIGNTYPE) {
            const Real& otherReal = dynamic_cast<const Real&>(*other);
            float newVal = value*otherReal.value;
            Expression result = declareReal(newVal);
            return result;
        } else {
            const Real& otherReal = dynamic_cast<const Real&>(*-other);
            float newVal = -value*otherReal.value;
            Expression result = declareReal(newVal);
            return result;
        }
    } else if(other->getTypeHash() == MULTYPE) {
        return combineProducts(*new Expression(this),other);
    }
    else  {
        return distribute(*new Expression(this), other);
    }
};
Expression Real::divide(Expression other) const {
    if(isSubtypeOf(other, REALTYPE)) {
        bool sign = other->getTypeHash() == SIGNTYPE;
        Expression temp = NULL;
        if(sign)
            temp = (-*other);
        else
            temp = other;
        const Real& otherReal = dynamic_cast<const Real&>(*temp);
        float newVal = value/otherReal.value;
        if(sign)
            newVal *= -1;
        Expression result = declareReal(newVal);
        return result;
    } else  {
        Expression reciprocalOf = new Frac(other);
        return distribute(*new Expression(this), reciprocalOf);
    }
};
String Real::print() const {
    String result;
    if(ceilf(value) == value)
        result = std::to_string((int)value);
    else
        result = std::to_string(value);
    return result;
}

Expression Zero::add(Expression other) const {
    return *new Expression(other);
};

Expression Zero::subtract(Expression other) const {
    return -*other;
};

Expression Zero::multiply(Expression other) const {
    return *new Expression(this);
};

Expression Zero::negate() const {
    return *new Expression(this);
};

Expression One::multiply(Expression other) const {
    return *new Expression(other);
};
