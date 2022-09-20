#include "ExpressionObject.hpp"
#include "Expression.hpp"
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
};
Expression NullObject::subtract(ExpressionObject* other) {
    throw std::logic_error("null object: "+origin);
};
Expression NullObject::negate() {
    throw std::logic_error("null object: "+origin);
};
Expression NullObject::multiply(ExpressionObject* other) {
    throw std::logic_error("null object: "+origin);
};
Expression NullObject::divide(ExpressionObject* other) {
    throw std::logic_error("null object: "+origin);
};
std::string NullObject::print() {
    throw std::logic_error("null object: "+origin);
    //return NULL;
};
