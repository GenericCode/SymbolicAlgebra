//
//  Operator.hpp
//  SymbolicSummation
//
//  Created by Laina Stahulak on 1/29/22.
//
#pragma once
#ifndef Operator_hpp
#define Operator_hpp

#include <stdio.h>
#include <vector>
#include "Expression.hpp"
//typedef std::vector<Expression> ExprVector;

class Container : public ExpressionObject {
protected:
    std::string name = "";
    Expression add(ExpressionObject* other);
    Expression multiply(ExpressionObject* other);
    Expression divide(ExpressionObject* other);
    Expression subtract(ExpressionObject* other);
public:
};

class Add : public Container {
protected:
    ExprVector members = *new ExprVector();
    //SignVector memberSigns = *new SignVector();
    Expression negate();
public:
    std::string print();
    Add(const Add& target);
    Add& operator=(const Add& target);
    Add(std::initializer_list<Expression> newMembers);
    Add(ExprVector newMembers);
    Add(ExpressionObject* left, ExpressionObject* right);
    ~Add();
    friend Expression distribute(ExpressionObject* left, ExpressionObject* right);
    friend ExprVector getFactors(ExpressionObject* factee);
    friend Expression combineProducts(ExpressionObject* left, ExpressionObject* right);
    friend Expression cancelTerms(ExpressionObject* target);
    friend Expression simplify(ExpressionObject* target);
    friend Expression simplifyPauliMatrices(ExpressionObject* target);
    friend Expression combineSums(ExpressionObject* left, ExpressionObject* right);
    friend bool areEqual(ExpressionObject& left, ExpressionObject& right);
    friend Expression getElementOfType(ExpressionObject* source, size_t type, bool rightToLeft);
    friend Expression replaceElementOfType(ExpressionObject* source, size_t type, ExpressionObject* value, bool rightToLeft);
    friend ExprVector getConstituentSymbols(ExpressionObject* target);
    friend Expression removeElementAdditively(ExpressionObject* source, ExpressionObject* target, bool rightToLeft);
    friend Expression substitute(ExpressionObject* source, ExpressionObject* target, ExpressionObject* value);
};

class Sign : public Container {
protected:
    Expression member;
    Expression negate();
    Expression multiply(ExpressionObject* other);
    Expression add(ExpressionObject* other);
    Expression subtract(ExpressionObject* other);
    std::string print();
public:
    Sign(const Sign& target);
    Sign& operator=(const Sign& target);
    Sign(ExpressionObject* member);
    ~Sign();
    friend Expression distribute(ExpressionObject* left, ExpressionObject* right);
    friend ExprVector getFactors(ExpressionObject* factee);
    friend Expression combineProducts(ExpressionObject* left, ExpressionObject* right);
    friend Expression combineTermsDifferingByCoefficientsAdditively(ExpressionObject* left, ExpressionObject* right);
    friend Expression cancelTerms(ExpressionObject* target);
    friend Expression simplify(ExpressionObject* target);
    friend Expression combineSums(ExpressionObject* left, ExpressionObject* right);
    friend bool areEqual(ExpressionObject& left, ExpressionObject& right);
    friend Expression getElementOfType(ExpressionObject* source, size_t type, bool rightToLeft);
    friend Expression replaceElementOfType(ExpressionObject* source, size_t type, ExpressionObject* value, bool rightToLeft);
    friend Expression removeElementMultiplicatively(ExpressionObject* source, ExpressionObject* target, bool rightToLeft);
    friend ExprVector getConstituentSymbols(ExpressionObject* target);
    friend Expression substitute(ExpressionObject* source, ExpressionObject* target, ExpressionObject* value);
    
};

class Mul : public Container {
protected:
    ExprVector members = *new ExprVector();
    Expression negate();
    std::string print();
public:
    Mul(const Mul& target);
    Mul& operator=(const Mul& target);
    Mul(ExprVector newMembers);
    Mul(ExpressionObject* right, ExpressionObject* left);
    ~Mul();
    friend Expression distribute(ExpressionObject* left, ExpressionObject* right);
    friend ExprVector getFactors(ExpressionObject* factee);
    friend Expression combineProducts(ExpressionObject* left, ExpressionObject* right);
    friend Expression combineSums(ExpressionObject* left, ExpressionObject* right);
    friend Expression simplify(ExpressionObject* target);
    friend Expression simplifyPauliMatrices(ExpressionObject* target);
    friend bool areEqual(ExpressionObject& left, ExpressionObject& right);
    friend Expression getElementOfType(ExpressionObject* source, size_t type, bool rightToLeft);
    friend Expression replaceElementOfType(ExpressionObject* source, size_t type, ExpressionObject* value, bool rightToLeft);
    friend ExprVector getConstituentSymbols(ExpressionObject* target);
    friend Expression removeElementMultiplicatively(ExpressionObject* source, ExpressionObject* target, bool rightToLeft);
    friend Expression substitute(ExpressionObject* source, ExpressionObject* target, ExpressionObject* value);
};

