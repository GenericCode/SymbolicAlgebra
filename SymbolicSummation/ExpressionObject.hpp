#pragma once
//#include "Expression.hpp"
#include <string>
class Expression;
/**
 *The abstract class from which all structures for the symbolic algebra library are derived. Like Expression objects, these also support algebraic operations (+, -, *, /, etc.)
 */
class ExpressionObject {
    friend Expression;
public:
//protected:
    virtual Expression add(ExpressionObject* other) = 0;
    virtual Expression subtract(ExpressionObject* other) = 0;
    virtual Expression negate() = 0;
    virtual Expression multiply(ExpressionObject* other) = 0;
    virtual Expression divide(ExpressionObject* other) = 0;
    
    bool simplified = false;
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

/**
 *The null object for ExpressionObjects. Created whenever it makes sense to have an 'unitialized' ExpressionObject. Cannot be safely used, as they will throw std::logic_error, and so the output of many functions must be tested with getTypeHash() for NULLTYPE. Contains a public origin string, describing why the object was created.
 */
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
