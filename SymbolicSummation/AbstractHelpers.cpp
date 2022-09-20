//
//  AbstractHelpers.cpp
//  SymbolicSummation
//
//  Created by Robert Stahulak on 6/22/22.
//

#include "AbstractHelpers.hpp"
#include "Real.hpp"
#include "Container.hpp"
#include "Symbol.hpp"
#include "PauliMatrices.hpp"
#include "ExpressionHelpers.hpp"
#include <unordered_map>
#include <iostream>
#include <fstream>

static std::unordered_map<std::string,Expression> declaredSymbols = *new std::unordered_map<std::string, Expression>();

/*bool isSubtypeOf(size_t sub, size_t super) {
    bool result = false;
    if(sub == super)
        result = true;
    if(super == REALTYPE && (sub == ONETYPE || sub == ZEROTYPE) )
        result = true;
    if(super == MATRIXTYPE && sub == PAULIMATRIXTYPE)
        result = true;
    if(super == typeid(ExpressionContainer).hash_code() && (sub == ADDTYPE || sub == MULTYPE || sub == FRACTYPE || sub == EXPTYPE || sub == SIGNTYPE))
        result = true;
    if(super == ADDTYPE && sub == SIGNTYPE)
        result = true;
    return result;
}*/

bool isSubtypeOf(ExpressionObject* sub, ExpressionObject* super) {
    size_t subType = sub->getTypeHash();
    if(subType == SIGNTYPE) {
        subType = (-*sub).getTypeHash();
    }
    size_t superType = super->getTypeHash();
    if(superType == SIGNTYPE) {
        superType = (-*super).getTypeHash();
    }
    bool result = false;
    if(subType == superType)
        result = true;
    if(superType == REALTYPE && (subType == ONETYPE || subType == ZEROTYPE) )
        result = true;
    if(superType == MATRIXTYPE && subType == PAULIMATRIXTYPE)
        result = true;
    if(superType == typeid(Container).hash_code() && (subType == ADDTYPE || subType == MULTYPE || subType == FRACTYPE || subType == EXPTYPE || subType == SIGNTYPE))
        result = true;
    return result;
}

bool isSubtypeOf(ExpressionObject* sub, size_t superType) {
    size_t subType = sub->getTypeHash();
    if(subType == SIGNTYPE) {
        subType = (-*sub).getTypeHash();
    }
    bool result = false;
    if(subType == superType)
        result = true;
    if(superType == REALTYPE && (subType == ONETYPE || subType == ZEROTYPE) )
        result = true;
    if(superType == MATRIXTYPE && subType == PAULIMATRIXTYPE)
        result = true;
    if(superType == typeid(Container).hash_code() && (subType == ADDTYPE || subType == MULTYPE || subType == FRACTYPE || subType == EXPTYPE || subType == SIGNTYPE))
        result = true;
    return result;
}

std::string getStringExpressionType(std::string exprString) {
    int parenthesisDepth = 0;
    bool containsOperator = false;
    bool containsLetter = false;
    bool containsNumber = false;
    bool containsNumberBeforeLetter = false;
    std::string topLevelOperator = "";
    for(int i = (int)exprString.length()-1; i>=0; i--) {
        char currChar = exprString.at(i);
        switch (currChar) {
            case ')':
                parenthesisDepth++;
                break;
            case '(':
                if(parenthesisDepth>0)
                    parenthesisDepth--;
                else return "badformat";
                break;
            case '+':
            case '-':
                if(parenthesisDepth==0) {
                    containsOperator |= true;
                    topLevelOperator = "add";
                }
                break;
                break;
            case '/':
                if(parenthesisDepth==0) {
                    containsOperator |= true;
                    if(topLevelOperator != "add")
                        topLevelOperator = "frac";
                }
                break;
            case '*':
                if(parenthesisDepth==0) {
                    containsOperator |= true;
                    if(topLevelOperator != "add" && topLevelOperator != "frac")
                        topLevelOperator = "mul";
                }
                break;
            case '^':
                if(parenthesisDepth==0) {
                    containsOperator |= true;
                    if(topLevelOperator != "add" && topLevelOperator != "mul" && topLevelOperator != "frac")
                        topLevelOperator = "exp";
                }
                break;
            default:
                if(parenthesisDepth == 0) {
                    containsLetter |= isalpha(currChar);
                    containsNumber |= isdigit(currChar);
                    containsNumberBeforeLetter |= isdigit(currChar) && !containsLetter;
                }
                break;
        }
    }
    if(parenthesisDepth!=0)
        return "badformat";
    if(containsOperator)
        return topLevelOperator;
    if(containsLetter && containsNumberBeforeLetter)
        return "real+symbol";
    if(containsLetter)
        return "symbol";
    if(containsNumber)
        return "real";
    return "badformat";
};

