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
    if(exprVectorContains(other.getFactors(), thisExpr)) {
        return *new Expression(new Fraction(cancelFactor(other, thisExpr)));
    }
    return *new Expression(new Fraction(thisExpr,other));
};
Expression Symbol::add(Expression other) const {
    Expression thisExpr = *new Expression(this);
    if(other.getTypeHash() == ZEROTYPE)
        return *new Expression(this);
    if(other == thisExpr)
        return 2*thisExpr;
    if(other.getTypeHash() == SUMTYPE) {
        const Sum& otherAdd = dynamic_cast<const Sum&>(*other);
        ExprVector newMembers = *new ExprVector();
        newMembers.push_back(thisExpr);
        newMembers = setUnion(newMembers, otherAdd.getMembers());
        return *new Expression(new Sum(newMembers));
    }
    return *new Expression(new Sum(thisExpr,other));
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
    if(other.getTypeHash() == SUMTYPE) {
        const Sum& otherNegativeAdd = dynamic_cast<const Sum&>(*-other);
        ExprVector newMembers = *new ExprVector();
        newMembers.push_back(thisExpr);
        newMembers = setUnion(newMembers, otherNegativeAdd.getMembers());
        return *new Expression(new Sum(newMembers));
    }
    return *new Expression(new Sum(thisExpr,-other));
};
Expression Symbol::multiply(Expression left, Expression right) const {
    size_t leftType = left.getTypeHash();
    size_t rightType = right.getTypeHash();
    if (leftType == ZEROTYPE || rightType == ZEROTYPE)
        return ZERO;
    if (leftType == ONETYPE)
        return right;
    if (rightType == ONETYPE)
        return left;
    if(leftType == rightType) {
        if (leftType == SYMBOLTYPE) {
            if (left == right) {
                return *new Expression(new Exponent(left, 2));
            }
        }
        if (leftType == IMAGINARYUNITTYPE) {
            return MINUSONE;
        }
        
    }
    return distribute(left, right);
}; 

Expression Symbol::simplify() const {
    return *new Expression(this);
};

/*
Expression Symbol::distribute(Expression other) const {
    size_t otherType = other.getTypeHash();
    Expression thisExpr = *new Expression(this);
    if(otherType == SUMTYPE) {
        const Sum& otherAdd = dynamic_cast<const Sum&>(*other);
        ExprVector newMembers = otherAdd.getMembers();
        for(int i = 0; i<newMembers.size(); i++) {
            newMembers[i] = distribute(newMembers[i]);
        }
        return *new Expression(new Sum(newMembers));
    }
    if(otherType == PRODUCTTYPE) {
        const Product& otherMul = dynamic_cast<const Product&>(*other);
        ExprVector newMembers = *new ExprVector();
        ExprVector otherMembers = otherMul.getMembers();
        newMembers.push_back(thisExpr);
        for(int i = 0; i<otherMembers.size(); i++) {
            newMembers.push_back(otherMembers[i]);
        }
        return *new Expression(new Product(newMembers));
    }
    if(otherType == FRACTIONTYPE) {
        const Fraction& otherFrac = dynamic_cast<const Fraction&>(*other);
        return *new Expression(new Fraction(distribute(otherFrac.getNumerator()),otherFrac.getDenomenator()));
    }
    if(otherType == SYMBOLTYPE) {
        if(other == thisExpr)
            return *new Expression(new Exponent(thisExpr,2));
    }
    return *new Expression(new Product(thisExpr,other));
};*/
Expression Symbol::factor() const {
    return *new Expression(this);
};
Expression Symbol::reciprocal() const {
    return *new Expression(new Fraction(*new Expression(this)));
};
Expression Symbol::determinant() const {
    return *new Expression(this);
};
Expression Symbol::transpose() const {
    return *new Expression(this);
};
ExprVector Symbol::getFactors() const {
    return {*new Expression(this)};
};

//ImaginaryUnit
ImaginaryUnit::ImaginaryUnit(const ImaginaryUnit& target) : Symbol("i") {
};

