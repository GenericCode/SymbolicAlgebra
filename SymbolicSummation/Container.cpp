//
//  Operator.cpp
//  SymbolicSummation
//
//  Created by Laina Stahulak on 1/29/22.
//

#include "Container.hpp"
#include "AlgebraicHelpers.hpp"
#include "AbstractHelpers.hpp"
#include "Real.hpp"
#include "Symbol.hpp"
#include "PauliMatrices.hpp"

//Container
Container::~Container() {
    //delete &name;
}
Expression Container::add(Expression other) const {
    Expression thisExpr = *new Expression(this);
    if(thisExpr == other) {
        if(isNegative(thisExpr))
            return -2*(-thisExpr);
        return 2*thisExpr;
    }
    if(other->getTypeHash() == ZEROTYPE)
        return thisExpr;
    if(getTypeHash() == ADDTYPE && other.getTypeHash() == ADDTYPE ) {
        const Add& thisAdd = dynamic_cast<const Add&>(*thisExpr);
        const Add& otherAdd = dynamic_cast<const Add&>(*other);
        return *new Expression(new Add(setUnion(thisAdd.getMembers(), otherAdd.getMembers())));
    }
    if(getTypeHash() == ADDTYPE) {
        const Add& thisAdd = dynamic_cast<const Add&>(*thisExpr);
        ExprVector newMembers = thisAdd.getMembers();
        newMembers.push_back(other);
        return *new Expression(new Add(newMembers));
    }
    if(other.getTypeHash() == ADDTYPE) {
        const Add& otherAdd = dynamic_cast<const Add&>(*other);
        ExprVector newMembers = *new ExprVector();
        newMembers.push_back(thisExpr);
        newMembers = setUnion(newMembers, otherAdd.getMembers());
        return *new Expression(new Add(newMembers));
    }
    return *new Expression(new Add(thisExpr,other));
};
Expression Container::multiply(Expression other) const {
    if(other->getTypeHash() == ZEROTYPE)
        return ZERO;
    if(other->getTypeHash() == ONETYPE)
        return *new Expression(this);
    return distribute(other);
};
Expression Container::divide(Expression other) const {
    Expression thisExpr = *new Expression(this);
    if(other == thisExpr) {
        return ONE;
    }
    ExprVector factors = getFactors();
    ExprVector otherFactors = other.getFactors();
    ExprVector diffAB = setDifference(factors, otherFactors);
    ExprVector diffBA = setDifference(otherFactors, factors);
    if( diffAB.size() == 0 ) {
        if(diffBA.size() == 0)
            return ONE;
        if(diffBA.size() == 1)
            return diffBA[0].reciprocal();
        Expression denomenator = *new Expression(new Mul(diffBA));
        Expression result = *new Expression(new Frac(denomenator));
        return result.simplify();
    }
    if( diffBA.size() == 0 ) {
        if(diffAB.size() == 1)
            return diffAB[0];
        Expression result = *new Expression(new Mul(diffAB));
        return result.simplify();
    }
    Expression numerator = (diffAB.size() == 1) ? diffAB[0] : *new Expression(new Mul(diffAB));
    Expression denomenator = (diffBA.size() == 1) ? diffBA[0] : *new Expression(new Mul(diffBA));
    return *new Expression(new Frac(numerator,denomenator));
};
Expression Container::subtract(Expression other) const {
    Expression thisExpr = *new Expression(this);
    if(thisExpr == other)
        return ZERO;
    Expression negativeOf = -other;
    return add(negativeOf);
};

Expression Container::negate() const {
    return *new Expression(new Sign(*new Expression(this)));
};
//Add
Add::~Add() {
    //delete &members;
    //delete &name;
}

Add& Add::operator=(const Add &target) {
    if(this == &target)
        return *this;
    members = *new ExprVector(target.getMembers());
    name = target.name;
    return *this;
};
Add::Add(const Add& target) {
    members = *new ExprVector(target.getMembers());
    name = target.name;
}

Add::Add(std::initializer_list<Expression> newMembers) {
    members = generateExprVector(newMembers);
    name = (*this).print();
};

Add::Add(ExprVector newMembers) {
    //ExprVector newMembersObject = *new ExprVector(*newMembers);
    members = *new ExprVector(newMembers);
    name = (*this).print();
};

