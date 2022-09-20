//
//  Symbol.cpp
//  SymbolicSummation
//
//  Created by Robert Stahulak on 2/3/22.
//

#include "Symbol.hpp"
#include "Real.hpp"
#include "Container.hpp"
#include "ExpressionHelpers.hpp"
#include "AbstractHelpers.hpp"

Symbol::Symbol(const Symbol& target) {
    name = target.name;
}
Expression Symbol::divide(ExpressionObject* other) {
    if(other == this) {
        return ONE;
    }
    
    Expression reciprocalOf = reciprocal(other);
    return distribute(this, reciprocalOf.get());
};
Expression Symbol::add(ExpressionObject* other) {
    if(other->getTypeHash() == ZEROTYPE)
        return *new Expression(this);
    if(other == this)
        return 2**this;
    return combineSums(this, other);
};
Expression Symbol::negate() {
    Expression result = *new Expression(new Sign(this));
    return result;
};
Expression Symbol::subtract(ExpressionObject* other) {
    if(other == this) {
        //Expression result(&ZERO);
        return ZERO;
    }
    
    Expression negativeOf = -*other;
    return combineSums(this, negativeOf.get());
};
Expression Symbol::multiply(ExpressionObject* other) {
    if(other->getTypeHash() == ZEROTYPE)
        return ZERO;
    if(other->getTypeHash() == ONETYPE)
        return *new Expression(this);
    if(other->getTypeHash() == SYMBOLTYPE && this == other) {
        return *new Expression(new Exp(this,2));
    }
    return distribute(this, other);
};
/*
bool Symbol::containsPerform(SymbolicObject* other) {
    return *other == *this;
};
bool Symbol::containsTypeOfPerform(size_t type) {
    return type == SYMBOLTYPE;
};
Expression Symbol::firstInstanceOfTypePerform(, size_t type, bool rightToLeft) {
    if(type == this->getTypeHash())
        return this;
    //Expression result(nullptr);
    return *new Expression(new NullObject("SYMBOLTYPE has no members"));
};
Expression Symbol::removePerform(SymbolicObject* other, bool rightToLeft) {
    if(other == this) {
        return ZERO;
    }
    return this;
};
*/
//ImaginaryUnit
ImaginaryUnit::ImaginaryUnit(const ImaginaryUnit& target) : Symbol(target.name) {
};
/*
Expression ImaginaryUnit::dividePerform(SymbolicObject* other) {
    
};
Expression ImaginaryUnit::addPerform(SymbolicObject* other) {
    
};
Expression ImaginaryUnit::subtractPerform(SymbolicObject* other) {
    
};
 */
Expression ImaginaryUnit::multiply(ExpressionObject* other) {
    if(*other == *this) {;
        return MINUSONE;
    }
    return distribute(this, other);
};


//Matrix

Matrix::~Matrix() {
    delete &elements;
    delete &name;
    delete &dimensions;
}

Matrix::Matrix(const Matrix& target) : Symbol(target.name) {
    elements = *new ExprMatrix(target.elements);
    dimensions = target.dimensions;
};
Matrix::Matrix(std::string name, ExprMatrix newElements) : Symbol(name) {
    elements = *new ExprMatrix(newElements);
    //dimensions = *new std::vector<int>();
    dimensions.first = (int)elements.size();
    dimensions.second = (int)elements[0].size();
};

Matrix::Matrix(std::string name, std::initializer_list<std::initializer_list<Expression>> newElements) : Symbol(name) {
    elements = *new ExprMatrix();
    for(const std::initializer_list<Expression>& x : newElements) {
        ExprVector nextColumn = x;
        elements.push_back(nextColumn);
    }
    dimensions.first = (int)elements.size();
    dimensions.second = (int)elements[0].size();
}

Matrix::Matrix(std::string name, std::vector<int> newDimensions) : Symbol(name) {
    
};//empty matrix
Matrix::Matrix(ExpressionObject* diag, int newDim)  : Symbol("I*"+diag->print()) {
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

Expression Matrix::divide(ExpressionObject* other) {
    Expression result = *new Expression(new Frac(this,other));
    return result;
};
Expression Matrix::add(ExpressionObject* other) {
    if(other->getTypeHash() == ADDTYPE) {
        return combineSums(this, other);
    }
    if(other->getTypeHash() == MATRIXTYPE) {
        Matrix& otherMat = dynamic_cast<Matrix&>(*other);
        if(dimensions != otherMat.dimensions) {
            Expression result = *new Expression(new NullObject("mismatch between dimensions for adding MATRIXTYPE"));
            return result;
        }
        ExprMatrix newElements = *new ExprMatrix();
        for(int i = 0; i<dimensions.first; i++) {
            ExprVector column = *new ExprVector();
            for(int j = 0; j<dimensions.second; j++) {
                Expression currentElement = (elements[i][j]+otherMat.elements[i][j]);
                column.push_back(currentElement);
            }
            newElements.push_back(column);
        }
        
        std::string newName = this->print()+"+"+other->print();
        Expression result = *new Expression(new Matrix(newName,newElements));
        return result;
    }
    //Expression result = combineTermsDifferingByCoefficientsAdditively(this, other);
    Expression matTarget = getElementOfType(other,MATRIXTYPE);
    if(matTarget.getTypeHash() == NULLTYPE)
        return matTarget;
    Matrix& otherMat = dynamic_cast<Matrix&>(*matTarget);
    Expression otherCoefficent = removeElementMultiplicatively(other, matTarget.get());//other->remove(matTarget);
    ExprMatrix distrElements = *new ExprMatrix();
    for(int i = 0; i<otherMat.dimensions.first; i++) {
        ExprVector currentColumn = *new ExprVector();
        for(int j = 0; j<otherMat.dimensions.second; j++) {
            currentColumn.push_back(otherCoefficent*otherMat.elements[i][j]);
        }
        distrElements.push_back(currentColumn);
    }
    ExpressionObject* distrMat = new Matrix("temp matrix",distrElements);
    Expression finalResult = *this+*distrMat;
    return finalResult;
};
Expression Matrix::negate() {
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
    //Expression result = *new Expression(new Add(this,true));
    return negativeMatrix;
};
Expression Matrix::subtract(ExpressionObject* other) {
    Expression negativeOf = *new Expression(-*other);
    return this->add(negativeOf.get());
};
Expression Matrix::multiply(ExpressionObject* other) {
    if(other->getTypeHash() == MATRIXTYPE) {
        Expression result = matMul(this, other);
        return result;
    }
    Expression matTarget = getElementOfType(other,MATRIXTYPE);
    
    if(matTarget.getTypeHash() == NULLTYPE)
        return distribute(this, other);
    Expression result = matMul(this, matTarget.get());
    Expression finalResult = replaceElementOfType(other,MATRIXTYPE,result.get());
    return finalResult;
};

ExprMatrix Matrix::getElements() {
    ExprMatrix result = elements;
    return result;
}

std::string Matrix::print() {
    std::string result = "";
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
