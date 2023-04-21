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

PauliMatrix::PauliMatrix(String name, String flavor, std::initializer_list<std::initializer_list<Expression>> newElements ) : Matrix(name,newElements) {
    (*this).flavor = flavor;
}



PauliMatrix::PauliMatrix(String name, String flavor, ExprMatrix elements) : Matrix(name,elements) {
    (*this).flavor = flavor;
}

PauliMatrix::PauliMatrix(const PauliMatrix& target) : Matrix(target.name) {
    (*this).elements = *new ExprMatrix(target.elements);
    (*this).dimensions = *new std::pair<int,int>(target.dimensions);
    (*this).flavor = target.flavor;
};

PauliMatrix& PauliMatrix::operator=(const PauliMatrix& target) {
    (*this).elements = *new ExprMatrix(target.elements);
    (*this).dimensions = *new std::pair<int,int>(target.dimensions);
    (*this).flavor = target.flavor;
    return *this;
};

PauliMatrix::PauliMatrix(int index, String flavor) :  Matrix(flavor+std::to_string(index)) {
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
        Expression resultingPauli = *new Expression(new PauliMatrix(flavor+resultingMatrix.print(),thisPauli.flavor,resultingMatrix.getElements()));
        return resultingPauli;
    }
    Expression pauliTarget = getElementOfType(other, PAULIMATRIXTYPE);
    //bool wasAVector = false;
    if(pauliTarget.getTypeHash() == NULLTYPE) {
        if(other.getTypeHash() == ADDTYPE) {
            const Add& otherAdd = dynamic_cast<const Add&>(*other);
            ExprVector newMembers = *new ExprVector();
            newMembers.push_back(thisExpr);
            newMembers = setUnion(newMembers, otherAdd.getMembers());
            return *new Expression(new Add(newMembers));
        }
        return *new Expression(new Add(thisExpr,other));
        //Expression result = combineSums(thisExpr, other);
        //return result;
    }
    const PauliMatrix& targetMatrix = dynamic_cast<const PauliMatrix&>(*pauliTarget);
    if(targetMatrix.flavor != thisPauli.flavor || rtype != MULTYPE) {
        if(other.getTypeHash() == ADDTYPE) {
            const Add& otherAdd = dynamic_cast<const Add&>(*other);
            ExprVector newMembers = *new ExprVector();
            newMembers.push_back(thisExpr);
            newMembers = setUnion(newMembers, otherAdd.getMembers());
            return *new Expression(new Add(newMembers));
        }
        return *new Expression(new Add(thisExpr,other));
        //Expression result = combineSums(thisExpr, other);
        //return result;
    }
    Expression otherCoefficent = cancelFactor(other, pauliTarget);//other->remove(pauliTarget);
    ExprMatrix distrElements = *new ExprMatrix();
    for(size_t i = 0; i<targetMatrix.dimensions.first; i++) {
        ExprVector currentColumn = *new ExprVector();
        for(size_t j =0; j<targetMatrix.dimensions.second; j++) {
            Expression distrResult = otherCoefficent*targetMatrix.elements[i][j];
            currentColumn.push_back(distrResult);
            //distrElements[i].push_back(distrResult);
        }
        distrElements.push_back(currentColumn);
    }
    Expression distrMat = *new Expression(new PauliMatrix("temp",targetMatrix.flavor,distrElements));
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
        Expression resultingPauli = *new Expression(new PauliMatrix(flavor+resultingMatrix.print(),thisPauli.flavor,resultingMatrix.getElements()));
        return resultingPauli;
    }
    
    Expression result = distribute(other);
    return result;
};

Expression PauliMatrix::simplify() const {
    return *new Expression(this);
    /*ExprMatrix newElements = *new ExprMatrix(elements);
    for(size_t i = 0; i<dimensions.first; i++) {
        for(size_t j =0; j<dimensions.second; j++) {
            newElements[i][j] = newElements[i][j].simplify();
        }
    }
    return *new Expression(new PauliMatrix(name+"Simplified",index,flavor,newElements));*/
};
Expression PauliMatrix::distribute(Expression other) const {
    size_t otherType = other.getTypeHash();
    Expression thisExpr = *new Expression(this);
    if(otherType == ZEROTYPE) {
        return ZERO;
    }
    if(otherType == ONETYPE) {
        return thisExpr;
    }
    if(otherType == ADDTYPE) {
        const Add& otherAdd = dynamic_cast<const Add&>(*other);
        ExprVector newMembers = otherAdd.getMembers();
        for(size_t i = 0; i<newMembers.size(); i++) {
            newMembers[i] = distribute(newMembers[i]);
        }
        if(newMembers.size() == 0)
            return newMembers[0];
        return *new Expression(new Add(newMembers));
    }
    if(otherType == MULTYPE) {
        Expression testTarget = getMatrixMatchingPauliFlavor(other, thisExpr);
        
        //This below is fucked.
        if(testTarget.getTypeHash() != NULLTYPE) {
            Expression product = thisExpr*testTarget;
            return product*cancelFactor(other, testTarget);
        }
        const Mul& otherMul = dynamic_cast<const Mul&>(*other);
        ExprVector newMembers = *new ExprVector();
        ExprVector otherMembers = otherMul.getMembers();
        newMembers.push_back(thisExpr);
        for(size_t i = 0; i<otherMembers.size(); i++) {
            newMembers.push_back(otherMembers[i]);
        }
        return *new Expression(new Mul(newMembers));
    }
    if(otherType == FRACTYPE) {
        const Frac& otherFrac = dynamic_cast<const Frac&>(*other);
        return *new Expression(new Frac(distribute(otherFrac.getNumerator()),otherFrac.getDenomenator()));
    }
    return *new Expression(new Mul(thisExpr,other));
    
};
Expression PauliMatrix::transpose() const {
    ExprMatrix transElements = *new ExprMatrix();
    for(size_t i = 0; i<dimensions.second; i++) {
        ExprVector newColumn = *new ExprVector();
        for(size_t j =0; j<dimensions.first; j++) {
           
            newColumn.push_back(elements[j][i]);
        }
        transElements.push_back(newColumn);
    }
    return *new Expression(new PauliMatrix(name+"T",flavor,transElements));
};