Add::Add(Expression left, Expression right) {
    Expression newMem = *new Expression(left);
    members.push_back(newMem);
    newMem = *new Expression(right);
    members.push_back(newMem);
    name = (*this).print();
};
Expression Add::negate() const {
    ExprVector newMembers = *new ExprVector(members);
    for(size_t i = 0; i<newMembers.size(); i++) {
        newMembers[i] = -newMembers[i];
    }
    Expression result = *new Expression(new Add(newMembers));
    return result;
};

String Add::print() const {
    String result = "";
    //std::cout << members.size();
    for(size_t i = 0; i<members.size(); i++) {
        Expression next = members[i];
        if(i>0 && next.getTypeHash() != SIGNTYPE)
            result+="+";
        bool nextIsNegative = isNegative(next);
        if(nextIsNegative && next.getTypeHash() != SIGNTYPE)
            result+="(";
        result+=next.print();
        if(nextIsNegative && next.getTypeHash() != SIGNTYPE)
            result+=")";
    }
    return result;
};

Expression Add::simplify() const {
    ExprVector simplifiedMembers = *new ExprVector();
    for(size_t i = 0; i< members.size(); i++) {
        Expression nextMem = members[i].simplify();
        if(nextMem != ZERO)
            simplifiedMembers.push_back(nextMem);
    }
    if(exprVectorContainsType(simplifiedMembers, ADDTYPE)) {
        ExprVector newerMembers = *new ExprVector();
        for(int i=0; i<simplifiedMembers.size(); i++) {
            if(simplifiedMembers[i].getTypeHash() == ADDTYPE) {
                const Add& subAdd = dynamic_cast<const Add&>(*simplifiedMembers[i]);
                newerMembers = setUnion(newerMembers, subAdd.getMembers());
            } else {
                newerMembers.push_back(simplifiedMembers[i]);
            }
        }
        simplifiedMembers = newerMembers;
    }
    const Add& newAdd = *new Add(simplifiedMembers);
    return newAdd.cancelTerms();
};
Expression Add::distribute(Expression other) const {
    ExprVector newMembers = *new ExprVector();
    for(size_t i = 0; i< members.size(); i++)
        newMembers.push_back(members[i].distribute(other));
    return *new Expression(new Add(newMembers));
};
Expression Add::factor() const {
    ExprVector factors = getFactors();
    if(factors.size() == 1)
        return *new Expression(this);
    return *new Expression(new Mul(factors));

};
Expression Add::reciprocal() const {
    Expression thisExpression = *new Expression(this);
    return *new Expression(new Frac(thisExpression));
};
Expression Add::determinant() const {
    Expression simpled = simplify();
    if(simpled.getTypeHash() == ADDTYPE)
        throw std::logic_error("no general formula for determinant of uncombined sum");
    return simpled.determinant();
};
Expression Add::transpose() const {
    ExprVector newMembers = *new ExprVector();
    for(size_t i = 0; i<members.size(); i++) {
        newMembers.push_back(members[i].transpose());
    }
    return *new Expression(new Add(newMembers));
};
Expression Add::cancelTerms() const {
    std::vector<int> accountedFor = *new std::vector<int>();
    ExprVector newMembers = *new ExprVector();
    for(size_t i = 0; i<members.size(); i++) {
        if(intVectorContains(accountedFor, i))
            continue;
        Expression runningSum = members[i];
        accountedFor.push_back(i);
        for(size_t j =i; j<members.size(); j++) {
            if(intVectorContains(accountedFor, j))
                continue;
            Expression currExpr = members[j];
            if(currExpr == ZERO) {
                accountedFor.push_back(j);
                continue;
            }
            if(currExpr == runningSum) {
                if(isNegative(runningSum))
                    runningSum = -2*(-runningSum);
                else
                    runningSum = 2*runningSum;
                accountedFor.push_back(j);
                continue;
            }
            if(currExpr == -runningSum || -currExpr == runningSum) {
                accountedFor.push_back(j);
                runningSum = ZERO;
                break;
            }
                
            ExprVector currCommonFactors = commonFactors({runningSum,currExpr});
            if(currCommonFactors.size() == 0)
                continue;
            Expression inCommon = ONE;
            Expression sumNotInCommon = runningSum;
            Expression otherNotInCommon = currExpr;
            for(int k = 0; k<currCommonFactors.size(); k++) {
                inCommon = inCommon*currCommonFactors[k];
                sumNotInCommon = sumNotInCommon/currCommonFactors[k];
                otherNotInCommon = otherNotInCommon/currCommonFactors[k];
            }
            //if(isTypeSimilarTo(sumNotInCommon, CONTAINERTYPE) || isTypeSimilarTo(otherNotInCommon, CONTAINERTYPE) || !areSimilarTypes(sumNotInCommon, otherNotInCommon))
            //    continue;
            //There are no cases I can think of where this will break commutators,
            //lack of evidence is not evidence of lack however.
            Expression testCombine = sumNotInCommon+otherNotInCommon;
            if(testCombine.getTypeHash() == ADDTYPE)
                continue;
            runningSum = testCombine*inCommon;
            accountedFor.push_back(j);
        }
        if(runningSum != ZERO)
            newMembers.push_back(runningSum);
    }
    if(newMembers.size() == 0)
        return ZERO;
    return *new Expression(new Add(newMembers));
};
ExprVector Add::getFactors() const {
    Expression thisExpr = *new Expression(this);
    ExprVector factors = commonFactors(members);
    if (factors.size() == 0)
        return { thisExpr };
    ExprVector memberRemainders = members;
    for (int i = 0; i < members.size(); i++) {
        for (int j = 0; j < factors.size(); j++) {
            memberRemainders[i] = cancelFactor(memberRemainders[i], factors[j]);
        }
    }
    Expression remainderSum = *new Expression(new Add(memberRemainders));
    factors.push_back(remainderSum);
    return factors;
};