std::vector<std::string> tokenize(std::string expr, std::string delimiters) {
    std::vector<std::string> tokens = *new std::vector<std::string>();
    std::string currentToken = "";
    int parenthesisDepth = 0;
    for(int i = 0; i<expr.length(); i++) {
        char currChar = expr[i];
        if(currChar == '(')
            parenthesisDepth++;
        if(i == expr.length()-1) {
            currentToken += currChar;
            tokens.push_back(currentToken);
            continue;
        }
        if(parenthesisDepth==0 && delimiters.find(currChar) != std::string::npos && currentToken != ""){
            tokens.push_back(currentToken);
            currentToken = "";
        } else
            currentToken += currChar;
        
        if(currChar == ')')
            parenthesisDepth--;
    }
    return tokens;
}

SignVector getTokenSigns(std::string expr) {
    SignVector memberSigns = *new SignVector();
    int parenthesisDepth = 0;
    memberSigns.push_back(expr[0] == '-');
    for(int i = 0; i<expr.length(); i++) {
        switch (expr[i]) {
            case '(':
                parenthesisDepth++;
                break;
            case ')':
                parenthesisDepth--;
                break;
            case '+':
                if(parenthesisDepth<=0)
                    memberSigns.push_back(false);
                break;
            case '-':
                if(parenthesisDepth<=0 && i>0)
                    memberSigns.push_back(true);
                break;
            default:
                break;
        }
    }
    return memberSigns;
}

std::string sanitize(std::string expr) {
    std::string result = expr;
    int i = 0;
    while(i<result.length()) {
        if(isspace(expr[i])) {//if(expr[i] == '(' || expr[i] == ')' || isspace(expr[i])) {
            result.erase(i,1);
        } else
            i++;
    }
    if(result[0] == '(' && result[result.size()-1] == ')') {
        bool parensActedUpon = false;
        int parenthesisDepth = 0;
        for(int i = 1; i<result.length()-1; i++) {
            char currChar = result[i];
            if(currChar == '(') {
                parenthesisDepth++;
            }
            if(currChar == ')') {
                if(parenthesisDepth == 0) {
                    parensActedUpon = true;
                    break;
                }
                parenthesisDepth--;
            }
            
        }
        if(!parensActedUpon) {
            result = result.substr(1,std::string::npos);
            result = result.substr(0,result.size()-1);
        }
    }
    return result;
}

void initializeDefaultSymbols() {
    if(declaredSymbols.empty()) {
        declaredSymbols["1"] = ONE;
        declaredSymbols["-1"] = MINUSONE;//*new Expression(new Sign(ONE));
        declaredSymbols["0"] = ZERO;
        float temp = 1;
        declaredSymbols[std::to_string(temp)] = ONE;
        temp = -1;
        declaredSymbols[std::to_string(temp)] = MINUSONE;//*new Expression(new Sign(ONE));
        temp = 0;
        declaredSymbols[std::to_string(temp)] = ZERO;
        declaredSymbols["pi"] = PI;
        declaredSymbols["Pi"] = PI;
        declaredSymbols["PI"] = PI;
        declaredSymbols["e"] = EULERNUM;
        declaredSymbols["E"] = EULERNUM;
        declaredSymbols["i"] = IMAGUNIT;
    }
}

Expression declareMatrix(ExpressionObject* value) {
    return declareSymbol(value->print(),value);
};

