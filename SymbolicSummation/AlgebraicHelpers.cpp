//
//  ExpressionHelpers.cpp
//  SymbolicSummation
//
//  Created by Laina Stahulak on 2/1/22.
//

#include "AlgebraicHelpers.hpp"
#include "Symbol.hpp"
#include "Real.hpp"
#include "Container.hpp"
#include "PauliMatrices.hpp"
#include "AbstractHelpers.hpp"
#include <cmath>

Expression complexConjugate(Expression target) {
    Expression result = substitute(target, IMAGUNIT, (-IMAGUNIT));
    if(result.getTypeHash() != NULLTYPE)
        return result;
    return *new Expression(target.get());
}

bool isNegative(Expression target) {
    bool negative = false;
    size_t targetType = target.getTypeHash();
    ExprVector elementsToCheck = *new ExprVector();
    if(targetType == SIGNTYPE) {
        const Sign& signObj = dynamic_cast<const Sign&>(*target);
        negative = true;
        return negative != isNegative(signObj.getMember());
    }
    if(targetType == SUMTYPE) {
        const Sum& addObj = dynamic_cast<const Sum&>(*target);
        elementsToCheck = addObj.getMembers();
        bool totallyNegative = true;
        for(size_t i = 0; i<elementsToCheck.size(); i++) {
            totallyNegative &= isNegative(elementsToCheck[i]);
        }
        return negative != totallyNegative;
    }
    if(targetType == PRODUCTTYPE) {
        const Product& mulObj = dynamic_cast<const Product&>(*target);
        elementsToCheck = mulObj.getMembers();
        for(size_t i = 0; i<elementsToCheck.size(); i++) {
            negative = negative != isNegative(elementsToCheck[i]);
        }
        return negative;
    }
    if(targetType == FRACTIONTYPE) {
        const Fraction& fracObj = dynamic_cast<const Fraction&>(*target);
        negative = negative != isNegative(fracObj.getNumerator());
        return negative != isNegative(fracObj.getDenomenator());
    }
    if(targetType == REALTYPE) {
        const Real& realObj = dynamic_cast<const Real&>(*target);
        negative = negative != realObj.getValue() < 0;
    }
    return negative;
}

/*Expression determinant(Expression target) {
    if(!isTypeSimilarTo(target, MATRIXTYPE)) {
        return *new Expression(target.get());
    }
    const Matrix& matTarget = dynamic_cast<const Matrix&>(*target);
    ExprMatrix currMatrix = matTarget.elements;
    if(matTarget.dimensions.first != matTarget.dimensions.second)
        return *new Expression(target.get());
    int n = matTarget.dimensions.first;
    Expression det = ZERO;
    if (n == 2)
        return ((currMatrix[0][0] * currMatrix[1][1]) - (currMatrix[1][0] * currMatrix[0][1]));
    else {
        for (int x = 0; x < n; x++) {
            ExprMatrix submatrix = *new ExprMatrix();
            int subi = 0;
            for (size_t i = 1; i < n; i++) {
                int subj = 0;
                ExprVector currentRow = *new ExprVector();
                for (size_t j =0; j < n; j++) {
                    if (j == x)
                        continue;
                    currentRow.push_back(currMatrix[i][j]);
                    //submatrix[subi][subj] = currMatrix[i][j];
                    subj++;
                }
                submatrix.push_back(currentRow);
                subi++;
            }
            Expression submatrixObj = new Matrix("submatrix",*new ExprMatrix(submatrix));
            det = det + ((float)pow(-1, x) * currMatrix[0][x] * determinant(submatrixObj));
        }
    }
    return det;
}*/

Expression matMul(Expression left, Expression right) {
    if( isTypeSimilarTo(right, MATRIXTYPE) && isTypeSimilarTo(left, MATRIXTYPE)) {
        const Matrix& leftMatrix = dynamic_cast<const Matrix&>(*left);
        const Matrix& rightMatrix = dynamic_cast<const Matrix&>(*right);
        if(leftMatrix.dimensions.second != rightMatrix.dimensions.first) {
            Expression result = *new Expression(new NullObject("mismatch in dimensions for multiplication of MATRIXTYPE"));
            return result;
        }
        std::pair<int, int> newDimensions = {leftMatrix.dimensions.first,rightMatrix.dimensions.second};
        
        ExprMatrix newElements = *new ExprMatrix();
        String newName = "{";
        for(size_t i = 0; i<newDimensions.first; i++) {
            ExprVector currentRow = *new ExprVector();
            newName+="{";
            if(i!=0)
                newName+=",";
            for(size_t j =0; j<newDimensions.second; j++) {
                Expression result = ZERO;
                for(int k = 0; k<leftMatrix.dimensions.second; k++) {
                    Expression leftEle = leftMatrix.elements[i][k];
                    Expression rightEle = rightMatrix.elements[k][j];
                    result = result+leftEle*rightEle;
                }
                if(j!=0)
                    newName+=",";
                currentRow.push_back(result);
                newName+=result.print();
            }
            newElements.push_back(currentRow);
            newName+="}";
        }
        newName+="}";
        if(newDimensions.first == 1 && newDimensions.second == 1) {
            return newElements[0][0];
        }
        Expression result = *new Expression(new Matrix(newName,*new ExprMatrix(newElements)));
        return declareSymbol(newName,result);
    }
    return left*right;
};

/*Expression transpose(Expression target) {
    if(!isTypeSimilarTo(target, MATRIXTYPE))
        return *new Expression(target.get());
    const Matrix& matTarget = dynamic_cast<const Matrix&>(*target);
    std::pair<int,int> dimensions = *new std::pair<int,int>();
    dimensions.first = matTarget.dimensions.second;
    dimensions.second = matTarget.dimensions.first;
    ExprMatrix newElements = *new ExprMatrix();
    for(size_t i = 0; i <dimensions.first; i++) {
        ExprVector currentRow = *new ExprVector();
        for(size_t j =0; j<dimensions.second; j++) {
            Expression currElement = matTarget.elements[j][i];
            currentRow.push_back(currElement);
        }
        newElements.push_back(currentRow);
    }
    if(target->getTypeHash() == PAULIMATRIXTYPE) {
        const PauliMatrix& pauliTarget = dynamic_cast<const PauliMatrix&>(*target);
        Expression result = *new Expression(new PauliMatrix(target->print()+"Transpose",pauliTarget.flavor,newElements));
        return result;
    }
    Expression result = *new Expression(new Matrix(matTarget.name+"Transpose", newElements));
    return result;
}*/

