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
Expression reciprocal(Expression self) {
    if(self->getTypeHash() == FRACTYPE) {
        const Frac& selfFrac = dynamic_cast<const Frac&>(*self);
        Expression result = *new Expression(new Frac(selfFrac.denomenator,selfFrac.numerator));
        return result;
    }
    Expression result = *new Expression(new Frac(self));
    return result;
};

Expression complexConjugate(Expression target) {
    Expression result = substitute(target, IMAGUNIT, (-IMAGUNIT));
    if(result.getTypeHash() != NULLTYPE)
        return result;
    return *new Expression(target.get());
}

Expression determinant(Expression target) {
    if(!isSubtypeOf(target, MATRIXTYPE)) {
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
            for (int i = 1; i < n; i++) {
                int subj = 0;
                ExprVector currentRow = *new ExprVector();
                for (int j = 0; j < n; j++) {
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
}

Expression getMatrixMatchingPauliFlavor(Expression target, Expression matrixToMatch) {
    Expression result = *new Expression(new NullObject("could not find PAULIMATRIXTYPE of matching flavor"));
    if(matrixToMatch->getTypeHash() != PAULIMATRIXTYPE) {
        return result;
    }
    const PauliMatrix& matrixObjToMatch = dynamic_cast<const PauliMatrix&>(*matrixToMatch);
    String flavorToFind = matrixObjToMatch.flavor;
    Expression matrixToCheck = getElementOfType(target, PAULIMATRIXTYPE);//target->getFirstInstanceOfType(PAULIMATRIXTYPE);
    bool sign = false;
    if(matrixToCheck.getTypeHash() == SIGNTYPE) {
        sign = true;
        matrixToCheck = -matrixToCheck;
    }
    Expression remainder = *new Expression(target.get());//removeElementAbsolutely(target, matrixToCheck);//target->remove(matrixToCheck);
    while(result.getTypeHash() == NULLTYPE && matrixToCheck.getTypeHash() != NULLTYPE) {
        remainder = removeElementAbsolutely(remainder, matrixToCheck);//remainder.remove(matrixToCheck);
        const PauliMatrix& matrixObjToCheck = dynamic_cast<const PauliMatrix&>(*matrixToCheck);
        if(matrixObjToCheck.flavor == flavorToFind ) {//&& *matrixToCheck != matrixToMatch) {
            if(sign)
                result = *new Expression(new Sign(matrixToCheck));
            else
                result = matrixToCheck;
        }
        matrixToCheck = getElementOfType(remainder, PAULIMATRIXTYPE);//remainder.getFirstInstanceOfType(PAULIMATRIXTYPE);
        if(matrixToCheck.getTypeHash() == SIGNTYPE) {
            sign = true;
            matrixToCheck = -matrixToCheck;
        }
    }
    return result;
}

Expression matMul(Expression left, Expression right) {
    if( isSubtypeOf(right, MATRIXTYPE) && isSubtypeOf(left, MATRIXTYPE)) {
        const Matrix& leftMatrix = dynamic_cast<const Matrix&>(*left);
        const Matrix& rightMatrix = dynamic_cast<const Matrix&>(*right);
        if(leftMatrix.dimensions.second != rightMatrix.dimensions.first) {
            Expression result = *new Expression(new NullObject("mismatch in dimensions for multiplication of MATRIXTYPE"));
            return result;
        }
        std::pair<int, int> newDimensions = {leftMatrix.dimensions.first,rightMatrix.dimensions.second};
        
        ExprMatrix newElements = *new ExprMatrix();
        String newName = "{";
        for(int i = 0; i<newDimensions.first; i++) {
            ExprVector currentRow = *new ExprVector();
            newName+="{";
            if(i!=0)
                newName+=",";
            for(int j = 0; j<newDimensions.second; j++) {
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
    return combineProducts(left, right);
};

Expression transpose(Expression target) {
    if(!isSubtypeOf(target, MATRIXTYPE))
        return *new Expression(target.get());
    const Matrix& matTarget = dynamic_cast<const Matrix&>(*target);
    std::pair<int,int> dimensions = *new std::pair<int,int>();
    dimensions.first = matTarget.dimensions.second;
    dimensions.second = matTarget.dimensions.first;
    ExprMatrix newElements = *new ExprMatrix();
    for(int i = 0; i <dimensions.first; i++) {
        ExprVector currentRow = *new ExprVector();
        for(int j = 0; j<dimensions.second; j++) {
            Expression currElement = matTarget.elements[j][i];
            currentRow.push_back(currElement);
        }
        newElements.push_back(currentRow);
    }
    if(target->getTypeHash() == PAULIMATRIXTYPE) {
        const PauliMatrix& pauliTarget = dynamic_cast<const PauliMatrix&>(*target);
        Expression result = *new Expression(new PauliMatrix(target->print()+"Transpose",0,pauliTarget.flavor,newElements));
        return result;
    }
    Expression result = *new Expression(new Matrix(matTarget.name+"Transpose", newElements));
    return result;
}

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
    if(isSubtypeOf(left, REALTYPE) && isSubtypeOf(right, REALTYPE))
        return left*right;
    if(leftType == MULTYPE) {
        const Mul& lMul = dynamic_cast<const Mul&>(*left);
        ExprVector newMembers = *new ExprVector(lMul.members);
        if(rightType == MULTYPE) {
            const Mul& rMul = dynamic_cast<const Mul&>(*right);
            ExprVector rightMembers = rMul.members;
            for(int i = 0; i<rightMembers.size(); i++) {
                newMembers.push_back(rightMembers[i]);
            }
        } else {
            newMembers.push_back(*new Expression(right));
        }
        if((int)newMembers.size() == 1)
            return newMembers[0];
        Expression result = *new Expression(new Mul(newMembers));
        return result;
    }
    if(rightType == MULTYPE) {
        const Mul& rMul = dynamic_cast<const Mul&>(*right);
        ExprVector newMembers = *new ExprVector();
        newMembers.push_back(*new Expression(left));
        for(int i = 0; i< rMul.members.size(); i++) {
            newMembers.push_back(rMul.members[i]);
        }
        Expression result = *new Expression(new Mul(newMembers));
        return result;
    } else {
        Expression result = *new Expression(new Mul(left,right));
        return result;
    }
};

Expression combineSums(Expression left, Expression right) {
    size_t leftType = left->getTypeHash();
    size_t rightType = right->getTypeHash();
    if(rightType == NULLTYPE)
        right*left;
    if(leftType == NULLTYPE)
        left*right;
    if(isSubtypeOf(left, ADDTYPE)) {
        const Add& lAdd = dynamic_cast<const Add&>(*left);
        ExprVector newMembers(lAdd.members);//copyExprVector(lAdd.members);
        if(isSubtypeOf(right, ADDTYPE)) {
            const Add& rAdd = dynamic_cast<const Add&>(*right);
            ExprVector rightMembers(rAdd.members);//copyExprVector(rAdd.members);
            if(lAdd.members.size() == 1 && rAdd.members.size() == 1) {
                Expression tempResult = lAdd.members[0]+rAdd.members[0];
                return tempResult;
            }
            for(int i = 0; i<rightMembers.size(); i++) {
                //Expression nextMember(rightMembers[i]);
                if(rightMembers[i] == ZERO)
                    continue;
                newMembers.push_back(rightMembers[i]);
            }
        } else {
            if(lAdd.members.size() == 1) {
                return lAdd.members[0]+right;
            }
            newMembers.push_back(*new Expression(right));
        }
        Expression result = *new Expression(new Add(newMembers));//cancelTerms(new Add(newMembers,newSigns));
        return result;
    }
    if(isSubtypeOf(right, ADDTYPE)) {
        const Add& rAdd = dynamic_cast<const Add&>(*right);
        ExprVector newMembers = *new ExprVector();
        newMembers.push_back(*new Expression(left));
        for(int i = 0; i<rAdd.members.size(); i++) {
            newMembers.push_back(rAdd.members[i]);
        }
        Expression result = *new Expression(new Add(newMembers));
        return result;
    } else {
        Expression result = *new Expression(new Add(left,right));
        return result;
    }
};

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

Expression cancelTerms(Expression target) {
    if(target->getTypeHash() != ADDTYPE)
        return *new Expression(target.get());
    const Add& addObj = dynamic_cast<const Add&>(*target);
    ExprVector members = *new ExprVector(addObj.members);
    ExprVector newMembers = *new ExprVector();
    std::vector<int> indicesAccountedFor = *new std::vector<int>();
    bool didAnyCancelation = false;
    for(int i = 0; i<members.size(); i++) {
        if(intVectorContains(indicesAccountedFor, i))
            continue;
        indicesAccountedFor.push_back(i);
        Expression total = members[i];
        for(int j = i+1; j<members.size(); j++) {
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
}

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
            total = distribute(total, second);
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

Expression simplifyPauliMatrices(Expression target) {
    bool isMul = target->getTypeHash() == MULTYPE;
    bool isAdd = target->getTypeHash() == ADDTYPE;
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
    if(total.getTypeHash() == MULTYPE) {
        const Mul& mulObj = dynamic_cast<const Mul&>(*total);
        if(mulObj.members.size() == 0)
            return ZERO;
        if(mulObj.members.size() == 1)
            return mulObj.members[0];
    }
    if(total.getTypeHash() == ADDTYPE) {
        const Add& addObj = dynamic_cast<const Add&>(*total);
        if(addObj.members.size() == 0)
            return ZERO;
        if(addObj.members.size() == 1) {
            return addObj.members[0];
        }
    }
    return total;
}

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
        Expression newNumerator = simplify(fracTarget.numerator);
        Expression newDenomenator = simplify(fracTarget.denomenator);
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
        ExprVector members = mulObj.members;
        if(members.size() == 1)
            return members[0];
        ExprVector newMembers = *new ExprVector();
        for(int i = 0; i<members.size(); i++) {
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
        ExprVector members = addObj.members;
        if(members.size() == 1)
            return *new Expression(target.get());
        ExprVector newMembers = *new ExprVector();
        for(int i = 0; i<members.size(); i++) {
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
        if(mulObj.members.size() == 0)
            return ZERO;
        if(mulObj.members.size() == 1)
            return mulObj.members[0];
    }
    if(result.getTypeHash() == ADDTYPE) {
        const Add& addObj = dynamic_cast<const Add&>(*result);
        if(addObj.members.size() == 0)
            return ZERO;
        if(addObj.members.size() == 1) {
            return addObj.members[0];
        }
    }
    return result;
}

ExprVector cancelCommonFactors(ExprVector targets) {
    ExprVector results = targets;
    ExprVector commonFactors = getCommonFactors(results);
    for(int i = 0; i<commonFactors.size(); i++) {
        for(int j = 0; j<results.size(); j++) {
            results[j] = factor(results[j]);
            //bool isContainer = isSubtypeOf(results[j], OPERATORTYPE);
            //if(isContainer)
                results[j] = removeElementMultiplicatively(results[j], commonFactors[i]);
            //else
            //    results[j] = results[j]/commonFactors[i];
            results[j] = simplify(results[j]);
            if(results[j].getTypeHash() == NULLTYPE) {
                const NullObject& nullObj = dynamic_cast<const NullObject&>(*results[j]);
                if(nullObj.origin == "this is what happens when you remove something from itself!")
                    results[j] = ONE;
                else
                    results[j] = targets[j];
            }
        }
    }
    return results;
}

ExprVector cancelSimpleCommonFactors(ExprVector targets) {
    ExprVector results = targets;
    ExprVector commonFactors = getSimpleCommonFactors(results);
    for(int i = 0; i<commonFactors.size(); i++) {
        for(int j = 0; j<results.size(); j++) {
            results[j] = factor(results[j]);
            results[j] = removeElementMultiplicatively(results[j], commonFactors[i]);
            if(results[j].getTypeHash() == NULLTYPE)
                results[j] = targets[j];
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
            return distribute(signLeft.member, signRight.member);
        }
        return -distribute(signLeft.member, right);
    }
    if(rightType == SIGNTYPE) {
        const Sign& signRight = dynamic_cast<const Sign&>(*right);
        return -distribute(left, signRight.member);
    }
    if(leftType == ADDTYPE) {
        const Add& leftObj = dynamic_cast<const Add&>(*left);
        if(rightType == ADDTYPE) {
            const Add& rightObj = dynamic_cast<const Add&>(*right);
            ExprVector newMembers = *new ExprVector();
            for(int i=0; i<leftObj.members.size(); i++) {
                for(int j=0; j<rightObj.members.size(); j++) {
                    Expression result = leftObj.members[i] * rightObj.members[j];
                    newMembers.push_back(result);
                }
            }
            if(newMembers.size() == 1)
                return newMembers[0];
            Expression result = *new Expression(new Add(newMembers));
            return result;
        }//end ltype == ADD, rtype == ADD
        if(rightType == FRACTYPE) {
            const Frac& rightObj = dynamic_cast<const Frac&>(*right);
            Expression newNumerator = distribute(left, rightObj.numerator);
            Expression newDenomenator = rightObj.denomenator;
            ExprVector targets = generateExprVector({newNumerator,newDenomenator});
            ExprVector results = cancelCommonFactors(targets);
            Expression finalResult = *new Expression(new Frac(results[0],results[1]));
            return finalResult;
            
        }//end ltype == ADD, rtype == FRAC
        if((int)leftObj.members.size() == 1) {
            Expression tempResult = leftObj.members[0]*right;
            return tempResult;
        }
        ExprVector newMembers = *new ExprVector();
        SignVector newSigns = *new SignVector();
        for(int i=0; i<leftObj.members.size(); i++) {
            Expression result = distribute(leftObj.members[i], right);
            newMembers.push_back(result);
        }
        Expression result = *new Expression(new Add(newMembers));
        return result;
        //end ltype == ADD, rtype == MUL or similar
    }//end ltype == ADD
    
    if(leftType == FRACTYPE) {
        const Frac& leftObj = dynamic_cast<const Frac&>(*left);
        if(rightType == FRACTYPE) {
            const Frac& rightObj = dynamic_cast<const Frac&>(*right);
            Expression newNumerator = distribute(leftObj.numerator, rightObj.numerator);
            Expression newDenomenator = distribute(leftObj.denomenator, rightObj.denomenator);
            ExprVector targets = generateExprVector({newNumerator,newDenomenator});
            ExprVector results = cancelCommonFactors(targets);
            Expression finalResult = *new Expression(new Frac(results[0],results[1]));
            return finalResult;
        }//end ltype = FRAC, rtype == FRAC
        Expression newNumerator = distribute(leftObj.numerator, right);
        Expression newDenomenator = leftObj.denomenator;
        ExprVector targets = generateExprVector({newNumerator,newDenomenator});
        ExprVector results = cancelCommonFactors(targets);
        Expression finalResult = *new Expression(new Frac(results[0],results[1]));
        return finalResult;
        //end ltype == FRAC, rtype = MUL or similar
    }//end ltype = FRAC
    
    if(rightType == ADDTYPE) {
        const Add& rightObj = dynamic_cast<const Add&>(*right);
        ExprVector newMembers = *new ExprVector();
        for(int i=0; i<rightObj.members.size(); i++) {
            Expression result = left*rightObj.members[i];
            //result = simplify(*result);//combineProducts(left, rightObj.members[i]);
            newMembers.push_back(result);
        }
        Expression result = *new Expression(new Add(newMembers));
        return result;
    }
    if(rightType == FRACTYPE) {
        const Frac& rightObj = dynamic_cast<const Frac&>(*right);
        Expression newNumerator = distribute(left, rightObj.numerator);
        Expression newDenomenator = rightObj.denomenator;
        ExprVector targets = generateExprVector({newNumerator,newDenomenator});
        ExprVector results = cancelCommonFactors(targets);
        Expression finalResult = *new Expression(new Frac(results[0],results[1]));
        return finalResult;
    }
    if(leftType == EXPTYPE && rightType == EXPTYPE) {
        const Exp& leftExp = dynamic_cast<const Exp&>(*left);
        const Exp& rightExp = dynamic_cast<const Exp&>(*right);
        if(leftExp.base == rightExp.base) {
            Expression newExponent = leftExp.exponent+rightExp.exponent;
            Expression result = *new Expression(new Exp(leftExp.exponent,newExponent));
            return result;
        }
    }
    Expression mul = combineProducts(left, right);
    return mul;
}


Expression factor(Expression factee) {
    ExprVector factors = getFactors(factee);
    if(factors.size()>1) {
        Expression result = *new Expression(new Mul(factors));
        return result;
    }
    return *new Expression(factee);
}

ExprVector getFactorsOfInt(Expression factee) {
    if(!isSubtypeOf(factee, REALTYPE))
        return {*new Expression(factee)};
    const Real& realObj = dynamic_cast<const Real&>(*factee);
    if(!(realObj.value == trunc(realObj.value)))
        return {*new Expression(factee)};
    int val = realObj.value;
    ExprVector factors = *new ExprVector();
    for(int i = 2; i <= val; i++) {
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

ExprVector getFactors(Expression factee) {
    size_t type = factee->getTypeHash();
    if(type == MULTYPE) {
        const Mul& mulObj = dynamic_cast<const Mul&>(*factee);
        ExprVector factors = *new ExprVector();
        for(int i = 0; i< mulObj.members.size(); i++) {
            ExprVector tempFactors = getFactors(mulObj.members[i]);
            for(int j = 0; j< tempFactors.size(); j++)
                //if(!containsElement(factors, tempFactors[j]))
                    factors.push_back(tempFactors[j]);
        }
        return factors;
    }
    if(type == ADDTYPE) {
        const Add& addObj = dynamic_cast<const Add&>(*factee);
        ExprVector commonFactors = getCommonFactors(addObj.members);
        if(commonFactors.size()>0)
            return commonFactors;
    }
    if(type == SIGNTYPE) {
        const Sign& signObj = dynamic_cast<const Sign&>(*factee);
        ExprVector factors = getFactors(signObj.member);
        factors.push_back(MINUSONE);
        if(factors.size()>0)
            return factors;
    }
    if(type == FRACTYPE) {
        const Frac& fracObj = dynamic_cast<const Frac&>(*factee);
        Expression result = reciprocal(fracObj.denomenator);
        ExprVector targets = generateExprVector({result});
        ExprVector factors = getFactors(fracObj.numerator);
        return combineExprVectors( factors, targets );
    }
    if(type == EXPTYPE) {
        const Exp& expObj = dynamic_cast<const Exp&>(*factee);
        Expression exponent = expObj.exponent;
        size_t exponentType = exponent.getTypeHash();
        if(exponentType == ADDTYPE) {
            ExprVector factors = *new ExprVector();
            const Add& addObj = dynamic_cast<const Add&>(*exponent);
            for(int i = 0; i<addObj.members.size(); i++) {
                Expression newExpression = *new Expression(new Exp(expObj.base,addObj.members[i]));
                factors.push_back(newExpression);
            }
        }
    }
    if(isSubtypeOf(factee, REALTYPE))
        return getFactorsOfInt(factee);
    return generateExprVector({*new Expression(factee)});
}

ExprVector getSimpleCommonFactors(ExprVector terms) {
    ExprVector commonFactors = *new ExprVector();
    ExprMatrix ithTermFactors = *new ExprMatrix();
    ExprVector firstTermFactors = getFactors(terms[0]);
    if(terms.size()<2)
        return firstTermFactors;
    std::vector<std::vector<int>> indicesAlreadyCounted = *new std::vector<std::vector<int>>();
    for(int i = 1; i<terms.size(); i++) {
        ExprVector currTermFactors = getFactors(terms[i]);
        ithTermFactors.push_back(currTermFactors);
        indicesAlreadyCounted.push_back(*new std::vector<int>());
    }
    
    for(int i = 0; i<firstTermFactors.size(); i++) {
        bool isCommonFactor = true;
        for(int j=0; j<(ithTermFactors).size(); j++) {
            bool containsFactor = false;
            for(int k = 0; k<ithTermFactors[j].size(); k++) {
                if(intVectorContains(indicesAlreadyCounted[j], k))
                    continue;
                containsFactor |= firstTermFactors[i] == (ithTermFactors[j])[k];
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
        if(isCommonFactor && !isSubtypeOf(firstTermFactors[i], MATRIXTYPE) && firstTermFactors[i].getTypeHash() != SYMBOLTYPE)
            commonFactors.push_back(firstTermFactors[i]);
    }
    return commonFactors;
}

ExprVector getCommonFactors(ExprVector terms) {
    ExprVector commonFactors = *new ExprVector();
    ExprMatrix ithTermFactors = *new ExprMatrix();
    ExprVector firstTermFactors = getFactors(terms[0]);
    if(terms.size()<2)
        return firstTermFactors;
    std::vector<std::vector<int>> indicesAlreadyCounted = *new std::vector<std::vector<int>>();
    for(int i = 1; i<terms.size(); i++) {
        ExprVector currTermFactors = getFactors(terms[i]);
        ithTermFactors.push_back(currTermFactors);
        indicesAlreadyCounted.push_back(*new std::vector<int>());
    }
    
    for(int i = 0; i<firstTermFactors.size(); i++) {
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
    return commonFactors;
}

//DOES NOT RESPECT COMMUTATION PROPERTIES... probably
Expression combineTermsDifferingByCoefficientsAdditively(Expression left, Expression right) {
    if(left->getTypeHash() == ZEROTYPE)
        return *new Expression(right.get());
    if(right->getTypeHash() == ZEROTYPE)
        return *new Expression(left.get());
    if(isSubtypeOf(left, REALTYPE) && isSubtypeOf(right, REALTYPE))
        return left+right;
    if(left->getTypeHash() == SIGNTYPE) {
        const Sign& signObj = dynamic_cast<const Sign&>(*left);
        if(signObj.member == right)
            return ZERO;
    }
    else if(right->getTypeHash() == SIGNTYPE) {
        const Sign& signObj = dynamic_cast<const Sign&>(*right);
        if(signObj.member == left)
            return ZERO;
    }
    if(left == right)
        return simplify(2*left);
    ExprVector targets = generateExprVector({*new Expression(left),*new Expression(right)});
    ExprVector commonFactors = getCommonFactors(targets);//getCommonFactors(targets);
    if(commonFactors.size() == 0)
        return *new Expression(new NullObject("no common factors"));
    targets = generateExprVector({*new Expression(left),*new Expression(right)});
    ExprVector results = cancelCommonFactors(targets);//cancelCommonFactors(targets);
    Expression inCommon = ONE;
    for(int i = 0; i< commonFactors.size(); i++) {
        inCommon = inCommon * commonFactors[i];
    }
    Expression combinedCoeffs = combineTermsDifferingByCoefficientsAdditively(results[0], results[1]);
    if(combinedCoeffs.getTypeHash() == NULLTYPE)
        return left+right;
    Expression temp = combinedCoeffs*inCommon;
    return temp;
}

Expression performActions(Expression target) {
    size_t sourceType = target->getTypeHash();
    if(isSubtypeOf(target, MATRIXTYPE)) {
        const Matrix& matObj = dynamic_cast<const Matrix&>(*target);
        ExprMatrix elements = *new ExprMatrix(matObj.elements);
        for(int i = 0; i<matObj.dimensions.first; i++) {
            for (int j = 0; j<matObj.dimensions.second; j++) {
                if(elements[i][j] == target) {
                    elements[i][j] = performActions(elements[i][j]);
                }
            }
        }
        if(sourceType == PAULIMATRIXTYPE) {
            const PauliMatrix& pauliSource = dynamic_cast<const PauliMatrix&>(*target);
            Expression newMat = declarePauliMatrix(new PauliMatrix(printExprMatrix(elements),0,pauliSource.flavor,elements));
            return newMat;
        }
        Expression newMat = declareMatrix(matObj.name+"with replacement", elements);
        return newMat;
    }
    if(sourceType == FRACTYPE) {
        const Frac& fracObj = dynamic_cast<const Frac&>(*target);
        Expression newNum = performActions(fracObj.numerator);
        Expression newDenom = performActions(fracObj.denomenator);
        return *new Expression(new Frac(newNum,newDenom));
    }
    if(sourceType == EXPTYPE) {
        const Exp& expObj = dynamic_cast<const Exp&>(*target);
        Expression newBase = performActions(expObj.base);
        Expression newExponent = performActions(expObj.exponent);
        return *new Expression(new Exp(newBase,newExponent));
    }
    if(sourceType == SIGNTYPE) {
        const Sign& signObj = dynamic_cast<const Sign&>(*target);
        return -performActions(signObj.member);
    }
    if(isSubtypeOf(target, FUNCTYPE)) {
        const Func& funcObj = dynamic_cast<const Func&>(*target);
        return funcObj.act();
    }
    ExprVector elementsToModify;
    ExprVector newElements = *new ExprVector();
    if(sourceType == ADDTYPE) {
        const Add& addObj = dynamic_cast<const Add&>(*target);
        elementsToModify = addObj.members;
    }
    if(sourceType == MULTYPE) {
        const Mul& mulObj = dynamic_cast<const Mul&>(*target);
        elementsToModify = mulObj.members;
    }
    for(int i = 0; i< elementsToModify.size(); i++) {
        Expression moddedEle = performActions(elementsToModify[i]);
        newElements.push_back(moddedEle);
    }
    if(sourceType == ADDTYPE) {
        return *new Expression(new Add(newElements));
    }
    if(sourceType == MULTYPE) {
        return *new Expression(new Mul(newElements));
    }
    return target;
}

Expression performActionsOn(Expression target, Expression var) {
    size_t sourceType = target->getTypeHash();
    if(isSubtypeOf(target, MATRIXTYPE)) {
        const Matrix& matObj = dynamic_cast<const Matrix&>(*target);
        ExprMatrix elements = *new ExprMatrix(matObj.elements);
        for(int i = 0; i<matObj.dimensions.first; i++) {
            for (int j = 0; j<matObj.dimensions.second; j++) {
                if(elements[i][j] == target) {
                    elements[i][j] = performActionsOn(elements[i][j],var);
                }
            }
        }
        if(sourceType == PAULIMATRIXTYPE) {
            const PauliMatrix& pauliSource = dynamic_cast<const PauliMatrix&>(*target);
            Expression newMat = declarePauliMatrix(new PauliMatrix(printExprMatrix(elements),0,pauliSource.flavor,elements));
            return newMat;
        }
        Expression newMat = declareMatrix(matObj.name+"with replacement", elements);
        return newMat;
    }
    if(sourceType == FRACTYPE) {
        const Frac& fracObj = dynamic_cast<const Frac&>(*target);
        Expression newNum = performActionsOn(fracObj.numerator, var);
        Expression newDenom = performActionsOn(fracObj.denomenator, var);
        return *new Expression(new Frac(newNum,newDenom));
    }
    if(sourceType == EXPTYPE) {
        const Exp& expObj = dynamic_cast<const Exp&>(*target);
        Expression newBase = performActionsOn(expObj.base, var);
        Expression newExponent = performActionsOn(expObj.exponent, var);
        return *new Expression(new Exp(newBase,newExponent));
    }
    if(sourceType == SIGNTYPE) {
        const Sign& signObj = dynamic_cast<const Sign&>(*target);
        return -performActionsOn(signObj.member, var);
    }
    if(isSubtypeOf(target, FUNCTYPE)) {
        const Func& funcObj = dynamic_cast<const Func&>(*target);
        return funcObj.resultOfActingOn(var);
    }
    ExprVector elementsToModify;
    ExprVector newElements = *new ExprVector();
    if(sourceType == ADDTYPE) {
        const Add& addObj = dynamic_cast<const Add&>(*target);
        elementsToModify = addObj.members;
    }
    if(sourceType == MULTYPE) {
        const Mul& mulObj = dynamic_cast<const Mul&>(*target);
        elementsToModify = mulObj.members;
    }
    for(int i = 0; i< elementsToModify.size(); i++) {
        Expression moddedEle = performActionsOn(elementsToModify[i], var);
        newElements.push_back(moddedEle);
    }
    if(sourceType == ADDTYPE) {
        return *new Expression(new Add(newElements));
    }
    if(sourceType == MULTYPE) {
        return *new Expression(new Mul(newElements));
    }
    return target;
}

Expression insertAsVariable(Expression target, Expression var) {
    size_t sourceType = target->getTypeHash();
    if(isSubtypeOf(target, MATRIXTYPE)) {
        const Matrix& matObj = dynamic_cast<const Matrix&>(*target);
        ExprMatrix elements = *new ExprMatrix(matObj.elements);
        for(int i = 0; i<matObj.dimensions.first; i++) {
            for (int j = 0; j<matObj.dimensions.second; j++) {
                if(elements[i][j] == target) {
                    elements[i][j] = insertAsVariable(elements[i][j],var);
                }
            }
        }
        if(sourceType == PAULIMATRIXTYPE) {
            const PauliMatrix& pauliSource = dynamic_cast<const PauliMatrix&>(*target);
            Expression newMat = declarePauliMatrix(new PauliMatrix(printExprMatrix(elements),0,pauliSource.flavor,elements));
            return newMat;
        }
        Expression newMat = declareMatrix(matObj.name+"with replacement", elements);
        return newMat;
    }
    if(sourceType == FRACTYPE) {
        const Frac& fracObj = dynamic_cast<const Frac&>(*target);
        Expression newNum = insertAsVariable(fracObj.numerator, var);
        Expression newDenom = insertAsVariable(fracObj.denomenator, var);
        return *new Expression(new Frac(newNum,newDenom));
    }
    if(sourceType == EXPTYPE) {
        const Exp& expObj = dynamic_cast<const Exp&>(*target);
        Expression newBase = insertAsVariable(expObj.base, var);
        Expression newExponent = insertAsVariable(expObj.exponent, var);
        return *new Expression(new Exp(newBase,newExponent));
    }
    if(sourceType == SIGNTYPE) {
        const Sign& signObj = dynamic_cast<const Sign&>(*target);
        return -insertAsVariable(signObj.member, var);
    }
    if(isSubtypeOf(target, FUNCTYPE)) {
        const Func& funcObj = dynamic_cast<const Func&>(*target);
        return funcObj.actingOn(var);
    }
    ExprVector elementsToModify;
    ExprVector newElements = *new ExprVector();
    if(sourceType == ADDTYPE) {
        const Add& addObj = dynamic_cast<const Add&>(*target);
        elementsToModify = addObj.members;
    }
    if(sourceType == MULTYPE) {
        const Mul& mulObj = dynamic_cast<const Mul&>(*target);
        elementsToModify = mulObj.members;
    }
    for(int i = 0; i< elementsToModify.size(); i++) {
        Expression moddedEle = insertAsVariable(elementsToModify[i], var);
        newElements.push_back(moddedEle);
    }
    if(sourceType == ADDTYPE) {
        return *new Expression(new Add(newElements));
    }
    if(sourceType == MULTYPE) {
        return *new Expression(new Mul(newElements));
    }
    return target;
}

Expression substitute(Expression source, Expression target, Expression value) {
    Expression valueExpr = *new Expression(value);
    if(source == target)
        return valueExpr;
    size_t sourceType = source->getTypeHash();
    if(isSubtypeOf(source, MATRIXTYPE)) {
        const Matrix& matObj = dynamic_cast<const Matrix&>(*source);
        ExprMatrix elements = *new ExprMatrix(matObj.elements);
        bool foundTarget = false;
        for(int i = 0; i<matObj.dimensions.first; i++) {
            for (int j = 0; j<matObj.dimensions.second; j++) {
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
            Expression newMat = declarePauliMatrix(new PauliMatrix(printExprMatrix(elements),0,pauliSource.flavor,elements));
            return newMat;
        }
        Expression newMat = declareMatrix(matObj.name+"with replacement", elements);
        return newMat;
    }
    if(sourceType == FRACTYPE) {
        const Frac& fracObj = dynamic_cast<const Frac&>(*source);
        Expression newNum;
        Expression newDenom;
        bool changedNum = false;
        bool changedDenom = false;
        Expression testResult = substitute(fracObj.numerator, target, value);
        if(testResult.getTypeHash() == NULLTYPE)
            newNum = fracObj.numerator;
        else {
            newNum = testResult;
            changedNum = true;
        }
        testResult = substitute(fracObj.denomenator, target, value);
        if(testResult.getTypeHash() == NULLTYPE)
            newDenom = fracObj.denomenator;
        else {
            newDenom = testResult;
            changedDenom = true;
        }
        if(changedNum || changedDenom) {
            return *new Expression(new Frac(newNum,newDenom));
        }
        return *new Expression(new NullObject("No substitutions possible"));
    }
    if(sourceType == EXPTYPE) {
        const Exp& expObj = dynamic_cast<const Exp&>(*source);
        Expression newBase;
        Expression newExponent;
        bool changedBase = false;
        bool changedExponent = false;
        Expression testResult = substitute(expObj.base, target, value);
        if(testResult.getTypeHash() == NULLTYPE)
            newBase = expObj.base;
        else {
            newBase = testResult;
            changedBase = true;
        }
        testResult = substitute(expObj.exponent, target, value);
        if(testResult.getTypeHash() == NULLTYPE)
            newExponent = expObj.exponent;
        else {
            newExponent = testResult;
            changedExponent = true;
        }
        if(changedBase || changedExponent) {
            return *new Expression(new Exp(newBase,newExponent));
        }
        return *new Expression(new NullObject("No substitutions possible"));
    }
    if(sourceType == SIGNTYPE) {
        const Sign& signObj = dynamic_cast<const Sign&>(*source);
        Expression testResult = substitute(signObj.member, target, value);
        if(testResult.getTypeHash() != NULLTYPE)
            return -testResult;
        return *new Expression(new NullObject("No substitutions possible"));
    }
    ExprVector elementsToModify;
    ExprVector newElements = *new ExprVector();
    if(sourceType == ADDTYPE) {
        const Add& addObj = dynamic_cast<const Add&>(*source);
        elementsToModify = addObj.members;
    }
    if(sourceType == MULTYPE) {
        const Mul& mulObj = dynamic_cast<const Mul&>(*source);
        elementsToModify = mulObj.members;
    }
    
    bool moddedAnyElements = false;
    for(int i = 0; i< elementsToModify.size(); i++) {
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
    if(sourceType == ADDTYPE) {
        return *new Expression(new Add(newElements));
    }
    if(sourceType == MULTYPE) {
        return *new Expression(new Mul(newElements));
    }
    return *new Expression(new NullObject("No substitutions possible"));
};

Expression substitute(Expression source, std::vector<std::pair<Expression,Expression>> substitutions) {
    Expression result = *new Expression(source);
    for(int i = 0; substitutions.size(); i++) {
        Expression temp = substitute(result, substitutions[i].first, substitutions[i].second);
        if(temp.getTypeHash() != NULLTYPE)
            result = temp;
    }
    return result;
};