Expression declarePauliMatrix(int index, std::string flavor) {
    ExpressionObject* pMat = new NullObject("invalid Pauli index");
    switch(index) {
        case 1:
            delete pMat;
            pMat = new PauliMatrix(index,flavor);
            //std::cout << pMat.print();
            break;
        case 2:
            delete pMat;
            pMat = new PauliMatrix(index,flavor);
            break;
        case 3:
            delete pMat;
            pMat = new PauliMatrix(index,flavor);
            break;
        case 0:
        default:
            return pMat;
    }
    return declareSymbol(flavor+std::to_string(index),pMat);
}

Expression declarePauliMatrix(ExpressionObject* value) {
    return declareSymbol(value->print(), value);
}

Expression declareMatrix(std::string matName, ExprMatrix elements) {
    initializeDefaultSymbols();
    if(!declaredSymbols.contains(matName)) {
        Expression newMatrix = *new Expression(new Matrix(matName,elements));
        declaredSymbols[matName] = newMatrix;
    }
    return declaredSymbols[matName];
};

Expression declareMatrix(std::string matName, std::initializer_list<std::initializer_list<Expression>> elements) {
    initializeDefaultSymbols();
    if(!declaredSymbols.contains(matName)) {
        Expression newMatrix = *new Expression(new Matrix(matName,elements));
        declaredSymbols[matName] = newMatrix;
    }
    return declaredSymbols[matName];
};

Expression declareSymbol(std::string name) {
    ExpressionObject* value = new NullObject("temp null obj to force creation of new symbol");
    return declareSymbol(name, value);
}

Expression declareReal(float value) {
    if(value == ceil(value)) {
        int actualVal = (int)value;
        std::string name = std::to_string(actualVal);
        return declareSymbol(name, new Real(actualVal) );
    }
    else {
        std::string name = std::to_string(value);
        return declareSymbol(name, new Real(value) );
    }
}

Expression declareSymbol(std::string name, ExpressionObject* value) {
    initializeDefaultSymbols();
    if(!declaredSymbols.contains(name)) {
        if(value->getTypeHash() != NULLTYPE) {
            declaredSymbols[name] = *new Expression(value);
            return declaredSymbols[name];
        }
        Expression newSymbol = *new Expression(new Symbol(name));
        declaredSymbols[name] = newSymbol;
    }
    return declaredSymbols[name];
};

Expression parseString(std::string exprString) {
    std::string exprType = getStringExpressionType(sanitize(exprString));
    if(exprType == "badformat") {
        Expression result = *new Expression( new NullObject("improper formatting in expression string"));
        return result;
    }
    if(exprType == "add") {
        ExprVector members = *new ExprVector();
        SignVector memberSigns = getTokenSigns(exprString);
        std::vector<std::string> tokens = tokenize(sanitize(exprString), "+-");
        for(int i = 0; i<tokens.size(); i++) {
            std::string nextToken = tokens[i];
            Expression next = parseString(nextToken);
            //declareSymbol(next.getNamePerform(),next);
            if(memberSigns[i])
                members.push_back(-next);
            else
                members.push_back(next);
        }
        //Expression result(std::make_shared<Add>(members,memberSigns));
        Expression result = *new Expression(new Add(members));
        return result;
    }
    if(exprType == "mul") {
        ExprVector members = *new ExprVector();
        std::vector<std::string> tokens = tokenize(sanitize(exprString), "*()");
        for(int i = 0; i<tokens.size(); i++) {
            Expression next = parseString(tokens[i]);
            members.push_back(next);
        }
        Expression result = *new Expression(new Mul(members));
        return result;
    }
    if(exprType == "frac") {
        std::vector<std::string> tokens = tokenize(sanitize(exprString), "/()");
        Expression numerator = parseString(tokens[0]);
        Expression denominator = parseString(tokens[1]);
        Expression result = *new Expression(new Frac(numerator.get(),denominator.get()));
        return result;
    }
    if(exprType == "exp") {
        std::vector<std::string> tokens = tokenize(sanitize(exprString), "^()");
        Expression base = parseString(tokens[0]);
        Expression exponent = parseString(tokens[1]);
        Expression result = *new Expression(new Exp(base.get(),exponent.get()));
        return result;
    }
    if(exprType == "real") {
        std::string name = sanitize(exprString);
        float value = std::stof(name);
        if(value == 1) {
            return ONE;
        }
        if(value == 0) {
            return ZERO;
        }
        Expression result = declareReal(value);
        return result;
    }
    if(exprType == "real+symbol") {
        std::string expr = sanitize(exprString);
        std::string realPart = "";
        std::string symbolPart = "";
        int i = 0;
        while(i<expr.size()) {
            if(isdigit(expr[i]))
                i++;
            else
                break;
        }
        realPart = expr.substr(0,i);
        symbolPart = expr.substr(i+1);
        ExpressionObject* rePart = parseString(realPart).get();
        ExpressionObject* symPart = parseString(symbolPart).get();
        Expression result = *new Expression(new Mul(rePart,symPart));
        return result;
    }
    if(exprType == "symbol") {
        Expression result = declareSymbol(sanitize(exprString));
        return result;
    }
    Expression result = *new Expression(new NullObject("parser could not identify expression type"));
    return result;
};

