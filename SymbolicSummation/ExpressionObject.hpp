#pragma once
//#include "Expression.hpp"
#include <string>
class Expression;
typedef std::string String;
typedef std::vector<Expression> ExprVector;
/**
 *The abstract class from which all structures for the symbolic algebra library are derived. Like Expression objects, these also support algebraic operations (+, -, *, /, etc.)
 */
class ExpressionObject {
    friend Expression;
public:
//protected:
    virtual Expression add(Expression other) const = 0;
    virtual Expression subtract(Expression other) const = 0;
    virtual Expression negate() const = 0;
    virtual Expression multiply(Expression other) const = 0;
    virtual Expression divide(Expression other) const = 0;
    virtual Expression simplify() const = 0;
    virtual Expression distribute(Expression other) const = 0;
    virtual Expression factor() const = 0;
    virtual Expression reciprocal() const = 0;
    virtual Expression determinant() const = 0;
    virtual Expression transpose() const = 0;
    virtual ExprVector getFactors() const = 0;
    
    bool simplified = false;
public:
    virtual String print() const = 0;
    friend bool areEqual(Expression left, Expression right);
    friend Expression simplify(Expression target);
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
    
    //virtual ~ExpressionObject() = 0;
    
    bool isSimplified(){return simplified;}
    size_t getTypeHash() const {
        size_t testName = typeid(*this).hash_code();
        return testName;
    }
};

/**
 *The null object for ExpressionObjects. Created whenever it makes sense to have an 'uninitialized' ExpressionObject. Cannot be safely used, as they will throw std::logic_error, and so the output of many functions must be tested with getTypeHash() for NULLTYPE. Contains a public origin string, describing why the object was created.
 */
class NullObject : public ExpressionObject {
public:
    NullObject(String newOrigin);
    String origin = "";
protected:
    Expression add(Expression other) const ;
    Expression subtract(Expression other) const;
    Expression negate() const;
    Expression multiply(Expression other) const;
    Expression divide(Expression other) const;
public:
    String print() const;
    Expression simplify() const;
    Expression distribute(Expression other) const;
    Expression factor() const;
    Expression reciprocal() const;
    Expression determinant() const;
    Expression transpose() const;
    ExprVector getFactors() const;
};

const size_t NULLTYPE = typeid(NullObject).hash_code();
Expression operator+(const ExpressionObject& self, const ExpressionObject& other);
Expression operator-(const ExpressionObject& self, const ExpressionObject& other);
Expression operator-(const ExpressionObject& self);
Expression operator*(const ExpressionObject& self, const ExpressionObject& other);
Expression operator/(const ExpressionObject& self, const ExpressionObject& other);

Expression operator+(const ExpressionObject& self, float other);
Expression operator-(const ExpressionObject& self, float other);
Expression operator*(const ExpressionObject& self, float other);
Expression operator/(const ExpressionObject& self, float other);
Expression operator+(float self, const ExpressionObject& other);
Expression operator-(float self, const ExpressionObject& other);
Expression operator*(float self, const ExpressionObject& other);
Expression operator/(float self, const ExpressionObject& other);
bool operator==(const ExpressionObject& left, const ExpressionObject& right);
