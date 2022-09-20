//
//  Operator.cpp
//  SymbolicSummation
//
//  Created by Robert Stahulak on 1/29/22.
//

#include "Container.hpp"
#include "AlgebraicHelpers.hpp"
#include "AbstractHelpers.hpp"
#include "Real.hpp"

//Operator
Expression Container::add(ExpressionObject* other) {
    /*if(*other == *-this)
        return ZERO;*/
    //
    if(this == other)
        return 2**this;
    if(other->getTypeHash() == ZEROTYPE)
        return *new Expression(this);
    return combineSums(this, other);
};
Expression Container::multiply(ExpressionObject* other){
    /*if(*other == *reciprocal(this)) {
        return ONE;
    }*/
    if(other->getTypeHash() == ZEROTYPE)
        return ZERO;
    if(other->getTypeHash() == ONETYPE)
        return *new Expression(this);
    return distribute(this, other);
};
Expression Container::divide(ExpressionObject* other) {
    if(other == this) {
        return ONE;
    }
    Expression reciprocalOf = reciprocal(other);
    return distribute(this, reciprocalOf.get());
};
Expression Container::subtract(ExpressionObject* other) {
    if(this == other)
        return ZERO;
    Expression negativeOf = -*other;
    return combineSums(this,negativeOf.get());
};
//Add
Add::~Add() {
    delete &members;
    delete &name;
}