int positionOfElement(ExprVector list, ExpressionObject* target, bool rightToLeft) {
    int i = 0;
    if(!rightToLeft) {
        while(i<list.size()) {
            if(*list[i] == *target)
                return i;
            i++;
        }
    } else {
        i = (int)list.size()-1;
        while(i>=0) {
            if(*list[i] == *target)
                return i;
            i--;
        }
    }
    return -1;
};

int positionOfElementIgnoringSign(ExprVector list, ExpressionObject* target, bool rightToLeft) {
    int i = 0;
    if(!rightToLeft) {
        while(i<list.size()) {
            if(*list[i] == *target || *-list[i] == *target)
                return i;
            i++;
        }
    } else {
        i = (int)list.size()-1;
        while(i>=0) {
            if(*list[i] == *target || *-list[i] == *target)
                return i;
            i--;
        }
    }
    return -1;
};

int positionOfType(ExprVector list, size_t type, bool rightToLeft) {
    int i = 0;
    if(!rightToLeft) {
        while(i<(int)list.size()) {
            if(isSubtypeOf(list[i].get(), type)) {
                return i;
            }
            i++;
        }
    } else {
        i = (int)list.size()-1;
        while(i>=0) {
            if(isSubtypeOf(list[i].get(), type)) {
                return i;
            }
            i--;
        }
    }
    return -1;
};

bool containsType(ExprVector list, size_t type, bool rightToLeft) {
    return positionOfType(list, type, rightToLeft) >= 0;
}

bool containsElement(ExprVector list, ExpressionObject* target, bool rightToLeft) {
    return positionOfElement(list, target, rightToLeft) >= 0;
}

ExprVector removeElementFromVector(ExprVector source, ExpressionObject* target, bool rightToLeft = false) {
    ExprVector result = *new ExprVector();
    int location = positionOfElement(source, target, rightToLeft);
    if(location < 0)
        return result;
    for(int i = 0; i<source.size(); i++) {
        if(i == location)
            continue;
        result.push_back(source[i]);
        
    }
    return result;
}