Expression combineProducts(Expression left, Expression right) {
    size_t leftType = left->getTypeHash();
    size_t rightType = right->getTypeHash();
    if(rightType == NULLTYPE)
        right*left;
    if(leftType == NULLTYPE)
        left*right;
    if(leftType == ZEROTYPE || rightType == ZEROTYPE)
        return ZERO;
    if(leftType == ONETYPE)
        return *new Expression(right.get());
    if(rightType == ONETYPE)
        return *new Expression(left.get());
    if(isTypeSimilarTo(left, REALTYPE) && isTypeSimilarTo(right, REALTYPE))
        return left*right;
    if(leftType == PRODUCTTYPE) {
        const Product& lMul = dynamic_cast<const Product&>(*left);
        ExprVector newMembers = *new ExprVector(lMul.getMembers());
        if(rightType == PRODUCTTYPE) {
            const Product& rMul = dynamic_cast<const Product&>(*right);
            ExprVector rightMembers = rMul.getMembers();
            for(size_t i = 0; i<rightMembers.size(); i++) {
                newMembers.push_back(rightMembers[i]);
            }
        } else {
            newMembers.push_back(*new Expression(right));
        }
        if((int)newMembers.size() == 1)
            return newMembers[0];
        Expression result = *new Expression(new Product(newMembers));
        return result;
    }
    if(rightType == PRODUCTTYPE) {
        const Product& rMul = dynamic_cast<const Product&>(*right);
        ExprVector newMembers = *new ExprVector();
        newMembers.push_back(*new Expression(left));
        for(size_t i = 0; i< rMul.getMembers().size(); i++) {
            newMembers.push_back(rMul.getMembers()[i]);
        }
        Expression result = *new Expression(new Product(newMembers));
        return result;
    } else {
        Expression result = *new Expression(new Product(left,right));
        return result;
    }
};

/*
Expression combineSums(Expression left, Expression right) {
    size_t leftType = left->getTypeHash();
    size_t rightType = right->getTypeHash();
    if(rightType == NULLTYPE)
        right*left;
    if(leftType == NULLTYPE)
        left*right;
    if(isTypeSimilarTo(left, ADDTYPE)) {
        const Add& lAdd = dynamic_cast<const Add&>(*left);
        ExprVector newMembers(lAdd.getMembers());//copyExprVector(lAdd.getMembers());
        if(isTypeSimilarTo(right, ADDTYPE)) {
            const Add& rAdd = dynamic_cast<const Add&>(*right);
            ExprVector rightMembers(rAdd.getMembers());//copyExprVector(rAdd.getMembers());
            if(lAdd.getMembers().size() == 1 && rAdd.getMembers().size() == 1) {
                Expression tempResult = lAdd.getMembers()[0]+rAdd.getMembers()[0];
                return tempResult;
            }
            for(size_t i = 0; i<rightMembers.size(); i++) {
                //Expression nextMember(rightMembers[i]);
                if(rightMembers[i] == ZERO)
                    continue;
                newMembers.push_back(rightMembers[i]);
            }
        } else {
            if(lAdd.getMembers().size() == 1) {
                return lAdd.getMembers()[0]+right;
            }
            newMembers.push_back(*new Expression(right));
        }
        Expression result = *new Expression(new Add(newMembers));//cancelTerms(new Add(newMembers,newSigns));
        return result;
    }
    if(isTypeSimilarTo(right, ADDTYPE)) {
        const Add& rAdd = dynamic_cast<const Add&>(*right);
        ExprVector newMembers = *new ExprVector();
        newMembers.push_back(*new Expression(left));
        for(size_t i = 0; i<rAdd.getMembers().size(); i++) {
            newMembers.push_back(rAdd.getMembers()[i]);
        }
        Expression result = *new Expression(new Add(newMembers));
        return result;
    } else {
        Expression result = *new Expression(new Add(left,right));
        return result;
    }
};
*/

bool commutesWith(Expression left, Expression right) {
    size_t ltype = left->getTypeHash();
    size_t rtype = right->getTypeHash();
    
    //check matrices
    if(ltype == MATRIXTYPE) {
        if(rtype == MATRIXTYPE)
            return false;
        Expression matTarget = getElementOfType(right, MATRIXTYPE);
        return matTarget.getTypeHash() == NULLTYPE;
    }
    if(rtype == MATRIXTYPE) {
        Expression matTarget = getElementOfType(left, MATRIXTYPE);
        return matTarget.getTypeHash() == NULLTYPE;
    }
    
    //check pauli matrices
    if(ltype == PAULIMATRIXTYPE) {
        if(rtype == PAULIMATRIXTYPE)
            return false;
        Expression matTarget = getElementOfType(right, PAULIMATRIXTYPE);
        return matTarget.getTypeHash() == NULLTYPE;
    }
    if(rtype == PAULIMATRIXTYPE) {
        Expression matTarget = getElementOfType(left, PAULIMATRIXTYPE);
        return matTarget.getTypeHash() == NULLTYPE;
    }
    
    
    if(ltype == IMAGINARYUNITTYPE) {
        if(rtype == IMAGINARYUNITTYPE)
            return false;
        Expression iTarget = getElementOfType(right, IMAGINARYUNITTYPE);//right->getFirstInstanceOfType(IMAGINARYUNITTYPE);
        return iTarget.getTypeHash() == NULLTYPE;
    }
    if(rtype == IMAGINARYUNITTYPE) {
        Expression iTarget = getElementOfType(left, IMAGINARYUNITTYPE);//left->getFirstInstanceOfType(IMAGINARYUNITTYPE);
        return iTarget.getTypeHash() == NULLTYPE;
    }
    
    return true;
}

