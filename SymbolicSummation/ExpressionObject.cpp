#include "ExpressionObject.h"
#include "Expression.h"
#include "AbstractHelpers.hpp"
ExpressionObject::~ExpressionObject(){};

Expression operator+(ExpressionObject& self, ExpressionObject& other) {
    return self.add(&other);
};
Expression operator-(ExpressionObject& self, ExpressionObject& other) {
    return self.subtract(&other);
};
Expression operator-(ExpressionObject& self) {
    return self.negate();
};
Expression operator*(ExpressionObject& self, ExpressionObject& other) {
    return self.multiply(&other);
};
Expression operator/(ExpressionObject& self, ExpressionObject& other) {
    return self.divide(&other);
};

Expression operator+(ExpressionObject& self, float other) {
    ExpressionObject& realObj = *declareReal(other);
    return self.add(&realObj);
};
Expression operator-(ExpressionObject& self, float other) {
    ExpressionObject& realObj = *declareReal(other);
    return self.subtract(&realObj);
};
Expression operator*(ExpressionObject& self, float other) {
    ExpressionObject& realObj = *declareReal(other);
    return self.multiply(&realObj);
};
Expression operator/(ExpressionObject& self, float other) {
    ExpressionObject& realObj = *declareReal(other);
    return self.divide(&realObj);
};
Expression operator+(float self, ExpressionObject& other) {
    ExpressionObject& realObj = *declareReal(self);
    return realObj.add(&other);
};
Expression operator-(float self, ExpressionObject& other) {
    ExpressionObject& realObj = *declareReal(self);
    return realObj.subtract(&other);
};
Expression operator*(float self, ExpressionObject& other) {
    ExpressionObject& realObj = *declareReal(self);
    return realObj.multiply(&other);
};
Expression operator/(float self, ExpressionObject& other) {
    ExpressionObject& realObj = *declareReal(self);
    return realObj.divide(&other);
};

bool operator==(ExpressionObject& left, ExpressionObject& right) {
    return areEqual(left, right);
};

NullObject::NullObject(std::string newOrigin) {
    origin = newOrigin;
}
Expression NullObject::add(ExpressionObject* other) {
    throw std::logic_error("null object: "+origin);
    //Expression result = *new Expression(new NullObject());
    //return result;
};
Expression NullObject::subtract(ExpressionObject* other) {
    throw std::logic_error("null object: "+origin);
    //Expression result = *new Expression(new NullObject());
    //return result;
};
Expression NullObject::negate() {
    throw std::logic_error("null object: "+origin);
    //Expression result = *new Expression(new NullObject());
    //return result;
};
Expression NullObject::multiply(ExpressionObject* other) {
    throw std::logic_error("null object: "+origin);
    //Expression result = *new Expression(new NullObject());
    //return result;
};
Expression NullObject::divide(ExpressionObject* other) {
    throw std::logic_error("null object: "+origin);
    //Expression result = *new Expression(new NullObject());
    //return result;
};
//virtual ptr commutator(SymbolicObject* other) = 0;
/*
bool NullObject::containsTypeOf(size_t type) {
    throw std::logic_error("null object: "+origin);
    //return false;
};
bool NullObject::contains(Expression target) {
    throw std::logic_error("null object: "+origin);
    //return false;
};
Expression NullObject::firstInstanceOfType(, size_t type, bool rightToLeft) {
    throw std::logic_error("null object: "+origin);
    //Expression result = *new Expression(new NullObject());
    //return result;
};
Expression NullObject::remove(, Expression target, bool rightToLeft) {
    throw std::logic_error("null object: "+origin);
    //Expression result = *new Expression(new NullObject());
    //return result;
};
 */
std::string NullObject::print() {
    throw std::logic_error("null object: "+origin);
    //return NULL;
};