Expression removeElementMultiplicatively(ExpressionObject* source, ExpressionObject* target, bool rightToLeft) {
    if(*source == *target)
        return *new Expression(new NullObject("this is what happens when you remove something from itself!"));
    size_t type = source->getTypeHash();
    if(type == SIGNTYPE) {
        Sign& sourceObj = dynamic_cast<Sign&>(*source);
        Expression result = removeElementMultiplicatively(sourceObj.member.get(), target);
        if(result.getTypeHash() == NULLTYPE)
            return result;
        return -result;
    }
    if(type == MULTYPE) {
        Mul& sourceObj = dynamic_cast<Mul&>(*source);
        ExprVector newMembers = removeElementFromVector(sourceObj.members, target,rightToLeft);
        if(newMembers == sourceObj.members)
            return *new Expression(source);
        if(newMembers.size() == 0) {
            return ONE;
        }
        if(newMembers.size() == 1)
            return newMembers[0];
        return *new Expression(new Mul(newMembers));
    }
    if(type == FRACTYPE) {
        Frac& sourceObj = dynamic_cast<Frac&>(*source);
        Expression result = removeElementMultiplicatively(sourceObj.numerator.get(), target, rightToLeft);
        if(result.getTypeHash() == NULLTYPE) {
            Expression recip = reciprocal(target);
            result = removeElementMultiplicatively(sourceObj.denomenator.get(), recip.get(), rightToLeft);
            if(result.getTypeHash() == NULLTYPE)
                return result;
            if(result == ONE)
                return sourceObj.numerator;
            else
                return *new Expression(new Frac(sourceObj.numerator.get(),result.get()));
        }
        return *new Expression(new Frac(result.get(),sourceObj.denomenator.get()));
    }
    if(type == EXPTYPE) {
        Exp& sourceObj = dynamic_cast<Exp&>(*source);
        if(sourceObj.base.get() == target) {
            Expression expOneDown = sourceObj.exponent-ONE;
            expOneDown = simplify(expOneDown.get());
            if(expOneDown == ONE)
                return sourceObj.base;
            if(expOneDown == ZERO)
                return ONE;
            return *new Expression(new Exp(sourceObj.base.get(),expOneDown.get()));
        }
        return *new Expression(new NullObject("could not find target to remove"));
    }
    if(target == source)
        return ONE;
    return *new Expression(new NullObject("could not remove target multiplicatively"));
};
Expression removeElementAdditively(ExpressionObject* source, ExpressionObject* target, bool rightToLeft) {
    if(source == target)
        return *new Expression(new NullObject("this is what happens when you remove something from itself!"));
    if(source->getTypeHash() != ADDTYPE)
        return *new Expression(new NullObject("could not remove target additively"));
    Add& sourceObj = dynamic_cast<Add&>(*source);
    ExprVector newMembers = removeElementFromVector(sourceObj.members, target);
    /*int location = positionOfElementIgnoringSign(sourceObj.members, target, rightToLeft);
    if(location < 0)
        return *new Expression(source);
    ExprVector newMembers = *new ExprVector();
    for(int i = 0; i<sourceObj.members.size(); i++) {
        if(i == location)
            continue;
        newMembers.push_back(sourceObj.members[i]);
    }*/
    if(newMembers == sourceObj.members)
        return *new Expression(new NullObject("could not find target to remove"));
    if(newMembers.size() == 0)
        return ZERO;
    return *new Expression(new Add(*new ExprVector(newMembers)));
};
Expression removeElementAbsolutely(ExpressionObject* source, ExpressionObject* target, bool rightToLeft) {
    if(source->getTypeHash() == ADDTYPE)
        return removeElementAdditively(source, target, rightToLeft);
    else
        return removeElementMultiplicatively(source, target,rightToLeft);
};

Expression getElementOfType(ExpressionObject* source, size_t type, bool rightToLeft) {
    size_t sourceType = source->getTypeHash();
    if(sourceType == NULLTYPE)
        return source;
    if(isSubtypeOf(source, type))
        return *new Expression(source);
    ExprVector elementsToCheck = *new ExprVector();
    if(sourceType == ADDTYPE) {
        Add& addObj = dynamic_cast<Add&>(*source);
        elementsToCheck = addObj.members;
    }
    if(sourceType == MULTYPE) {
        Mul& mulObj = dynamic_cast<Mul&>(*source);
        elementsToCheck = mulObj.members;
    }
    if(sourceType == FRACTYPE) {
        Frac& fracObj = dynamic_cast<Frac&>(*source);
        return getElementOfType(fracObj.numerator.get(), type, rightToLeft);
    }
    
    if(!rightToLeft) {
        for(int i = 0; i<elementsToCheck.size(); i++) {
            if(isSubtypeOf(elementsToCheck[i].get(), type) )// || (elementsToCheck[i].getTypeHash() == SIGNTYPE && isSubclassOf((-elementsToCheck[i]).getTypeHash(), type) ))
                return *new Expression(elementsToCheck[i]);
            /*Expression candidate = getElementOfType(elementsToCheck[i], type, rightToLeft);
            if(!(candidate.getTypeHash() == NULLTYPE) )
                return candidate;*/
        }
        Expression result = *new Expression(new NullObject("could not find element of specified type"));
        return result;
    } else {
        for(int i = (int)elementsToCheck.size()-1; i>=0; i--) {
            if(isSubtypeOf(elementsToCheck[i].get(), type) ) {//|| (elementsToCheck[i].getTypeHash() == SIGNTYPE && isSubclassOf((-elementsToCheck[i]).getTypeHash(), type) ))
                return *new Expression(elementsToCheck[i]);
            }
            /*Expression candidate = getElementOfType(elementsToCheck[i], type, rightToLeft);
            if(!(candidate.getTypeHash() == NULLTYPE) )
                return candidate;*/
        }
        Expression result = *new Expression(new NullObject("could not find element of specified type"));
        return result;
    }
};