//Sign

Sign::~Sign() {
    delete &member;
    delete &name;
}

Expression Sign::negate() const {
    return member;
};

Expression Sign::add(Expression other) const {
    if(other->getTypeHash() == SIGNTYPE) {
        const Sign& otherSign = dynamic_cast<const Sign&>(*other);
        Expression result = member+otherSign.member;
        return -result;
    }
    if(other->getTypeHash() == ZEROTYPE)
        return *new Expression(this);
    return -(member-other);
}

Expression Sign::subtract(Expression other) const {
    if(other->getTypeHash() == ZEROTYPE)
        return *new Expression(this);
    Expression result = member+other;
    return -result;
}

Expression Sign::multiply(Expression other) const {
    if(other->getTypeHash() == SIGNTYPE) {
        const Sign& otherSign = dynamic_cast<const Sign&>(*other);
        Expression result = member*otherSign.member;
        return result;
    }
    if(other->getTypeHash() == ZEROTYPE)
        return ZERO;
    if(other->getTypeHash() == ONETYPE)
        return *new Expression(this);
    return -(member*other);
}

String Sign::print() const {
    String result = "-";
    result += member.print();
    return result;
};

Expression Sign::simplify() const {
    return -member.simplify();
};

Expression Sign::distribute(Expression other) const {
    return -member.distribute(other);
};

Expression Sign::factor() const {
    return -member.factor();
};

Expression Sign::reciprocal() const {
    return -member.reciprocal();
};
Expression Sign::determinant() const {
    return -member.determinant();
};
Expression Sign::transpose() const {
    return -member.transpose();
};
ExprVector Sign::getFactors() const {
    ExprVector factors = *new ExprVector();
    factors.push_back(MINUSONE);
    factors = setUnion(factors, member.getFactors());
    return factors;
};

Sign::Sign(const Sign& target) {
    member = *new Expression(target.member);
};
const Sign& Sign::operator=(const Sign& target) {
    if(this == &target)
        return *this;
    member = *new Expression(target.member);
    return *this;
};
Sign::Sign(Expression expr) {
    (*this).member = *new Expression(expr);
    (*this).name = "-"+member.print();
};
//Mul
Mul::~Mul() {
    //delete &members;
    //delete &name;
}
Mul& Mul::operator=(const Mul &target) {
    if(this == &target)
        return *this;
    members = *new ExprVector(target.getMembers());
    return *this;
};
Mul::Mul(const Mul& target) {
    members = *new ExprVector(target.getMembers());
    name = target.name;
};
Mul::Mul(ExprVector newMembers) {
    if (newMembers.size() <= 1) {
        std::cout << "blah";
    }
    members = *new ExprVector(newMembers);
    name = (*this).print();
};
Mul::Mul(Expression right, Expression left) {
    members.push_back(*new Expression(right));
    members.push_back(*new Expression(left));
    name = (*this).print();
};
Expression Mul::negate() const {
    Expression thisExpr = *new Expression(this);
    Expression negateTarget = getElementOfType(thisExpr, SIGNTYPE);//this.getFirstInstanceOfType(SIGNTYPE);
    if(negateTarget.getTypeHash() == NULLTYPE) {
        negateTarget = getElementOfType(thisExpr, REALTYPE);
        if(negateTarget.getTypeHash() == NULLTYPE) {
            Expression result = *new Expression(new Sign(thisExpr));
            return result;
        }
    }
    Expression negatedTarget = -negateTarget;
    ExprVector newMembers = replaceElementInVector(members, negateTarget, negatedTarget);
    Expression result = *new Expression(new Mul(newMembers));
    return result;
};

