//
//  Symbol.cpp
//  SymbolicSummation
//
//  Created by Laina Stahulak on 2/3/22.
//

#include "Symbol.hpp"
#include "Real.hpp"
#include "Container.hpp"
#include "AlgebraicHelpers.hpp"
#include "AbstractHelpers.hpp"
Symbol::~Symbol() {
    //delete &name;
}

Symbol::Symbol(const Symbol& target) {
    name = target.name;
}

Symbol& Symbol::operator=(const Symbol &target) {
    name = target.name;
    return *this;
}
Expression Symbol::divide(Expression other) const {
    Expression thisExpr = *new Expression(this);
    if(other == thisExpr) {
        return ONE;
    }
    
    Expression reciprocalOf = reciprocal(other);
    return distribute(thisExpr, reciprocalOf);
};
Expression Symbol::add(Expression other) const {
    Expression thisExpr = *new Expression(this);
    if(other->getTypeHash() == ZEROTYPE)
        return *new Expression(this);
    if(other == thisExpr)
        return 2*thisExpr;
    return combineSums(thisExpr, other);
};
Expression Symbol::negate() const {
    Expression thisExpr = *new Expression(this);
    return *new Expression(new Sign(thisExpr));
};
Expression Symbol::subtract(Expression other) const {
    Expression thisExpr = *new Expression(this);
    if(other == thisExpr) {
        return ZERO;
    }
    
    Expression negativeOf = -other;
    return combineSums(thisExpr, negativeOf);
};
Expression Symbol::multiply(Expression other) const {
    Expression thisSymbol = *new Expression(this);
    if(other->getTypeHash() == ZEROTYPE)
        return ZERO;
    if(other->getTypeHash() == ONETYPE)
        return *new Expression(this);
    if(other->getTypeHash() == SYMBOLTYPE) {
        if(thisSymbol == other) {
            return *new Expression(new Exp(thisSymbol,2));
        }
    }
    return distribute(thisSymbol, other);
};

//ImaginaryUnit
ImaginaryUnit::ImaginaryUnit(const ImaginaryUnit& target) : Symbol("i") {
};

ImaginaryUnit& ImaginaryUnit::operator=(const ImaginaryUnit &target) {
    return *this;
}

Expression ImaginaryUnit::multiply(Expression other) const {
    Expression thisExpr = *new Expression(this);
    if(other == thisExpr) {;
        return MINUSONE;
    }
    return distribute(thisExpr, other);
};

Expression ImaginaryUnit::negate() const {
    return *new Expression(new Sign(IMAGUNIT));
}


//Matrix

Matrix::~Matrix() {
    //delete &elements;
    //delete &name;
    //delete &dimensions;
}

Matrix::Matrix(const Matrix& target) : Symbol(target.name) {
    elements = *new ExprMatrix(target.elements);
    dimensions = *new std::pair<int,int>(target.dimensions);
};

Matrix& Matrix::operator=(const Matrix &target) {
    elements = *new ExprMatrix(target.elements);
    dimensions = *new std::pair<int,int>(target.dimensions);
    return *this;
}

Matrix::Matrix(String name, ExprMatrix newElements) : Symbol(name) {
    elements = *new ExprMatrix(newElements);
    //dimensions = *new std::vector<int>();
    dimensions.first = (int)elements.size();
    dimensions.second = (int)elements[0].size();
};

Matrix::Matrix(String name, std::initializer_list<std::initializer_list<Expression>> newElements) : Symbol(name) {
    elements = *new ExprMatrix();
    for(const std::initializer_list<Expression>& x : newElements) {
        ExprVector nextColumn = x;
        elements.push_back(nextColumn);
    }
    dimensions.first = (int)elements.size();
    dimensions.second = (int)elements[0].size();
}

Matrix::Matrix(String name, std::vector<int> newDimensions) : Symbol(name) {
    
};//empty matrix
Matrix::Matrix(Expression diag, int newDim)  : Symbol("I*"+diag->print()) {
    ExprMatrix newElements = *new ExprMatrix();
    Expression diagElement = *new Expression(diag);
    for(int i = 0; i< newDim; i++) {
    ExprVector currColumn = *new ExprVector();
        for(int j = 0; j< newDim; j++) {
            if(i == j)
                currColumn.push_back(diagElement);
            else
                currColumn.push_back(ZERO);
        }
    }
};//Identity matrix times const expression

