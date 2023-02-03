//
//  Real.hpp
//  SymbolicSummation
//
//  Created by Laina Stahulak on 1/28/22.
//
#pragma once
#ifndef Real_hpp
#define Real_hpp

#include <stdio.h>
#include "Expression.hpp"
//typedef std::vector<Expression> ExprVector;

class Real : public ExpressionObject {
protected:
    float value;
    Expression add(Expression other) const;
    Expression subtract(Expression other) const;
    Expression negate() const;
    Expression multiply(Expression other) const;
    Expression divide(Expression other) const;
public:
    String print() const;
    Expression simplify() const;
    Expression distribute(Expression other) const;
    Expression factor() const;
    Expression reciprocal() const;
    Expression determinant() const;
    Expression transpose() const;
    Expression cancelTerms() const;
    ExprVector getFactors() const;
    float getValue() const {
        return value;
    };
    Real(const Real& target);
    const Real& operator=(const Real& target);
    Real(float newVal);
    friend bool areEqual(const ExpressionObject& left, const ExpressionObject& right);
    friend ExprVector getFactorsOfInt(Expression factee);
    friend Expression simplify(Expression target);
};

class Zero : public Real {
public:
    Zero(const Zero& target) : Real(0) {};
    Zero() : Real(0) {
        
    }
protected:
    Expression add(Expression other) const;
    Expression subtract(Expression other) const;
    Expression multiply(Expression other) const;
    Expression negate() const;
};

class One : public Real {
public:
    One(const One& target) : Real(1) {};
    One() : Real(1) {
        
    }
protected:
    Expression multiply(Expression other) const;
};
#include "Container.hpp"
static const size_t REALTYPE = typeid(Real).hash_code();
static const size_t ONETYPE = typeid(One).hash_code();
static const size_t ZEROTYPE = typeid(Zero).hash_code();

static const Expression ZERO = *new Expression(new Zero());
static const Expression ONE = *new Expression(new One());
static const Expression MINUSONE = *new Expression(new Sign(ONE));
static const Expression PI = *new Expression(new Real(3.14159));
static const Expression EULERNUM = *new Expression(new Real(2.71828));

#endif /* Real_hpp */
