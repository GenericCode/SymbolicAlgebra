#pragma once
//#include "Expression.hpp"
#include <string>
class Expression;
class ExpressionObject {
    friend Expression;
public:
//protected:
    virtual Expression add(ExpressionObject* other) = 0;
    virtual Expression subtract(ExpressionObject* other) = 0;
    virtual Expression negate() = 0;
    virtual Expression multiply(ExpressionObject* other) = 0;
    virtual Expression divide(ExpressionObject* other) = 0;
    //virtual ptr commutator(SymbolicObject* other) = 0;
    /* should be handled elsewhere
    virtual bool containsTypeOfPerform(size_t type) = 0;
    virtual bool containsPerform(Expression target) = 0;
    virtual Expression firstInstanceOfTypePerform(, size_t type, bool rightToLeft) = 0;
    virtual Expression removePerform(, Expression target, bool rightToLeft) = 0;
     */
    
    bool simplified = false;
    //DO THIS INSTEAD OF DOING INDIVIDUAL FINDING
    //virtual std::vector<size_t> typesDoesNotCommuteWith() = 0;
public:
    virtual std::string print() = 0;
    friend bool areEqual(ExpressionObject* left,ExpressionObject* right);
    friend Expression simplify(ExpressionObject* target);
    friend void setSimplified(bool simpled);
    friend Expression operator+(ExpressionObject& self, ExpressionObject& other);
    friend Expression operator-(ExpressionObject& self, ExpressionObject& other);
    friend Expression operator-(ExpressionObject& self);
    friend Expression operator*(ExpressionObject& self, ExpressionObject& other);
    friend Expression operator/(ExpressionObject& self, ExpressionObject& other);
    friend Expression operator+(ExpressionObject& self, float other);
    friend Expression operator-(ExpressionObject& self, float other);
    friend Expression operator*(ExpressionObject& self, float other);
    friend Expression operator/(ExpressionObject& self, float other);
    friend Expression operator+(float self, ExpressionObject& other);
    friend Expression operator-(float self, ExpressionObject& other);
    friend Expression operator*(float self, ExpressionObject& other);
    friend Expression operator/(float self, ExpressionObject& other);
    
    virtual ~ExpressionObject() = 0;
    
    bool isSimplified(){return simplified;}
    const size_t getTypeHash() {
        const size_t testName = typeid(*this).hash_code();
        return testName;
    }
};

class NullObject : public ExpressionObject {
public:
    NullObject(std::string newOrigin);
    std::string origin = "";
protected:
    Expression add(ExpressionObject* other);
    Expression subtract(ExpressionObject* other);
    Expression negate();
    Expression multiply(ExpressionObject* other);
    Expression divide(ExpressionObject* other);
    //virtual ptr commutator(SymbolicObject* other) = 0;
    /*
    bool containsTypeOfPerform(size_t type);
    bool containsPerform(Expression target);
    Expression firstInstanceOfTypePerform(, size_t type, bool rightToLeft);
    Expression removePerform(, Expression target, bool rightToLeft);
     */
    std::string print();
};

const size_t NULLTYPE = typeid(NullObject).hash_code();
Expression operator+(ExpressionObject& self, ExpressionObject& other);
Expression operator-(ExpressionObject& self, ExpressionObject& other);
Expression operator-(ExpressionObject& self);
Expression operator*(ExpressionObject& self, ExpressionObject& other);
Expression operator/(ExpressionObject& self, ExpressionObject& other);

Expression operator+(ExpressionObject& self, float other);
Expression operator-(ExpressionObject& self, float other);
Expression operator*(ExpressionObject& self, float other);
Expression operator/(ExpressionObject& self, float other);
Expression operator+(float self, ExpressionObject& other);
Expression operator-(float self, ExpressionObject& other);
Expression operator*(float self, ExpressionObject& other);
Expression operator/(float self, ExpressionObject& other);
bool operator==(ExpressionObject& left, ExpressionObject& right);