String Mul::print() const {
    String result = "";
    for(size_t i = 0; i<members.size(); i++) {
        if(i>0)
            result+="*";
        if(members[i].getTypeHash() == ADDTYPE)
            result+="(";
        result+=members[i].print();
        if(members[i].getTypeHash() == ADDTYPE)
            result+=")";
    }
    return result;
};

Expression simplifyMulWithPauliMatrices(Expression target) {
    Expression total;
    Expression remainder = target;
    Expression firstPauli = getElementOfType(target, PAULIMATRIXTYPE);
    if(firstPauli.getTypeHash() == NULLTYPE)
        return target;
    total = ONE;
    
    while(firstPauli.getTypeHash() != NULLTYPE && remainder != ZERO && remainder != ONE) {
        remainder = removeElementMultiplicatively(remainder, firstPauli);//cancelFactor(remainder, firstPauli);
        Expression secondPauli = getMatrixMatchingPauliFlavor(remainder, firstPauli);
        Expression tempResult = firstPauli;
        while(secondPauli.getTypeHash() != NULLTYPE && remainder != ZERO && remainder != ONE) {
            remainder = removeElementMultiplicatively(remainder, secondPauli);//cancelFactor(remainder, secondPauli);//remainder = remainder.remove(secondPauli);
            tempResult = tempResult*secondPauli;
            secondPauli = getMatrixMatchingPauliFlavor(remainder, firstPauli);
        }
        
        firstPauli = getElementOfType(remainder, PAULIMATRIXTYPE);
        total = total*tempResult;
    }
    if(remainder.getTypeHash() != ZEROTYPE) {
        total = total*remainder;
    }
    if(total.getTypeHash() == MULTYPE) {
        const Mul& mulObj = dynamic_cast<const Mul&>(*total);
        if(mulObj.getMembers().size() == 0)
            return ZERO;
        if(mulObj.getMembers().size() == 1)
            return mulObj.getMembers()[0];
    }
    if(total.getTypeHash() == ADDTYPE) {
        const Add& addObj = dynamic_cast<const Add&>(*total);
        if(addObj.getMembers().size() == 0)
            return ZERO;
        if(addObj.getMembers().size() == 1) {
            return addObj.getMembers()[0];
        }
    }
    return total;
}