//this is pointless
ExprVector replaceElementOfTypeInVector(ExprVector source, size_t type, ExpressionObject* value, bool rightToLeft) {
    ExprVector result = *new ExprVector();
    int location = positionOfType(source, type, rightToLeft);
    if(location >= 0) {
        ExprVector newMembers = *new ExprVector();//members;
        for(int i = 0; i< source.size(); i++ ) {
            if(i == location)
                newMembers.push_back(*new Expression(value));
            newMembers.push_back(source[i]);
        }
        return *new ExprVector(result);
    }
    return source;
};
ExprVector generateExprVector(std::initializer_list<Expression> elements) {
    ExprVector vecPtr = *new ExprVector(elements);
    return *new ExprVector(vecPtr);
};
ExprMatrix generateExprMatrix(std::initializer_list<std::initializer_list<Expression>> elements) {
    ExprMatrix matPtr = *new ExprMatrix();
    for(const std::initializer_list<Expression>& x : elements) {
        ExprVector objPtr = *new ExprVector(x);
        ExprVector nextColumn = *new ExprVector(objPtr);
        matPtr.push_back(nextColumn);
    }
    return *new ExprMatrix(matPtr);
};

ExprVector replaceElementInVector(ExprVector source, ExpressionObject* target, ExpressionObject* value, bool rightToLeft) {
    ExprVector result = *new ExprVector();
    int location = positionOfElement(source, target);
    if(location >= 0) {
        for(int i = 0; i< source.size(); i++ ) {
            if(i == location)
                result.push_back(*new Expression(value));
            else
                result.push_back(source[i]);
        }
        return result;
    }
    return source;
};
/*
Expression replaceElement(SymbolicObject* source, SymbolicObject* target, SymbolicObject* value, bool rightToLeft) {
    
};*/
Expression replaceElementOfType(ExpressionObject* source, size_t type, ExpressionObject* value, bool rightToLeft) {
    size_t sourceType = source->getTypeHash();
    if(isSubtypeOf(source, type))
        return *new Expression(value);
    ExprVector elementsToCheck;
    if(sourceType == ADDTYPE) {
        Add& addObj = dynamic_cast<Add&>(*source);
        elementsToCheck = addObj.members;
    }
    if(sourceType == MULTYPE) {
        Mul& mulObj = dynamic_cast<Mul&>(*source);
        elementsToCheck = mulObj.members;
    }
    if(sourceType == FRACTYPE) {
        Frac& fracObj = dynamic_cast<Frac&>(*source);
        Expression result = replaceElementOfType(fracObj.numerator.get(), type, value, rightToLeft);
        if(result == fracObj.numerator)
            return *new Expression(source);
        Expression finalResult = *new Expression(new Frac(result.get(),fracObj.denomenator.get()));
        return finalResult;
    }
    elementsToCheck = replaceElementOfTypeInVector(elementsToCheck, type, value, rightToLeft);
    /*
    if(!rightToLeft) {
        for(int i = 0; i<elementsToCheck.size(); i++) {
            if(isSubclassOf(elementsToCheck[i].getTypeHash(), type)) {
                elementsToCheck.erase(elementsToCheck.begin()+i);
                elementsToCheck.insert(elementsToCheck.begin()+i, value);
                break;
            }
            Expression newElement = replaceElementOfType(elementsToCheck[i], type, value, rightToLeft);
            if(newElement != elementsToCheck[i]) {
                elementsToCheck.erase(elementsToCheck.begin()+i);
                elementsToCheck.insert(elementsToCheck.begin()+i, newElement);
                break;
            }
        }
    } else {
        for(int i = (int)elementsToCheck.size()-1; i>=0; i--) {
            if(isSubclassOf(elementsToCheck[i].getTypeHash(), type)) {
                elementsToCheck.erase(elementsToCheck.begin()+i);
                elementsToCheck.insert(elementsToCheck.begin()+i, value);
                break;
            }
            Expression newElement = replaceElementOfType(elementsToCheck[i], type, value, rightToLeft);
            if(newElement != elementsToCheck[i]) {
                elementsToCheck.erase(elementsToCheck.begin()+i);
                elementsToCheck.insert(elementsToCheck.begin()+i, newElement);
                break;
            }
        }
    }*/
    if(sourceType == ADDTYPE) {
        Add& addObj = dynamic_cast<Add&>(*source);
        if(elementsToCheck == addObj.members)
            return source;
        Expression result = *new Expression(new Add(elementsToCheck));
        return result;
    }
    if(sourceType == MULTYPE) {
        Mul& mulObj = dynamic_cast<Mul&>(*source);
        if(elementsToCheck == mulObj.members)
            return source;
        Expression result = *new Expression(new Mul(elementsToCheck));
        return result;
    }
    return *new Expression(source);
};