Expression Matrix::divide(Expression other) const {
    Expression thisExpr = *new Expression(this);
    Expression result = *new Expression(new Frac(thisExpr,other));
    return result;
};
Expression Matrix::add(Expression other) const {
    Expression thisExpr = *new Expression(this);
    if(other->getTypeHash() == ADDTYPE) {
        return combineSums(thisExpr, other);
    }
    if(other->getTypeHash() == MATRIXTYPE) {
        const Matrix& otherMat = dynamic_cast<const Matrix&>(*other);
        bool zeroMat = true;
        if(dimensions != otherMat.dimensions) {
            Expression result = *new Expression(new NullObject("mismatch between dimensions for adding MATRIXTYPE"));
            return result;
        }
        ExprMatrix newElements = *new ExprMatrix();
        for(int i = 0; i<dimensions.first; i++) {
            ExprVector column = *new ExprVector();
            for(int j = 0; j<dimensions.second; j++) {
                Expression currentElement = (elements[i][j]+otherMat.elements[i][j]);
                zeroMat &= currentElement == ZERO;
                column.push_back(currentElement);
            }
            newElements.push_back(column);
        }
        if(zeroMat)
            return ZERO;
        String newName = this->print()+"+"+other->print();
        Expression result = *new Expression(new Matrix(newName,newElements));
        return result;
    }
    Expression matTarget = getElementOfType(other,MATRIXTYPE);
    if(matTarget.getTypeHash() == NULLTYPE)
        return matTarget;
    const Matrix& otherMat = dynamic_cast<const Matrix&>(*matTarget);
    Expression otherCoefficent = removeElementMultiplicatively(other, matTarget);//other->remove(matTarget);
    ExprMatrix distrElements = *new ExprMatrix();
    for(int i = 0; i<otherMat.dimensions.first; i++) {
        ExprVector currentColumn = *new ExprVector();
        for(int j = 0; j<otherMat.dimensions.second; j++) {
            currentColumn.push_back(otherCoefficent*otherMat.elements[i][j]);
        }
        distrElements.push_back(currentColumn);
    }
    Expression distrMat = new Matrix("temp matrix",distrElements);
    Expression finalResult = *this+*distrMat;
    return finalResult;
};
Expression Matrix::negate() const {
    ExprMatrix newElements = *new ExprMatrix();
    for(int i = 0; i<dimensions.first; i++) {
        ExprVector newColumn = *new ExprVector();
        for(int j = 0; j< dimensions.second; j++) {
            Expression negateElement = -elements[i][j];
            newColumn.push_back(negateElement);
        }
        newElements.push_back(newColumn);
    }
    Expression negativeMatrix = *new Expression(new Matrix("negative "+name,newElements));
    return negativeMatrix;
};
Expression Matrix::subtract(Expression other) const {
    Expression thisExpr = *new Expression(this);
    Expression negativeOf = -other;
    return thisExpr+negativeOf;
};
Expression Matrix::multiply(Expression other) const {
    Expression thisExpr = *new Expression(this);
    if(other->getTypeHash() == MATRIXTYPE) {
        Expression result = matMul(*new Expression(this), other);
        return result;
    }
    Expression matTarget = getElementOfType(other,MATRIXTYPE);
    
    if(matTarget.getTypeHash() == NULLTYPE)
        return distribute(thisExpr, other);
    Expression result = matMul(thisExpr, matTarget);
    Expression finalResult = replaceElementOfType(other,MATRIXTYPE,result);
    return finalResult;
};

ExprMatrix Matrix::getElements() const {
    ExprMatrix result = *new ExprMatrix(elements);
    return result;
}

String Matrix::print() const {
    String result = "";
    result += "{";
    for(int i = 0; i<dimensions.first; i++)
    {
        result += "{";
        for(int j = 0; j<dimensions.second; j++) {
            result += elements[i][j].print();
            if(j != dimensions.second-1)
                result += ",";
        }
        result += "}";
        if(i != dimensions.first-1)
            result += ",";
    }
    result += "}";
    return result;
}

Expression EuclidVector::add(Expression other) const {
    Expression temp = Matrix::add(other);
    if(isSubtypeOf(temp, MATRIXTYPE)) {
        const Matrix& tempMat = dynamic_cast<const Matrix&>(*temp);
        Expression result = *new Expression(new EuclidVector(tempMat.print(),tempMat.getElements()[0]));
        return result;
    }
    return temp;
};
Expression EuclidVector::multiply(Expression other) const {
    if(other.getTypeHash() == EUCLIDVECTORTYPE) {
        Expression thisExpr = *new Expression(this);
        return matMul(thisExpr, transpose(other));
    }
    return Matrix::multiply(other);
};
String EuclidVector::print() const {
    String result = "{";
    for(int i = 0; i<elements[0].size(); i++) {
        result += elements[0][i].print();
    }
    result += "}";
    return result;
};
EuclidVector::EuclidVector(const EuclidVector& target) : Matrix(target) {
    
};
EuclidVector& EuclidVector::operator=(const EuclidVector& target) {
    elements = *new ExprMatrix(target.elements);
    dimensions = *new std::pair<int,int>(target.dimensions);
    return *this;
};
EuclidVector::EuclidVector(String name, ExprVector newElements) : Matrix(name,{newElements}) {};
EuclidVector::EuclidVector(String name, std::initializer_list<Expression> newElements) : Matrix(name,{newElements}) {};
EuclidVector::EuclidVector(String name, int newDimension) : Matrix(name, {1,newDimension}) {};//empty matrix
EuclidVector::~EuclidVector() {
    
};