/*
Expression cancelTerms(Expression target) {
    if(target->getTypeHash() != ADDTYPE)
        return *new Expression(target.get());
    const Add& addObj = dynamic_cast<const Add&>(*target);
    ExprVector members = *new ExprVector(addObj.getMembers());
    ExprVector newMembers = *new ExprVector();
    std::vector<int> indicesAccountedFor = *new std::vector<int>();
    bool didAnyCancelation = false;
    for(size_t i = 0; i<members.size(); i++) {
        if(intVectorContains(indicesAccountedFor, i))
            continue;
        indicesAccountedFor.push_back(i);
        Expression total = members[i];
        for(size_t j =i+1; j<members.size(); j++) {
            if(intVectorContains(indicesAccountedFor, j))
                continue;
            Expression currMember = members[j];
            Expression tempValue = combineTermsDifferingByCoefficientsAdditively(total, currMember);
            if(tempValue.getTypeHash() != ADDTYPE && tempValue.getTypeHash() != NULLTYPE) {
                indicesAccountedFor.push_back(j);
                total = tempValue;
                if(total == ZERO)
                    break;
            }
        }
        if(total != ZERO) {
            newMembers.push_back(total);
        }
        if(total != members[i])
            didAnyCancelation = true;
    }
    if(didAnyCancelation) {
        if(newMembers.size() == 0) {
            return ZERO;
        }
        return *new Expression(new Add(newMembers));
    }
    return *new Expression(target.get());
}*/

/*
Expression simplifySubExpressions(Expression target) {
    bool isMul = target->getTypeHash() == MULTYPE;
    bool isAdd = target->getTypeHash() == ADDTYPE;
    Expression total;
    Expression remainder = *new Expression(target.get());
    Expression first;
    size_t typeToGet = 0;
    if(isMul) {
        total = ONE;
        typeToGet = ADDTYPE;
    }
    if(isAdd) {
        return cancelTerms(target);
    }
    first = getElementOfType(target, typeToGet);
    if(first.getTypeHash() == NULLTYPE || (!isMul&&!isAdd))
        return *new Expression(target.get());
    total = first;
    if(isMul)
        remainder = removeElementMultiplicatively(remainder, first);
    Expression second = getElementOfType(remainder, typeToGet);
    while(second.getTypeHash() != NULLTYPE) {
        if(isMul) {
            remainder = removeElementMultiplicatively(remainder, second);
            total = total.distribute(second);
        }
        
        second = getElementOfType(remainder, ADDTYPE);
        
    }
    if(remainder.getTypeHash() != ZEROTYPE) {
        if(isMul)
            total = total*remainder;//distribute(total, remainder);
        if(isAdd)
            total = total+remainder;//combineSums(total, remainder);
    }
    return total;
}
*/

/*
Expression simplifyPauliMatrices(Expression target) {
    bool isMul = target->getTypeHash() == PRODUCTTYPE;
    bool isAdd = target->getTypeHash() == SUMTYPE;
    Expression total;
    Expression remainder = *new Expression(target.get());
    Expression firstPauli = getElementOfType(target, PAULIMATRIXTYPE);
    if(firstPauli.getTypeHash() == NULLTYPE)
        return *new Expression(target.get());
    if(isMul) {
        total = ONE;
    }
    if(isAdd) {
        total = ZERO;
    }
    
    while(firstPauli.getTypeHash() != NULLTYPE && remainder != ZERO && remainder != ONE) {
        if(isMul)
            remainder = removeElementMultiplicatively(remainder, firstPauli);//remainder = remainder.remove(firstPauli);
        if(isAdd)
            remainder = removeElementAdditively(remainder, firstPauli);
        Expression secondPauli = getMatrixMatchingPauliFlavor(remainder, firstPauli);
        Expression tempResult = firstPauli;
        while(secondPauli.getTypeHash() != NULLTYPE && remainder != ZERO && remainder != ONE) {
            if(isMul) {
                remainder = removeElementMultiplicatively(remainder, secondPauli);//remainder = remainder.remove(secondPauli);
                tempResult = tempResult*secondPauli;
            }
            if(isAdd) {
                remainder = removeElementAdditively(remainder, secondPauli);//remainder = remainder.remove(secondPauli);
                tempResult = tempResult+secondPauli;
            }
            
            secondPauli = getMatrixMatchingPauliFlavor(remainder, firstPauli);
            
        }
        //if(tempResult.getTypeHash() == NULLTYPE)
        //    tempResult = firstPauli;
        
        firstPauli = getElementOfType(remainder, PAULIMATRIXTYPE);//remainder.getFirstInstanceOfType(PAULIMATRIXTYPE);
        if(isMul)
            total = total*tempResult;//distribute(total, firstPauli);
        if(isAdd)
            total = total+tempResult;//combineSums(total, firstPauli);
    }
    if(remainder.getTypeHash() != ZEROTYPE) {
        if(isMul)
            total = total*remainder;//distribute(total, remainder);
        if(isAdd)
            total = total+remainder;//combineSums(total, remainder);
    }
    if(total.getTypeHash() == PRODUCTTYPE) {
        const Product& mulObj = dynamic_cast<const Product&>(*total);
        if(mulObj.getMembers().size() == 0)
            return ZERO;
        if(mulObj.getMembers().size() == 1)
            return mulObj.getMembers()[0];
    }
    if(total.getTypeHash() == SUMTYPE) {
        const Sum& addObj = dynamic_cast<const Sum&>(*total);
        if(addObj.getMembers().size() == 0)
            return ZERO;
        if(addObj.getMembers().size() == 1) {
            return addObj.getMembers()[0];
        }
    }
    return total;
}*/

