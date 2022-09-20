//
//  Real.hpp
//  SymbolicSummation
//
//  Created by Robert Stahulak on 1/28/22.
//
#pragma once
#ifndef Real_hpp
#define Real_hpp

#include <stdio.h>
#include "Expression.h"
//typedef std::vector<Expression> ExprVector;

class Real : public ExpressionObject {
protected:
    float value;
    Expression add(ExpressionObject* other);
    Expression subtract(ExpressionObject* other);
    Expression negate();
    Expression multiply(ExpressionObject* other);
    Expression divide(ExpressionObject* other);
    /*
    bool containsTypeOfPerform(size_t type);
    bool containsPerform(Expression target);
    Expression firstInstanceOfTypePerform(, size_t type, bool rightToLeft);
    Expression removePerform(, Expression target, bool rightToLeft);
     */
public:
    std::string print();
    Real(const Real& target);
    Real& operator=(const Real& target);
    Real(float newVal);
    friend bool areEqual(ExpressionObject& left, ExpressionObject& right);
    friend ExprVector getFactorsOfInt(ExpressionObject* factee);
    friend Expression simplify(Expression target);
};

class Zero : public Real {
public:
    Zero(const Zero& target) : Real(0) {};
    Zero() : Real(0) {
        
    }
protected:
    Expression add(ExpressionObject* other);
    Expression subtract(ExpressionObject* other);
    Expression multiply(ExpressionObject* other);
    Expression negate();
};

class One : public Real {
public:
    One(const One& target) : Real(1) {};
    One() : Real(1) {
        
    }
protected:
    Expression multiply(ExpressionObject* other);
};
#include "Container.hpp"
static const size_t REALTYPE = typeid(Real).hash_code();
static const size_t ONETYPE = typeid(One).hash_code();
static const size_t ZEROTYPE = typeid(Zero).hash_code();
/*
static SymbolicObject* ZERO = *new Zero();
static SymbolicObject* ONE = *new One();
static SymbolicObject* MINUSONE = *new Sign(ONE);
static SymbolicObject* PI = *new Real(3.14159);
static SymbolicObject* E = *new Real(2.71828);

static const Expression zero = *new Expression(&ZERO);
static const Expression one = *new Expression(&ONE);
static const Expression minusone = *new Expression(&MINUSONE);
static const Expression pi = *new Expression(&PI);
static const Expression eulernum = *new Expression(&E);
*/

static const Expression ZERO = *new Expression(new Zero());
static const Expression ONE = *new Expression(new One());
static const Expression MINUSONE = *new Expression(new Sign(ONE.get()));
static const Expression PI = *new Expression(new Real(3.14159));
static const Expression EULERNUM = *new Expression(new Real(2.71828));
/*
static const size_t REALTYPE = typeid(Real).hash_code();
static Zero& ZERO = *new Zero();
static One& ONE = *new One();
static Real& PI = *new Real(3.14159);
static Real& E = *new Real(2.71828);*/
#endif /* Real_hpp */
