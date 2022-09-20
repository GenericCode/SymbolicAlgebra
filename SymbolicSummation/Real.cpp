//
//  Real.cpp
//  SymbolicSummation
//
//  Created by Robert Stahulak on 1/28/22.
//
#include "Real.hpp"
#include "Expression.hpp"
#include "Container.hpp"
#include "AlgebraicHelpers.hpp"
#include "AbstractHelpers.hpp"


Real::Real(const Real& target) {
    value = target.value;
};

Real::Real(float newVal)  {
    if(ceil(newVal) == newVal)
        value = (int)newVal;
    else
        value = newVal;
};

Real& Real::operator=(const Real& target) {
    value = target.value;
    return *this;
};
Expression Real::add(ExpressionObject* other) {
    if(isSubtypeOf(other, REALTYPE)) {
        Real& thisObj = *this;
        bool sign = other->getTypeHash() == SIGNTYPE;
        float otherVal;
        if(sign) {
            Real& otherObj = dynamic_cast<Real&>(*-*other);
            otherVal = -otherObj.value;
        } else {
            Real& otherObj = dynamic_cast<Real&>(*other);
            otherVal = otherObj.value;
        }
        float newVal = thisObj.value + otherVal;
        Expression result = declareReal(newVal);
        return result;
    } else if(other->getTypeHash() == ADDTYPE) {
        Expression negativeOf;
        return combineSums(this,other);
    }
    else {
        Expression result = *new Expression(new Add(this,other));
        return result;
    }
};
Expression Real::subtract(ExpressionObject* other) {
    if(isSubtypeOf(other, REALTYPE)) {
        Real& otherReal = dynamic_cast<Real&>(*other);
        float newVal = value-otherReal.value;
        Expression result = declareReal(newVal);
        return result;
    } else if(other->getTypeHash() == ADDTYPE) {
        Expression negativeOf = -*other;
        return combineSums(this,negativeOf.get());
    }
    else {
        Expression result = *new Expression(new Add(this,other));
        return result;
    }
};
Expression Real::negate() {
    
    //Expression newAdd = *new Expression(new Add(this,true));
    //return newAdd;
    Expression result = declareReal(-value);//*new Expression(new Sign(this));
    return result;
};
Expression Real::multiply(ExpressionObject* other) {
    if(isSubtypeOf(other, REALTYPE)) {
        Real& otherReal = dynamic_cast<Real&>(*other);
        float newVal = value*otherReal.value;
        Expression result = declareReal(newVal);
        return result;
    } else if(other->getTypeHash() == MULTYPE) {
        return combineProducts(this,other);
    }
    else  {
        return distribute(this, other);
    }
};
Expression Real::divide(ExpressionObject* other) {
    if(isSubtypeOf(other, REALTYPE)) {
        bool sign = other->getTypeHash() == SIGNTYPE;
        ExpressionObject* temp = NULL;
        if(sign)
            temp = (-*other).get();
        else
            temp = other;
        Real& otherReal = dynamic_cast<Real&>(*temp);
        float newVal = value/otherReal.value;
        if(sign)
            newVal *= -1;
        Expression result = declareReal(newVal);
        return result;
    } else  {
        ExpressionObject* reciprocalOf = new Frac(other);
        return distribute(this, reciprocalOf);
    }
};
/*
bool Real::containsTypeOfPerform(size_t type) {
    return type == REALTYPE;
};

bool Real::containsPerform(Expression target) {
    return (*this) == *target;
};

Expression Real::removePerform(, Expression target, bool rightToLeft) {
    if(target == this)
        return ZERO;
    return this;
}

Expression Real::firstInstanceOfTypePerform(, size_t type, bool rightToLeft) {
    Expression result = *new Expression(new NullObject("REALTYPE has no members"));
    return result;
}
*/
std::string Real::print() {
    std::string result;
    if(ceilf(value) == value)
        result = std::to_string((int)value);
    else
        result = std::to_string(value);
    return result;
}

Expression Zero::add(ExpressionObject* other) {
    return *new Expression(other);
};

Expression Zero::subtract(ExpressionObject* other) {
    return -*other;
};

Expression Zero::multiply(ExpressionObject* other) {
    return *new Expression(this);
};

Expression Zero::negate() {
    return *new Expression(this);
};

Expression One::multiply(ExpressionObject* other) {
    return *new Expression(other);
};