/*
Expression simplify(Expression target) {
    //std::vector<size_t> types = {REALTYPE,IMAGINARYUNITTYPE,SYMBOLTYPE,MATRIXTYPE,PAULIMATRIXTYPE,FRACTYPE};
    std::vector<size_t> types = {FRACTYPE,EXPTYPE,SYMBOLTYPE,EUCLIDVECTORTYPE,PAULIMATRIXTYPE,MATRIXTYPE,IMAGINARYUNITTYPE,REALTYPE};
    Expression result = *new Expression(target.get());
    size_t targetType = target->getTypeHash();
    bool isAdd = (targetType == ADDTYPE);
    bool isMul = (targetType == MULTYPE);
    bool overallSign = false;
    if( targetType == FRACTYPE ) {
        const Frac& fracTarget = dynamic_cast<const Frac&>(*target);
        Expression newNumerator = simplify(fracTarget.getNumerator());
        Expression newDenomenator = simplify(fracTarget.getDenomenator());
        Expression result = *new Expression(new Frac(newNumerator,newDenomenator));
        return *new Expression(result);
    }
    if(targetType == SIGNTYPE) {
        return -simplify(-target);
    }
    if(!isMul && !isAdd) {// !isSubclassOf(target, typeid(Operator).hash_code())) {
        return target;
    }
    if(isMul) {
        const Mul& mulObj = dynamic_cast<const Mul&>(*target);
        ExprVector members = mulObj.getMembers();
        if(members.size() == 1)
            return members[0];
        ExprVector newMembers = *new ExprVector();
        for(size_t i = 0; i<members.size(); i++) {
            Expression simplified = simplify(members[i]);
            if(simplified == ZERO)
                return ZERO;
            if(simplified.getTypeHash() == SIGNTYPE) {
                simplified = -simplified;
                overallSign = overallSign != true;
            }
            newMembers.push_back(simplified);
        }
        if(newMembers.size() == 0)
            return ZERO;
        if(newMembers.size() == 1)
            return newMembers[0];
        result = *new Expression(new Mul(newMembers));
    }
    if(isAdd) {
        const Add& addObj = dynamic_cast<const Add&>(*target);
        ExprVector members = addObj.getMembers();
        if(members.size() == 1)
            return *new Expression(target.get());
        ExprVector newMembers = *new ExprVector();
        for(size_t i = 0; i<members.size(); i++) {
            Expression simplified = simplify(members[i]);
            if(simplified == ZERO)
                continue;
            newMembers.push_back(simplified);
        }
        if(newMembers.size() == 0)
            return ZERO;
        if(newMembers.size() == 1) {
            return newMembers[0];
        }
        Expression simplifiedTarget = *new Expression(new Add(newMembers));
        //result = cancelTerms(simplifiedTarget);
        result = simplifiedTarget;
        
    }
    for(size_t type : types) {
        isAdd = (result.getTypeHash() == ADDTYPE);
        isMul = (result.getTypeHash() == MULTYPE);
        if(type == PAULIMATRIXTYPE) {
            result = simplifyPauliMatrices(result);
        }
        else {//all cases but simplifying pauli matrices
            Expression first = getElementOfType(result, type);
            if(first.getTypeHash() == NULLTYPE)
                continue;
            Expression remainder;// = result.remove(first);
            if(isMul) {
                remainder = removeElementMultiplicatively(result, first);
            }
            if(isAdd) {
                remainder = removeElementAdditively(result, first);
            }
            Expression second = getElementOfType(remainder, type);//remainder.getFirstInstanceOfType(type);
            Expression total = first;
            while(second.getTypeHash() != NULLTYPE && remainder != ZERO && remainder != ONE) {
                if(isMul) {
                    remainder = removeElementMultiplicatively(remainder, second);//remainder = remainder.remove(second);
                }
                if(isAdd) {
                    remainder = removeElementAdditively(remainder, second);
                }
                if(isMul)
                    total = total*second;
                if(isAdd)
                    total = total+second;
                second = getElementOfType(remainder, type);//remainder.getFirstInstanceOfType(type);
            }
            //if(tempResult.getTypeHash() == NULLTYPE)
            //    tempResult = first;
            if(remainder.getTypeHash() != NULLTYPE) {
                if(isMul)
                    result = total*remainder;//distribute(first, remainder);
                if(isAdd)
                    result = total+remainder;//combineSums(first, remainder);
            } else
                result = total;
        }
    }
    //if(isMul)
    if(overallSign)
        result = -result;
    result = simplifySubExpressions(result);
    if(result.getTypeHash() == MULTYPE) {
        const Mul& mulObj = dynamic_cast<const Mul&>(*result);
        if(mulObj.getMembers().size() == 0)
            return ZERO;
        if(mulObj.getMembers().size() == 1)
            return mulObj.getMembers()[0];
    }
    if(result.getTypeHash() == ADDTYPE) {
        const Add& addObj = dynamic_cast<const Add&>(*result);
        if(addObj.getMembers().size() == 0)
            return ZERO;
        if(addObj.getMembers().size() == 1) {
            return addObj.getMembers()[0];
        }
    }
    return result;
}*/

ExprVector cancelCommonFactors(ExprVector targets) {
    ExprVector results = targets;
    ExprVector factors = commonFactors(results);
    for(size_t i = 0; i<results.size(); i++) {
        for(size_t j = 0; j<factors.size(); j++) {
            results[i] = cancelFactor(results[i], factors[j]);
        }
    }
    return results;
}

