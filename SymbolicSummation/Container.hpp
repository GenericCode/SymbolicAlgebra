//
//  Operator.hpp
//  SymbolicSummation
//
//  Created by Laina Stahulak on 1/29/22.
//
#pragma once
#include <stdio.h>
#include <vector>
#include <functional>
#include "Expression.hpp"
#include "AlgebraicHelpers.hpp"
//typedef std::vector<Expression> ExprVector;

class Container : public ExpressionObject {
protected:
    String name = "";
    Expression add(Expression other) const;
    Expression multiply(Expression other) const;
    Expression divide(Expression other) const;
    Expression subtract(Expression other) const;
    Expression negate() const;
public:
    ~Container();
};

class Add : public Container {
private:
    ExprVector members;
protected:
    //SignVector memberSigns = *new SignVector();
    Expression negate() const;
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
    ExprVector getMembers() const {
        return *new ExprVector(members);
    };
    
    Add(const Add& target);
    Add& operator=(const Add& target);
    Add(std::initializer_list<Expression> newMembers);
    Add(ExprVector newMembers);
    Add(Expression left, Expression right);
    ~Add();/*
    friend Expression distribute(Expression left, Expression right);
    friend ExprVector getFactors(Expression factee);
    friend Expression combineProducts(Expression left, Expression right);
    friend Expression cancelTerms(Expression target);
    friend Expression simplify(Expression target);
    friend Expression simplifyPauliMatrices(Expression target);
    friend Expression combineSums(Expression left, Expression right);
    friend bool areEqual(const ExpressionObject& left, const ExpressionObject& right);
    friend Expression getElementOfType(Expression source, size_t type, bool rightToLeft);
    friend Expression replaceElement(Expression source, Expression target, Expression value, bool rightToLeft);
    friend Expression replaceElementOfType(Expression source, size_t type, Expression value, bool rightToLeft);
    friend ExprVector getConstituentSymbols(Expression target);
    friend Expression removeElementAdditively(Expression source, Expression target, bool rightToLeft);
    friend Expression substitute(Expression source, Expression target, Expression value);
    friend Expression performActions(Expression target);
    friend Expression performActionsOn(Expression target, Expression var);
    friend Expression insertAsVariable(Expression target, Expression var);
    friend Expression getElementMatchingCondition(Expression source, std::function<bool(Expression)> condition, bool rightToLeft);*/
};

class Sign : public Container {
private:
    Expression member;
protected:
    Expression negate() const;
    Expression multiply(Expression other) const;
    Expression add(Expression other) const;
    Expression subtract(Expression other) const;
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
    Expression getMember() const {
        return member;
    };
    
    Sign(const Sign& target);
    const Sign& operator=(const Sign& target);
    Sign(Expression member);
    ~Sign();
    /*
    friend Expression distribute(Expression left, Expression right);
    friend ExprVector getFactors(Expression factee);
    friend Expression combineProducts(Expression left, Expression right);
    friend Expression combineTermsDifferingByCoefficientsAdditively(Expression left, Expression right);
    friend Expression cancelTerms(Expression target);
    friend Expression simplify(Expression target);
    friend Expression combineSums(Expression left, Expression right);
    friend bool areEqual(const ExpressionObject& left, const ExpressionObject& right);
    friend Expression getElementOfType(Expression source, size_t type, bool rightToLeft);
    friend Expression replaceElement(Expression source, Expression target, Expression value, bool rightToLeft);
    friend Expression replaceElementOfType(Expression source, size_t type, Expression value, bool rightToLeft);
    friend Expression removeElementMultiplicatively(Expression source, Expression target, bool rightToLeft);
    friend ExprVector getConstituentSymbols(Expression target);
    friend Expression substitute(Expression source, Expression target, Expression value);
    friend Expression performActions(Expression target);
    friend Expression performActionsOn(Expression target, Expression var);
    friend Expression insertAsVariable(Expression target, Expression var);
    friend Expression getElementMatchingCondition(Expression source, std::function<bool(Expression)> condition, bool rightToLeft);*/
    
};

