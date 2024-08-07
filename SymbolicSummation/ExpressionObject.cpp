#include "ExpressionObject.hpp"
#include "Expression.hpp"
#include "AbstractHelpers.hpp"
ExpressionObject::~ExpressionObject(){};
/*
Expression operator+(const ExpressionObject& self, const ExpressionObject& other) {
    return self.add(&other);
};
Expression operator-(const ExpressionObject& self, const ExpressionObject& other) {
    return self.subtract(&other);
};
Expression operator-(const ExpressionObject& self) {
    return self.negate();
};
Expression operator*(const ExpressionObject& self, const ExpressionObject& other) {
    return self.multiply(&other);
};
Expression operator/(const ExpressionObject& self, const ExpressionObject& other) {
    return self.divide(&other);
};

Expression operator+(const ExpressionObject& self, float other) {
    const ExpressionObject& realObj = *declareReal(other);
    return self.add(&realObj);
};
Expression operator-(const ExpressionObject& self, float other) {
    const ExpressionObject& realObj = *declareReal(other);
    return self.subtract(&realObj);
};
Expression operator*(const ExpressionObject& self, float other) {
    const ExpressionObject& realObj = *declareReal(other);
    return self.multiply(&realObj);
};
Expression operator/(const ExpressionObject& self, float other) {
    const ExpressionObject& realObj = *declareReal(other);
    return self.divide(&realObj);
};
Expression operator+(float self, const ExpressionObject& other) {
    const ExpressionObject& realObj = *declareReal(self);
    return realObj.add(&other);
};
Expression operator-(float self, const ExpressionObject& other) {
    const ExpressionObject& realObj = *declareReal(self);
    return realObj.subtract(&other);
};
Expression operator*(float self, const ExpressionObject& other) {
    const ExpressionObject& realObj = *declareReal(self);
    return realObj.multiply(&other);
};
Expression operator/(float self, const ExpressionObject& other) {
    const ExpressionObject& realObj = *declareReal(self);
    return realObj.divide(&other);
};

bool operator==(const ExpressionObject& left, const ExpressionObject& right) {
    return areEqual(left, right);
};*/

NullObject::NullObject(String newOrigin) {
    origin = newOrigin;
}
Expression NullObject::add(Expression other) const {
    throw std::runtime_error("null object: "+origin);
};
Expression NullObject::subtract(Expression other) const {
    throw std::runtime_error("null object: "+origin);
};
Expression NullObject::negate() const {
    throw std::runtime_error("null object: "+origin);
};
Expression NullObject::multiply(Expression left, Expression right) const {
    throw std::runtime_error("null object: "+origin);
};
Expression NullObject::divide(Expression other) const {
    throw std::runtime_error("null object: "+origin);
};
String NullObject::print() const {
    throw std::runtime_error("null object: "+origin);
};
Expression NullObject::simplify() const {
    throw std::runtime_error("null object: "+origin);
};
/*Expression NullObject::distribute(Expression other) const {
    throw std::runtime_error("null object: "+origin);
};*/
Expression NullObject::factor() const {
    throw std::runtime_error("null object: "+origin);
};
Expression NullObject::reciprocal() const {
    throw std::runtime_error("null object: "+origin);
};
Expression NullObject::determinant() const {
    throw std::runtime_error("null object: "+origin);
};
Expression NullObject::transpose() const {
    throw std::runtime_error("null object: "+origin);
};
ExprVector NullObject::getFactors() const {
    throw std::runtime_error("null object: "+origin);
};