Add& Add::operator=(const Add &target) {
    if(this == &target)
        return *this;
    members = target.members;//*new ExprVector(*new ExprVector(*target.members));
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

Add::Add(ExpressionObject* left, ExpressionObject* right) {
    Expression newMem = *new Expression(left);
    members.push_back(newMem);
    newMem = *new Expression(right);
    members.push_back(newMem);
    name = (*this).print();
};
Expression Add::negate() {
    ExprVector newMembers = *new ExprVector(members);
    for(int i = 0; i<newMembers.size(); i++) {
        newMembers[i] = -newMembers[i];
    }
    Expression result = *new Expression(new Add(newMembers));
    return result;
};
/*
bool Add::containsTypeOfPerform(size_t type) {
    return positionOfType(members, type) >= 0;
};
bool Add::containsPerform(Expression target) {
    return positionOfElementIgnoringSign(members, target) >= 0;
};
Expression Add::firstInstanceOfTypePerform(, size_t type, bool rightToLeft) {
    int location = positionOfType(members, type, rightToLeft);
    if(location >= 0) {
        if(memberSigns[location]) {
            Expression result = -members[location];
            return result;
        }
        return members[location];
    }
    Expression result = *new Expression(new NullObject("could not find target in ADDTYPE"));
    return result;
};
Expression Add::removePerform(, Expression target, bool rightToLeft) {
    //int location = positionOfElementIgnoringSign(members, target, rightToLeft);
    SignVector newSigns = *new SignVector();
    ExprVector newMembers = *new ExprVector();//members;
    for(int i = 0; i< members.size(); i++ ) {
        if(members[i] == target || (memberSigns[i] && -target == members[i]))
            continue;
        newMembers.push_back(members[i]);
        newSigns.push_back(memberSigns[i]);
    }
    if(newMembers.size() == 0)
        return ZERO;
    if(newMembers.size() == 1 && !newSigns[0])
        return newMembers[0];
    Expression result = *new Expression(new Add(newMembers,newSigns));
    return result;
    return this;
};
 */
std::string Add::print() {
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
    delete &member;
    delete &name;
}

Expression Sign::negate() {
    return member;
};

Expression Sign::add(ExpressionObject* other) {
    if(other->getTypeHash() == SIGNTYPE) {
        Sign& otherSign = dynamic_cast<Sign&>(*other);
        Expression result = member+otherSign.member;
        return -result;
    }
    if(other->getTypeHash() == ZEROTYPE)
        return *new Expression(this);
    return -(*member-*other);//combineSums(this, other);
}

Expression Sign::subtract(ExpressionObject* other) {
    if(other->getTypeHash() == ZEROTYPE)
        return *new Expression(this);
    Expression result = *member+*other;
    return -result;
}

Expression Sign::multiply(ExpressionObject* other) {
    if(other->getTypeHash() == SIGNTYPE) {
        Sign& otherSign = dynamic_cast<Sign&>(*other);
        Expression result = member*otherSign.member;
        return result;
    }
    if(other->getTypeHash() == ZEROTYPE)
        return ZERO;
    if(other->getTypeHash() == ONETYPE)
        return *new Expression(this);
    return -(*member**other);
}
/*
bool Sign::containsTypeOfPerform(size_t type) {
    return member.getTypeHash() == type;
};
bool Sign::containsPerform(Expression target) {
    return target == member;
};
Expression Sign::firstInstanceOfTypePerform(, size_t type, bool rightToLeft) {
    if(type == member.getTypeHash()) {
        return this;
    }
    Expression result = *new Expression(new NullObject("does not contain target type"));
    return result;
};
Expression Sign::removePerform(, Expression target, bool rightToLeft) {
    if(target == this)
        return ZERO;
    else
        return this;
};
 */
std::string Sign::print() {
    std::string result = "-";
    result += member.print();
    return result;
};
Sign::Sign(const Sign& target) {
    member = target.member;
};
Sign& Sign::operator=(const Sign& target) {
    if(this == &target)
        return *this;
    member = target.member;
    return *this;
};
Sign::Sign(ExpressionObject* object) {
    (*this).member = *new Expression(object);
    (*this).name = "-"+member.print();
};
//Mul
Mul::~Mul() {
    delete &members;
    delete &name;
}
Mul& Mul::operator=(const Mul &target) {
    if(this == &target)
        return *this;
    members = target.members;
    return *this;
};
Mul::Mul(const Mul& target) {
    members = target.members;
    name = target.name;
};
Mul::Mul(ExprVector newMembers) {
    members = newMembers;
    name = (*this).print();
};
Mul::Mul(ExpressionObject* right, ExpressionObject* left) {
    members.push_back(*new Expression(right));
    members.push_back(*new Expression(left));
    name = (*this).print();
};
Expression Mul::negate() {
    Expression negateTarget = getElementOfType(this, SIGNTYPE);//this.getFirstInstanceOfType(SIGNTYPE);
    if(negateTarget.getTypeHash() == NULLTYPE) {
        Expression result = *new Expression(new Sign(this));
        return result;
    }
    Expression negatedTarget = -negateTarget;
    ExprVector newMembers = replaceElementInVector(members, negateTarget.get(), negatedTarget.get());
    Expression result = *new Expression(new Mul(newMembers));
    return result;
};
/*
bool Mul::containsTypeOfPerform(size_t type) {
    return positionOfType(members, type) >= 0;
};
bool Mul::containsPerform(Expression target) {
    return positionOfElement(members, target) >= 0;
};
Expression Mul::firstInstanceOfTypePerform(, size_t type, bool rightToLeft) {
    int location = positionOfType(members, type, rightToLeft);
    if(location >= 0) {
        return members[location];
    }
    Expression result = *new Expression(new NullObject("could not find target in MULTYPE"));
    return result;
    
};
Expression Mul::removePerform(, Expression target, bool rightToLeft) {
    int location = positionOfElement(members, target, rightToLeft);
    if(location >= 0) {
        ExprVector newMembers = *new ExprVector();//members;
        for(int i = 0; i< members.size(); i++ ) {
            if(i == location)
                continue;
            newMembers.push_back(members[i]);
        }
        //std::cout << (newMembers.size());
        //std::cout << "\n";
        if(newMembers.size() == 0)
            return ZERO;
        if(newMembers.size() == 1) {
            return newMembers[0];
        }
        Expression resultExpr = *new Expression(new Mul(newMembers));
        return resultExpr;
    }
    //Expression result(this);
    return this;
};
 */
std::string Mul::print() {
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
    delete &numerator;
    delete &denomenator;
    delete &name;
}
Frac& Frac::operator=(const Frac &target) {
    if(this == &target)
        return *this;
    numerator = target.numerator;
    denomenator = target.denomenator;
    return *this;
};
Frac::Frac(const Frac& target) {
    numerator = target.numerator;
    denomenator = target.denomenator;
    name = target.name;
};

Frac::Frac(ExpressionObject* denom) {
    numerator = ONE;
    denomenator = *new Expression(denom);
    name = (*this).print();
};
Frac::Frac(ExpressionObject* num, ExpressionObject* denom) {
    numerator = *new Expression(num);
    denomenator = *new Expression(denom);
    name = (*this).print();
};
Expression Frac::negate() {
    Expression negativeOf = -numerator;
    Expression result = *new Expression(new Frac(negativeOf.get(),denomenator.get()));
    return result;
};
/*
bool Frac::containsTypeOfPerform(size_t type) {
    if(type == FRACTYPE)
        return true;
    bool contains = false;
    contains |= denomenator.containsTypeOf(type);
    contains |= numerator.containsTypeOf(type);
    return contains;
};
bool Frac::containsPerform(Expression target) {
    if(*target == *this)
        return true;
    bool contains = false;
    contains |= (denomenator).contains(reciprocal(target));
    contains |= (numerator).contains(target);
    return contains;
};
Expression Frac::firstInstanceOfTypePerform(, size_t type, bool rightToLeft) {
    if(!rightToLeft) {
        Expression candidate = numerator.getFirstInstanceOfType(type);
        if(candidate.getTypeHash() != NULLTYPE )
            return candidate;
        candidate = denomenator.getFirstInstanceOfType(type);
        if(candidate.getTypeHash() != NULLTYPE )
            return reciprocal(candidate);
        Expression result = * new Expression(new NullObject("could not find target in FRACTYPE"));
        return result;
    } else {
        Expression candidate = *new Expression(denomenator.getFirstInstanceOfType(type));
        if(candidate.getTypeHash() != NULLTYPE )
            return reciprocal(candidate);
        candidate = numerator.getFirstInstanceOfType(type);
        if(candidate.getTypeHash() != NULLTYPE )
            return candidate;
        Expression result = * new Expression(new NullObject("could not find target in FRACTYPE"));
        return result;
    }
};

//not sure this function is necessary or useful
Expression Frac::removePerform(, Expression target, bool rightToLeft) {
    if(numerator.contains(target)) {
        Expression removeResult = *new Expression(numerator.remove(target,rightToLeft));
        Expression result = * new Expression(new Frac(removeResult,denomenator));
        return result;
    }
    Expression recip = reciprocal(target);
    if( denomenator.contains(recip) )
    {
        Expression removeResult = *new Expression(denomenator.remove(recip,rightToLeft));
        Expression result = * new Expression(new Frac(removeResult,denomenator));
        return result;
    }
    
    return this;
};
 */
std::string Frac::print() {
    std::string result = "(";
    result += numerator.print();
    result += ")/(";
    result += denomenator.print();
    result += ")";
    return result;
};

//Exp
Exp::~Exp() {
    delete &exponent;
    delete &base;
    delete &name;
}
Exp& Exp::operator=(const Exp &target) {
    if(this == &target)
        return *this;
    base = target.base;
    exponent = target.exponent;
    return *this;
};
Exp::Exp(const Exp& target) {
    base = target.base;
    exponent = target.exponent;
    name = target.name;
};
Exp::Exp(ExpressionObject* newBase, ExpressionObject* newExponent) {
    base = *new Expression(newBase);
    exponent = *new Expression(newExponent);
    name = (*this).print();
}

Exp::Exp(ExpressionObject* newBase, int newExponent) {
    base = *new Expression(newBase);
    exponent = declareReal(newExponent);
    name = (*this).print();
}

Expression Exp::negate() {
    Expression result = *new Expression(new Sign(this));
    return result;
};

Expression Exp::multiply(ExpressionObject* other) {
    if(other->getTypeHash() == ZEROTYPE)
        return ZERO;
    if(other->getTypeHash() == ONETYPE)
        return *new Expression(this);
    if(other->getTypeHash() == EXPTYPE) {
        Exp& otherExp = dynamic_cast<Exp&>(*other);
        if(otherExp.base == base || -otherExp.base == base) {
            Expression newExponent = exponent + otherExp.exponent;
            Expression result =  *new Expression(new Exp(base.get(),newExponent.get()));
            if(-otherExp.base == base)
                return -result;
            return result;
        }
        return *new Expression(new Mul(this,other));
    }
    if(*other == *base || *-*other == *base) {
        Expression expUpOne = exponent+ONE;
        expUpOne = simplify(expUpOne.get());
        Expression result = *new Expression(new Exp(base.get(),expUpOne.get()));
        if(-*other == base)
            return -result;
        return result;
    }
    return distribute(this, other);
}
/*
bool Exp::containsTypeOfPerform(size_t type) {
    if(type == EXPTYPE)
        return true;
    return base.containsTypeOf(type) || exponent.containsTypeOf(type);
};
bool Exp::containsPerform(Expression target) {
    if(*target == *this)
        return true;
    return base.contains(target) || exponent.contains(target);
};
Expression Exp::firstInstanceOfTypePerform(, size_t type, bool rightToLeft) {
    return *new Expression(base.getFirstInstanceOfType(type,rightToLeft));
};
Expression Exp::removePerform(, Expression target, bool rightToLeft) {
    if(base == target) {
        Expression expOneDown = exponent-1;
        expOneDown = simplify(expOneDown);
        if(expOneDown == ONE)
            return base;
        if(expOneDown == ZERO)
            return ONE;
        return *new Expression(new Exp(base,expOneDown));
    }
    else return this;
};
 */
std::string Exp::print() {
    std::string result = "";
    if(isSubtypeOf(base.get(), OPERATORTYPE))
        result += "(";
    result += base.print();
    if(isSubtypeOf(base.get(), OPERATORTYPE))
        result += ")";
    result += "^";
    if(isSubtypeOf(exponent.get(), OPERATORTYPE))
        result += "(";
    result += exponent.print();
    if(isSubtypeOf(exponent.get(), OPERATORTYPE))
        result += ")";
    return result;
    
};