bool areEqual(ExpressionObject& left, ExpressionObject& right) {
    bool areEqual = true;
    size_t leftType = typeid(left).hash_code();
    size_t rightType = typeid(right).hash_code();
    areEqual &=  (leftType == rightType);
    if(!areEqual)
        return areEqual;
    if(leftType == REALTYPE) {
        Real& leftObj = dynamic_cast<Real&>(left);
        Real& rightObj = dynamic_cast<Real&>(right);
        areEqual &= (leftObj.value == rightObj.value);
    }
    if(leftType == SYMBOLTYPE) {
        areEqual &= (left.print() == right.print());
    }
    if(leftType == ADDTYPE) {
        Add& leftObj = dynamic_cast<Add&>(left);
        Add& rightObj = dynamic_cast<Add&>(right);
        for(int i = 0; i<leftObj.members.size(); i++) {
            areEqual &= containsElement(rightObj.members, leftObj.members[i].get());
        }
    }
    if(leftType == SIGNTYPE) {
        Sign& leftObj = dynamic_cast<Sign&>(left);
        Sign& rightObj = dynamic_cast<Sign&>(right);
        areEqual &= leftObj.member == rightObj.member;
    }
    if(leftType == MULTYPE) {
        Mul& leftObj = dynamic_cast<Mul&>(left);
        Mul& rightObj = dynamic_cast<Mul&>(right);
        areEqual &= leftObj.members.size() == rightObj.members.size();
        for(int i = 0; i<leftObj.members.size(); i++) {
            areEqual &= containsElement(rightObj.members, leftObj.members[i].get());
        }
    }
    if(leftType == FRACTYPE) {
        Frac& leftObj = dynamic_cast<Frac&>(left);
        Frac& rightObj = dynamic_cast<Frac&>(right);
        areEqual &= (leftObj.numerator == rightObj.numerator && leftObj.denomenator == rightObj.denomenator);
    }
    if(leftType == EXPTYPE) {
        Exp& leftObj = dynamic_cast<Exp&>(left);
        Exp& rightObj = dynamic_cast<Exp&>(right);
        areEqual &= (leftObj.base == rightObj.base && leftObj.exponent == rightObj.exponent);
    }
    if(leftType == MATRIXTYPE) {
        Matrix& leftObj = dynamic_cast<Matrix&>(left);
        Matrix& rightObj = dynamic_cast<Matrix&>(right);
        if(leftObj.dimensions != rightObj.dimensions)
            return false;
        for(int i = 0; i<leftObj.dimensions.first; i++) {
            for(int j = 0; j<leftObj.dimensions.second; j++) {
                areEqual &= leftObj.elements[i][j] == rightObj.elements[i][j];
            }
        }
    }
    if(leftType == PAULIMATRIXTYPE) {
        PauliMatrix& leftObj = dynamic_cast<PauliMatrix&>(left);
        PauliMatrix& rightObj = dynamic_cast<PauliMatrix&>(right);
        if(leftObj.dimensions != rightObj.dimensions || leftObj.flavor != rightObj.flavor)
            return false;
        for(int i = 0; i<leftObj.dimensions.first; i++) {
            for(int j = 0; j<leftObj.dimensions.second; j++) {
                areEqual &= leftObj.elements[i][j] == rightObj.elements[i][j];
            }
        }
    }
    return areEqual;
}

