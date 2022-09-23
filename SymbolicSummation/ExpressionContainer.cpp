//
//  Operator.cpp
//  SymbolicSummation
//
//  Created by Robert Stahulak on 1/29/22.
//

#include "ExpressionContainer.hpp"
#include "ExpressionHelpers.hpp"
#include "AbstractHelpers.hpp"
#include "Real.hpp"

//Operator
Expression ExpressionContainer::add(SymbolicObject& self, SymbolicObject& other) {
    /*if(*other == *-self)
        return ZERO;*/
    //
    if(self == other)
        return 2*self;
    if(other.getTypeHash() == ZEROTYPE)
        return self;
    return combineSums(self, other);
};
Expression ExpressionContainer::multiply(SymbolicObject& self, SymbolicObject& other){
    /*if(*other == *reciprocal(this)) {
        return ONE;
    }*/
    if(other.getTypeHash() == ZEROTYPE)
        return ZERO;
    if(other.getTypeHash() == ONETYPE)
        return self;
    return distribute(self, other);
};
Expression ExpressionContainer::divide(SymbolicObject& self, SymbolicObject& other) {
    if(*other == *self) {
        return ONE;
    }
    Expression reciprocalOf = reciprocal(other);
    Expression thisExpr = *new Expression(this);
    return distribute(thisExpr, reciprocalOf);
};
Expression ExpressionContainer::subtract(SymbolicObject& self, SymbolicObject& other) {
    if(self == other)
        return ZERO;
    Expression negativeOf = -other;
    return combineSums(self,negativeOf);
};
//Add
Add& Add::operator=(const Add &target) {
    if(this == &target)
        return *this;
    members = *new ExprVector();
    memberSigns = *new std::vector<bool>();
    for(int i = 0; i<target.members.size(); i++) {
        members.push_back(target.members[i]);
        memberSigns.push_back(target.memberSigns[i]);
    }
    return *this;
};
Add::Add(const Add& target) {
    members = *new ExprVector();
    memberSigns = *new std::vector<bool>();
    name = target.name;
    for(int i = 0; i<target.members.size(); i++) {
        members.push_back(target.members[i]);
        memberSigns.push_back(target.memberSigns[i]);
    }
}

Add::Add(ExprVector newMembers,std::vector<bool> newMemberSigns) {
    members = newMembers;
    if(members.size() == 0)
        members.push_back(ZERO);
    if(newMemberSigns.size() == 0) {
        int i = 0;
        while(i<members.size()) {
            memberSigns.push_back(false);
            i++;
        }
    } else
        memberSigns = newMemberSigns;
    /*for(int i = 0; i<newMembers.size(); i++) {
        members.push_back(newMembers[i]);
        memberSigns.push_back(newMemberSigns[i]);
    }*/
    name = (*this).print();
};
Add::Add(Expression right, Expression left, std::vector<bool> newMemberSigns) {
    Expression newMem = right;
    members.push_back(newMem);
    newMem = left;
    members.push_back(newMem);
    memberSigns.push_back(newMemberSigns[0]);
    memberSigns.push_back(newMemberSigns[1]);
    name = (*this).print();
};
Expression Add::negate(SymbolicObject& self) {
    ExprVector newMembers = members;
    std::vector<bool> newSigns = *new std::vector<bool>();
    if(members.size() == 1 && memberSigns[0])
        return members[0];
    for(int i = 0; i<newMembers.size(); i++) {
        newSigns.push_back(!memberSigns[i]);
    }
    Expression result = *new Expression(new Add(newMembers,newSigns));
    return result;
};
/*
bool Add::containsTypeOfPerform(size_t type) {
    return positionOfType(members, type) >= 0;
};
bool Add::containsPerform(Expression target) {
    return positionOfElementIgnoringSign(members, target) >= 0;
};
Expression Add::firstInstanceOfTypePerform(SymbolicObject& self, size_t type, bool rightToLeft) {
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
Expression Add::removePerform(SymbolicObject& self, Expression target, bool rightToLeft) {
    //int location = positionOfElementIgnoringSign(members, target, rightToLeft);
    std::vector<bool> newSigns = *new std::vector<bool>();
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
    return self;
};
 */
std::string Add::print() {
    std::string result = "";
    //std::cout << members.size();
    for(int i = 0; i<members.size(); i++) {
        Expression next = members[i];
        if(memberSigns[i])
            result+="-";
        if(!memberSigns[i] && i>0)
            result+="+";
        result+=next.print();
    }
    return result;
};
//Sign
Expression Sign::negate(SymbolicObject& self) {
    return member;
};

Expression Sign::add(SymbolicObject& self, SymbolicObject& other) {
    if(other.getTypeHash() == SIGNTYPE) {
        Sign& otherSign = dynamic_cast<Sign&>(*other);
        Expression result = member+otherSign.member;
        return -result;
    }
    if(other.getTypeHash() == ZEROTYPE)
        return self;
    return combineSums(self, other);
}

Expression Sign::subtract(SymbolicObject& self, SymbolicObject& other) {
    if(other.getTypeHash() == ZEROTYPE)
        return self;
    Expression result = member+other;
    return -result;
}