Expression distribute(Expression left, Expression right) {
    size_t leftType = left->getTypeHash();
    size_t rightType = right->getTypeHash();
    if(rightType == NULLTYPE)
        right*left;
    if(leftType == NULLTYPE)
        left*right;
    if(leftType == ZEROTYPE || rightType == ZEROTYPE)
        return ZERO;
    if(leftType == ONETYPE)
        return *new Expression(right);
    if(rightType == ONETYPE)
        return *new Expression(left);
    if(leftType == SIGNTYPE) {
        const Sign& signLeft = dynamic_cast<const Sign&>(*left);
        if(rightType == SIGNTYPE) {
            const Sign& signRight = dynamic_cast<const Sign&>(*right);
            return distribute(signLeft.getMember(), signRight.getMember());
        }
        return -distribute(signLeft.getMember(), right);
    }
    if(rightType == SIGNTYPE) {
        const Sign& signRight = dynamic_cast<const Sign&>(*right);
        return -distribute(left, signRight.getMember());
    }
    if(leftType == SUMTYPE) {
        const Sum& leftObj = dynamic_cast<const Sum&>(*left);
        if(rightType == SUMTYPE) {
            const Sum& rightObj = dynamic_cast<const Sum&>(*right);
            ExprVector newMembers = *new ExprVector();
            for(int i=0; i<leftObj.getMembers().size(); i++) {
                for(int j=0; j<rightObj.getMembers().size(); j++) {
                    Expression result = leftObj.getMembers()[i] * rightObj.getMembers()[j];
                    newMembers.push_back(result);
                }
            }
            if(newMembers.size() == 1)
                return newMembers[0];
            Expression result = *new Expression(new Sum(newMembers));
            return result;
        }//end ltype == ADD, rtype == ADD
        if(rightType == FRACTIONTYPE) {
            const Fraction& rightObj = dynamic_cast<const Fraction&>(*right);
            Expression newNumerator = distribute(left, rightObj.getNumerator());
            Expression newDenomenator = rightObj.getDenomenator();
            ExprVector targets = generateExprVector({newNumerator,newDenomenator});
            ExprVector results = cancelCommonFactors(targets);
            Expression finalResult = *new Expression(new Fraction(results[0],results[1]));
            return finalResult;
            
        }//end ltype == ADD, rtype == FRAC
        if((int)leftObj.getMembers().size() == 1) {
            Expression tempResult = leftObj.getMembers()[0]*right;
            return tempResult;
        }
        ExprVector newMembers = *new ExprVector();
        SignVector newSigns = *new SignVector();
        for(int i=0; i<leftObj.getMembers().size(); i++) {
            Expression result = distribute(leftObj.getMembers()[i], right);
            newMembers.push_back(result);
        }
        Expression result = *new Expression(new Sum(newMembers));
        return result;
        //end ltype == ADD, rtype == MUL or similar
    }//end ltype == ADD
    
    if(leftType == FRACTIONTYPE) {
        const Fraction& leftObj = dynamic_cast<const Fraction&>(*left);
        if(rightType == FRACTIONTYPE) {
            const Fraction& rightObj = dynamic_cast<const Fraction&>(*right);
            Expression newNumerator = distribute(leftObj.getNumerator(), rightObj.getNumerator());
            Expression newDenomenator = distribute(leftObj.getDenomenator(), rightObj.getDenomenator());
            ExprVector targets = generateExprVector({newNumerator,newDenomenator});
            ExprVector results = cancelCommonFactors(targets);
            Expression finalResult = *new Expression(new Fraction(results[0],results[1]));
            return finalResult;
        }//end ltype = FRAC, rtype == FRAC
        Expression newNumerator = distribute(leftObj.getNumerator(), right);
        Expression newDenomenator = leftObj.getDenomenator();
        ExprVector targets = generateExprVector({newNumerator,newDenomenator});
        ExprVector results = cancelCommonFactors(targets);
        Expression finalResult = *new Expression(new Fraction(results[0],results[1]));
        return finalResult;
        //end ltype == FRAC, rtype = MUL or similar
    }//end ltype = FRAC
    
    if(rightType == SUMTYPE) {
        const Sum& rightObj = dynamic_cast<const Sum&>(*right);
        ExprVector newMembers = *new ExprVector();
        for(int i=0; i<rightObj.getMembers().size(); i++) {
            Expression result = left*rightObj.getMembers()[i];
            //result = simplify(*result);//combineProducts(left, rightObj.getMembers()[i]);
            newMembers.push_back(result);
        }
        Expression result = *new Expression(new Sum(newMembers));
        return result;
    }
    if(rightType == FRACTIONTYPE) {
        const Fraction& rightObj = dynamic_cast<const Fraction&>(*right);
        Expression newNumerator = distribute(left, rightObj.getNumerator());
        Expression newDenomenator = rightObj.getDenomenator();
        ExprVector targets = generateExprVector({newNumerator,newDenomenator});
        ExprVector results = cancelCommonFactors(targets);
        Expression finalResult = *new Expression(new Fraction(results[0],results[1]));
        return finalResult;
    }
    if(leftType == EXPONENTTYPE && rightType == EXPONENTTYPE) {
        const Exponent& leftExp = dynamic_cast<const Exponent&>(*left);
        const Exponent& rightExp = dynamic_cast<const Exponent&>(*right);
        if(leftExp.getBase() == rightExp.getBase()) {
            Expression newExponent = leftExp.getExponent()+rightExp.getExponent();
            Expression result = *new Expression(new Exponent(leftExp.getExponent(),newExponent));
            return result;
        }
    }
    Expression mul = combineProducts(left, right);
    return mul;
}


/*Expression factor(Expression factee) {
    ExprVector factors = getFactors(factee);
    if(factors.size()>1) {
        Expression result = *new Expression(new Mul(factors));
        return result;
    }
    return *new Expression(factee);
}*/

ExprVector getFactorsOfInt(Expression factee) {
    if(!isTypeSimilarTo(factee, REALTYPE))
        return {*new Expression(factee)};
    const Real& realObj = dynamic_cast<const Real&>(*factee);
    if(!(realObj.value == trunc(realObj.value)))
        return {*new Expression(factee)};
    int val = (int)realObj.value;
    ExprVector factors = *new ExprVector();
    if(val<0) {
        factors.push_back(MINUSONE);
        val = -val;
    }
    for(size_t i = 2; i <= val; i++) {
        int remain = val%i;
        if(remain == 0) {
            int temp = val;
            while(temp%i == 0) {
                factors.push_back(declareReal(i));
                temp /= i;
            }
            val = temp;
        }
    }
    return factors;
}

/*
ExprVector getFactors(Expression factee) {
    size_t type = factee->getTypeHash();
    if(type == MULTYPE) {
        const Mul& mulObj = dynamic_cast<const Mul&>(*factee);
        ExprVector factors = *new ExprVector();
        for(size_t i = 0; i< mulObj.getMembers().size(); i++) {
            ExprVector tempFactors = getFactors(mulObj.getMembers()[i]);
            for(size_t j =0; j< tempFactors.size(); j++)
                //if(!containsElement(factors, tempFactors[j]))
                    factors.push_back(tempFactors[j]);
        }
        return factors;
    }
    if(type == ADDTYPE) {
        const Add& addObj = dynamic_cast<const Add&>(*factee);
        ExprVector factors = commonFactors(addObj.getMembers());
        if(factors.size()>0)
            return factors;
    }
    if(type == SIGNTYPE) {
        const Sign& signObj = dynamic_cast<const Sign&>(*factee);
        ExprVector factors = getFactors(signObj.getMember());
        factors.push_back(MINUSONE);
        if(factors.size()>0)
            return factors;
    }
    if(type == FRACTYPE) {
        const Frac& fracObj = dynamic_cast<const Frac&>(*factee);
        Expression result = reciprocal(fracObj.getDenomenator());
        ExprVector targets = generateExprVector({result});
        ExprVector factors = getFactors(fracObj.getNumerator());
        return combineExprVectors( factors, targets );
    }
    if(type == EXPTYPE) {
        const Exp& expObj = dynamic_cast<const Exp&>(*factee);
        Expression exponent = expObj.getExponent();
        size_t exponentType = exponent.getTypeHash();
        if(exponentType == ADDTYPE) {
            ExprVector factors = *new ExprVector();
            const Add& addObj = dynamic_cast<const Add&>(*exponent);
            for(size_t i = 0; i<addObj.getMembers().size(); i++) {
                Expression newExpression = *new Expression(new Exp(expObj.getBase(),addObj.getMembers()[i]));
                factors.push_back(newExpression);
            }
        }
    }
    if(isTypeSimilarTo(factee, REALTYPE))
        return getFactorsOfInt(factee);
    return generateExprVector({*new Expression(factee)});
}*/

