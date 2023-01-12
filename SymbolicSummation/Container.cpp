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

//Operator
Expression Container::add(Expression other) const {
    if(*this == *other)
        return 2**this;
    if(other->getTypeHash() == ZEROTYPE)
        return *new Expression(this);
    return combineSums(*new Expression(this), other);
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
    return reciprocal().distribute(other);
};
Expression Container::subtract(Expression other) const {
    Expression thisExpr = *new Expression(this);
    if(thisExpr == other)
        return ZERO;
    Expression negativeOf = -other;
    return combineSums(thisExpr,negativeOf);
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
    members = *new ExprVector(target.members);
    name = target.name;
    return *this;
};
Add::Add(const Add& target) {
    members = *new ExprVector(target.members);
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
    for(int i = 0; i<newMembers.size(); i++) {
        newMembers[i] = -newMembers[i];
    }
    Expression result = *new Expression(new Add(newMembers));
    return result;
};

String Add::print() const {
    String result = "";
    //std::cout << members.size();
    for(int i = 0; i<members.size(); i++) {
        Expression next = members[i];
        if(members[i]->getTypeHash() != SIGNTYPE && i>0)
            result+="+";
        result+=next.print();
    }
    return result;
};

Expression Add::simplify() const {
    ExprVector simplifiedMembers = *new ExprVector();
    for(int i = 0; i< members.size(); i++) {
        simplifiedMembers.push_back(members[i].simplify());
    }
    Expression newAdd = *new Expression(new Add(simplifiedMembers));
    return newAdd.cancelTerms();
};
Expression Add::distribute(Expression other) const {
    ExprVector newMembers = *new ExprVector();
    for(int i = 0; i< members.size(); i++)
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
    for(int i = 0; i<members.size(); i++) {
        newMembers.push_back(members[i].transpose());
    }
    return *new Expression(new Add(newMembers));
};
Expression Add::cancelTerms() const {
    std::vector<int> accountedFor = *new std::vector<int>();
    ExprVector newMembers = *new ExprVector();
    for(int i = 0; i<members.size(); i++) {
        if(intVectorContains(accountedFor, i))
            continue;
        Expression runningSum = members[i];
        for(int j = i; j<members.size(); j++) {
            if(intVectorContains(accountedFor, j))
                continue;
            Expression currExpr = members[j];
            if(currExpr == runningSum) {
                runningSum = 2*runningSum;
                accountedFor.push_back(j);
                continue;
            }
            ExprVector currCommonFactors = runningSum.getCommonFactors(currExpr);
            if(currCommonFactors.size() == 0)
                continue;
            Expression inCommon = ONE;
            Expression sumNotInCommon = runningSum;
            Expression otherNotInCommon = currExpr;
            for(int k = 0; k<currCommonFactors.size(); k++) {
                inCommon = inCommon*currCommonFactors[k];
                sumNotInCommon = removeElementMultiplicatively(sumNotInCommon, currCommonFactors[k]);
                otherNotInCommon = removeElementMultiplicatively(otherNotInCommon, currCommonFactors[k]);
            }
            if(isTypeSimilarTo(sumNotInCommon, CONTAINERTYPE) || isTypeSimilarTo(otherNotInCommon, CONTAINERTYPE) || !areSimilarTypes(sumNotInCommon, otherNotInCommon))
                continue;
            //There are no cases I can think of where this will break commutators,
            //lack of evidence is not evidence of lack however.
            Expression testCombine = sumNotInCommon+otherNotInCommon;
            if(isTypeSimilarTo(testCombine, CONTAINERTYPE))
                continue;
            runningSum = testCombine*inCommon;
        }
        accountedFor.push_back(i);
        newMembers.push_back(runningSum);
    }
    return *new Expression(new Add(newMembers));
};
ExprVector Add::getFactors() const {
    return commonFactors(members);
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
Expression Sign::cancelTerms() const {
    return -member.cancelTerms();
};
ExprVector Sign::getFactors() const {
    ExprVector memFactors = member.getFactors();
    if(!exprVectorContains(memFactors, MINUSONE))
        memFactors.push_back(MINUSONE);
    return memFactors;
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
    members = *new ExprVector(target.members);
    return *this;
};
Mul::Mul(const Mul& target) {
    members = *new ExprVector(target.members);
    name = target.name;
};
Mul::Mul(ExprVector newMembers) {
    members = *new ExprVector(newMembers);
    name = (*this).print();
};
Mul::Mul(Expression right, Expression left) {
    members.push_back(*new Expression(right));
    members.push_back(*new Expression(left));
    name = (*this).print();
};
Expression Mul::negate() const {
    Expression negateTarget = getElementOfType(*new Expression(this), SIGNTYPE);//this.getFirstInstanceOfType(SIGNTYPE);
    if(negateTarget.getTypeHash() == NULLTYPE) {
        Expression result = *new Expression(new Sign(*new Expression(this)));
        return result;
    }
    Expression negatedTarget = -negateTarget;
    ExprVector newMembers = replaceElementInVector(members, negateTarget, negatedTarget);
    Expression result = *new Expression(new Mul(newMembers));
    return result;
};

String Mul::print() const {
    String result = "";
    for(int i = 0; i<members.size(); i++) {
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

Expression Mul::simplify() const {
    ExprVector newMembers = *new ExprVector();
    for(int i = 0; i< members.size(); i++)
        newMembers.push_back(members[i].simplify());
    Expression newMul = *new Expression(new Mul(newMembers));
    return newMul;
};
Expression Mul::distribute(Expression other) const {
    if(other.getTypeHash() == MULTYPE) {
        const Mul& otherMul = dynamic_cast<const Mul&>(*other);
        return *new Expression(new Mul(setUnion(members, otherMul.members)));
    }
    if(other.getTypeHash() == ADDTYPE) {
        const Add& otherAdd= dynamic_cast<const Add&>(*other);
        ExprVector newMembers = *new ExprVector();
        for(int i = 0; i< otherAdd.members.size(); i++) {
            newMembers.push_back(distribute(otherAdd.members[i]));
        }
        return *new Expression(new Add(newMembers));
    }
    ExprVector newMembers = *new ExprVector(members);
    newMembers.push_back(other);
    return *new Expression(new Mul(newMembers));
};
Expression Mul::factor() const {
    ExprVector newMembers = *new ExprVector();
    for(int i = 0; i< members.size(); i++) {
        Expression memberFactored = members[i].factor();
        newMembers.push_back(memberFactored);
    }
    Expression newMul = *new Expression(new Mul(newMembers));
    return newMul;
};
Expression Mul::reciprocal() const {
    ExprVector newMembers = *new ExprVector();
    for(int i = (int)members.size()-1; i>=0; i--) {
        newMembers.push_back(members[i].reciprocal());
    }
    Expression newMul = *new Expression(new Mul(newMembers));
    return newMul;
};
Expression Mul::determinant() const {
    ExprVector newMembers = *new ExprVector();
    for(int i = 0; i<members.size(); i++) {
        newMembers.push_back(members[i].determinant());
    }
    Expression newMul = *new Expression(new Mul(newMembers));
    return newMul;
};
Expression Mul::transpose() const {
    ExprVector newMembers = *new ExprVector();
    for(int i = (int)members.size()-1; i>=0; i--) {
        newMembers.push_back(members[i].transpose());
    }
    Expression newMul = *new Expression(new Mul(newMembers));
    return newMul;
};
Expression Mul::cancelTerms() const {
    Expression simpledSelf = simplify();
    if(simpledSelf.getTypeHash() != ADDTYPE)
        return *new Expression(this);
    return simpledSelf.cancelTerms();
};
ExprVector Mul::getFactors() const {
    ExprVector factors = *new ExprVector();
    for(int i = 0; i<members.size(); i++) {
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
Expression Frac::cancelTerms() const {
    Expression simpledSelf = simplify();
    if(simpledSelf.getTypeHash() != ADDTYPE)
        return *new Expression(this);
    return simpledSelf.cancelTerms();
};
ExprVector Frac::getFactors() const {
    ExprVector numFactors = numerator.getFactors();
    ExprVector denomFactors = denomenator.getFactors();
    for(int i = 0; i<denomFactors.size(); i++) {
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
    Expression result = *new Expression(new Sign(this));
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
};
Expression Exp::distribute(Expression other) const {
};
Expression Exp::factor() const {
};
Expression Exp::reciprocal() const {
};
Expression Exp::determinant() const {
};
Expression Exp::transpose() const {
};
Expression Exp::cancelTerms() const {
};
ExprVector Exp::getFactors() const {
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

};
Expression Func::distribute(Expression other) const {

};
Expression Func::factor() const {

};
Expression Func::reciprocal() const {
};
Expression Func::determinant() const {
};
Expression Func::transpose() const {
};
Expression Func::cancelTerms() const {
};
ExprVector Func::getFactors() const {
};
