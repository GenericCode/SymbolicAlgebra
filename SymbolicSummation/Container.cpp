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
    return combineSums(this, other);
};
Expression Container::multiply(Expression other) const {
    if(other->getTypeHash() == ZEROTYPE)
        return ZERO;
    if(other->getTypeHash() == ONETYPE)
        return *new Expression(this);
    return distribute(this, other);
};
Expression Container::divide(Expression other) const {
    if(other == this) {
        return ONE;
    }
    Expression reciprocalOf = reciprocal(other);
    return distribute(this, reciprocalOf);
};
Expression Container::subtract(Expression other) const {
    if(this == other)
        return ZERO;
    Expression negativeOf = -*other;
    return combineSums(this,negativeOf);
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
    members = newMembers;
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

std::string Add::print() const {
    std::string result = "";
    //std::cout << members.size();
    for(int i = 0; i<members.size(); i++) {
        Expression next = members[i];
        if(members[i]->getTypeHash() != SIGNTYPE && i>0)
            result+="+";
        result+=next.print();
    }
    return result;
};
//Sign

Sign::~Sign() {
    //delete &member;
    //delete &name;
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
    return -(*member-*other);//combineSums(this, other);
}

Expression Sign::subtract(Expression other) const {
    if(other->getTypeHash() == ZEROTYPE)
        return *new Expression(this);
    Expression result = *member+*other;
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
    return -(*member**other);
}

std::string Sign::print() const {
    std::string result = "-";
    result += member.print();
    return result;
};
Sign::Sign(const Sign& target) {
    member = *new Expression(target.member.get());
};
const Sign& Sign::operator=(const Sign& target) {
    if(this == &target)
        return *this;
    member = *new Expression(target.member.get());
    return *this;
};
Sign::Sign(Expression expr) {
    (*this).member = *new Expression(expr.get());
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
    Expression negateTarget = getElementOfType(this, SIGNTYPE);//this.getFirstInstanceOfType(SIGNTYPE);
    if(negateTarget.getTypeHash() == NULLTYPE) {
        Expression result = *new Expression(new Sign(this));
        return result;
    }
    Expression negatedTarget = -negateTarget;
    ExprVector newMembers = replaceElementInVector(members, negateTarget, negatedTarget);
    Expression result = *new Expression(new Mul(newMembers));
    return result;
};

std::string Mul::print() const {
    std::string result = "";
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
//Frac
Frac::~Frac() {
    //delete &numerator;
    //delete &denomenator;
    //delete &name;
}
Frac& Frac::operator=(const Frac &target) {
    if(this == &target)
        return *this;
    numerator = *new Expression(target.numerator.get());
    denomenator = *new Expression(target.denomenator.get());
    return *this;
};
Frac::Frac(const Frac& target) {
    numerator = *new Expression(target.numerator.get());
    denomenator = *new Expression(target.denomenator.get());
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

std::string Frac::print() const {
    std::string result = "(";
    result += numerator.print();
    result += ")/(";
    result += denomenator.print();
    result += ")";
    return result;
};

//Exp
Exp::~Exp() {
    //delete &exponent;
    //delete &base;
    //delete &name;
}
const Exp& Exp::operator=(const Exp &target) {
    if(this == &target)
        return *this;
    base = *new Expression(target.base.get());
    exponent = *new Expression(target.exponent.get());
    return *this;
};
Exp::Exp(const Exp& target) {
    base = *new Expression(target.base.get());
    exponent = *new Expression(target.exponent.get());
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
        return *new Expression(new Mul(this,other));
    }
    if(*other == *base || *-*other == *base) {
        Expression expUpOne = exponent+ONE;
        expUpOne = simplify(expUpOne);
        Expression result = *new Expression(new Exp(base,expUpOne));
        if(-*other == base)
            return -result;
        return result;
    }
    return distribute(this, other);
}

std::string Exp::print() const {
    std::string result = "";
    if(isSubtypeOf(base, OPERATORTYPE))
        result += "(";
    result += base.print();
    if(isSubtypeOf(base, OPERATORTYPE))
        result += ")";
    result += "^";
    if(isSubtypeOf(exponent, OPERATORTYPE))
        result += "(";
    result += exponent.print();
    if(isSubtypeOf(exponent, OPERATORTYPE))
        result += ")";
    return result;
    
};