ExprVector commonFactors(ExprVector terms) {
    
    ExprVector firstTermFactors = terms[0].getFactors();
    ExprVector commonFactors = firstTermFactors;
    if(terms.size() == 1)
        return firstTermFactors;
    for(size_t i = 1; i< terms.size(); i++) {
        ExprVector nextFactors = terms[i].getFactors();
        commonFactors = setIntersect(commonFactors, nextFactors);
    }
    return commonFactors;
    /*
    ExprVector commonFactors = *new ExprVector();
    ExprMatrix ithTermFactors = *new ExprMatrix();
    ExprVector firstTermFactors = getFactors(terms[0]);
    if(terms.size()<2)
        return firstTermFactors;
    std::vector<std::vector<int>> indicesAlreadyCounted = *new std::vector<std::vector<int>>();
    for(size_t i = 1; i<terms.size(); i++) {
        ExprVector currTermFactors = getFactors(terms[i]);
        ithTermFactors.push_back(currTermFactors);
        indicesAlreadyCounted.push_back(*new std::vector<int>());
    }
    
    for(size_t i = 0; i<firstTermFactors.size(); i++) {
        bool isCommonFactor = true;
        for(int j=0; j<(ithTermFactors).size(); j++) {
            bool containsFactor = false;
            for(int k = 0; k<ithTermFactors[j].size(); k++) {
                if(intVectorContains(indicesAlreadyCounted[j], k))
                    continue;
                Expression firstFactor = *new Expression(firstTermFactors[i]);
                Expression ithFactor = *new Expression(ithTermFactors[j][k]);
                containsFactor |= firstFactor == ithFactor;
                containsFactor |= -firstFactor == ithFactor;
                containsFactor |= firstFactor == -ithFactor;
                containsFactor &= !isSubtypeOf(firstTermFactors[i], MATRIXTYPE);
                if(containsFactor) {
                    indicesAlreadyCounted[j].push_back(k);
                    break;
                }
            }
            isCommonFactor &= containsFactor;
            if(!isCommonFactor)
                break;
            //isCommonFactor &= containsElement(ithTermFactors[j], firstTermFactors[i]);
        }
        if(isCommonFactor)
            commonFactors.push_back(firstTermFactors[i]);
    }
    return commonFactors;*/
}

//DOES NOT RESPECT COMMUTATION PROPERTIES... probably
/*
Expression combineTermsDifferingByCoefficientsAdditively(Expression left, Expression right) {
    if(left->getTypeHash() == ZEROTYPE)
        return *new Expression(right.get());
    if(right->getTypeHash() == ZEROTYPE)
        return *new Expression(left.get());
    if(isTypeSimilarTo(left, REALTYPE) && isTypeSimilarTo(right, REALTYPE))
        return left+right;
    if(left->getTypeHash() == SIGNTYPE) {
        const Sign& signObj = dynamic_cast<const Sign&>(*left);
        if(signObj.getMember() == right)
            return ZERO;
    }
    else if(right->getTypeHash() == SIGNTYPE) {
        const Sign& signObj = dynamic_cast<const Sign&>(*right);
        if(signObj.getMember() == left)
            return ZERO;
    }
    if(left == right)
        return simplify(2*left);
    ExprVector targets = generateExprVector({*new Expression(left),*new Expression(right)});
    ExprVector factors = commonFactors(targets);//getCommonFactors(targets);
    if(factors.size() == 0)
        return *new Expression(new NullObject("no common factors"));
    targets = generateExprVector({*new Expression(left),*new Expression(right)});
    ExprVector results = cancelCommonFactors(targets);//cancelCommonFactors(targets);
    Expression inCommon = ONE;
    for(size_t i = 0; i< factors.size(); i++) {
        inCommon = inCommon * factors[i];
    }
    Expression combinedCoeffs = combineTermsDifferingByCoefficientsAdditively(results[0], results[1]);
    if(combinedCoeffs.getTypeHash() == NULLTYPE)
        return left+right;
    Expression temp = combinedCoeffs*inCommon;
    return temp;
}*/