class Mul : public Container {
private:
    ExprVector members;
protected:
    Expression negate()  const;
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
    ExprVector getMembers() const {
        return *new ExprVector(members);
    };
    
    Mul(const Mul& target);
    Mul& operator=(const Mul& target);
    Mul(ExprVector newMembers);
    Mul(Expression right, Expression left);
    ~Mul();
    /*
    friend Expression simplifyMulWithPauliMatrices(Expression target);
    friend Expression distribute(Expression left, Expression right);
    friend ExprVector getFactors(Expression factee);
    friend Expression combineProducts(Expression left, Expression right);
    friend Expression combineSums(Expression left, Expression right);
    friend Expression simplify(Expression target);
    friend Expression simplifyPauliMatrices(Expression target);
    friend bool areEqual(const ExpressionObject& left, const ExpressionObject& right);
    friend Expression getElementOfType(Expression source, size_t type, bool rightToLeft);
    friend Expression replaceElement(Expression source, Expression target, Expression value, bool rightToLeft);
    friend Expression replaceElementOfType(Expression source, size_t type, Expression value, bool rightToLeft);
    friend ExprVector getConstituentSymbols(Expression target);
    friend Expression removeElementMultiplicatively(Expression source, Expression target, bool rightToLeft);
    friend Expression substitute(Expression source, Expression target, Expression value);
    friend Expression performActions(Expression target);
    friend Expression performActionsOn(Expression target, Expression var);
    friend Expression insertAsVariable(Expression target, Expression var);
    friend Expression getElementMatchingCondition(Expression source, std::function<bool(Expression)> condition, bool rightToLeft);*/
};

class Frac : public Container {
private:
    Expression numerator;
    Expression denomenator;
protected:
    Expression negate() const;
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
    Expression getNumerator() const {
        return numerator;
    };
    Expression getDenomenator() const {
        return denomenator;
    };
    
    Frac& operator=(const Frac& target);
    Frac(const Frac& target);
    Frac(Expression denom);
    Frac(Expression num, Expression denom);
    ~Frac();
    /*
    friend Expression distribute(Expression left, Expression right);
    friend ExprVector getFactors(Expression factee);
    friend Expression combineProducts(Expression left, Expression right);
    friend Expression combineSums(Expression left, Expression right);
    friend Expression simplify(Expression target);
    friend bool areEqual(const ExpressionObject& left, const ExpressionObject& right);
    friend Expression getElementOfType(Expression source, size_t type, bool rightToLeft);
    friend Expression replaceElement(Expression source, Expression target, Expression value, bool rightToLeft);
    friend Expression replaceElementOfType(Expression source, size_t type, Expression value, bool rightToLeft);
    friend Expression reciprocal(Expression self);
    friend ExprVector getConstituentSymbols(Expression target);
    friend Expression removeElementMultiplicatively(Expression source, Expression target, bool rightToLeft);
    friend Expression substitute(Expression source, Expression target, Expression value);
    friend Expression performActions(Expression target);
    friend Expression performActionsOn(Expression target, Expression var);
    friend Expression insertAsVariable(Expression target, Expression var);
    friend Expression getElementMatchingCondition(Expression source, std::function<bool(Expression)> condition, bool rightToLeft);*/
};

class Exp : public Container {
private:
    Expression base;
    Expression exponent;
protected:
    Expression negate() const;
    Expression multiply(Expression other) const;
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
    Expression getBase() const {
        return base;
    };
    Expression getExponent() const {
        return exponent;
    };
    
