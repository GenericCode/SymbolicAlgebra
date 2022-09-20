//
//  ExpressionHelpers.cpp
//  SymbolicSummation
//
//  Created by Robert Stahulak on 2/1/22.
//

#include "ExpressionHelpers.hpp"
#include "Symbol.hpp"
#include "Real.hpp"
#include "Container.hpp"
#include "PauliMatrices.hpp"
#include "AbstractHelpers.hpp"

Expression reciprocal(ExpressionObject* self) {
    if(self->getTypeHash() == FRACTYPE) {
        Frac& selfFrac = dynamic_cast<Frac&>(*self);
        Expression result = *new Expression(new Frac(selfFrac.denomenator.get(),selfFrac.numerator.get()));
        return result;
    }
    Expression result = *new Expression(new Frac(self));
    return result;
};

Expression complexConjugate(ExpressionObject* target) {
    Expression result = substitute(target, IMAGUNIT.get(), (-IMAGUNIT).get());
    if(result.getTypeHash() != NULLTYPE)
        return result;
    return *new Expression(target);
}

Expression determinant(ExpressionObject* target) {
    if(!isSubtypeOf(target, MATRIXTYPE)) {
        return *new Expression(target);
    }
    Matrix& matTarget = dynamic_cast<Matrix&>(*target);
    ExprMatrix currMatrix = matTarget.elements;
    if(matTarget.dimensions.first != matTarget.dimensions.second)
        return *new Expression(target);
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
            ExpressionObject* submatrixObj = new Matrix("submatrix",*new ExprMatrix(submatrix));
            det = det + ((float)pow(-1, x) * currMatrix[0][x] * determinant(submatrixObj));
        }
    }
    return det;
}

Expression getMatrixMatchingPauliFlavor(ExpressionObject* target, ExpressionObject* matrixToMatch) {
    Expression result = *new Expression(new NullObject("could not find PAULIMATRIXTYPE of matching flavor"));
    if(matrixToMatch->getTypeHash() != PAULIMATRIXTYPE) {
        return result;
    }
    PauliMatrix& matrixObjToMatch = dynamic_cast<PauliMatrix&>(*matrixToMatch);
    std::string flavorToFind = matrixObjToMatch.flavor;
    Expression matrixToCheck = getElementOfType(target, PAULIMATRIXTYPE);//target->getFirstInstanceOfType(PAULIMATRIXTYPE);
    bool sign = false;
    if(matrixToCheck.getTypeHash() == SIGNTYPE) {
        sign = true;
        matrixToCheck = -matrixToCheck;
    }
    Expression remainder = *new Expression(target);//removeElementAbsolutely(target, matrixToCheck);//target->remove(matrixToCheck);
    while(result.getTypeHash() == NULLTYPE && matrixToCheck.getTypeHash() != NULLTYPE) {
        remainder = removeElementAbsolutely(remainder.get(), matrixToCheck.get());//remainder.remove(matrixToCheck);
        PauliMatrix& matrixObjToCheck = dynamic_cast<PauliMatrix&>(*matrixToCheck);
        if(matrixObjToCheck.flavor == flavorToFind ) {//&& *matrixToCheck != matrixToMatch) {
            if(sign)
                result = *new Expression(new Sign(matrixToCheck.get()));
            else
                result = matrixToCheck;
        }
        matrixToCheck = getElementOfType(remainder.get(), PAULIMATRIXTYPE);//remainder.getFirstInstanceOfType(PAULIMATRIXTYPE);
        if(matrixToCheck.getTypeHash() == SIGNTYPE) {
            sign = true;
            matrixToCheck = -matrixToCheck;
        }
    }
    return result;
}

Expression matMul(ExpressionObject* left, ExpressionObject* right) {
    if( isSubtypeOf(right, MATRIXTYPE) && isSubtypeOf(left, MATRIXTYPE)) {
        Matrix& leftMatrix = dynamic_cast<Matrix&>(*left);
        Matrix& rightMatrix = dynamic_cast<Matrix&>(*right);
        if(leftMatrix.dimensions.second != rightMatrix.dimensions.first) {
            Expression result = *new Expression(new NullObject("mismatch in dimensions for multiplication of MATRIXTYPE"));
            return result;
        }
        std::pair<int, int> newDimensions = {leftMatrix.dimensions.first,rightMatrix.dimensions.second};
        
        ExprMatrix newElements = *new ExprMatrix();
        std::string newName = "{";
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
        ExpressionObject* result = new Matrix(newName,*new ExprMatrix(newElements));
        return declareSymbol(newName,result);
    }
    return combineProducts(left, right);
};