ImaginaryUnit& ImaginaryUnit::operator=(const ImaginaryUnit &target) {
    return *this;
}

/*
Expression ImaginaryUnit::multiply(Expression left, Expression) const {
    Expression thisExpr = *new Expression(this);
    if(other.getTypeHash() == ZEROTYPE)
        return ZERO;
    if(other.getTypeHash() == ONETYPE)
        return *new Expression(this);
    if(other == thisExpr) {
        return MINUSONE;
    }
    if(other.getTypeHash() == PRODUCTTYPE) {
        const Product& otherMul = dynamic_cast<const Product&>(*other);
        ExprVector newMembers = *new ExprVector();
        newMembers.push_back(thisExpr);
        newMembers = setUnion(newMembers, otherMul.getMembers());
        return *new Expression(new Product(newMembers));
    }
    return *new Expression(new Product(thisExpr,other));
};*/

/*
Expression ImaginaryUnit::negate() const {
    return *new Expression(new Sign(IMAGUNIT));
}*/


//Matrix

Matrix::~Matrix() {
    //delete &elements;
    //delete &name;
    //delete &dimensions;
}

Matrix::Matrix(const Matrix& target) : Symbol(target.name) {
    elements = *new ExprMatrix(target.elements);
    dimensions = target.dimensions;
};

