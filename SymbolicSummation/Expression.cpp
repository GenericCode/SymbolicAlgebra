#include "Real.hpp"
#include "Container.hpp"
#include "AlgebraicHelpers.hpp"
#include "AbstractHelpers.hpp"

void Expression::setSimplified(bool simpled) {
    (**this).simplified = simpled;
};

size_t Expression::getTypeHash() const {
    return (**this).getTypeHash();
};

Expression Expression::add(Expression other) {
    //Expression thisObj = **this;
    return (**this).add(other.get());
};
Expression Expression::subtract(Expression other) {
    return (**this).subtract(other.get());
};
Expression Expression::negate() {
    return (**this).negate();
};
Expression Expression::multiply(Expression other) {
    return (**this).multiply(other.get());
};
Expression Expression::divide(Expression other) {
    return (**this).divide(other.get());
};

std::string Expression::print() {
    return (**this).print();
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
    return (*self) == (*other);
};
