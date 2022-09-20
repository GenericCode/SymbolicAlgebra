//
//  Symbol.hpp
//  SymbolicSummation
//
//  Created by Robert Stahulak on 2/3/22.
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
    std::string name = "";
    
    Expression divide(ExpressionObject* other);
    Expression add(ExpressionObject* other);
    Expression negate();
    Expression subtract(ExpressionObject* other);
    Expression multiply(ExpressionObject* other);
    /*
    bool containsPerform(Expression target);
    bool containsTypeOfPerform(size_t type);
    Expression firstInstanceOfTypePerform(, size_t type, bool rightToLeft);
    Expression removePerform(SymbolicObject* other, bool rightToLeft);
     */
    std::string print() {
        return name;
    };
public:
    Symbol(const Symbol& target);
    Symbol(std::string newName) {
        name = newName;
    };
    
};

class ImaginaryUnit : public Symbol {
protected:
    /*
    Expression dividePerform(SymbolicObject* other);
    Expression addPerform(SymbolicObject* other);
    Expression subtractPerform(SymbolicObject* other);
     */
    Expression multiply(ExpressionObject* other);
     
public:
    ImaginaryUnit(const ImaginaryUnit& target);
    ImaginaryUnit() : Symbol("i") {};
};

class Matrix : public Symbol {
protected:
    //{rows,columns}
    std::pair<int,int> dimensions;// = {0,0};
    ExprMatrix elements = *new ExprMatrix();
    Expression divide(ExpressionObject* other);
    Expression add(ExpressionObject* other);
    Expression negate();
    Expression subtract(ExpressionObject* other);
    Expression multiply(ExpressionObject* other);
public:
    std::string print();
    Matrix(const Matrix& target);
    Matrix(std::string name, ExprMatrix newElements);
    Matrix(std::string name, std::initializer_list<std::initializer_list<Expression>> newElements);
    Matrix(std::string name, std::vector<int> newDimensions = {0,0});//empty matrix
    Matrix(ExpressionObject* diag, int newDim = 0);//Identity matrix times const expression
    ~Matrix();
    ExprMatrix getElements();
    friend Expression matMul(ExpressionObject* left, ExpressionObject* right);
    friend ExprVector getConstituentSymbols(ExpressionObject* target);
    friend Expression determinant(ExpressionObject* target);
    friend Expression transpose(ExpressionObject* target);
    friend Expression substitute(ExpressionObject* source, ExpressionObject* target, ExpressionObject* value);
    friend bool areEqual(ExpressionObject& left, ExpressionObject& right);
};

static const size_t SYMBOLTYPE = typeid(Symbol).hash_code();
static const size_t MATRIXTYPE = typeid(Matrix).hash_code();
static const size_t IMAGINARYUNITTYPE = typeid(ImaginaryUnit).hash_code();
//static SymbolicObject* IUNIT = *new ImaginaryUnit();
//static Expression iunit = *new Expression(&IUNIT);
static Expression IMAGUNIT = *new Expression(new ImaginaryUnit());
#include "Real.hpp"
//static SymbolicObject* IDENTITYMAT = *new Matrix("I",{{one,zero},{zero,one}});
//static Expression identitymat = *new Expression(&IDENTITYMAT);
static Expression IDENMAT = *new Expression(new Matrix("I",{{ONE,ZERO},{ZERO,ONE}}));
#endif /* Symbol_hpp */