Expression performActions(Expression target) {
    size_t sourceType = target->getTypeHash();
    if(isTypeSimilarTo(target, MATRIXTYPE)) {
        const Matrix& matObj = dynamic_cast<const Matrix&>(*target);
        ExprMatrix elements = *new ExprMatrix(matObj.elements);
        for(size_t i = 0; i<matObj.dimensions.first; i++) {
            for (size_t j =0; j<matObj.dimensions.second; j++) {
                if(elements[i][j] == target) {
                    elements[i][j] = performActions(elements[i][j]);
                }
            }
        }
        if(sourceType == PAULIMATRIXTYPE) {
            const PauliMatrix& pauliSource = dynamic_cast<const PauliMatrix&>(*target);
            Expression newMat = declarePauliMatrix(new PauliMatrix(printExprMatrix(elements),pauliSource.flavor,elements));
            return newMat;
        }
        Expression newMat = declareMatrix(matObj.name+"with replacement", elements);
        return newMat;
    }
    if(sourceType == FRACTIONTYPE) {
        const Fraction& fracObj = dynamic_cast<const Fraction&>(*target);
        Expression newNum = performActions(fracObj.getNumerator());
        Expression newDenom = performActions(fracObj.getDenomenator());
        return *new Expression(new Fraction(newNum,newDenom));
    }
    if(sourceType == EXPONENTTYPE) {
        const Exponent& expObj = dynamic_cast<const Exponent&>(*target);
        Expression newBase = performActions(expObj.getBase());
        Expression newExponent = performActions(expObj.getExponent());
        return *new Expression(new Exponent(newBase,newExponent));
    }
    if(sourceType == SIGNTYPE) {
        const Sign& signObj = dynamic_cast<const Sign&>(*target);
        return -performActions(signObj.getMember());
    }
    if(isTypeSimilarTo(target, FUNCTIONTYPE)) {
        const Function& funcObj = dynamic_cast<const Function&>(*target);
        return funcObj.act();
    }
    ExprVector elementsToModify;
    ExprVector newElements = *new ExprVector();
    if(sourceType == SUMTYPE) {
        const Sum& addObj = dynamic_cast<const Sum&>(*target);
        elementsToModify = addObj.getMembers();
    }
    if(sourceType == PRODUCTTYPE) {
        const Product& mulObj = dynamic_cast<const Product&>(*target);
        elementsToModify = mulObj.getMembers();
    }
    for(size_t i = 0; i< elementsToModify.size(); i++) {
        Expression moddedEle = performActions(elementsToModify[i]);
        newElements.push_back(moddedEle);
    }
    if(sourceType == SUMTYPE) {
        return *new Expression(new Sum(newElements));
    }
    if(sourceType == PRODUCTTYPE) {
        return *new Expression(new Product(newElements));
    }
    return target;
}

Expression performActionsOn(Expression target, Expression var) {
    size_t sourceType = target->getTypeHash();
    if(isTypeSimilarTo(target, MATRIXTYPE)) {
        const Matrix& matObj = dynamic_cast<const Matrix&>(*target);
        ExprMatrix elements = *new ExprMatrix(matObj.elements);
        for(size_t i = 0; i<matObj.dimensions.first; i++) {
            for (size_t j =0; j<matObj.dimensions.second; j++) {
                if(elements[i][j] == target) {
                    elements[i][j] = performActionsOn(elements[i][j],var);
                }
            }
        }
        if(sourceType == PAULIMATRIXTYPE) {
            const PauliMatrix& pauliSource = dynamic_cast<const PauliMatrix&>(*target);
            Expression newMat = declarePauliMatrix(new PauliMatrix(printExprMatrix(elements),pauliSource.flavor,elements));
            return newMat;
        }
        Expression newMat = declareMatrix(matObj.name+"with replacement", elements);
        return newMat;
    }
    if(sourceType == FRACTIONTYPE) {
        const Fraction& fracObj = dynamic_cast<const Fraction&>(*target);
        Expression newNum = performActionsOn(fracObj.getNumerator(), var);
        Expression newDenom = performActionsOn(fracObj.getDenomenator(), var);
        return *new Expression(new Fraction(newNum,newDenom));
    }
    if(sourceType == EXPONENTTYPE) {
        const Exponent& expObj = dynamic_cast<const Exponent&>(*target);
        Expression newBase = performActionsOn(expObj.getBase(), var);
        Expression newExponent = performActionsOn(expObj.getExponent(), var);
        return *new Expression(new Exponent(newBase,newExponent));
    }
    if(sourceType == SIGNTYPE) {
        const Sign& signObj = dynamic_cast<const Sign&>(*target);
        return -performActionsOn(signObj.getMember(), var);
    }
    if(isTypeSimilarTo(target, FUNCTIONTYPE)) {
        const Function& funcObj = dynamic_cast<const Function&>(*target);
        return funcObj.resultOfActingOn(var);
    }
    ExprVector elementsToModify;
    ExprVector newElements = *new ExprVector();
    if(sourceType == SUMTYPE) {
        const Sum& addObj = dynamic_cast<const Sum&>(*target);
        elementsToModify = addObj.getMembers();
    }
    if(sourceType == PRODUCTTYPE) {
        const Product& mulObj = dynamic_cast<const Product&>(*target);
        elementsToModify = mulObj.getMembers();
    }
    for(size_t i = 0; i< elementsToModify.size(); i++) {
        Expression moddedEle = performActionsOn(elementsToModify[i], var);
        newElements.push_back(moddedEle);
    }
    if(sourceType == SUMTYPE) {
        return *new Expression(new Sum(newElements));
    }
    if(sourceType == PRODUCTTYPE) {
        return *new Expression(new Product(newElements));
    }
    return target;
}

Expression insertAsVariable(Expression target, Expression var) {
    size_t sourceType = target->getTypeHash();
    if(isTypeSimilarTo(target, MATRIXTYPE)) {
        const Matrix& matObj = dynamic_cast<const Matrix&>(*target);
        ExprMatrix elements = *new ExprMatrix(matObj.elements);
        for(size_t i = 0; i<matObj.dimensions.first; i++) {
            for (size_t j =0; j<matObj.dimensions.second; j++) {
                if(elements[i][j] == target) {
                    elements[i][j] = insertAsVariable(elements[i][j],var);
                }
            }
        }
        if(sourceType == PAULIMATRIXTYPE) {
            const PauliMatrix& pauliSource = dynamic_cast<const PauliMatrix&>(*target);
            Expression newMat = declarePauliMatrix(new PauliMatrix(printExprMatrix(elements),pauliSource.flavor,elements));
            return newMat;
        }
        Expression newMat = declareMatrix(matObj.name+"with replacement", elements);
        return newMat;
    }
    if(sourceType == FRACTIONTYPE) {
        const Fraction& fracObj = dynamic_cast<const Fraction&>(*target);
        Expression newNum = insertAsVariable(fracObj.getNumerator(), var);
        Expression newDenom = insertAsVariable(fracObj.getDenomenator(), var);
        return *new Expression(new Fraction(newNum,newDenom));
    }
    if(sourceType == EXPONENTTYPE) {
        const Exponent& expObj = dynamic_cast<const Exponent&>(*target);
        Expression newBase = insertAsVariable(expObj.getBase(), var);
        Expression newExponent = insertAsVariable(expObj.getExponent(), var);
        return *new Expression(new Exponent(newBase,newExponent));
    }
    if(sourceType == SIGNTYPE) {
        const Sign& signObj = dynamic_cast<const Sign&>(*target);
        return -insertAsVariable(signObj.getMember(), var);
    }
    if(isTypeSimilarTo(target, FUNCTIONTYPE)) {
        const Function& funcObj = dynamic_cast<const Function&>(*target);
        return funcObj.actingOn(var);
    }
    ExprVector elementsToModify;
    ExprVector newElements = *new ExprVector();
    if(sourceType == SUMTYPE) {
        const Sum& addObj = dynamic_cast<const Sum&>(*target);
        elementsToModify = addObj.getMembers();
    }
    if(sourceType == PRODUCTTYPE) {
        const Product& mulObj = dynamic_cast<const Product&>(*target);
        elementsToModify = mulObj.getMembers();
    }
    for(size_t i = 0; i< elementsToModify.size(); i++) {
        Expression moddedEle = insertAsVariable(elementsToModify[i], var);
        newElements.push_back(moddedEle);
    }
    if(sourceType == SUMTYPE) {
        return *new Expression(new Sum(newElements));
    }
    if(sourceType == PRODUCTTYPE) {
        return *new Expression(new Product(newElements));
    }
    return target;
}