Expression Sign::multiply(SymbolicObject& self, SymbolicObject& other) {
    if(other.getTypeHash() == SIGNTYPE) {
        Sign& otherSign = dynamic_cast<Sign&>(*other);
        Expression result = member*otherSign.member;
        return result;
    }
    if(other.getTypeHash() == ZEROTYPE)
        return ZERO;
    if(other.getTypeHash() == ONETYPE)
        return self;
    return distribute(self, other);
}
/*
bool Sign::containsTypeOfPerform(size_t type) {
    return member.getTypeHash() == type;
};
bool Sign::containsPerform(Expression target) {
    return target == member;
};
Expression Sign::firstInstanceOfTypePerform(SymbolicObject& self, size_t type, bool rightToLeft) {
    if(type == member.getTypeHash()) {
        return self;
    }
    Expression result = *new Expression(new NullObject("does not contain target type"));
    return result;
};
Expression Sign::removePerform(SymbolicObject& self, Expression target, bool rightToLeft) {
    if(target == self)
        return ZERO;
    else
        return self;
};
 */
std::string Sign::print() {
    std::string result = "-";
    result += member.print();
    return result;
};
Sign::Sign(const Sign& target) : Add({target.member},{true}) {
    member = target.member;
};
Sign& Sign::operator=(const Sign& target) {
    if(this == &target)
        return *this;
    members = *new ExprVector();
    memberSigns = *new std::vector<bool>();
    for(int i = 0; i<target.members.size(); i++) {
        members.push_back(target.members[i]);
        memberSigns.push_back(target.memberSigns[i]);
    }
    return *this;
};
Sign::Sign(Expression member) : Add({member},{true}) {
    (*this).member = member;
};
//Mul
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
Mul::Mul(Expression right, Expression left) {
    members.push_back(right);
    members.push_back(left);
    name = (*this).print();
};
Expression Mul::negate(SymbolicObject& self) {
    Expression negateTarget = getElementOfType(self, SIGNTYPE);//self.getFirstInstanceOfType(SIGNTYPE);
    if(negateTarget.getTypeHash() == NULLTYPE) {
        Expression result = *new Expression(new Sign(self));
        return result;
    }
    Expression negatedTarget = -negateTarget;
    ExprVector newMembers = replaceElementInVector(members, negateTarget, negatedTarget);
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
Expression Mul::firstInstanceOfTypePerform(SymbolicObject& self, size_t type, bool rightToLeft) {
    int location = positionOfType(members, type, rightToLeft);
    if(location >= 0) {
        return members[location];
    }
    Expression result = *new Expression(new NullObject("could not find target in MULTYPE"));
    return result;
    
};
Expression Mul::removePerform(SymbolicObject& self, Expression target, bool rightToLeft) {
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
    return self;
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

Frac::Frac(Expression denom) {
    numerator = ONE;
    denomenator = denom;
    name = (*this).print();
};
Frac::Frac(Expression num, Expression denom) {
    numerator = num;
    denomenator = denom;
    name = (*this).print();
};
Expression Frac::negate(SymbolicObject& self) {
    Expression negativeOf = -numerator;
    Expression result = *new Expression(new Frac(negativeOf,denomenator));
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
Expression Frac::firstInstanceOfTypePerform(SymbolicObject& self, size_t type, bool rightToLeft) {
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
Expression Frac::removePerform(SymbolicObject& self, Expression target, bool rightToLeft) {
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
    
    return self;
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
Exp::Exp(Expression newBase, Expression newExponent) {
    base = newBase;
    exponent = newExponent;
    name = (*this).print();
}

Exp::Exp(Expression newBase, int newExponent) {
    base = newBase;
    exponent = declareReal(newExponent);
    name = (*this).print();
}

Expression Exp::negate(SymbolicObject& self) {
    Expression result = *new Expression(new Sign(self));
    return result;
};

Expression Exp::multiply(SymbolicObject& self, SymbolicObject& other) {
    if(other.getTypeHash() == ZEROTYPE)
        return ZERO;
    if(other.getTypeHash() == ONETYPE)
        return self;
    if(other.getTypeHash() == EXPTYPE) {
        Exp& otherExp = dynamic_cast<Exp&>(*other);
        if(otherExp.base == base || -otherExp.base == base) {
            Expression newExponent = exponent + otherExp.exponent;
            Expression result =  *new Expression(new Exp(base,newExponent));
            if(-otherExp.base == base)
                return -result;
            return result;
        }
        return *new Expression(new Mul(self,other));
    }
    if(other == base || -other == base) {
        Expression expUpOne = exponent+ONE;
        expUpOne = simplify(expUpOne);
        Expression result = *new Expression(new Exp(base,expUpOne));
        if(-other == base)
            return -result;
        return result;
    }
    return distribute(self, other);
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
Expression Exp::firstInstanceOfTypePerform(SymbolicObject& self, size_t type, bool rightToLeft) {
    return *new Expression(base.getFirstInstanceOfType(type,rightToLeft));
};
Expression Exp::removePerform(SymbolicObject& self, Expression target, bool rightToLeft) {
    if(base == target) {
        Expression expOneDown = exponent-1;
        expOneDown = simplify(expOneDown);
        if(expOneDown == ONE)
            return base;
        if(expOneDown == ZERO)
            return ONE;
        return *new Expression(new Exp(base,expOneDown));
    }
    else return self;
};
 */
std::string Exp::print() {
    std::string result = "";
    if(isSubclassOf(base.getTypeHash(), OPERATORTYPE))
        result += "(";
    result += base.print();
    if(isSubclassOf(base.getTypeHash(), OPERATORTYPE))
        result += ")";
    result += "^";
    if(isSubclassOf(exponent.getTypeHash(), OPERATORTYPE))
        result += "(";
    result += exponent.print();
    if(isSubclassOf(exponent.getTypeHash(), OPERATORTYPE))
        result += ")";
    return result;
    
};