class Frac : public Container {
protected:
    Expression numerator;
    Expression denomenator;
    Expression negate();
    std::string print();
public:
    Frac& operator=(const Frac& target);
    Frac(const Frac& target);
    Frac(ExpressionObject* denom);
    Frac(ExpressionObject* num, ExpressionObject* denom);
    ~Frac();
    friend Expression distribute(ExpressionObject* left, ExpressionObject* right);
    friend ExprVector getFactors(ExpressionObject* factee);
    friend Expression combineProducts(ExpressionObject* left, ExpressionObject* right);
    friend Expression combineSums(ExpressionObject* left, ExpressionObject* right);
    friend Expression simplify(ExpressionObject* target);
    friend bool areEqual(ExpressionObject& left, ExpressionObject& right);
    friend Expression getElementOfType(ExpressionObject* source, size_t type, bool rightToLeft);
    friend Expression replaceElementOfType(ExpressionObject* source, size_t type, ExpressionObject* value, bool rightToLeft);
    friend Expression reciprocal(ExpressionObject* self);
    friend ExprVector getConstituentSymbols(ExpressionObject* target);
    friend Expression removeElementMultiplicatively(ExpressionObject* source, ExpressionObject* target, bool rightToLeft);
    friend Expression substitute(ExpressionObject* source, ExpressionObject* target, ExpressionObject* value);
};

class Exp : public Container {
protected:
    Expression base;
    Expression exponent;
    Expression negate();
    Expression multiply(ExpressionObject* other);
    std::string print();
public:
    Exp& operator=(const Exp& target);
    Exp(const Exp& target);
    Exp(ExpressionObject* base, ExpressionObject* exponent);
    Exp(ExpressionObject* base, int exponent);
    ~Exp();
    friend Expression distribute(ExpressionObject* left, ExpressionObject* right);
    friend ExprVector getFactors(ExpressionObject* factee);
    friend Expression combineProducts(ExpressionObject* left, ExpressionObject* right);
    friend Expression combineSums(ExpressionObject* left, ExpressionObject* right);
    friend Expression simplify(ExpressionObject* target);
    friend bool areEqual(ExpressionObject& left, ExpressionObject& right);
    friend Expression getElementOfType(ExpressionObject* source, size_t type, bool rightToLeft);
    friend Expression replaceElementOfType(ExpressionObject* source, size_t type, ExpressionObject* value, bool rightToLeft);
    friend ExprVector getConstituentSymbols(ExpressionObject* target);
    friend Expression removeElementMultiplicatively(ExpressionObject* source, ExpressionObject* target, bool rightToLeft);
    friend Expression substitute(ExpressionObject* source, ExpressionObject* target, ExpressionObject* value);
};

class Func : public Container {
protected:
    std::string funcName;
    ExprVector members = *new ExprVector();
    Expression add(ExpressionObject* other);
    Expression multiply (ExpressionObject* other);
    Expression divide (ExpressionObject* other);
    Expression subtract(ExpressionObject* other);
    Expression negate();
    std::string print();
public:
    Func& operator=(const Func& target);
    friend Expression distribute(ExpressionObject* left, ExpressionObject* right);
    friend ExprVector getFactors(ExpressionObject* factee);
    friend Expression combineProducts(ExpressionObject* left, ExpressionObject* right);
    friend Expression combineSums(ExpressionObject* left, ExpressionObject* right);
    friend bool areEqual(ExpressionObject& left, ExpressionObject& right);
    friend Expression getElementOfType(ExpressionObject* source, size_t type, bool rightToLeft);
    friend Expression replaceElementOfType(ExpressionObject* source, size_t type, ExpressionObject* value, bool rightToLeft);
    friend ExprVector getConstituentSymbols(ExpressionObject* target);
};

static const size_t OPERATORTYPE = typeid(Container).hash_code();
static const size_t ADDTYPE = typeid(Add).hash_code();
static const size_t SIGNTYPE = typeid(Sign).hash_code();
static const size_t MULTYPE = typeid(Mul).hash_code();
static const size_t FRACTYPE = typeid(Frac).hash_code();
static const size_t EXPTYPE = typeid(Exp).hash_code();
#endif /* Operator_hpp */