    const Exp& operator=(const Exp& target);
    Exp(const Exp& target);
    Exp(Expression base, Expression exponent);
    Exp(Expression base, int exponent);
    ~Exp();
    /*
    friend Expression distribute(Expression left, Expression right);
    friend ExprVector getFactors(Expression factee);
    friend Expression combineProducts(Expression left, Expression right);
    friend Expression combineSums(Expression left, Expression right);
    friend Expression simplify(Expression target);
    friend bool areEqual(const ExpressionObject& left, const ExpressionObject& right);
    friend Expression getElementOfType(Expression source, size_t type, bool rightToLeft);
    friend Expression replaceElementOfType(Expression source, size_t type, Expression value, bool rightToLeft);
    friend ExprVector getConstituentSymbols(Expression target);
    friend Expression removeElementMultiplicatively(Expression source, Expression target, bool rightToLeft);
    friend Expression substitute(Expression source, Expression target, Expression value);
    friend Expression performActions(Expression target);
    friend Expression performActionsOn(Expression target, Expression var);
    friend Expression insertAsVariable(Expression target, Expression var);
    friend Expression getElementMatchingCondition(Expression source, std::function<bool(Expression)> condition, bool rightToLeft);*/
};

//typedef std::function<Expression(Expression)> ExprActionObj;
typedef std::function<Expression(Expression)> ExprAction;

/*
class ExprAction : public std::shared_ptr<const ExprActionObj> {
public:
    ExprAction() : std::shared_ptr<const ExprActionObj>(NULL) {};
    ExprAction(const ExprActionObj& obj) : std::shared_ptr<const ExprActionObj>(&obj) {};
    Expression operator()(Expression var) const {
        return (**this)(var);
    }
};*/

class Func : public Container {
protected:
    String funcName;
    ExprAction functionAction;
    Expression member = *new Expression(new NullObject("stand-in for a generic variable"));
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
    Expression act() const {
        return functionAction(member);
    };
    //actingOn() guaranteed to be pointing at a Func object?
    Expression actingOn(Expression variable) const;
    Expression resultOfActingOn(Expression variable) const {
        return functionAction(variable);
    };
    ExprAction getAction() const {
        return *new ExprAction(functionAction);
    };
    Expression getMember() const {
        return member;
    }
    
    Func(const Func& target);
    Func& operator=(const Func& target);
    //Should probably automatically register ANY generic (not containing structure i.e. no members) function that is created.
    //need to add a helper function that will attempt to insert an expression as variable to any functions in another expression
    //common functions like transpose, exp or whatever should probably have private/protected constructors? no need to make more
    Func(String name);
    //Func(String name, ExprActionObj action);
    Func(String name, ExprAction action);
    Func(String name, ExprAction action, Expression member);
    /*
    friend Expression parseString(String expr);
    friend Expression distribute(Expression left, Expression right);
    friend ExprVector getFactors(Expression factee);
    friend Expression combineProducts(Expression left, Expression right);
    friend Expression combineSums(Expression left, Expression right);
    friend Expression simplify(Expression target);
    friend bool areEqual(const ExpressionObject& left, const ExpressionObject& right);
    friend Expression getElementOfType(Expression source, size_t type, bool rightToLeft);
    friend Expression replaceElementOfType(Expression source, size_t type, Expression value, bool rightToLeft);
    friend ExprVector getConstituentSymbols(Expression target);
    friend Expression removeElementMultiplicatively(Expression source, Expression target, bool rightToLeft);
    friend Expression substitute(Expression source, Expression target, Expression value);
    friend Expression performActions(Expression target);
    friend Expression performActionsOn(Expression target, Expression var);
    friend Expression insertAsVariable(Expression target, Expression var);
    friend Expression getElementMatchingCondition(Expression source, std::function<bool(Expression)> condition, bool rightToLeft);*/
};
static const Expression TRANSPOSE = *new Expression(new Func("transpose",transpose));

static const size_t CONTAINERTYPE = typeid(Container).hash_code();
static const size_t ADDTYPE = typeid(Add).hash_code();
static const size_t SIGNTYPE = typeid(Sign).hash_code();
static const size_t MULTYPE = typeid(Mul).hash_code();
static const size_t FRACTYPE = typeid(Frac).hash_code();
static const size_t EXPTYPE = typeid(Exp).hash_code();
static const size_t FUNCTYPE = typeid(Func).hash_code();
