#pragma once
#include <memory>
#include <string>
#include <vector>
#include <typeinfo>
#include "ExpressionObject.hpp"
#include <iostream>


/**
 *Class derived from std::shared_ptr; may be used in algebraic expressions with +,-,*,/ etc.
 *
 *This is the preferred object to deal with when performing symbolic algebra operations, as extracting the ExpressionObject
 an Expression points to may transfer ownership.
 */
class Expression : public std::shared_ptr<ExpressionObject> {
protected:
    Expression add(Expression other);
    Expression subtract(Expression other);
    Expression negate();
    Expression multiply(Expression other);
    Expression divide(Expression other);
    void setSimplified(bool simpled);
    public:
    
    /**
     *Default initialization of Expression smart pointer. The resulting object cannot be used without throwing a logic error, due to containing a NullObject.
    */
    Expression() : std::shared_ptr<ExpressionObject>(new NullObject("default initialzation of Expression")) {
        
    };
    
    /**
     *Basic constructor for an Expression object. Expected to be used with 'new' keyword, allocating memory on the heap.
     *@param ptr a newly constructed or extracted ExpressionObject pointer
     */
    Expression(ExpressionObject* ptr) : std::shared_ptr<ExpressionObject>(ptr) {
    };
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
typedef std::vector<ExprVector> ExprMatrix;
typedef std::vector<bool> SignVector;


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
bool operator==(Expression self, Expression other);


//#include "Real.hpp"
//#include "Operator.hpp"
//#include "ExpressionHelpers.hpp"