Expression Mul::simplify() const {
    ExprVector newMembers = *new ExprVector();
    bool signOfThis = false;
    for(size_t i = 0; i< members.size(); i++)
        newMembers.push_back(members[i].simplify());
    ExprVector newerMembers = *new ExprVector();
    for(int i=0; i<newMembers.size(); i++) {
        if(newMembers[i].getTypeHash() == MULTYPE) {
            const Mul& subMul = dynamic_cast<const Mul&>(*newMembers[i]);
            newerMembers = setUnion(newerMembers, subMul.getMembers());
        } else if( isNegative(newMembers[i]) ) {
            newerMembers.push_back(-newMembers[i]);
            signOfThis = signOfThis != true;
        } else {
            newerMembers.push_back(newMembers[i]);
        }
    }
    newMembers = newerMembers;
    Expression result = *new Expression(new Mul(newMembers));
    std::vector<size_t> types = {ADDTYPE,FRACTYPE,EXPTYPE,SYMBOLTYPE,EUCLIDVECTORTYPE,PAULIMATRIXTYPE,MATRIXTYPE,IMAGINARYUNITTYPE,REALTYPE};
    std::function<bool(Expression)> checker;
    for(size_t type : types) {
        if(type == PAULIMATRIXTYPE) {
            result = simplifyMulWithPauliMatrices(result);
        }
        else {//all cases but simplifying pauli matrices
            Expression first = getElementOfType(result, type);
            if(first.getTypeHash() == NULLTYPE)
                continue;
            Expression remainder;// = result.remove(first);
            remainder = removeElementMultiplicatively(result, first);//cancelFactor(result, first);
            Expression second = getElementOfType(remainder, type);//remainder.getFirstInstanceOfType(type);
            Expression total = first;
            while(second.getTypeHash() != NULLTYPE && remainder != ONE) {
                remainder = removeElementMultiplicatively(remainder, second);//cancelFactor(remainder, second);//remainder = remainder.remove(second);
                total = total*second;
                second = getElementOfType(remainder, type);//remainder.getFirstInstanceOfType(type);
            }
            //if(tempResult.getTypeHash() == NULLTYPE)
            //    tempResult = first;
            if(remainder.getTypeHash() != NULLTYPE) {
                result = total*remainder;//distribute(first, remainder);
            } else
                result = total;
        }
    }
    if(signOfThis) {
        if(result.getTypeHash() != MULTYPE)
            return -result.simplify();
        return -result;
    }
    if(result.getTypeHash() != MULTYPE)
        return result.simplify();
    return result;
};
Expression Mul::distribute(Expression other) const {
    if(other.getTypeHash() == MULTYPE) {
        const Mul& otherMul = dynamic_cast<const Mul&>(*other);
        return *new Expression(new Mul(setUnion(members, otherMul.getMembers())));
    }
    if(other.getTypeHash() == ADDTYPE) {
        const Add& otherAdd= dynamic_cast<const Add&>(*other);
        ExprVector newMembers = *new ExprVector();
        for(size_t i = 0; i< otherAdd.getMembers().size(); i++) {
            newMembers.push_back(distribute(otherAdd.getMembers()[i]).simplify());
        }
        return *new Expression(new Add(newMembers));
    }
    ExprVector newMembers = *new ExprVector(members);
    newMembers.push_back(other);
    return *new Expression(new Mul(newMembers));
};
Expression Mul::factor() const {
    ExprVector newMembers = *new ExprVector();
    for(size_t i = 0; i< members.size(); i++) {
        Expression memberFactored = members[i].factor();
        newMembers.push_back(memberFactored);
    }
    Expression newMul = *new Expression(new Mul(newMembers));
    return newMul;
};
Expression Mul::reciprocal() const {
    ExprVector newMembers = *new ExprVector();
    for(size_t i = (int)members.size()-1; i>=0; i--) {
        newMembers.push_back(members[i].reciprocal());
    }
    Expression newMul = *new Expression(new Mul(newMembers));
    return newMul;
};
Expression Mul::determinant() const {
    ExprVector newMembers = *new ExprVector();
    for(size_t i = 0; i<members.size(); i++) {
        newMembers.push_back(members[i].determinant());
    }
    Expression newMul = *new Expression(new Mul(newMembers));
    return newMul;
};
Expression Mul::transpose() const {
    ExprVector newMembers = *new ExprVector();
    for(size_t i = (int)members.size()-1; i>=0; i--) {
        newMembers.push_back(members[i].transpose());
    }
    Expression newMul = *new Expression(new Mul(newMembers));
    return newMul;
};
ExprVector Mul::getFactors() const {
    ExprVector factors = *new ExprVector();
    for(size_t i = 0; i<members.size(); i++) {
        factors = setUnion(factors, members[i].getFactors());
    }
    return factors;
};
//Frac
Frac::~Frac() {
    delete &numerator;
    delete &denomenator;
    delete &name;
}
Frac& Frac::operator=(const Frac &target) {
    if(this == &target)
        return *this;
    numerator = *new Expression(target.numerator);
    denomenator = *new Expression(target.denomenator);
    return *this;
};
Frac::Frac(const Frac& target) {
    numerator = *new Expression(target.numerator);
    denomenator = *new Expression(target.denomenator);
    name = target.name;
};

Frac::Frac(Expression denom) {
    numerator = ONE;
    denomenator = *new Expression(denom);
    name = (*this).print();
};
Frac::Frac(Expression num, Expression denom) {
    numerator = *new Expression(num);
    denomenator = *new Expression(denom);
    name = (*this).print();
};
Expression Frac::negate() const {
    Expression negativeOf = -numerator;
    Expression result = *new Expression(new Frac(negativeOf,denomenator));
    return result;
};