Matrix& Matrix::operator=(const Matrix &target) {
    elements = *new ExprMatrix(target.elements);
    dimensions = target.dimensions;
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
Matrix::Matrix(Expression diag, int newDim)  : Symbol("I*"+diag.print()) {
    ExprMatrix newElements = *new ExprMatrix();
    for(int i = 0; i< newDim; i++) {
    ExprVector currColumn = *new ExprVector();
        for(int j = 0; j< newDim; j++) {
            if(i == j)
                currColumn.push_back(diag);
            else
                currColumn.push_back(ZERO);
        }
    }
};//Identity matrix times const expression

Expression Matrix::divide(Expression other) const {
    Expression thisExpr = *new Expression(this);
    Expression result = *new Expression(new Fraction(thisExpr,other));
    return result;
};
Expression Matrix::add(Expression other) const {
    Expression thisExpr = *new Expression(this);
    if(other.getTypeHash() == SUMTYPE) {
        const Sum& otherAdd = dynamic_cast<const Sum&>(*other);
        ExprVector newMembers = *new ExprVector();
        newMembers.push_back(thisExpr);
        newMembers = setUnion(newMembers, otherAdd.getMembers());
        return *new Expression(new Sum(newMembers));
    }
    if(other.getTypeHash() == MATRIXTYPE) {
        const Matrix& otherMat = dynamic_cast<const Matrix&>(*other);
        bool zeroMat = true;
        if(dimensions != otherMat.dimensions) {
            throw std::runtime_error("mismatch between dimensions for adding MATRIXTYPE");
            Expression result = *new Expression(new NullObject("mismatch between dimensions for adding MATRIXTYPE"));
            return result;
        }
        ExprMatrix newElements = *new ExprMatrix();
        for(int i = 0; i<dimensions.first; i++) {
            ExprVector column = *new ExprVector();
            for(int j =0; j<dimensions.second; j++) {
                Expression currentElement = (elements[i][j]+otherMat.elements[i][j]);
                zeroMat &= currentElement == ZERO;
                column.push_back(currentElement);
            }
            newElements.push_back(column);
        }
        if(zeroMat)
            return ZERO;
        String newName = print()+"+"+other.print();
        Expression result = *new Expression(new Matrix(newName,newElements));
        return result;
    }
    Expression matTarget = getElementOfType(other,MATRIXTYPE);
    if(matTarget.getTypeHash() == NULLTYPE)
        return *new Expression(new Sum(thisExpr,other));
    const Matrix& otherMat = dynamic_cast<const Matrix&>(*matTarget);
    Expression otherCoefficent = cancelFactor(other, matTarget);//other.remove(matTarget);
    ExprMatrix distrElements = *new ExprMatrix();
    for(int i = 0; i<otherMat.dimensions.first; i++) {
        ExprVector currentColumn = *new ExprVector();
        for(int j =0; j<otherMat.dimensions.second; j++) {
            currentColumn.push_back(otherCoefficent*otherMat.elements[i][j]);
        }
        distrElements.push_back(currentColumn);
    }
    Expression distrMat = new Matrix("temp matrix",distrElements);
    return add(distrMat);
};
Expression Matrix::negate() const {
    ExprMatrix newElements = *new ExprMatrix();
    for(int i = 0; i<dimensions.first; i++) {
        ExprVector newColumn = *new ExprVector();
        for(int j =0; j< dimensions.second; j++) {
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
/*
    Matrices should absorb EVERYTHING. if a matrix multiplies anything that isn't a Mul or Frac containing another matrix of the same type (valid for multiplication),
        then the result should just be that matrix with all its elements multiplied by the target
    Do we need some kind of operational priority system so that things like this always behave a certain way, no matter of left or right position?

*/
Expression Matrix::multiply(Expression left, Expression right) const {
    size_t leftType = left.getTypeHash();
    size_t rightType = right.getTypeHash();
    if (leftType == ZEROTYPE || rightType == ZEROTYPE)
        return ZERO;
    if (leftType == ONETYPE)
        return right;
    if (rightType == ONETYPE)
        return left;
    if(leftType == rightType) {
        return matMul(left, right);
    }

    //Matrix
    if (leftType == MATRIXTYPE) {//leftToRight
        ExprVector factors = right.getFactors();
        ExprVector newElements = *new ExprVector();
        bool foundOne = false;
        for (int i = 0; i < factors.size(); i++) {
            if (factors[i].getTypeHash() == MATRIXTYPE && !foundOne) {
                newElements.push_back(matMul(left, factors[i]));
                foundOne = true;
            }
            else {
                newElements.push_back(factors[i]);
            }
        }
        if (!foundOne) {
            const Matrix& leftObj = dynamic_cast<const Matrix&>(*left);
            ExprMatrix newContents = leftObj.getElements();
            for (int i = 0; i < leftObj.getDimensions().first; i++) {
                for (int j = 0; j < leftObj.getDimensions().second; j++) {
                    newContents[i][j] = newContents[i][j] * right;
                }
            }
            return *new Expression(new Matrix(left.print()+"*"+right.print(), newContents));
        }
        return *new Expression(new Product(newElements));
    }
    if (rightType == MATRIXTYPE) {//rightToLeft
        ExprVector factors = left.getFactors();
        ExprVector newElements = *new ExprVector();
        bool foundOne = false;
        for (int i = factors.size()-1; i >= 0; i--) {
            if (factors[i].getTypeHash() == MATRIXTYPE && !foundOne) {
                newElements.push_back(matMul(factors[i],right));
                foundOne = true;
            }
            else {
                newElements.push_back(factors[i]);
            }
        }
        if (!foundOne) {
            const Matrix& rightObj = dynamic_cast<const Matrix&>(*right);
            ExprMatrix newContents = rightObj.getElements();
            for (int i = 0; i < rightObj.getDimensions().first; i++) {
                for (int j = 0; j < rightObj.getDimensions().second; j++) {
                    newContents[i][j] = left * newContents[i][j];
                }
            }
            return *new Expression(new Matrix(left.print() + "*" + right.print(),newContents));
        }
        return *new Expression(new Product(newElements));
    }

    return distribute(left, right);
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

Expression Matrix::simplify() const {
    return *new Expression(this);
};
/*
Expression Matrix::distribute(Expression other) const {
    size_t otherType = other.getTypeHash();
    Expression thisExpr = *new Expression(this);
    if(otherType == SUMTYPE) {
        const Sum& otherAdd = dynamic_cast<const Sum&>(*other);
        ExprVector newMembers = otherAdd.getMembers();
        for(int i = 0; i<newMembers.size(); i++) {
            newMembers[i] = distribute(newMembers[i]);
        }
        return *new Expression(new Sum(newMembers));
    }
    if(otherType == PRODUCTTYPE) {
        Expression testTarget = getElementOfType(other, MATRIXTYPE);
        if(testTarget.getTypeHash() != NULLTYPE) {
            Expression product = thisExpr*testTarget;
            return product*cancelFactor(other, testTarget);
        }
        const Product& otherMul = dynamic_cast<const Product&>(*other);
        ExprVector newMembers = *new ExprVector();
        ExprVector otherMembers = otherMul.getMembers();
        newMembers.push_back(thisExpr);
        for(int i = 0; i<otherMembers.size(); i++) {
            newMembers.push_back(otherMembers[i]);
        }
        return *new Expression(new Product(newMembers));
    }
    if(otherType == FRACTIONTYPE) {
        const Fraction& otherFrac = dynamic_cast<const Fraction&>(*other);
        return *new Expression(new Fraction(distribute(otherFrac.getNumerator()),otherFrac.getDenomenator()));
    }
    return *new Expression(new Product(thisExpr,other));
};*/
Expression Matrix::factor() const {
    return *new Expression(this);
};
Expression Matrix::determinant() const {
    if(dimensions.first != dimensions.second)
        return *new Expression(this);
    int n = dimensions.first;
    Expression det = ZERO;
    if (n == 2)
        return ((elements[0][0] * elements[1][1]) - (elements[1][0] * elements[0][1]));
    else {
        for (int x = 0; x < n; x++) {
            ExprMatrix submatrix = *new ExprMatrix();
            int subi = 0;
            for (int i = 1; i < n; i++) {
                int subj = 0;
                ExprVector currentRow = *new ExprVector();
                for (int j = 0 ; j < n; j++) {
                    if (j == x)
                        continue;
                    currentRow.push_back(elements[i][j]);
                    //submatrix[subi][subj] = currMatrix[i][j];
                    subj++;
                }
                submatrix.push_back(currentRow);
                subi++;
            }
            Expression submatrixExpr = *new Expression(new Matrix("submatrix",*new ExprMatrix(submatrix)));
            det = det + ((float)pow(-1, x) * elements[0][x] * submatrixExpr.determinant());
        }
    }
    return det;
};
Expression Matrix::reciprocal() const {
    return *new Expression(new Fraction(*new Expression(this)));
};
Expression Matrix::transpose() const {
    //if( dimensions.first != dimensions.second )
    //    return *new Expression(new NullObject("Transpose of non-square matrix"));
    ExprMatrix transElements = *new ExprMatrix();
    for(int i = 0; i<dimensions.second; i++) {
        ExprVector newColumn = *new ExprVector();
        for(int j =0; j<dimensions.first; j++) {
           
            newColumn.push_back(elements[j][i]);
        }
        transElements.push_back(newColumn);
    }
    return *new Expression(new Matrix(name+"Transpose",transElements));
};
ExprVector Matrix::getFactors() const {
    return {*new Expression(this)};
};

//EuclidVector
Expression EuclidVector::add(Expression other) const {
    size_t otherType = other.getTypeHash();
    Expression thisExpression = *new Expression(this);
    if (otherType == EUCLIDVECTORTYPE) {

    }
    if (otherType == SUMTYPE) {

    }
    //return *new Expression(new Sum(thisExpression, other));
};
Expression EuclidVector::multiply(Expression left, Expression right) const {
    size_t leftType = left.getTypeHash();
    size_t rightType = right.getTypeHash();
    if (leftType == ZEROTYPE || rightType == ZEROTYPE)
        return ZERO;
    if (leftType == ONETYPE)
        return right;
    if (rightType == ONETYPE)
        return left;
    if (leftType == rightType) {

    }
    if(leftType == PRODUCTTYPE) {
        const Product& otherMul = dynamic_cast<const Product&>(*left);
        ExprVector newMembers = otherMul.getMembers();
        bool foundOne = false;
        for (int i = newMembers.size() - 1; i >= 0; i--) {
            if (newMembers[i].getTypeHash() == EUCLIDVECTORTYPE) {
                Expression newVal = matMul(newMembers[i], right);
                foundOne = true;
            }
        }
        if (!foundOne)
            newMembers.push_back(right);
        return *new Expression(new Product(newMembers));
    }
    if (rightType == PRODUCTTYPE) {
        const Product& otherMul = dynamic_cast<const Product&>(*right);
        ExprVector newMembers = otherMul.getMembers();
        bool foundOne = false;
        for (int i = 0; i < newMembers.size(); i++) {
            if (newMembers[i].getTypeHash() == EUCLIDVECTORTYPE) {
                Expression newVal = dotProduct(left, newMembers[i]);
                foundOne = true;
            }
        }
        if (!foundOne) {
            ExprVector temp;
            temp.push_back(left);
            newMembers = setUnion(temp, newMembers);
        }
            
        return *new Expression(new Product(newMembers));
    }
    //if(leftType == SUMTYPE ||)
    return distribute(left, right);
};
String EuclidVector::print() const {
    /*String result = "{";
    for(int i = 0; i<elements[0].size(); i++) {
        result += elements[0][i].print();
    }
    result += "}";*/
    String result = name;
    return result;
};

EuclidVector::EuclidVector(String name, ExprVector components) : Symbol(name) {
    (*this).components = components;
}

EuclidVector::EuclidVector(const EuclidVector& target) : Symbol(target.print()) {
    
};
EuclidVector& EuclidVector::operator=(const EuclidVector& target) {
    components = *new ExprVector(target.components);
    return *this;
};
EuclidVector::~EuclidVector() {
    
};

Expression EuclidVector::simplify() const {
    return *new Expression(this);
};

/*
Expression EuclidVector::distribute(Expression other) const {
    size_t otherType = other.getTypeHash();
    Expression thisExpr = *new Expression(this);
    if(otherType == SUMTYPE) {
        const Sum& otherAdd = dynamic_cast<const Sum&>(*other);
        ExprVector newMembers = otherAdd.getMembers();
        for(int i = 0; i<newMembers.size(); i++) {
            newMembers[i] = distribute(newMembers[i]);
        }
        return *new Expression(new Sum(newMembers));
    }
    if(otherType == PRODUCTTYPE) {
        Expression testTarget = getElementOfType(other, EUCLIDVECTORTYPE);
        if(testTarget.getTypeHash() != NULLTYPE) {
            Expression product = thisExpr*testTarget;
            return product*cancelFactor(other, testTarget);
        }
        const Product& otherMul = dynamic_cast<const Product&>(*other);
        ExprVector newMembers = *new ExprVector();
        ExprVector otherMembers = otherMul.getMembers();
        newMembers.push_back(thisExpr);
        for(int i = 0; i<otherMembers.size(); i++) {
            newMembers.push_back(otherMembers[i]);
        }
        return *new Expression(new Product(newMembers));
    }
    if(otherType == FRACTIONTYPE) {
        const Fraction& otherFrac = dynamic_cast<const Fraction&>(*other);
        return *new Expression(new Fraction(distribute(otherFrac.getNumerator()),otherFrac.getDenomenator()));
    }
    return *new Expression(new Product(thisExpr,other));
};*/

Expression EuclidVector::factor() const {
    return *new Expression(this);
};
Expression EuclidVector::reciprocal() const {
    return *new Expression(new Fraction(*new Expression(this)));
};
Expression EuclidVector::determinant() const {
    return *new Expression(this);
};
/*
Expression EuclidVector::transpose() const {
    return *new Expression(this);
};*/
ExprVector EuclidVector::getFactors() const {
    return {*new Expression(this)};
}
ExprVector EuclidVector::getComponents() const
{
    return components;
}
;
