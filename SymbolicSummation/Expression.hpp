#pragma once
#include <memory>
#include <string>
#include <vector>
#include <typeinfo>
#include "ExpressionObject.hpp"
#include <iostream>

class Expression : public std::shared_ptr<ExpressionObject> {
protected:
    Expression add(Expression other);
    Expression subtract(Expression other);
    Expression negate();
    Expression multiply(Expression other);
    Expression divide(Expression other);
    void setSimplified(bool simpled);
    public:
    
    Expression() : std::shared_ptr<ExpressionObject>(new NullObject("default initialzation of Expression")) {
        
    };
    
    Expression(ExpressionObject* ptr) : std::shared_ptr<ExpressionObject>(ptr) {
        //if((*ptr).getTypeHash() != NULLTYPE)
        //    std::cout << (*ptr).print() + "\n";
    };
    
    /*Expression(SymbolicObject& obj) : std::shared_ptr<SymbolicObject>(&obj) {
        //if((obj).getTypeHash() != NULLTYPE)
        //    std::cout << (obj).print() + "\n";
    };*/
    
    /*
    Expression(SymbolicObject* ptr, bool isSimplified) : std::shared_ptr<SymbolicObject>(ptr) {
        simplified = isSimplified;
    };*/
    
    //bool containsTypeOf(size_t type);
    //bool contains(Expression target);
    //Expression getFirstInstanceOfType(size_t type, bool rightToLeft = false);
    //Expression remove(Expression target, bool rightToLeft = false);
    size_t getTypeHash() const;
    std::string print();
    friend Expression operator+(Expression self, Expression other);
    friend Expression operator-(Expression self, Expression other);
    friend Expression operator-(Expression self);
    friend Expression operator*(Expression self, Expression other);
    friend Expression operator/(Expression self, Expression other);
    friend Expression operator+(Expression self, float other);
    friend Expression operator-(Expression self, float other);
    friend Expression operator*(Expression self, float other);
    friend Expression operator/(Expression self, float other);
    friend Expression operator+(float self, Expression other);
    friend Expression operator-(float self, Expression other);
    friend Expression operator*(float self, Expression other);
    friend Expression operator/(float self, Expression other);
    friend bool operator==(Expression self, Expression other);
    
    friend Expression simplify(ExpressionObject* target);
};

typedef std::vector<Expression> ExprVector;
//typedef std::vector<ExprVector> ExprMatrix;
//typedef std::shared_ptr<ExprVectorObj> ExprVector;

/*class ExprVector : public std::shared_ptr<ExprVectorObject> {
public:
    ExprVector(ExprVectorObject* ptr) : std::shared_ptr<ExprVectorObject>(ptr) {
        
    }
    
    ExprVector() : std::shared_ptr<ExprVectorObject>(new ExprVectorObject()) {
        
    }
    
    Expression& operator[](int index) const {
        return (**this)[index];
    }
    
    void push_back(Expression newMember) {
        get()->push_back(newMember);
    }
    
    ExprVector& operator=(const ExprVector& target) {
        if(this == &target)
            return *this;
        ExprVectorObject* newObj = new ExprVectorObject(*target);
        (*this).reset(newObj);
        return *this;
    }
    
    std::vector<ExprVectorObject>::size_type size() const {
        return get()->size();
    }
    
    
};*/

typedef std::vector<ExprVector> ExprMatrix;
/*
class ExprMatrix : public std::shared_ptr<ExprMatrixObject> {
public:
    ExprMatrix(ExprMatrixObject* ptr) : std::shared_ptr<ExprMatrixObject>(ptr) {
        
    }
    
    ExprMatrix() : std::shared_ptr<ExprMatrixObject>(new ExprMatrixObject()) {
        
    }
    
    ExprVector& operator[](int index) {
        return (**this)[index];
    }
    
    void push_back(ExprVector newMember) {
        get()->push_back(newMember);
    }
    
    ExprMatrix& operator=(const ExprMatrix& target) {
        if(this == &target)
            return *this;
        ExprMatrixObject* newObj = new ExprMatrixObject(*target);
        (*this).reset(newObj);
        return *this;
    }
    
    size_t size() {
        return get()->size();
    }
};*/

typedef std::vector<bool> SignVector;
/*
class SignVector : public std::shared_ptr<SignVectorObject> {
public:
    SignVector(SignVectorObject* ptr) : std::shared_ptr<SignVectorObject>(ptr) {
        
    }
    
    SignVector() : std::shared_ptr<SignVectorObject>(new SignVectorObject()) {
        
    }
    
    SignVector(std::initializer_list<bool> vals) : std::shared_ptr<SignVectorObject>(new SignVectorObject(vals)) {
        
    }
    
    bool operator[](int index) {
        return (**this)[index];
    }
    
    void push_back(bool newMember) {
        get()->push_back(newMember);
    }
    
    SignVector& operator=(const SignVector& target) {
        if(this == &target)
            return *this;
        SignVectorObject* newObj = new SignVectorObject(*target);
        (*this).reset(newObj);
        return *this;
    }
    
    size_t size() {
        return get()->size();
    }
};*/


Expression operator+(Expression self, Expression other);
Expression operator-(Expression self, Expression other);
Expression operator-(Expression self);
Expression operator*(Expression self, Expression other);
Expression operator/(Expression self, Expression other);
Expression operator+(Expression self, float other);
Expression operator-(Expression self, float other);
Expression operator*(Expression self, float other);
Expression operator/(Expression self, float other);
Expression operator+(float self, Expression other);
Expression operator-(float self, Expression other);
Expression operator*(float self, Expression other);
Expression operator/(float self, Expression other);
/*
void operator+=(Expression self, Expression other);
void operator-=(Expression self, Expression other);
void operator*=(Expression self, Expression other);
void operator/=(Expression self, Expression other);
void operator+=(Expression self, float other);
void operator-=(Expression self, float other);
void operator*=(Expression self, float other);
void operator/=(Expression self, float other);
void operator+=(float self, Expression other);
void operator-=(float self, Expression other);
void operator*=(float self, Expression other);
void operator/=(float self, Expression other);*/
bool operator==(Expression self, Expression other);


//#include "Real.hpp"
//#include "Operator.hpp"
//#include "ExpressionHelpers.hpp"