String Frac::print() const {
    String result = "(";
    result += numerator.print();
    result += ")/(";
    result += denomenator.print();
    result += ")";
    return result;
};
Expression Frac::simplify() const {
    return *new Expression(new Frac(numerator.simplify(),denomenator.simplify()));
};
Expression Frac::distribute(Expression other) const {
    if(other.getTypeHash() != FRACTYPE)
        return *new Expression(new Frac(numerator.distribute(other),denomenator));
    const Frac& otherFrac = dynamic_cast<const Frac&>(*other);
    return *new Expression(new Frac(numerator.distribute(otherFrac.numerator),denomenator.distribute(otherFrac.denomenator)));
};
Expression Frac::factor() const {
    return *new Expression(new Frac(numerator.factor(),denomenator.factor()));

};
Expression Frac::reciprocal() const {
    if(numerator == ONE)
        return denomenator;
    return *new Expression(new Frac(denomenator,numerator));
};
Expression Frac::determinant() const {
    return *new Expression(new Frac(numerator.determinant(),denomenator.transpose()));
};
Expression Frac::transpose() const {
    return *new Expression(new Mul(denomenator.transpose().reciprocal(),numerator.transpose()));
};
ExprVector Frac::getFactors() const {
    ExprVector numFactors = numerator.getFactors();
    ExprVector denomFactors = denomenator.getFactors();
    for(size_t i = 0; i<denomFactors.size(); i++) {
        denomFactors[i] = denomFactors[i].reciprocal();
    }
    return setUnion(numFactors, denomFactors);
};

//Exp
Exp::~Exp() {
    delete &exponent;
    delete &base;
    delete &name;
}
const Exp& Exp::operator=(const Exp &target) {
    if(this == &target)
        return *this;
    base = *new Expression(target.base);
    exponent = *new Expression(target.exponent);
    return *this;
};
Exp::Exp(const Exp& target) {
    base = *new Expression(target.base);
    exponent = *new Expression(target.exponent);
    name = target.name;
};
Exp::Exp(Expression newBase, Expression newExponent) {
    base = *new Expression(newBase);
    exponent = *new Expression(newExponent);
    name = (*this).print();
}

Exp::Exp(Expression newBase, int newExponent) {
    base = *new Expression(newBase);
    exponent = declareReal(newExponent);
    name = (*this).print();
}

Expression Exp::negate() const {
    Expression thisExpr = *new Expression(this);
    Expression result = *new Expression(new Sign(thisExpr));
    return result;
};

Expression Exp::multiply(Expression other) const {
    if(other->getTypeHash() == ZEROTYPE)
        return ZERO;
    if(other->getTypeHash() == ONETYPE)
        return *new Expression(this);
    if(other->getTypeHash() == EXPTYPE) {
        const Exp& otherExp = dynamic_cast<const Exp&>(*other);
        if(otherExp.base == base || -otherExp.base == base) {
            Expression newExponent = exponent + otherExp.exponent;
            Expression result =  *new Expression(new Exp(base,newExponent));
            if(-otherExp.base == base)
                return -result;
            return result;
        }
        return *new Expression(new Mul(*new Expression(this),other));
    }
    if(other == base || -other == base) {
        Expression expUpOne = exponent+ONE;
        expUpOne = simplify();
        Expression result = *new Expression(new Exp(base,expUpOne));
        if(-other == base)
            return -result;
        return result;
    }
    return distribute(other);
}

String Exp::print() const {
    String result = "";
    if(isTypeSimilarTo(base, CONTAINERTYPE))
        result += "(";
    result += base.print();
    if(isTypeSimilarTo(base, CONTAINERTYPE))
        result += ")";
    result += "^";
    if(isTypeSimilarTo(exponent, CONTAINERTYPE))
        result += "(";
    result += exponent.print();
    if(isTypeSimilarTo(exponent, CONTAINERTYPE))
        result += ")";
    return result;
    
};

