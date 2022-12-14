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
    //delete &elements;
    //delete &name;
    //delete &flavor;
    //delete &dimensions;
}

PauliMatrix::PauliMatrix(String name, int index, String flavor, std::initializer_list<std::initializer_list<Expression>> newElements ) : Matrix(name,newElements) {
    (*this).index = index;
    (*this).flavor = flavor;
}



PauliMatrix::PauliMatrix(String name, int index, String flavor, ExprMatrix elements) : Matrix(name,elements) {
    (*this).flavor = flavor;
    (*this).index = index;
}

PauliMatrix::PauliMatrix(const PauliMatrix& target) : Matrix(target.name) {
    (*this).elements = *new ExprMatrix(target.elements);
    (*this).dimensions = *new std::pair<int,int>(target.dimensions);
    (*this).flavor = target.flavor;
    (*this).index = target.index;
};

PauliMatrix& PauliMatrix::operator=(const PauliMatrix& target) {
    (*this).elements = *new ExprMatrix(target.elements);
    (*this).dimensions = *new std::pair<int,int>(target.dimensions);
    (*this).flavor = target.flavor;
    (*this).index = target.index;
    return *this;
};

PauliMatrix::PauliMatrix(int index, String flavor) :  Matrix(flavor+std::to_string(index)) {
    (*this).index = index;
    (*this).flavor = flavor;
    dimensions = {2,2};
    Expression minusImag = -IMAGUNIT;
    switch(index) {
        case 1:
            (*this).elements = generateExprMatrix({{ZERO,ONE},{ONE,ZERO}});
            break;
        case 2:
            (*this).elements = generateExprMatrix({{ZERO,minusImag},{IMAGUNIT,ZERO}});
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

Expression PauliMatrix::add(Expression other) const {
    Expression thisExpr = *new Expression(this);
    size_t rtype = other->getTypeHash();
    const PauliMatrix& thisPauli = dynamic_cast<const PauliMatrix&>(*this);
    if(rtype == PAULIMATRIXTYPE) {
        const PauliMatrix& otherPauli = dynamic_cast<const PauliMatrix&>(*other);
        if(thisPauli.flavor != otherPauli.flavor) {
            Expression result = *new Expression(new Add(thisExpr,other));
            return result;
        }
        const PauliMatrix& leftPauli = dynamic_cast<const PauliMatrix&>(*this);
        const PauliMatrix& rightPauli = dynamic_cast<const PauliMatrix&>(*other);
        Matrix leftAsMat = leftPauli;
        Matrix rightAsMat = rightPauli;
        Expression leftAsMatExpr = *new Expression(&leftAsMat);
        Expression rightAsMatExpr = *new Expression(&rightAsMat);
        Expression tempResult = leftAsMatExpr+rightAsMatExpr;
        if(tempResult.getTypeHash() != MATRIXTYPE)
            return tempResult;
        const Matrix& resultingMatrix = dynamic_cast<const Matrix&>(*tempResult);
        Expression resultingPauli = *new Expression(new PauliMatrix(flavor+resultingMatrix.print(),0,thisPauli.flavor,resultingMatrix.getElements()));
        return resultingPauli;
    }
    Expression pauliTarget = getElementOfType(other, PAULIMATRIXTYPE);
    //bool wasAVector = false;
    if(pauliTarget.getTypeHash() == NULLTYPE) {
        Expression result = combineSums(thisExpr, other);
        return result;
    }
    const PauliMatrix& targetMatrix = dynamic_cast<const PauliMatrix&>(*pauliTarget);
    if(targetMatrix.flavor != thisPauli.flavor || rtype != MULTYPE) {
        Expression result = combineSums(thisExpr, other);
        return result;
    }
    Expression otherCoefficent = removeElementMultiplicatively(other, pauliTarget);//other->remove(pauliTarget);
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
    Expression distrMat = *new Expression(new PauliMatrix("temp",0,targetMatrix.flavor,distrElements));
    Expression resultingMatrix = thisExpr+distrMat;
    return resultingMatrix;
};
Expression PauliMatrix::subtract(Expression other) const {
    Expression thisExpr =  *new Expression(this);
    Expression negativeOf = -other;
    return thisExpr+negativeOf;
};
Expression PauliMatrix::negate() const {
    return *new Expression(new Sign(*new Expression(this)));
}
Expression PauliMatrix::multiply(Expression other) const {
    size_t rtype = other->getTypeHash();
    const PauliMatrix& thisPauli = dynamic_cast<const PauliMatrix&>(*this);
    if(rtype == PAULIMATRIXTYPE) {
        const PauliMatrix& otherPauli = dynamic_cast<const PauliMatrix&>(*other);
        if(thisPauli.flavor != otherPauli.flavor) {
            Expression result = *new Expression(new Mul(*new Expression(this),other));
            return result;
        }
        
        Expression tempResult = matMul(*new Expression(this), other);//leftAsMat*rightAsMat;
        if(tempResult.getTypeHash() != MATRIXTYPE)
            return tempResult;
        const Matrix& resultingMatrix = dynamic_cast<const Matrix&>(*tempResult);
        Expression resultingPauli = *new Expression(new PauliMatrix(flavor+resultingMatrix.print(),0,thisPauli.flavor,resultingMatrix.getElements()));
        return resultingPauli;
    }
    
    Expression result = distribute(other);
    return result;
};

Expression EuclidVector::simplify() const {
};
Expression EuclidVector::distribute(Expression other) const {
};
Expression EuclidVector::factor() const {
};
Expression EuclidVector::reciprocal() const {
};
Expression EuclidVector::determinant() const {
};
Expression EuclidVector::transpose() const {
};
Expression EuclidVector::cancelTerms() const {
};
ExprVector EuclidVector::getFactors() const {
};
ExprVector EuclidVector::getCommonFactors(ExprVector terms) const {
};
