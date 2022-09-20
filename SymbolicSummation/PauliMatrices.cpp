//
//  PauliMatrices.cpp
//  SymbolicSummation
//
//  Created by Laina Stahulak on 4/8/22.
//

#include "PauliMatrices.hpp"
#include "Real.hpp"
#include "Container.hpp"
#include "AlgebraicHelpers.hpp"
#include "AbstractHelpers.hpp"

PauliMatrix::~PauliMatrix() {
    delete &elements;
    delete &name;
    delete &flavor;
    delete &dimensions;
}

PauliMatrix::PauliMatrix(std::string name, int index, std::string flavor, std::initializer_list<std::initializer_list<Expression>> newElements ) : Matrix(name+flavor+std::to_string(index),newElements) {
    (*this).index = index;
    (*this).flavor = flavor;
}



PauliMatrix::PauliMatrix(std::string name, int index, std::string flavor, ExprMatrix elements) : Matrix(name+flavor+std::to_string(index),elements) {
    (*this).flavor = flavor;
    (*this).index = index;
}

PauliMatrix::PauliMatrix(const PauliMatrix& target) : Matrix(target.name) {
    (*this).elements = *new ExprMatrix(target.elements);
    (*this).dimensions = target.dimensions;
    (*this).flavor = target.flavor;
    (*this).index = target.index;
};

PauliMatrix::PauliMatrix(int index, std::string flavor) :  Matrix(flavor+std::to_string(index)) {
    (*this).index = index;
    (*this).flavor = flavor;
    dimensions = {2,2};
    switch(index) {
        case 1:
            (*this).elements = generateExprMatrix({{ZERO,ONE},{ONE,ZERO}});
            break;
        case 2:
            (*this).elements = generateExprMatrix({{ZERO,-IMAGUNIT},{IMAGUNIT,ZERO}});
            break;
        case 3:
            (*this).elements = generateExprMatrix({{ONE,ZERO},{ZERO,-ONE}});
            break;
        case 0:
            break;
        default:
            throw std::invalid_argument("pauli index must be 1-3");
    }
}

Expression PauliMatrix::add(ExpressionObject* other) {
    size_t rtype = other->getTypeHash();
    PauliMatrix& thisPauli = dynamic_cast<PauliMatrix&>(*this);
    if(rtype == PAULIMATRIXTYPE) {
        PauliMatrix& otherPauli = dynamic_cast<PauliMatrix&>(*other);
        if(thisPauli.flavor != otherPauli.flavor) {
            Expression result = *new Expression(new Add(this,other));
            return result;
        }
        PauliMatrix& leftPauli = dynamic_cast<PauliMatrix&>(*this);
        PauliMatrix& rightPauli = dynamic_cast<PauliMatrix&>(*other);
        Matrix leftAsMat = leftPauli;
        Matrix rightAsMat = rightPauli;
        Expression leftAsMatExpr = *new Expression(&leftAsMat);
        Expression rightAsMatExpr = *new Expression(&rightAsMat);
        Expression tempResult = leftAsMatExpr+rightAsMatExpr;
        if(tempResult.getTypeHash() != MATRIXTYPE)
            return tempResult;
        Matrix& resultingMatrix = dynamic_cast<Matrix&>(*tempResult);
        Expression resultingPauli = *new Expression(new PauliMatrix(flavor+resultingMatrix.print(),0,thisPauli.flavor,resultingMatrix.getElements()));
        return resultingPauli;
    }
    Expression pauliTarget = getElementOfType(other, PAULIMATRIXTYPE);
    //bool wasAVector = false;
    if(pauliTarget.getTypeHash() == NULLTYPE) {
        Expression result = combineSums(other, this);
        return result;
    }
    PauliMatrix& targetMatrix = dynamic_cast<PauliMatrix&>(*pauliTarget);
    if(targetMatrix.flavor != thisPauli.flavor) {
        Expression result = combineSums(this, other);
        return result;
    }
    Expression otherCoefficent = removeElementMultiplicatively(other, pauliTarget.get());//other->remove(pauliTarget);
    ExprMatrix distrElements = *new ExprMatrix();
    for(int i = 0; i<targetMatrix.dimensions.first; i++) {
        ExprVector currentColumn = *new ExprVector();
        for(int j = 0; j<targetMatrix.dimensions.second; j++) {
            Expression distrResult = otherCoefficent*targetMatrix.elements[i][j];
            currentColumn.push_back(distrResult);
            //distrElements[i].push_back(distrResult);
        }
        distrElements.push_back(currentColumn);
    }
    Expression distrMat = *new Expression(new Matrix("temp matrix",distrElements));
    Matrix& leftMat = dynamic_cast<Matrix&>(*this);
    Expression leftAsMatExpr = *new Expression(&leftMat);
    Matrix& resultingMatrix = dynamic_cast<Matrix&>(*(leftAsMatExpr+distrMat));
    Expression resultingPauli = *new Expression(new PauliMatrix(flavor+resultingMatrix.print(),0,thisPauli.flavor,resultingMatrix.getElements()));
    Expression result;
    result = replaceElementOfType(other, PAULIMATRIXTYPE, resultingPauli.get());
    return result;
};
Expression PauliMatrix::subtract(ExpressionObject* other) {
    Expression negativeOf = -*other;
    return this->add(negativeOf.get());
};
Expression PauliMatrix::negate() {
    return *new Expression(new Sign(this));
}
Expression PauliMatrix::multiply(ExpressionObject* other) {
    size_t rtype = other->getTypeHash();
    PauliMatrix& thisPauli = dynamic_cast<PauliMatrix&>(*this);
    if(rtype == PAULIMATRIXTYPE) {
        PauliMatrix& otherPauli = dynamic_cast<PauliMatrix&>(*other);
        if(thisPauli.flavor != otherPauli.flavor) {
            Expression result = *new Expression(new Mul(this,other));
            return result;
        }
        
        Matrix& leftAsMat = dynamic_cast<Matrix&>(*this);
        Matrix& rightAsMat = dynamic_cast<Matrix&>(*other);
        Expression tempResult = matMul(&leftAsMat, &rightAsMat);//leftAsMat*rightAsMat;
        if(tempResult.getTypeHash() != MATRIXTYPE)
            return tempResult;
        Matrix& resultingMatrix = dynamic_cast<Matrix&>(*tempResult);
        Expression resultingPauli = *new Expression(new PauliMatrix(flavor+resultingMatrix.print(),0,thisPauli.flavor,resultingMatrix.getElements()));
        return resultingPauli;
    }
    
    Expression result = distribute(this, other);
    return result;
};
