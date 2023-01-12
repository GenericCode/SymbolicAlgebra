//
//  AbstractHelpers.cpp
//  SymbolicSummation
//
//  Created by Laina Stahulak on 6/22/22.
//

#include "AbstractHelpers.hpp"
#include "Real.hpp"
#include "Container.hpp"
#include "Symbol.hpp"
#include "PauliMatrices.hpp"
#include "AlgebraicHelpers.hpp"
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <cmath>

static std::unordered_map<String,Expression> declaredSymbols = *new std::unordered_map<String, Expression>();
static std::unordered_map<String,Expression> declaredFunctions = *new std::unordered_map<String, Expression>();

bool areSimilarTypes(Expression exprA, Expression exprB) {
    size_t subType = exprA->getTypeHash();
    if(subType == SIGNTYPE) {
        subType = (-exprA).getTypeHash();
    }
    size_t superType = exprB->getTypeHash();
    if(superType == SIGNTYPE) {
        superType = (-exprB).getTypeHash();
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

bool isTypeSimilarTo(Expression subject, size_t type) {
    size_t subType = subject->getTypeHash();
    if(subType == SIGNTYPE) {
        subType = (-subject).getTypeHash();
    }
    bool result = false;
    if(subType == type)
        result = true;
    if(type == REALTYPE && (subType == ONETYPE || subType == ZEROTYPE) )
        result = true;
    if(type == MATRIXTYPE && (subType == PAULIMATRIXTYPE || subType == EUCLIDVECTORTYPE))
        result = true;
    if(type == typeid(Container).hash_code() && (subType == ADDTYPE || subType == MULTYPE || subType == FRACTYPE || subType == EXPTYPE || subType == SIGNTYPE))
        result = true;
    return result;
}

String getStringExpressionType(String exprString) {
    int parenthesisDepth = 0;
    int sqbracketDepth = 0;
    bool containsOperator = false;
    bool containsLetter = false;
    bool containsNumber = false;
    bool containsNumberBeforeLetter = false;
    String topLevelOperator = "";
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
            case '[':
                if(sqbracketDepth != 1)
                    return "badformat";
                if(parenthesisDepth == 0 && topLevelOperator == "") {
                    containsOperator |= true;
                    topLevelOperator = "func";
                }
                break;
            case ']':
                sqbracketDepth++;
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

std::vector<String> tokenize(String expr, String delimiters) {
    std::vector<String> tokens = *new std::vector<String>();
    String currentToken = "";
    int parenthesisDepth = 0;
    for(int i = 0; i<expr.length(); i++) {
        char currChar = expr[i];
        if(currChar == '(')
            parenthesisDepth++;
        if(i == expr.length()-1) {
            if(delimiters.find(currChar) == String::npos)
                currentToken += currChar;
            tokens.push_back(currentToken);
            continue;
        }
        if(parenthesisDepth==0 && delimiters.find(currChar) != String::npos && currentToken != ""){
            tokens.push_back(currentToken);
            currentToken = "";
        } else
            currentToken += currChar;
        
        if(currChar == ')')
            parenthesisDepth--;
    }
    return tokens;
}

SignVector getTokenSigns(String expr) {
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

String sanitize(String expr) {
    String result = expr;
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
            result = result.substr(1,String::npos);
            result = result.substr(0,result.size()-1);
        }
    }
    return result;
}

void initializeDefaultSymbols(bool force) {
    if(declaredSymbols.empty() || force) {
        declaredSymbols["1"] = ONE;
        declaredSymbols["-1"] = MINUSONE;
        declaredSymbols["0"] = ZERO;
        float temp = 1;
        declaredSymbols[std::to_string(temp)] = ONE;
        temp = -1;
        declaredSymbols[std::to_string(temp)] = MINUSONE;
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

void initializeDefaultFunctions(bool force) {
    if(declaredFunctions.empty() || force) {
        declaredFunctions["TRANSPOSE"] = TRANSPOSE;
        declaredFunctions["Transpose"] = TRANSPOSE;
        declaredFunctions["transpose"] = TRANSPOSE;
    }
}

bool assignActionToFunction(String name, ExprAction action) {
    initializeDefaultFunctions();
    if(!declaredFunctions.contains(name))
        return false;
    declaredFunctions[name] = *new Expression(new Func(name, action));
    return true;
};

Expression declareFunction(String name) {
    initializeDefaultFunctions();
    if(declaredFunctions.contains(name))
        return declaredFunctions[name];
    Expression newFunc = *new Expression(new Func(name));
    declaredFunctions[name] = newFunc;
    return newFunc;
}

Expression declareFunction(String name, ExprAction action) {
    initializeDefaultFunctions();
    if(declaredFunctions.contains(name))
        return declaredFunctions[name];
    Expression newFunc = *new Expression(new Func(name,action));
    declaredFunctions[name] = newFunc;
    return newFunc;
}

Expression declareEuclidVector(std::string name, ExprVector components) {
    Expression vector = *new Expression(new EuclidVector(name,components));
    return declareSymbol(name, vector);
}

Expression declareEuclidVector(std::string name, std::initializer_list<Expression> components) {
    Expression vector = *new Expression(new EuclidVector(name,components));
    return declareSymbol(name, vector);
}

Expression declareMatrix(Expression value) {
    return declareSymbol(value->print(),value);
};

Expression declarePauliVector(bool up, String flavor) {
    if(up) {
        Expression result = new PauliMatrix(flavor+"Up",0,flavor,{{ONE},{ZERO}});
        return declareSymbol(flavor+"Up", result);
    } else {
        Expression result = new PauliMatrix(flavor+"Down",0,flavor,{{ZERO},{ONE}});
        return declareSymbol(flavor+"Down", result);
    }
}

Expression declarePauliMatrix(int index, String flavor) {
    Expression pMat = *new Expression(new PauliMatrix(index,flavor));
    return declareSymbol(flavor+std::to_string(index),pMat);
}

Expression declarePauliMatrix(Expression value) {
    return declareSymbol(value->print(), value);
}

Expression declareMatrix(String matName, ExprMatrix elements) {
    initializeDefaultSymbols();
    if(!declaredSymbols.contains(matName)) {
        Expression newMatrix = *new Expression(new Matrix(matName,elements));
        declaredSymbols[matName] = newMatrix;
    }
    return declaredSymbols[matName];
};

Expression declareMatrix(String matName, std::initializer_list<std::initializer_list<Expression>> elements) {
    initializeDefaultSymbols();
    if(!declaredSymbols.contains(matName)) {
        Expression newMatrix = *new Expression(new Matrix(matName,elements));
        declaredSymbols[matName] = newMatrix;
    }
    return declaredSymbols[matName];
};

Expression declareSymbol(String name) {
    Expression value = new NullObject("temp null obj to force creation of new symbol");
    return declareSymbol(name, value);
}

Expression declareReal(float value) {
    if(value == ceil(value)) {
        int actualVal = (int)value;
        String name = std::to_string(actualVal);
        return declareSymbol(name, new Real(actualVal) );
    }
    else {
        String name = std::to_string(value);
        return declareSymbol(name, new Real(value) );
    }
}

Expression declareSymbol(String name, Expression value) {
    initializeDefaultSymbols();
    if(!declaredSymbols.contains(name)) {
        if(value->getTypeHash() != NULLTYPE) {
            declaredSymbols[name] = value;
            return *new Expression(declaredSymbols[name].get());
        }
        declaredSymbols[name] = *new Expression(new Symbol(name));;
    }
    return *new Expression(declaredSymbols[name].get());
};

Expression parseString(String exprString) {
    String exprType = getStringExpressionType(sanitize(exprString));
    if(exprType == "badformat") {
        Expression result = *new Expression( new NullObject("improper formatting in expression string"));
        return result;
    }
    if(exprType == "add") {
        ExprVector members = *new ExprVector();
        SignVector memberSigns = getTokenSigns(exprString);
        std::vector<String> tokens = tokenize(sanitize(exprString), "+-");
        if(tokens.size() == 1) {
            return *new Expression(new Sign(parseString(tokens[0].substr(1))));
        }
        for(int i = 0; i<tokens.size(); i++) {
            String nextToken = tokens[i];
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
        std::vector<String> tokens = tokenize(sanitize(exprString), "*");
        for(int i = 0; i<tokens.size(); i++) {
            Expression next = parseString(sanitize(tokens[i]));
            members.push_back(next);
        }
        Expression result = *new Expression(new Mul(members));
        return result;
    }
    if(exprType == "frac") {
        std::vector<String> tokens = tokenize(sanitize(exprString), "/()");
        Expression numerator = parseString(tokens[0]);
        Expression denominator = parseString(tokens[1]);
        Expression result = *new Expression(new Frac(numerator,denominator));
        return result;
    }
    if(exprType == "exp") {
        std::vector<String> tokens = tokenize(sanitize(exprString), "^()");
        Expression base = parseString(tokens[0]);
        Expression exponent = parseString(tokens[1]);
        Expression result = *new Expression(new Exp(base,exponent));
        return result;
    }
    if(exprType == "func") {
        std::vector<String> tokens = tokenize(sanitize(exprString), "[]");
        Expression funcParam = parseString(tokens[1]);
        Expression bareFunc = declareFunction(tokens[0]);
        const Func& bareFuncObj = dynamic_cast<const Func&>(*bareFunc);
        return bareFuncObj.actingOn(funcParam);
    }
    if(exprType == "real") {
        String name = sanitize(exprString);
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
        String expr = sanitize(exprString);
        String realPart = "";
        String symbolPart = "";
        int i = 0;
        while(i<expr.size()) {
            if(isdigit(expr[i]))
                i++;
            else
                break;
        }
        realPart = expr.substr(0,i);
        symbolPart = expr.substr(i+1);
        Expression rePart = parseString(realPart);
        Expression symPart = parseString(symbolPart);
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

int positionOfElement(ExprVector list, Expression target, bool rightToLeft) {
    int i = 0;
    if(!rightToLeft) {
        while(i<list.size()) {
            if(list[i] == target)
                return i;
            i++;
        }
    } else {
        i = (int)list.size()-1;
        while(i>=0) {
            if(list[i] == target)
                return i;
            i--;
        }
    }
    return -1;
};

int positionOfElementIgnoringSign(ExprVector list, Expression target, bool rightToLeft) {
    int i = 0;
    if(!rightToLeft) {
        while(i<list.size()) {
            if(list[i] == target || -list[i] == target)
                return i;
            i++;
        }
    } else {
        i = (int)list.size()-1;
        while(i>=0) {
            if(list[i] == target || -list[i] == target)
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
            if(isTypeSimilarTo(list[i], type)) {
                return i;
            }
            i++;
        }
    } else {
        i = (int)list.size()-1;
        while(i>=0) {
            if(isTypeSimilarTo(list[i], type)) {
                return i;
            }
            i--;
        }
    }
    return -1;
};

bool exprVectorContainsType(ExprVector list, size_t type, bool rightToLeft) {
    return positionOfType(list, type, rightToLeft) >= 0;
}

bool exprVectorContains(ExprVector list, Expression target, bool rightToLeft) {
    return positionOfElement(list, target, rightToLeft) >= 0;
}

ExprVector removeElementFromVector(ExprVector source, Expression target, bool rightToLeft = false) {
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

Expression removeElementMultiplicatively(Expression source, Expression target, bool rightToLeft) {
    if(source == target)
        return ONE;//*new Expression(new NullObject("this is what happens when you remove something from itself!"));
    size_t type = source->getTypeHash();
    if(type == SIGNTYPE) {
        const Sign& sourceObj = dynamic_cast<const Sign&>(*source);
        Expression result = removeElementMultiplicatively(sourceObj.member, target);
        if(result.getTypeHash() == NULLTYPE)
            return result;
        return -result;
    }
    if(type == MULTYPE) {
        const Mul& sourceObj = dynamic_cast<const Mul&>(*source);
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
        const Frac& sourceObj = dynamic_cast<const Frac&>(*source);
        Expression result = removeElementMultiplicatively(sourceObj.numerator, target, rightToLeft);
        if(result.getTypeHash() == NULLTYPE) {
            Expression recip = reciprocal(target);
            result = removeElementMultiplicatively(sourceObj.denomenator, recip, rightToLeft);
            if(result.getTypeHash() == NULLTYPE)
                return result;
            if(result == ONE)
                return sourceObj.numerator;
            else
                return *new Expression(new Frac(sourceObj.numerator,result));
        }
        return *new Expression(new Frac(result,sourceObj.denomenator));
    }
    if(type == EXPTYPE) {
        const Exp& sourceObj = dynamic_cast<const Exp&>(*source);
        if(sourceObj.base == target) {
            Expression expOneDown = sourceObj.exponent-ONE;
            expOneDown = simplify(expOneDown);
            if(expOneDown == ONE)
                return sourceObj.base;
            if(expOneDown == ZERO)
                return ONE;
            return *new Expression(new Exp(sourceObj.base,expOneDown));
        }
        return *new Expression(new NullObject("could not find target to remove"));
    }
    if(target == source)
        return ONE;
    return *new Expression(new NullObject("could not remove target multiplicatively"));
};
Expression removeElementAdditively(Expression source, Expression target, bool rightToLeft) {
    if(source == target)
        return ZERO;//*new Expression(new NullObject("this is what happens when you remove something from itself!"));
    if(source->getTypeHash() != ADDTYPE)
        return *new Expression(new NullObject("could not remove target additively"));
    const Add& sourceObj = dynamic_cast<const Add&>(*source);
    ExprVector newMembers = removeElementFromVector(sourceObj.members, target);
    if(newMembers == sourceObj.members)
        return *new Expression(new NullObject("could not find target to remove"));
    if(newMembers.size() == 1)
        return newMembers[0];
    if(newMembers.size() == 0)
        return ZERO;
    return *new Expression(new Add(*new ExprVector(newMembers)));
};
Expression removeElementAbsolutely(Expression source, Expression target, bool rightToLeft) {
    if(source->getTypeHash() == ADDTYPE)
        return removeElementAdditively(source, target, rightToLeft);
    else
        return removeElementMultiplicatively(source, target,rightToLeft);
};

Expression getElementOfType(Expression source, size_t type, bool rightToLeft) {
    size_t sourceType = source->getTypeHash();
    if(sourceType == NULLTYPE)
        return source;
    if(isTypeSimilarTo(source, type))
        return *new Expression(source);
    ExprVector elementsToCheck = *new ExprVector();
    if(sourceType == ADDTYPE) {
        const Add& addObj = dynamic_cast<const Add&>(*source);
        elementsToCheck = addObj.members;
    }
    if(sourceType == MULTYPE) {
        const Mul& mulObj = dynamic_cast<const Mul&>(*source);
        elementsToCheck = mulObj.members;
    }
    if(sourceType == FRACTYPE) {
        const Frac& fracObj = dynamic_cast<const Frac&>(*source);
        return getElementOfType(fracObj.numerator, type, rightToLeft);
    }
    
    if(!rightToLeft) {
        for(int i = 0; i<elementsToCheck.size(); i++) {
            if(isTypeSimilarTo(elementsToCheck[i], type) )
                return *new Expression(elementsToCheck[i]);
        }
        Expression result = *new Expression(new NullObject("could not find element of specified type"));
        return result;
    } else {
        for(int i = (int)elementsToCheck.size()-1; i>=0; i--) {
            if(isTypeSimilarTo(elementsToCheck[i], type) ) {
                return *new Expression(elementsToCheck[i]);
            }
        }
        Expression result = *new Expression(new NullObject("could not find element of specified type"));
        return result;
    }
};

Expression getElementMatchingCondition(Expression source, std::function<bool(Expression)> condition, bool rightToLeft) {
    size_t sourceType = source->getTypeHash();
    if(sourceType == NULLTYPE)
        return source;
    if(condition(source))
        return *new Expression(source);
    ExprVector elementsToCheck = *new ExprVector();
    if(sourceType == ADDTYPE) {
        const Add& addObj = dynamic_cast<const Add&>(*source);
        elementsToCheck = addObj.members;
    }
    if(sourceType == MULTYPE) {
        const Mul& mulObj = dynamic_cast<const Mul&>(*source);
        elementsToCheck = mulObj.members;
    }
    if(sourceType == FRACTYPE) {
        const Frac& fracObj = dynamic_cast<const Frac&>(*source);
        return getElementMatchingCondition(fracObj.numerator, condition, rightToLeft);
    }
    
    if(!rightToLeft) {
        for(int i = 0; i<elementsToCheck.size(); i++) {
            if(condition(elementsToCheck[i]) )
                return *new Expression(elementsToCheck[i]);
        }
        Expression result = *new Expression(new NullObject("could not find element matching condition"));
        return result;
    } else {
        for(int i = (int)elementsToCheck.size()-1; i>=0; i--) {
            if(condition(elementsToCheck[i]) ) {
                return *new Expression(elementsToCheck[i]);
            }
        }
        Expression result = *new Expression(new NullObject("could not find element matching condition"));
        return result;
    }
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

ExprVector replaceElementOfTypeInVector(ExprVector source, size_t type, Expression value, bool rightToLeft) {
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
    return *new ExprVector(elements);
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

ExprVector replaceElementInVector(ExprVector source, Expression target, Expression value, bool rightToLeft) {
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

Expression replaceElement(Expression source, Expression target, Expression value, bool rightToLeft) {
    size_t sourceType = source->getTypeHash();
    if(source == target)
        return *new Expression(value);
    ExprVector elementsToCheck;
    if(sourceType == ADDTYPE) {
        const Add& addObj = dynamic_cast<const Add&>(*source);
        elementsToCheck = addObj.members;
    }
    if(sourceType == MULTYPE) {
        const Mul& mulObj = dynamic_cast<const Mul&>(*source);
        elementsToCheck = mulObj.members;
    }
    if(sourceType == FRACTYPE) {
        const Frac& fracObj = dynamic_cast<const Frac&>(*source);
        Expression result = replaceElement(fracObj.numerator, target, value, rightToLeft);
        if(result == fracObj.numerator)
            return *new Expression(source);
        Expression finalResult = *new Expression(new Frac(result,fracObj.denomenator));
        return finalResult;
    }
    elementsToCheck = replaceElementInVector(elementsToCheck, target, value, rightToLeft);
    if(sourceType == ADDTYPE) {
        const Add& addObj = dynamic_cast<const Add&>(*source);
        if(elementsToCheck == addObj.members)
            return source;
        Expression result = *new Expression(new Add(elementsToCheck));
        return result;
    }
    if(sourceType == MULTYPE) {
        const Mul& mulObj = dynamic_cast<const Mul&>(*source);
        if(elementsToCheck == mulObj.members)
            return source;
        Expression result = *new Expression(new Mul(elementsToCheck));
        return result;
    }
    return *new Expression(source);
};
Expression replaceElementOfType(Expression source, size_t type, Expression value, bool rightToLeft) {
    size_t sourceType = source->getTypeHash();
    if(isTypeSimilarTo(source, type))
        return *new Expression(value);
    ExprVector elementsToCheck;
    if(sourceType == ADDTYPE) {
        const Add& addObj = dynamic_cast<const Add&>(*source);
        elementsToCheck = addObj.members;
    }
    if(sourceType == MULTYPE) {
        const Mul& mulObj = dynamic_cast<const Mul&>(*source);
        elementsToCheck = mulObj.members;
    }
    if(sourceType == FRACTYPE) {
        const Frac& fracObj = dynamic_cast<const Frac&>(*source);
        Expression result = replaceElementOfType(fracObj.numerator, type, value, rightToLeft);
        if(result == fracObj.numerator)
            return *new Expression(source);
        Expression finalResult = *new Expression(new Frac(result,fracObj.denomenator));
        return finalResult;
    }
    elementsToCheck = replaceElementOfTypeInVector(elementsToCheck, type, value, rightToLeft);
    if(sourceType == ADDTYPE) {
        const Add& addObj = dynamic_cast<const Add&>(*source);
        if(elementsToCheck == addObj.members)
            return source;
        Expression result = *new Expression(new Add(elementsToCheck));
        return result;
    }
    if(sourceType == MULTYPE) {
        const Mul& mulObj = dynamic_cast<const Mul&>(*source);
        if(elementsToCheck == mulObj.members)
            return source;
        Expression result = *new Expression(new Mul(elementsToCheck));
        return result;
    }
    return *new Expression(source);
};

bool areEqual(const ExpressionObject& left, const ExpressionObject& right) {
    bool areEqual = true;
    size_t leftType = typeid(left).hash_code();
    size_t rightType = typeid(right).hash_code();
    areEqual &=  (leftType == rightType);
    if(!areEqual)
        return areEqual;
    if(leftType == REALTYPE) {
        const Real& leftObj = dynamic_cast<const Real&>(left);
        const Real& rightObj = dynamic_cast<const Real&>(right);
        areEqual &= (leftObj.value == rightObj.value);
    }
    if(leftType == SYMBOLTYPE) {
        areEqual &= (left.print() == right.print());
    }
    if(leftType == ADDTYPE) {
        const Add& leftObj = dynamic_cast<const Add&>(left);
        const Add& rightObj = dynamic_cast<const Add&>(right);
        for(int i = 0; i<leftObj.members.size(); i++) {
            areEqual &= exprVectorContains(rightObj.members, leftObj.members[i]);
        }
    }
    if(leftType == SIGNTYPE) {
        const Sign& leftObj = dynamic_cast<const Sign&>(left);
        const Sign& rightObj = dynamic_cast<const Sign&>(right);
        areEqual &= leftObj.member == rightObj.member;
    }
    if(leftType == MULTYPE) {
        const Mul& leftObj = dynamic_cast<const Mul&>(left);
        const Mul& rightObj = dynamic_cast<const Mul&>(right);
        areEqual &= leftObj.members.size() == rightObj.members.size();
        for(int i = 0; i<leftObj.members.size(); i++) {
            areEqual &= exprVectorContains(rightObj.members, leftObj.members[i]);
        }
    }
    if(leftType == FRACTYPE) {
        const Frac& leftObj = dynamic_cast<const Frac&>(left);
        const Frac& rightObj = dynamic_cast<const Frac&>(right);
        areEqual &= (leftObj.numerator == rightObj.numerator && leftObj.denomenator == rightObj.denomenator);
    }
    if(leftType == EXPTYPE) {
        const Exp& leftObj = dynamic_cast<const Exp&>(left);
        const Exp& rightObj = dynamic_cast<const Exp&>(right);
        areEqual &= (leftObj.base == rightObj.base && leftObj.exponent == rightObj.exponent);
    }
    if(leftType == FUNCTYPE) {
        const Func& leftObj = dynamic_cast<const Func&>(left);
        const Func& rightObj = dynamic_cast<const Func&>(right);
        areEqual &= (leftObj.member == rightObj.member && &leftObj.functionAction == &rightObj.functionAction);
    }
    if(leftType == MATRIXTYPE) {
        const Matrix& leftObj = dynamic_cast<const Matrix&>(left);
        const Matrix& rightObj = dynamic_cast<const Matrix&>(right);
        if(leftObj.dimensions != rightObj.dimensions)
            return false;
        for(int i = 0; i<leftObj.dimensions.first; i++) {
            for(int j = 0; j<leftObj.dimensions.second; j++) {
                areEqual &= leftObj.elements[i][j] == rightObj.elements[i][j];
            }
        }
    }
    if(leftType == PAULIMATRIXTYPE) {
        const PauliMatrix& leftObj = dynamic_cast<const PauliMatrix&>(left);
        const PauliMatrix& rightObj = dynamic_cast<const PauliMatrix&>(right);
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

String printExprMatrix(ExprMatrix target) {
    String result = "{";
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

ExprVector getConstituentSymbols(Expression target) {
    size_t targetType = target->getTypeHash();
    if(targetType == ADDTYPE) {
        ExprVector symbols = *new ExprVector;
        const Add& addTarget = dynamic_cast<const Add&>(*target);
        for(int i = 0; i< addTarget.members.size(); i++) {
            symbols = combineExprVectors(symbols, getConstituentSymbols(addTarget.members[i]));
        }
        return symbols;
    }
    if(targetType == MULTYPE) {
        ExprVector symbols = *new ExprVector;
        const Mul& mulTarget = dynamic_cast<const Mul&>(*target);
        for(int i = 0; i< mulTarget.members.size(); i++) {
            symbols = combineExprVectors(symbols, getConstituentSymbols(mulTarget.members[i]));
        }
        return symbols;
    }
    if(targetType == FRACTYPE) {
        const Frac& fracTarget = dynamic_cast<const Frac&>(*target);
        ExprVector symbols = combineExprVectors(getConstituentSymbols(fracTarget.numerator),getConstituentSymbols(fracTarget.numerator));
        return symbols;
    }
    if(targetType == EXPTYPE) {
        const Exp& expTarget = dynamic_cast<const Exp&>(*target);
        ExprVector symbols = combineExprVectors(getConstituentSymbols(expTarget.base),getConstituentSymbols(expTarget.exponent));
        return symbols;
    }
    if(targetType == MATRIXTYPE) {
        const Matrix& matTarget = dynamic_cast<const Matrix&>(*target);
        ExprVector symbols = *new ExprVector();
        
        for(int i = 0; i<matTarget.dimensions.first; i++) {
            for(int j = 0; j<matTarget.dimensions.second; j++) {
                symbols = combineExprVectors(symbols, getConstituentSymbols(matTarget.elements[i][j]));
            }
        }
        return symbols;
    }
    if(targetType == SYMBOLTYPE) {
        ExprVector symbols = *new ExprVector();
        symbols.push_back(target);
        return *new ExprVector(symbols);
    }
    ExprVector symbols = *new ExprVector();
    return symbols;
};

void lambdifyToFile(Expression func, String funcName = "foo", String filePath = "") {
    std::ofstream funcFile;
    String fullPath = filePath+"/"+funcName+".cpp";
    funcFile.open(fullPath);
    String funcToPrint  = func->print();
    ExprVector variables = getConstituentSymbols(func);
    String variableList = "";
    for(int i = 0; i<variables.size(); i++) {
        variableList += "float "+variables[i].print();
        if(i != variables.size()-1)
            variableList += ",";
        
    }
    String funcLine = "float "+funcName+"("+variableList+"){return "+funcToPrint+";}";
    funcFile << funcLine;
    funcFile.close();
}
