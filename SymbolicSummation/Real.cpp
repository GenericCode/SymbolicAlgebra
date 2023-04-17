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
#include <cmath>


Real::Real( const Real& target) {
    value = target.value;
};

Real::Real(double newVal)  {
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
    Expression thisExpr = *new Expression(this);
    if(isTypeSimilarTo(other, REALTYPE)) {
        bool sign = other.getTypeHash() == SIGNTYPE;
        double otherVal;
        if(sign) {
            const Real& otherObj = dynamic_cast<const Real&>(*-other);
            otherVal = -otherObj.value;
        } else {
            const Real& otherObj = dynamic_cast<const Real&>(*other);
            otherVal = otherObj.value;
        }
        double newVal = value + otherVal;
        Expression result = declareReal(newVal);
        return result;
    }
    if(other.getTypeHash() == SUMTYPE) {
        const Sum& otherAdd = dynamic_cast<const Sum&>(*other);
        ExprVector newMembers = *new ExprVector();
        newMembers.push_back(thisExpr);
        newMembers = setUnion(newMembers, otherAdd.getMembers());
        return *new Expression(new Sum(newMembers));
    }
    return *new Expression(new Sum(thisExpr,other));
};
Expression Real::subtract(Expression other) const {
    Expression thisExpr = *new Expression(this);
    if(isTypeSimilarTo(other, REALTYPE)) {
        const Real& otherReal = dynamic_cast<const Real&>(*other);
        double newVal = value-otherReal.value;
        Expression result = declareReal(newVal);
        return result;
    }
    if(other.getTypeHash() == SUMTYPE) {
        const Sum& otherNegativeAdd = dynamic_cast<const Sum&>(*-other);
        ExprVector newMembers = *new ExprVector();
        newMembers.push_back(thisExpr);
        newMembers = setUnion(newMembers, otherNegativeAdd.getMembers());
        return *new Expression(new Sum(newMembers));
    }
    return *new Expression(new Sum(thisExpr,-other));
};
Expression Real::negate() const {
    Expression result = declareReal(-value);
    return result;
};
Expression Real::multiply(Expression other) const {
    Expression thisExpr = *new Expression(this);
    if(isTypeSimilarTo(other, REALTYPE)) {
        if(other.getTypeHash() != SIGNTYPE) {
            const Real& otherReal = dynamic_cast<const Real&>(*other);
            double newVal = value*otherReal.value;
            Expression result = declareReal(newVal);
            return result;
        } else {
            const Real& otherReal = dynamic_cast<const Real&>(*-other);
            double newVal = -value*otherReal.value;
            Expression result = declareReal(newVal);
            return result;
        }
    }
    return distribute(other);
};
Expression Real::divide(Expression other) const {
    Expression thisExpr = *new Expression(this);
    if(isTypeSimilarTo(other, REALTYPE)) {
        bool sign = other.getTypeHash() == SIGNTYPE;
        Expression temp = NULL;
        if(sign)
            temp = (-*other);
        else
            temp = other;
        const Real& otherReal = dynamic_cast<const Real&>(*temp);
        double newVal = value/otherReal.value;
        if(sign)
            newVal *= -1;
        Expression result = declareReal(newVal);
        return result;
    } else  {
        return *new Expression(new Fraction(thisExpr,other));
    }
};
String Real::print() const {
    String result;
    if((double)ceilf(value) == value)
        result = std::to_string((int)value);
    else
        result = std::to_string(value);
    return result;
}

Expression Real::simplify() const {
    return *new Expression(this);
};
Expression Real::distribute(Expression other) const {
    Expression thisExpr = *new Expression(this);
    if(other.getTypeHash() == PRODUCTTYPE) {
        const Product& otherMul = dynamic_cast<const Product&>(*other);
        ExprVector newMembers = *new ExprVector();
        ExprVector mulMembers = otherMul.getMembers();
        int index = positionOfType(mulMembers, REALTYPE);
        Expression result = index >= 0? thisExpr*mulMembers[index] : thisExpr;
        newMembers.push_back(result);
        for(size_t i = 0; i< mulMembers.size(); i++) {
            if(i == index)
                continue;
            newMembers.push_back(mulMembers[i]);
        }
        return *new Expression(new Product(newMembers));
    }
    if(other.getTypeHash() == SUMTYPE) {
        const Sum& otherAdd= dynamic_cast<const Sum&>(*other);
        ExprVector newMembers = *new ExprVector();
        ExprVector addMembers = otherAdd.getMembers();
        for(size_t i = 0; i< addMembers.size(); i++) {
            newMembers.push_back(distribute(addMembers[i]));
        }
        return *new Expression(new Sum(newMembers));
    }
    if(other.getTypeHash() == SIGNTYPE) {
        const Sign& signObj = dynamic_cast<const Sign&>(*other);
        return (-thisExpr).distribute(signObj.getMember());
    }
    ExprVector newMembers = *new ExprVector();
    newMembers.push_back(*new Expression(this));
    newMembers.push_back(other);
    return *new Expression(new Product(newMembers));
};
Expression Real::factor() const {
    Expression thisExpr = *new Expression(this);
    ExprVector factors = getFactorsOfInt(thisExpr);
    if(factors.size() > 1)
        return *new Expression(new Mul(factors));
    return thisExpr;

};
Expression Real::reciprocal() const {
    return *new Expression(new Fraction(*new Expression(this)));
};
Expression Real::determinant() const {
    return *new Expression(this);
};
Expression Real::transpose() const {
    return *new Expression(this);
};
Expression Real::cancelTerms() const {
    return *new Expression(this);
};
ExprVector Real::getFactors() const {
    return getFactorsOfInt(*new Expression(this));
};

Expression Zero::add(Expression other) const {
    return *new Expression(other);
};

Expression Zero::subtract(Expression other) const {
    return -other;
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