Expression Exp::simplify() const {
    if(isTypeSimilarTo(base, CONTAINERTYPE) && isInteger(exponent)) {
        const Real& realObj = dynamic_cast<const Real&>(*exponent);
        int val = realObj.getValue();
        Expression expandedProduct = ONE;
        for(int i = 0; i<val; i++) {
            expandedProduct = expandedProduct*base;
        }
        return expandedProduct.simplify();
    }
        
    return *new Expression(new Exp(base.simplify(),exponent.simplify()));
};
Expression Exp::distribute(Expression other) const {
    size_t otherType = other.getTypeHash();
    Expression thisExpr = *new Expression(this);
    if(otherType == ADDTYPE) {
        const Add& otherAdd = dynamic_cast<const Add&>(*other);
        ExprVector newMembers = otherAdd.getMembers();
        for(size_t i = 0; i<newMembers.size(); i++) {
            newMembers[i] = distribute(newMembers[i]);
        }
        return *new Expression(new Add(newMembers));
    }
    if(otherType == MULTYPE) {
        Expression testTarget = getElementOfType(other, EXPTYPE);
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
Expression Exp::factor() const {
    ExprVector factors = getFactors();
    if(factors.size() > 1)
        return *new Expression(new Mul(factors));
    return *new Expression(this);
};
Expression Exp::reciprocal() const {
    return *new Expression(new Exp(base,-exponent));
};
Expression Exp::determinant() const {
    return *new Expression(this);
};
Expression Exp::transpose() const {
    return *new Expression(this);
};
ExprVector Exp::getFactors() const {
    size_t expType = exponent.getTypeHash();
    if(expType == REALTYPE) {
        const Real& realExp = dynamic_cast<const Real&>(*exponent);
        if(floor(realExp.getValue()) == realExp.getValue()) {
            int expVal = realExp.getValue();
            ExprVector factors = *new ExprVector();
            for(size_t i = 0; i<expVal; i++ ) {
                factors.push_back(base);
            }
            return factors;
        }
    }
    if(expType == ADDTYPE) {
        const Add& addExp = dynamic_cast<const Add&>(*exponent);
        ExprVector expMembers = addExp.getMembers();
        ExprVector factors = *new ExprVector();
        for(size_t i = 0; i<expMembers.size(); i++) {
            factors.push_back(*new Expression(new Exp(base,expMembers[i])));
        }
        return factors;
    }
    return {*new Expression(this)};
};

//Func
Func::Func(const Func& target) {
    functionAction = target.functionAction;
    member = *new Expression(target.member);
    funcName = target.funcName;
}

Func& Func::operator=(const Func &target) {
    if(this == &target)
        return *this;
    functionAction = target.functionAction;
    member = *new Expression(target.member);
    funcName = target.funcName;
    return *this;
}

Expression nonFunc(Expression var) {
    return var;
}

Func::Func(String name) {
    funcName = name;
    //const ExprActionObj actionObj = [] (Expression var) -> Expression {return var;};
    functionAction = *new ExprAction(nonFunc);
    this->name = this->print();
};

/*Func::Func(String name, ExprActionObj actionObj) {
    funcName = name;
    functionAction = *new ExprAction(actionObj);
    this->name = this->print();
};*/

Func::Func(String name, ExprAction action) {
    funcName = name;
    //functionAction = *new ExprAction(action);
    functionAction = action;
    this->name = this->print();
};

Func::Func(String name, ExprAction action, Expression targetedExpression) {
    funcName = name;
    //functionAction = *new ExprAction(action);
    functionAction = action;
    member = targetedExpression;
    this->name = this->print();
};

Expression Func::actingOn(Expression variable) const {
    Func* thisFunc = new Func(*this);
    thisFunc->member = variable;
    thisFunc->name = thisFunc->print();
    return *new Expression(thisFunc);
};

String Func::print() const {
    String result = "";
    String varName = "";
    if(member.getTypeHash() == NULLTYPE) {
        varName = "var";
    } else {
        varName = member.print();
    }
    result += funcName;
    result += "[";
    result += varName;
    result += "]";
    return result;
}

Expression Func::simplify() const {
    return *new Expression(new Func(funcName,functionAction,member.simplify()));
};
Expression Func::distribute(Expression other) const {
    size_t otherType = other.getTypeHash();
    Expression thisExpr = *new Expression(this);
    if(otherType == ADDTYPE) {
        const Add& otherAdd = dynamic_cast<const Add&>(*other);
        ExprVector newMembers = otherAdd.getMembers();
        for(size_t i = 0; i<newMembers.size(); i++) {
            newMembers[i] = distribute(newMembers[i]);
        }
        return *new Expression(new Add(newMembers));
    }
    if(otherType == MULTYPE) {
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
Expression Func::factor() const {
    return *new Expression(this);
};
Expression Func::reciprocal() const {
    return *new Expression(new Frac(*new Expression(this)));
};
Expression Func::determinant() const {
    return *new Expression(this);
};
Expression Func::transpose() const {
    return *new Expression(this);
};
ExprVector Func::getFactors() const {
    return {*new Expression(this)};
};