Expression transpose(ExpressionObject* target) {
    if(!isSubtypeOf(target, MATRIXTYPE))
        return *new Expression(target);
    Matrix& matTarget = dynamic_cast<Matrix&>(*target);
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
        PauliMatrix& pauliTarget = dynamic_cast<PauliMatrix&>(*target);
        Expression result = *new Expression(new PauliMatrix(target->print()+"Transpose",0,pauliTarget.flavor,newElements));
        return result;
    }
    Expression result = *new Expression(new Matrix(matTarget.name+"Transpose", newElements));
    return result;
}

Expression combineProducts(ExpressionObject* left, ExpressionObject* right) {
    size_t leftType = left->getTypeHash();
    size_t rightType = right->getTypeHash();
    if(rightType == NULLTYPE)
        *right**left;
    if(leftType == NULLTYPE)
        *left**right;
    if(leftType == ZEROTYPE || rightType == ZEROTYPE)
        return ZERO;
    if(leftType == ONETYPE)
        return *new Expression(right);
    if(rightType == ONETYPE)
        return *new Expression(left);
    if(isSubtypeOf(left, REALTYPE) && isSubtypeOf(right, REALTYPE))
        return *left**right;
    if(leftType == MULTYPE) {
        Mul& lMul = dynamic_cast<Mul&>(*left);
        ExprVector newMembers = *new ExprVector(lMul.members);
        if(rightType == MULTYPE) {
            Mul& rMul = dynamic_cast<Mul&>(*right);
            ExprVector rightMembers = rMul.members;
            for(int i = 0; i<rightMembers.size(); i++) {
                /*bool commutes = true;
                for(int j = 0; j<newMembers.size(); j++) {
                    commutes = commutesWith(newMembers[j], rightMembers[i]);
                    if(!commutes) {
                        Expression temp = newMembers[j];
                        newMembers[j] = temp*rightMembers[i];
                        break;
                    }
                }
                if(commutes)*/
                newMembers.push_back(rightMembers[i]);
            }
        } else {
            /*bool commutes = true;
            for(int i = 0; i<newMembers.size(); i++) {
                commutes = commutesWith(newMembers[i],right);
                if(!commutes) {
                    Expression temp = newMembers[i];
                    newMembers[i] = temp*right;
                    break;
                }
            }
            if(commutes)*/
            newMembers.push_back(*new Expression(right));
        }
        if((int)newMembers.size() == 1)
            return newMembers[0];
        Expression result = *new Expression(new Mul(newMembers));
        return result;
    }
    if(rightType == MULTYPE) {
        Mul& rMul = dynamic_cast<Mul&>(*right);
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

Expression combineSums(ExpressionObject* left, ExpressionObject* right) {
    size_t leftType = left->getTypeHash();
    size_t rightType = right->getTypeHash();
    if(rightType == NULLTYPE)
        *right**left;
    if(leftType == NULLTYPE)
        *left**right;
    if(isSubtypeOf(left, ADDTYPE)) {
        Add& lAdd = dynamic_cast<Add&>(*left);
        ExprVector newMembers(lAdd.members);//copyExprVector(lAdd.members);
        if(isSubtypeOf(right, ADDTYPE)) {
            Add& rAdd = dynamic_cast<Add&>(*right);
            ExprVector rightMembers(rAdd.members);//copyExprVector(rAdd.members);
            if(lAdd.members.size() == 1 && rAdd.members.size() == 1) {
                Expression tempResult = lAdd.members[0]+rAdd.members[0];
                return tempResult;
            }
            for(int i = 0; i<rightMembers.size(); i++) {
                //Expression nextMember(rightMembers[i].get());
                if(rightMembers[i] == ZERO)
                    continue;
                newMembers.push_back(rightMembers[i]);
            }
        } else {
            if(lAdd.members.size() == 1) {
                return *lAdd.members[0]+*right;
            }
            newMembers.push_back(*new Expression(right));
        }
        Expression result = *new Expression(new Add(newMembers));//cancelTerms(new Add(newMembers,newSigns));
        return result;
    }
    if(isSubtypeOf(right, ADDTYPE)) {
        Add& rAdd = dynamic_cast<Add&>(*right);
        //ExprVector newMembers = rAdd.members;
        //SignVector newSigns = rAdd.memberSigns;
        //int positionOfLeftInRight = positionOfElement(newMembers, left);
        /*if(positionOfLeftInRight >= 0) {
            newMembers.erase(newMembers.begin()+positionOfLeftInRight);
            newMembers.insert(newMembers.begin()+positionOfLeftInRight, 2*left);
            newSigns.erase(newSigns.begin()+positionOfLeftInRight);
            newSigns.insert(newSigns.begin()+positionOfLeftInRight, false);
        } else {*/
        /*if(rAdd.members.size() == 1) {
            if(rAdd.memberSigns[0]) {
                Expression result = -left+rAdd.members[0];
                result = -result;
                return result;
            }
            return *new Expression(left)+rAdd.members[0];
        }*/
        ExprVector newMembers = *new ExprVector();
        newMembers.push_back(*new Expression(left));
        for(int i = 0; i<rAdd.members.size(); i++) {
            newMembers.push_back(rAdd.members[i]);
        }
        //newMembers.insert(newMembers.begin(), left);
        //newSigns.insert(newSigns.begin(), false);
        //}
        Expression result = *new Expression(new Add(newMembers));//cancelTerms(new Add(newMembers,newSigns));
        //Expression finalResult = cancelTerms(result);
        return result;
    } else {
        //if(!isSubclassOf(left, OPERATORTYPE) && right == left)
        //    return *new Expression(left)+right;
        Expression result = *new Expression(new Add(left,right));
        return result;
    }
};

bool commutesWith(ExpressionObject* left, ExpressionObject* right) {
    size_t ltype = left->getTypeHash();
    size_t rtype = right->getTypeHash();
    
    //check matrices
    if(ltype == MATRIXTYPE) {
        if(rtype == MATRIXTYPE)
            return false;
        Expression matTarget = getElementOfType(right, MATRIXTYPE);//right->getFirstInstanceOfType(MATRIXTYPE);
        return matTarget.getTypeHash() == NULLTYPE;
    }
    if(rtype == MATRIXTYPE) {
        Expression matTarget = getElementOfType(left, MATRIXTYPE);//left->getFirstInstanceOfType(MATRIXTYPE);
        return matTarget.getTypeHash() == NULLTYPE;
    }
    
    //check pauli matrices
    if(ltype == PAULIMATRIXTYPE) {
        if(rtype == PAULIMATRIXTYPE)
            return false;
        Expression matTarget = getElementOfType(right, PAULIMATRIXTYPE);//right->getFirstInstanceOfType(PAULIMATRIXTYPE);
        return matTarget.getTypeHash() == NULLTYPE;
    }
    if(rtype == PAULIMATRIXTYPE) {
        Expression matTarget = getElementOfType(left, PAULIMATRIXTYPE);//left->getFirstInstanceOfType(PAULIMATRIXTYPE);
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

Expression cancelTerms(ExpressionObject* target) {
    if(target->getTypeHash() != ADDTYPE)
        return *new Expression(target);
    Add& addObj = dynamic_cast<Add&>(*target);
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
            Expression tempValue = combineTermsDifferingByCoefficientsAdditively(total.get(), currMember.get());
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
    return *new Expression(target);
}

/*
//this is unsophisticated and will only cancel terms that are identical not including their sign
Expression cancelTerms(SymbolicObject* target) {
    if(target != ADDTYPE)
        return *new Expression(target);
    Add& addObj = dynamic_cast<Add&>(*target);
    ExprVector members = addObj.members;
    if(members.size() == 1)
        return *new Expression(target);
    ExprVector newMembers = *new ExprVector();
    ExprVector newNewMembers = *new ExprVector();
    SignVector newSigns = *new SignVector(addObj.memberSigns);
    SignVector newNewSigns = *new SignVector();
    for(int i = 0; i<members.size(); i++) {
        Expression currExpr = members[i];
        if(isSubtypeOf(currExpr.getTypeHash(), ADDTYPE)) {
            currExpr = -currExpr;
            newSigns[i] = newSigns[i] != true;
        }
        newMembers.push_back(currExpr);
    }
    std::vector<int> indicesToSkip = *new std::vector<int>();
    for(int i = 0; i<newMembers.size(); i++) {
        if(intVectorContains(indicesToSkip, i))
            continue;
        Expression currExpr = newMembers[i];
        if(newSigns[i])
            currExpr = -currExpr;
        bool combined = false;
        //int total = 1;
        Expression total = currExpr;
        for(int j = i+1; j<newMembers.size(); j++) {
            if(intVectorContains(indicesToSkip, j))
                continue;
            Expression nextExpr = newMembers[j];
            if(newSigns[j])
                nextExpr = -nextExpr;
            Expression tempResult = combineTermsDifferingByCoefficientsAdditively(total.get(), nextExpr.get());
            if(tempResult.getTypeHash() != NULLTYPE && tempResult.getTypeHash() != ADDTYPE ) {
                total = tempResult;
                combined = true;
                indicesToSkip.push_back(i);
                indicesToSkip.push_back(j);
                //if(tempResult.getTypeHash()==ZEROTYPE)
                break;
            }
        }
        
        if(!combined) {
            newNewMembers.push_back(newMembers[i]);
            newNewSigns.push_back(newSigns[i]);
        } else if(total != currExpr) {
            if(total == ZERO)
                continue;
            bool newSign = total.getTypeHash() == SIGNTYPE;
            if(newSign)
                newNewMembers.push_back(-total);
            else
                newNewMembers.push_back(total);
            newNewSigns.push_back(newSign);
        }
    }
    //std::cout<< target->print() + "\n";
    if(newNewMembers.size() == 0)
        return ZERO;
    if(newNewMembers.size() == 1) {
        if(newNewSigns[0])
            return -newNewMembers[0];
        return newNewMembers[0];
    }
    Expression simplifiedTarget = *new Expression(new Add(newNewMembers,newNewSigns));
    //delete &newSigns;
    //delete &newMembers;
    return simplifiedTarget;
};
*/

Expression simplifyPauliMatrices(ExpressionObject* target) {
    bool isMul = target->getTypeHash() == MULTYPE;
    bool isAdd = target->getTypeHash() == ADDTYPE;
    Expression total;
    Expression remainder = *new Expression(target);
    Expression firstPauli = getElementOfType(target, PAULIMATRIXTYPE);
    if(firstPauli.getTypeHash() == NULLTYPE)
        return *new Expression(target);
    if(isMul) {
        total = ONE;
    }
    if(isAdd) {
        total = ZERO;
    }
    
    while(firstPauli.getTypeHash() != NULLTYPE) {
        if(isMul)
            remainder = removeElementMultiplicatively(remainder.get(), firstPauli.get());//remainder = remainder.remove(firstPauli);
        if(isAdd)
            remainder = removeElementAdditively(remainder.get(), firstPauli.get());
        Expression secondPauli = getMatrixMatchingPauliFlavor(remainder.get(), firstPauli.get());
        Expression tempResult = firstPauli;
        while(secondPauli.getTypeHash() != NULLTYPE) {
            if(isMul) {
                remainder = removeElementMultiplicatively(remainder.get(), secondPauli.get());//remainder = remainder.remove(secondPauli);
                tempResult = tempResult*secondPauli;
            }
            if(isAdd) {
                remainder = removeElementAdditively(remainder.get(), secondPauli.get());//remainder = remainder.remove(secondPauli);
                tempResult = tempResult+secondPauli;
            }
            
            secondPauli = getMatrixMatchingPauliFlavor(remainder.get(), firstPauli.get());
            
        }
        //if(tempResult.getTypeHash() == NULLTYPE)
        //    tempResult = firstPauli;
        
        firstPauli = getElementOfType(remainder.get(), PAULIMATRIXTYPE);//remainder.getFirstInstanceOfType(PAULIMATRIXTYPE);
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
        Mul& mulObj = dynamic_cast<Mul&>(*total);
        if(mulObj.members.size() == 0)
            return ZERO;
        if(mulObj.members.size() == 1)
            return mulObj.members[0];
    }
    if(total.getTypeHash() == ADDTYPE) {
        Add& addObj = dynamic_cast<Add&>(*total);
        if(addObj.members.size() == 0)
            return ZERO;
        if(addObj.members.size() == 1) {
            return addObj.members[0];
        }
    }
    return total;
}

Expression simplify(ExpressionObject* target) {
    if(target->isSimplified())
        return *new Expression(target);
    //std::vector<size_t> types = {REALTYPE,IMAGINARYUNITTYPE,SYMBOLTYPE,MATRIXTYPE,PAULIMATRIXTYPE,FRACTYPE};
    std::vector<size_t> types = {FRACTYPE,EXPTYPE,SYMBOLTYPE,PAULIMATRIXTYPE,MATRIXTYPE,IMAGINARYUNITTYPE,REALTYPE};
    Expression result = *new Expression(target);
    size_t targetType = target->getTypeHash();
    bool isAdd = (targetType == ADDTYPE);
    bool isMul = (targetType == MULTYPE);
    //if(isAdd)
    //    types.push_back(MULTYPE);
    //if(isMul)
    //    types.push_back(ADDTYPE);
    bool overallSign = false;
    if( targetType == FRACTYPE ) {
        Frac& fracTarget = dynamic_cast<Frac&>(*target);
        Expression newNumerator = simplify(fracTarget.numerator.get());
        Expression newDenomenator = simplify(fracTarget.denomenator.get());
        Expression result = *new Expression(new Frac(newNumerator.get(),newDenomenator.get()));
        result.setSimplified(true);
        return *new Expression(result);
    }
    if(targetType == SIGNTYPE) {
        return -simplify((-*target).get());
    }
    if(!isMul && !isAdd) {// !isSubclassOf(target, typeid(Operator).hash_code())) {
        return *new Expression(target);
    }
    if(isMul) {
        Mul& mulObj = dynamic_cast<Mul&>(*target);
        ExprVector members = mulObj.members;
        if(members.size() == 1)
            return members[0];
        ExprVector newMembers = *new ExprVector();
        for(int i = 0; i<members.size(); i++) {
            Expression simplified = simplify(members[i].get());
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
        Add& addObj = dynamic_cast<Add&>(*target);
        ExprVector members = addObj.members;
        if(members.size() == 1)
            return *new Expression(target);
        ExprVector newMembers = *new ExprVector();
        for(int i = 0; i<members.size(); i++) {
            Expression simplified = simplify(members[i].get());
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
        //result = cancelTerms(simplifiedTarget.get());
        result = simplifiedTarget;
        
    }
    for(size_t type : types) {
        if(type == PAULIMATRIXTYPE) {
            result = simplifyPauliMatrices(result.get());
        }
        else {//all cases but simplifying pauli matrices
            Expression first = getElementOfType(result.get(), type);
            if(first.getTypeHash() == NULLTYPE)
                continue;
            /*if(isMul && first.getTypeHash() == REALTYPE) {
                Real& firstReal = dynamic_cast<Real&>(*first);
                if(firstReal.value == 0)
                    return zero;
                if(firstReal.value < 0)
                    overallSign = overallSign != true;
                first = -first;
            }*/
            Expression remainder;// = result.remove(first);
            if(isMul) {
                remainder = removeElementMultiplicatively(result.get(), first.get());
            }
            if(isAdd) {
                remainder = removeElementAdditively(result.get(), first.get());
            }
            Expression second = getElementOfType(remainder.get(), type);//remainder.getFirstInstanceOfType(type);
            Expression total = first;
            while(second.getTypeHash() != NULLTYPE) {
                if(isMul) {
                    remainder = removeElementMultiplicatively(remainder.get(), second.get());//remainder = remainder.remove(second);
                }
                if(isAdd) {
                    remainder = removeElementAdditively(remainder.get(), second.get());
                }
                /*if(isMul && type == REALTYPE) {
                    Real& secondReal = dynamic_cast<Real&>(*second);
                    if(secondReal.value == 0)
                        return zero;
                    if(secondReal.value < 0)
                        overallSign = overallSign != true;
                    second = -second;
                }*/
                if(isMul)
                    total = total*second;
                if(isAdd)
                    total = total+second;
                second = getElementOfType(remainder.get(), type);//remainder.getFirstInstanceOfType(type);
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
    
    result.setSimplified(true);
    if(result.getTypeHash() == MULTYPE) {
        Mul& mulObj = dynamic_cast<Mul&>(*result);
        if(mulObj.members.size() == 0)
            return ZERO;
        if(mulObj.members.size() == 1)
            return mulObj.members[0];
    }
    if(result.getTypeHash() == ADDTYPE) {
        Add& addObj = dynamic_cast<Add&>(*result);
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
            results[j] = factor(results[j].get());
            //bool isContainer = isSubtypeOf(results[j].get(), OPERATORTYPE);
            //if(isContainer)
                results[j] = removeElementMultiplicatively(results[j].get(), commonFactors[i].get());
            //else
            //    results[j] = results[j]/commonFactors[i];
            results[j] = simplify(results[j].get());
            if(results[j].getTypeHash() == NULLTYPE) {
                NullObject& nullObj = dynamic_cast<NullObject&>(*results[j]);
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
            results[j] = factor(results[j].get());
            results[j] = removeElementMultiplicatively(results[j].get(), commonFactors[i].get());
            if(results[j].getTypeHash() == NULLTYPE)
                results[j] = targets[j];
        }
    }
    return results;
}

Expression distribute(ExpressionObject* left, ExpressionObject* right) {
    size_t leftType = left->getTypeHash();
    size_t rightType = right->getTypeHash();
    if(rightType == NULLTYPE)
        *right**left;
    if(leftType == NULLTYPE)
        *left**right;
    if(leftType == ZEROTYPE || rightType == ZEROTYPE)
        return ZERO;
    if(leftType == ONETYPE)
        return *new Expression(right);
    if(rightType == ONETYPE)
        return *new Expression(left);
    if(leftType == SIGNTYPE) {
        Sign& signLeft = dynamic_cast<Sign&>(*left);
        if(rightType == SIGNTYPE) {
            Sign& signRight = dynamic_cast<Sign&>(*right);
            return distribute(signLeft.member.get(), signRight.member.get());
        }
        return -distribute(signLeft.member.get(), right);
    }
    if(rightType == SIGNTYPE) {
        Sign& signRight = dynamic_cast<Sign&>(*right);
        return -distribute(left, signRight.member.get());
    }
    if(leftType == ADDTYPE) {
        Add& leftObj = dynamic_cast<Add&>(*left);
        if(rightType == ADDTYPE) {
            Add& rightObj = dynamic_cast<Add&>(*right);
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
            Frac& rightObj = dynamic_cast<Frac&>(*right);
            Expression newNumerator = distribute(left, rightObj.numerator.get());
            Expression newDenomenator = rightObj.denomenator;
            ExprVector targets = generateExprVector({newNumerator,newDenomenator});
            ExprVector results = cancelCommonFactors(targets);
            Expression finalResult = *new Expression(new Frac(results[0].get(),results[1].get()));
            return finalResult;
            
        }//end ltype == ADD, rtype == FRAC
        if((int)leftObj.members.size() == 1) {
            Expression tempResult = *leftObj.members[0]**right;
            return tempResult;
        }
        ExprVector newMembers = *new ExprVector();
        SignVector newSigns = *new SignVector();
        for(int i=0; i<leftObj.members.size(); i++) {
            Expression result = distribute(leftObj.members[i].get(), right);
            newMembers.push_back(result);
        }
        Expression result = *new Expression(new Add(newMembers));
        return result;
        //end ltype == ADD, rtype == MUL or similar
    }//end ltype == ADD
    
    if(leftType == FRACTYPE) {
        Frac& leftObj = dynamic_cast<Frac&>(*left);
        if(rightType == FRACTYPE) {
            Frac& rightObj = dynamic_cast<Frac&>(*right);
            Expression newNumerator = distribute(leftObj.numerator.get(), rightObj.numerator.get());
            Expression newDenomenator = distribute(leftObj.denomenator.get(), rightObj.denomenator.get());
            ExprVector targets = generateExprVector({newNumerator,newDenomenator});
            ExprVector results = cancelCommonFactors(targets);
            Expression finalResult = *new Expression(new Frac(results[0].get(),results[1].get()));
            return finalResult;
        }//end ltype = FRAC, rtype == FRAC
        Expression newNumerator = distribute(leftObj.numerator.get(), right);
        Expression newDenomenator = leftObj.denomenator;
        ExprVector targets = generateExprVector({newNumerator,newDenomenator});
        ExprVector results = cancelCommonFactors(targets);
        Expression finalResult = *new Expression(new Frac(results[0].get(),results[1].get()));
        return finalResult;
        //end ltype == FRAC, rtype = MUL or similar
    }//end ltype = FRAC
    
    if(rightType == ADDTYPE) {
        Add& rightObj = dynamic_cast<Add&>(*right);
        ExprVector newMembers = *new ExprVector();
        for(int i=0; i<rightObj.members.size(); i++) {
            Expression result = *left*(*rightObj.members[i]);
            //result = simplify(*result);//combineProducts(left, rightObj.members[i]);
            newMembers.push_back(result);
        }
        Expression result = *new Expression(new Add(newMembers));
        return result;
    }
    if(rightType == FRACTYPE) {
        Frac& rightObj = dynamic_cast<Frac&>(*right);
        Expression newNumerator = distribute(left, rightObj.numerator.get());
        Expression newDenomenator = rightObj.denomenator;
        ExprVector targets = generateExprVector({newNumerator,newDenomenator});
        ExprVector results = cancelCommonFactors(targets);
        Expression finalResult = *new Expression(new Frac(results[0].get(),results[1].get()));
        return finalResult;
    }
    if(leftType == EXPTYPE && rightType == EXPTYPE) {
        Exp& leftExp = dynamic_cast<Exp&>(*left);
        Exp& rightExp = dynamic_cast<Exp&>(*right);
        if(leftExp.base == rightExp.base) {
            Expression newExponent = leftExp.exponent+rightExp.exponent;
            Expression result = *new Expression(new Exp(leftExp.exponent.get(),newExponent.get()));
            return result;
        }
    }
    Expression mul = combineProducts(left, right);
    return mul;
}


Expression factor(ExpressionObject* factee) {
    ExprVector factors = getFactors(factee);
    if(factors.size()>1) {
        Expression result = *new Expression(new Mul(factors));
        return result;
    }
    return *new Expression(factee);
}

ExprVector getFactorsOfInt(ExpressionObject* factee) {
    if(!isSubtypeOf(factee, REALTYPE))
        return {*new Expression(factee)};
    Real& realObj = dynamic_cast<Real&>(*factee);
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

ExprVector getFactors(ExpressionObject* factee) {
    size_t type = factee->getTypeHash();
    if(type == MULTYPE) {
        Mul& mulObj = dynamic_cast<Mul&>(*factee);
        ExprVector factors = *new ExprVector();
        for(int i = 0; i< mulObj.members.size(); i++) {
            ExprVector tempFactors = getFactors(mulObj.members[i].get());
            for(int j = 0; j< tempFactors.size(); j++)
                //if(!containsElement(factors, tempFactors[j].get()))
                    factors.push_back(tempFactors[j]);
        }
        return factors;
    }
    if(type == ADDTYPE) {
        Add& addObj = dynamic_cast<Add&>(*factee);
        ExprVector commonFactors = getCommonFactors(addObj.members);
        if(commonFactors.size()>0)
            return commonFactors;
    }
    if(type == SIGNTYPE) {
        Sign& signObj = dynamic_cast<Sign&>(*factee);
        ExprVector factors = getFactors(signObj.member.get());
        factors.push_back(MINUSONE);
        if(factors.size()>0)
            return factors;
    }
    if(type == FRACTYPE) {
        Frac& fracObj = dynamic_cast<Frac&>(*factee);
        Expression result = reciprocal(fracObj.denomenator.get());
        ExprVector targets = generateExprVector({result});
        ExprVector factors = getFactors(fracObj.numerator.get());
        return combineExprVectors( factors, targets );
    }
    if(type == EXPTYPE) {
        Exp& expObj = dynamic_cast<Exp&>(*factee);
        Expression exponent = expObj.exponent;
        size_t exponentType = exponent.getTypeHash();
        if(exponentType == ADDTYPE) {
            ExprVector factors = *new ExprVector();
            Add& addObj = dynamic_cast<Add&>(*exponent);
            for(int i = 0; i<addObj.members.size(); i++) {
                Expression newExpression = *new Expression(new Exp(expObj.base.get(),addObj.members[i].get()));
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
    ExprVector firstTermFactors = getFactors(terms[0].get());
    if(terms.size()<2)
        return firstTermFactors;
    std::vector<std::vector<int>> indicesAlreadyCounted = *new std::vector<std::vector<int>>();
    for(int i = 1; i<terms.size(); i++) {
        ExprVector currTermFactors = getFactors(terms[i].get());
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
        if(isCommonFactor && !isSubtypeOf(firstTermFactors[i].get(), MATRIXTYPE) && firstTermFactors[i].getTypeHash() != SYMBOLTYPE)
            commonFactors.push_back(firstTermFactors[i]);
    }
    return commonFactors;
}

ExprVector getCommonFactors(ExprVector terms) {
    ExprVector commonFactors = *new ExprVector();
    ExprMatrix ithTermFactors = *new ExprMatrix();
    ExprVector firstTermFactors = getFactors(terms[0].get());
    if(terms.size()<2)
        return firstTermFactors;
    std::vector<std::vector<int>> indicesAlreadyCounted = *new std::vector<std::vector<int>>();
    for(int i = 1; i<terms.size(); i++) {
        ExprVector currTermFactors = getFactors(terms[i].get());
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
                containsFactor |= -firstTermFactors[i] == (ithTermFactors[j])[k];
                containsFactor |= firstTermFactors[i] == -(ithTermFactors[j])[k];
                containsFactor &= !isSubtypeOf(firstTermFactors[i].get(), MATRIXTYPE);
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

/*ExprVector getFactorsWhichAreNotCommon(ExprVector terms) {
    ExprVector uncommonFactors = *new ExprVector();
    ExprMatrix ithTermFactors = *new ExprMatrix();
    ExprVector firstTermFactors = getFactors(terms[0]);
    if(terms.size()<2) {
        return {ZERO};
    }
    for(int i = 1; i<terms.size(); i++)
        ithTermFactors.push_back(getFactors(terms[i]));
    for(int i = 0; i<firstTermFactors.size(); i++) {
        bool isCommonFactor = true;
        for(int j=0; j<(ithTermFactors).size(); j++) {
            isCommonFactor &= containsElement(ithTermFactors[j], firstTermFactors[i]);
        }
        if(!isCommonFactor)
            uncommonFactors.push_back(firstTermFactors[i]);
    }
    return uncommonFactors;
}*/

//DOES NOT RESPECT COMMUTATION PROPERTIES
Expression combineTermsDifferingByCoefficientsAdditively(ExpressionObject* left, ExpressionObject* right) {
    if(left->getTypeHash() == ZEROTYPE)
        return *new Expression(right);
    if(right->getTypeHash() == ZEROTYPE)
        return *new Expression(left);
    if(isSubtypeOf(left, REALTYPE) && isSubtypeOf(right, REALTYPE))
        return *left+*right;
    if(left->getTypeHash() == SIGNTYPE) {
        Sign& signObj = dynamic_cast<Sign&>(*left);
        if(*signObj.member == *right)
            return ZERO;
    }
    else if(right->getTypeHash() == SIGNTYPE) {
        Sign& signObj = dynamic_cast<Sign&>(*right);
        if(*signObj.member == *left)
            return ZERO;
    }
    if(*left == *right)
        return simplify((2**left).get());
    ExprVector targets = generateExprVector({*new Expression(left),*new Expression(right)});
    ExprVector commonFactors = getCommonFactors(targets);//getCommonFactors(targets);
    /*ExprVector curatedCommonFactors = *new ExprVector();
    for(int i = 0; i< commonFactors.size(); i++) {
        if(isSubTypeOf(commonFactors[i].getTypeHash(), MATRIXTYPE))
            continue;
        curatedCommonFactors.push_back(commonFactors[i]);
    }
    commonFactors = curatedCommonFactors;*/
    if(commonFactors.size() == 0)
        return *new Expression(new NullObject("no common factors"));
    //ExprVector uncommonFactors = getFactorsWhichAreNotCommon({left,right});
    targets = generateExprVector({*new Expression(left),*new Expression(right)});
    ExprVector results = cancelCommonFactors(targets);//cancelCommonFactors(targets);
    Expression inCommon = ONE;
    for(int i = 0; i< commonFactors.size(); i++) {
        inCommon = inCommon * commonFactors[i];
    }
    Expression combinedCoeffs = combineTermsDifferingByCoefficientsAdditively(results[0].get(), results[1].get());
    if(combinedCoeffs.getTypeHash() == NULLTYPE)
        return *left+*right;
    Expression temp = combinedCoeffs*inCommon;
    return temp;
}

Expression substitute(ExpressionObject* source, ExpressionObject* target, ExpressionObject* value) {
    Expression valueExpr = *new Expression(value);
    if(*source == *target)
        return valueExpr;
    size_t sourceType = source->getTypeHash();
    if(isSubtypeOf(source, MATRIXTYPE)) {
        Matrix& matObj = dynamic_cast<Matrix&>(*source);
        ExprMatrix elements = *new ExprMatrix(matObj.elements);
        bool foundTarget = false;
        for(int i = 0; i<matObj.dimensions.first; i++) {
            for (int j = 0; j<matObj.dimensions.second; j++) {
                if(*elements[i][j] == *target) {
                    elements[i][j] = valueExpr;
                    foundTarget = true;
                    break;
                }
                if(*-elements[i][j] == *target || *elements[i][j] == *-*target) {
                    elements[i][j] = -valueExpr;
                    foundTarget = true;
                    break;
                }
            }
            if(foundTarget)
                break;
        }
        if(!foundTarget)
            return *new Expression(new NullObject("No substitutions possible"));
        if(sourceType == PAULIMATRIXTYPE) {
            PauliMatrix* pauliSource = dynamic_cast<PauliMatrix*>(source);
            Expression newMat = declarePauliMatrix(new PauliMatrix(printExprMatrix(elements),0,pauliSource->flavor,elements));
            return newMat;
        }
        Expression newMat = declareMatrix(matObj.name+"with replacement", elements);
        return newMat;
    }
    if(sourceType == FRACTYPE) {
        Frac& fracObj = dynamic_cast<Frac&>(*source);
        Expression newNum;
        Expression newDenom;
        bool changedNum = false;
        bool changedDenom = false;
        Expression testResult = substitute(fracObj.numerator.get(), target, value);
        if(testResult.getTypeHash() == NULLTYPE)
            newNum = fracObj.numerator;
        else {
            newNum = testResult;
            changedNum = true;
        }
        testResult = substitute(fracObj.denomenator.get(), target, value);
        if(testResult.getTypeHash() == NULLTYPE)
            newDenom = fracObj.denomenator;
        else {
            newDenom = testResult;
            changedDenom = true;
        }
        if(changedNum || changedDenom) {
            return *new Expression(new Frac(newNum.get(),newDenom.get()));
        }
        return *new Expression(new NullObject("No substitutions possible"));
    }
    if(sourceType == EXPTYPE) {
        Exp& expObj = dynamic_cast<Exp&>(*source);
        Expression newBase;
        Expression newExponent;
        bool changedBase = false;
        bool changedExponent = false;
        Expression testResult = substitute(expObj.base.get(), target, value);
        if(testResult.getTypeHash() == NULLTYPE)
            newBase = expObj.base;
        else {
            newBase = testResult;
            changedBase = true;
        }
        testResult = substitute(expObj.exponent.get(), target, value);
        if(testResult.getTypeHash() == NULLTYPE)
            newExponent = expObj.exponent;
        else {
            newExponent = testResult;
            changedExponent = true;
        }
        if(changedBase || changedExponent) {
            return *new Expression(new Exp(newBase.get(),newExponent.get()));
        }
        return *new Expression(new NullObject("No substitutions possible"));
    }
    if(sourceType == SIGNTYPE) {
        Sign& signObj = dynamic_cast<Sign&>(*source);
        Expression testResult = substitute(signObj.member.get(), target, value);
        if(testResult.getTypeHash() != NULLTYPE)
            return -testResult;
        return *new Expression(new NullObject("No substitutions possible"));
    }
    ExprVector elementsToModify;
    ExprVector newElements = *new ExprVector();
    if(sourceType == ADDTYPE) {
        Add& addObj = dynamic_cast<Add&>(*source);
        elementsToModify = addObj.members;
    }
    if(sourceType == MULTYPE) {
        Mul& mulObj = dynamic_cast<Mul&>(*source);
        elementsToModify = mulObj.members;
    }
    
    bool moddedAnyElements = false;
    for(int i = 0; i< elementsToModify.size(); i++) {
        Expression moddedEle = substitute(elementsToModify[i].get(), target, value);
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
Expression substitute(ExpressionObject* source, std::vector<std::pair<Expression,Expression>> substitutions) {
    return *new Expression(new NullObject("WTF"));
};