std::string printExprMatrix(ExprMatrix target) {
    std::string result = "{";
    for(int i = 0; i<target.size(); i++) {
        result+="{";
        if(i!=0)
            result+=",";
        for(int j = 0; j<target[0].size(); j++) {
            if(j!=0)
                result+=",";
            result+=target[i][j].print();
        }
        result+="}";
    }
    result+="}";
    return result;
}

Expression declarePauliVector(bool up, std::string flavor) {
    if(up) {
        ExpressionObject* result = new PauliMatrix(flavor+"Up",0,flavor,{{ONE},{ZERO}});
        return declareSymbol(flavor+"Up", result);
    } else {
        ExpressionObject* result = new PauliMatrix(flavor+"Down",0,flavor,{{ZERO},{ONE}});
        return declareSymbol(flavor+"Down", result);
    }
}

bool intVectorContains(std::vector<int> container, int target) {
    bool result = false;
    for(int i = 0; i< container.size(); i++) {
        if(container[i] == target)
            return true;
        //result |= container[i] == target;
    }
    return result;
}

ExprVector combineExprVectors(ExprVector left, ExprVector right) {
    ExprVector newVector = *new ExprVector(left);
    for(int i = 0; i<right.size(); i++) {
        newVector.push_back(right[i]);
    }
    return *new ExprVector(newVector);
}

ExprVector getConstituentSymbols(ExpressionObject* target) {
    size_t targetType = target->getTypeHash();
    if(targetType == ADDTYPE) {
        ExprVector symbols = *new ExprVector;
        Add& addTarget = dynamic_cast<Add&>(*target);
        for(int i = 0; i< addTarget.members.size(); i++) {
            symbols = combineExprVectors(symbols, getConstituentSymbols(addTarget.members[i].get()));
        }
        return symbols;
    }
    if(targetType == MULTYPE) {
        ExprVector symbols = *new ExprVector;
        Mul& mulTarget = dynamic_cast<Mul&>(*target);
        for(int i = 0; i< mulTarget.members.size(); i++) {
            symbols = combineExprVectors(symbols, getConstituentSymbols(mulTarget.members[i].get()));
        }
        return symbols;
    }
    if(targetType == FRACTYPE) {
        Frac& fracTarget = dynamic_cast<Frac&>(*target);
        ExprVector symbols = combineExprVectors(getConstituentSymbols(fracTarget.numerator.get()),getConstituentSymbols(fracTarget.numerator.get()));
        return symbols;
    }
    if(targetType == EXPTYPE) {
        Exp& expTarget = dynamic_cast<Exp&>(*target);
        ExprVector symbols = combineExprVectors(getConstituentSymbols(expTarget.base.get()),getConstituentSymbols(expTarget.exponent.get()));
        return symbols;
    }
    if(targetType == MATRIXTYPE) {
        Matrix& matTarget = dynamic_cast<Matrix&>(*target);
        ExprVector symbols = *new ExprVector();
        
        for(int i = 0; i<matTarget.dimensions.first; i++) {
            for(int j = 0; j<matTarget.dimensions.second; j++) {
                symbols = combineExprVectors(symbols, getConstituentSymbols(matTarget.elements[i][j].get()));
            }
        }
        return symbols;
    }
    if(targetType == SYMBOLTYPE) {
        ExprVector symbols = *new ExprVector();
        symbols.push_back(*new Expression(target));
        return *new ExprVector(symbols);
    }
    ExprVector symbols = *new ExprVector();
    return symbols;
};

void lambdifyToFile(ExpressionObject* func, std::string funcName = "foo", std::string filePath = "") {
    std::ofstream funcFile;
    std::string fullPath = filePath+"/"+funcName+".cpp";
    funcFile.open(fullPath);
    std::string funcToPrint  = func->print();
    ExprVector variables = getConstituentSymbols(func);
    std::string variableList = "";
    for(int i = 0; i<variables.size(); i++) {
        variableList += "float "+variables[i].print();
        if(i != variables.size()-1)
            variableList += ",";
        
    }
    std::string funcLine = "float "+funcName+"("+variableList+"){return "+funcToPrint+";}";
    funcFile << funcLine;
    funcFile.close();
}
