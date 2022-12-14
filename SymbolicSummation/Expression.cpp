#include "Real.hpp"
#include "Container.hpp"
#include "AlgebraicHelpers.hpp"
#include "AbstractHelpers.hpp"

size_t Expression::getTypeHash() const {
    return (**this).getTypeHash();
};

Expression Expression::add(Expression other) const {
    //Expression thisObj = **this;
    return (**this).add(other);
};
Expression Expression::subtract(Expression other) const {
    return (**this).subtract(other);
};
Expression Expression::negate() const {
    return (**this).negate();
};
Expression Expression::multiply(Expression other) const {
    return (**this).multiply(other);
};
Expression Expression::divide(Expression other) const {
    return (**this).divide(other);
};

String Expression::print() const {
    return (**this).print();
};

Expression Expression::simplify() const {
    return (**this).simplify();
};
Expression Expression::distribute(Expression other) const {
    return (**this).distribute(other);
};
Expression Expression::factor() const {
    return (**this).factor();
};

Expression operator+(Expression self, Expression other) {
    return self.add(other);
};
Expression operator-(Expression self, Expression other) {
    return self.subtract(other);
};
Expression operator-(Expression self) {
    return self.negate();
};
Expression operator*(Expression self, Expression other) {
    return self.multiply(other);
};
Expression operator/(Expression self, Expression other) {
    return self.divide(other);
};

Expression operator+(Expression self, float other) {
    Expression newReal = declareReal(other);
    return self.add(newReal);
};
Expression operator-(Expression self, float other) {
    Expression newReal = declareReal(other);
    return self.subtract(newReal);
};
Expression operator*(Expression self, float other) {
    Expression newReal = declareReal(other);
    return self.multiply(newReal);
};
Expression operator/(Expression self, float other) {
    Expression newReal = declareReal(other);
    return self.divide(newReal);
};
Expression operator+(float self, Expression other) {
    Expression newReal = declareReal(self);
    return newReal.add(other);
};
Expression operator-(float self, Expression other) {
    Expression newReal = declareReal(self);
    return newReal.subtract(other);
};
Expression operator*(float self, Expression other) {
    Expression newReal = declareReal(self);
    return newReal.multiply(other);
};
Expression operator/(float self, Expression other) {
    Expression newReal = declareReal(self);
    return newReal.divide(other);
};
bool operator==(Expression self, Expression other) {
    return areEqual(*self, *other);
};
