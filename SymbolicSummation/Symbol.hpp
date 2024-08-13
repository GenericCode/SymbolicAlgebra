//
//  Symbol.hpp
//  SymbolicSummation
//
//  Created by Laina Stahulak on 2/3/22.
//

#ifndef Symbol_hpp
#define Symbol_hpp
#pragma once
#include <stdio.h>
#include <vector>
#include "Expression.hpp"

class Expression;
//typedef std::vector<Expression> ExprVector;
class Symbol : public ExpressionObject {
protected:
    String name = "";
    
    Expression divide(Expression other) const;
    Expression add(Expression other) const;
    Expression negate() const;
    Expression subtract(Expression other) const;
    Expression multiply(Expression left, Expression right) const;
public:
    String print() const {
        return name;
    };
    Expression simplify() const;
    Expression factor() const;
    Expression reciprocal() const;
    Expression determinant() const;
    Expression transpose() const;
    ExprVector getFactors() const;
    
    Symbol(const Symbol& target);
    Symbol& operator=(const Symbol& target);
    Symbol(String newName) {
        name = newName;
    };
    ~Symbol();
    
};

class ImaginaryUnit : public Symbol {
protected:
    /*
    Expression dividePerform(SymbolicObject* other);
    Expression addPerform(SymbolicObject* other);
    Expression subtractPerform(SymbolicObject* other);
     */
    //Expression multiply(Expression left, Expression right) const;
    //Expression negate() const;
     
public:
    ImaginaryUnit(const ImaginaryUnit& target);
    ImaginaryUnit& operator=(const ImaginaryUnit& target);
    ImaginaryUnit() : Symbol("i") {};
};

class Matrix : public Symbol {
private:
protected:
    std::pair<int, int> dimensions;// = {0,0};
    ExprMatrix elements = *new ExprMatrix();
    //{rows,columns}
    Expression divide(Expression other) const;
    Expression add(Expression other) const;
    Expression negate() const;
    Expression subtract(Expression other) const;
    Expression multiply(Expression left, Expression right) const;
public:
    String print() const;
    Expression simplify() const;
    //Expression distribute(Expression other) const;
    Expression factor() const;
    Expression reciprocal() const;
    Expression determinant() const;
    Expression transpose() const;
    //Expression cancelTerms() const;
    ExprVector getFactors() const;
    
    Matrix(const Matrix& target);
    Matrix& operator=(const Matrix& target);
    Matrix(String name, ExprMatrix newElements);
    Matrix(String name, std::initializer_list<std::initializer_list<Expression>> newElements);
    Matrix(String name, std::vector<int> newDimensions = {0,0});//empty matrix
    Matrix(Expression diag, int newDim = 0);//Identity matrix times const expression
    ~Matrix();
    ExprMatrix getElements() const;
    std::pair<int, int> getDimensions() const {
        return dimensions;
    }
    friend Expression matMul(Expression left, Expression right);
    friend ExprVector getConstituentSymbols(Expression target);
    friend Expression substitute(Expression source, Expression target, Expression value);
    friend bool areEqual(const ExpressionObject& left, const ExpressionObject& right);
    friend Expression performActions(Expression target);
    friend Expression performActionsOn(Expression target, Expression var);
    friend Expression insertAsVariable(Expression target, Expression var);
};

class EuclidVector : public Symbol {
private:
    ExprVector components;
protected:
    Expression add(Expression other) const;
    Expression multiply(Expression left, Expression right) const;
public:
    String print() const;
    Expression simplify() const;
    Expression factor() const;
    Expression reciprocal() const;
    Expression determinant() const;
    ExprVector getFactors() const;
    ExprVector getComponents() const;
    
    EuclidVector(const EuclidVector& target);
    EuclidVector& operator=(const EuclidVector& target);

    String getEuclidVectorName(ExprVector components) {
        String name = "(";
        for (int i = 0; i < 3; i++) {
            name += components[i].print();
            if (i < 2)
                name += ",";
        }
        name += ")";
        return name;
    }


    EuclidVector(String name, ExprVector elements);
    EuclidVector(ExprVector elements) : EuclidVector(getEuclidVectorName(elements), elements) {};
    //EuclidVector(std::initializer_list<Expression> newElements, String name = "");
    ~EuclidVector();
    friend ExprVector getConstituentSymbols(Expression target);
    friend Expression substitute(Expression source, Expression target, Expression value);
    friend bool areEqual(const ExpressionObject& left, const ExpressionObject& right);
    friend Expression performActions(Expression target);
    friend Expression performActionsOn(Expression target, Expression var);
    friend Expression insertAsVariable(Expression target, Expression var);
};

static const size_t SYMBOLTYPE = typeid(Symbol).hash_code();
static const size_t MATRIXTYPE = typeid(Matrix).hash_code();
static const size_t EUCLIDVECTORTYPE = typeid(EuclidVector).hash_code();
static const size_t IMAGINARYUNITTYPE = typeid(ImaginaryUnit).hash_code();
//static SymbolicObject* IUNIT = *new ImaginaryUnit();
//static Expression iunit = *new Expression(&IUNIT);
static Expression IMAGUNIT = *new Expression(new ImaginaryUnit());
#include "Real.hpp"
//static SymbolicObject* IDENTITYMAT = *new Matrix("I",{{one,zero},{zero,one}});
//static Expression identitymat = *new Expression(&IDENTITYMAT);
static Expression IDENMAT = *new Expression(new Matrix("I",{{ONE,ZERO},{ZERO,ONE}}));
static Expression XUNITVECTOR = *new Expression(new EuclidVector("xUnitVector", {ONE,ZERO,ZERO}));
static Expression YUNITVECTOR = *new Expression(new EuclidVector("yUnitVector", {ZERO,ONE,ZERO}));
static Expression ZUNITVECTOR = *new Expression(new EuclidVector("zUnitVector", {ZERO,ZERO,ONE}));
#endif /* Symbol_hpp */