Expression substitute(Expression source, Expression target, Expression value) {
    Expression valueExpr = *new Expression(value);
    if(source == target)
        return valueExpr;
    size_t sourceType = source->getTypeHash();
    if(isTypeSimilarTo(source, MATRIXTYPE)) {
        const Matrix& matObj = dynamic_cast<const Matrix&>(*source);
        ExprMatrix elements = *new ExprMatrix(matObj.elements);
        bool foundTarget = false;
        for(size_t i = 0; i<matObj.dimensions.first; i++) {
            for (size_t j =0; j<matObj.dimensions.second; j++) {
                if(elements[i][j] == target) {
                    elements[i][j] = valueExpr;
                    foundTarget = true;
                }
                if(-elements[i][j] == target || elements[i][j] == -target) {
                    elements[i][j] = -valueExpr;
                    foundTarget = true;
                }
            }
        }
        if(!foundTarget) {
            return source;
            return *new Expression(new NullObject("No substitutions possible"));
        }
        if(sourceType == PAULIMATRIXTYPE) {
            const PauliMatrix& pauliSource = dynamic_cast<const PauliMatrix&>(*source);
            Expression newMat = declarePauliMatrix(new PauliMatrix(printExprMatrix(elements),pauliSource.flavor,elements));
            return newMat;
        }
        Expression newMat = declareMatrix(matObj.name+"with replacement", elements);
        return newMat;
    }
    if(sourceType == FRACTIONTYPE) {
        const Fraction& fracObj = dynamic_cast<const Fraction&>(*source);
        Expression newNum;
        Expression newDenom;
        bool changedNum = false;
        bool changedDenom = false;
        Expression testResult = substitute(fracObj.getNumerator(), target, value);
        if(testResult.getTypeHash() == NULLTYPE)
            newNum = fracObj.getNumerator();
        else {
            newNum = testResult;
            changedNum = true;
        }
        testResult = substitute(fracObj.getDenomenator(), target, value);
        if(testResult.getTypeHash() == NULLTYPE)
            newDenom = fracObj.getDenomenator();
        else {
            newDenom = testResult;
            changedDenom = true;
        }
        if(changedNum || changedDenom) {
            return *new Expression(new Fraction(newNum,newDenom));
        }
        return *new Expression(new NullObject("No substitutions possible"));
    }
    if(sourceType == EXPONENTTYPE) {
        const Exponent& expObj = dynamic_cast<const Exponent&>(*source);
        Expression newBase;
        Expression newExponent;
        bool changedBase = false;
        bool changedExponent = false;
        Expression testResult = substitute(expObj.getBase(), target, value);
        if(testResult.getTypeHash() == NULLTYPE)
            newBase = expObj.getBase();
        else {
            newBase = testResult;
            changedBase = true;
        }
        testResult = substitute(expObj.getExponent(), target, value);
        if(testResult.getTypeHash() == NULLTYPE)
            newExponent = expObj.getExponent();
        else {
            newExponent = testResult;
            changedExponent = true;
        }
        if(changedBase || changedExponent) {
            return *new Expression(new Exponent(newBase,newExponent));
        }
        return *new Expression(new NullObject("No substitutions possible"));
    }
    if(sourceType == SIGNTYPE) {
        const Sign& signObj = dynamic_cast<const Sign&>(*source);
        Expression testResult = substitute(signObj.getMember(), target, value);
        if(testResult.getTypeHash() != NULLTYPE)
            return -testResult;
        return *new Expression(new NullObject("No substitutions possible"));
    }
    ExprVector elementsToModify;
    ExprVector newElements = *new ExprVector();
    if(sourceType == SUMTYPE) {
        const Sum& addObj = dynamic_cast<const Sum&>(*source);
        elementsToModify = addObj.getMembers();
    }
    if(sourceType == PRODUCTTYPE) {
        const Product& mulObj = dynamic_cast<const Product&>(*source);
        elementsToModify = mulObj.getMembers();
    }
    
    bool moddedAnyElements = false;
    for(size_t i = 0; i< elementsToModify.size(); i++) {
        Expression moddedEle = substitute(elementsToModify[i], target, value);
        if(moddedEle.getTypeHash() == NULLTYPE) {
            newElements.push_back(elementsToModify[i]);
        } else {
            newElements.push_back(moddedEle);
            moddedAnyElements = true;
        }
    }
    if(!moddedAnyElements)
        return *new Expression(new NullObject("No substitutions possible"));
    if(sourceType == SUMTYPE) {
        return *new Expression(new Sum(newElements));
    }
    if(sourceType == PRODUCTTYPE) {
        return *new Expression(new Product(newElements));
    }
    return *new Expression(new NullObject("No substitutions possible"));
};

Expression substitute(Expression source, std::vector<std::pair<Expression,Expression>> substitutions) {
    Expression result = *new Expression(source);
    for(size_t i = 0; substitutions.size(); i++) {
        Expression temp = substitute(result, substitutions[i].first, substitutions[i].second);
        if(temp.getTypeHash() != NULLTYPE)
            result = temp;
    }
    return result;
};
