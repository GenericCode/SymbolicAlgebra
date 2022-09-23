//
//  PauliMatrices.hpp
//  SymbolicSummation
//
//  Created by Laina Stahulak on 4/8/22.
//

#ifndef PauliMatrices_hpp
#define PauliMatrices_hpp

#include <stdio.h>
#include "Symbol.hpp"

class PauliMatrix : public Matrix {
protected:
    int index = 0;
    std::string flavor = "";
    Expression add(Expression other) const;
    Expression subtract(Expression other) const;
    Expression multiply(Expression other) const;
    Expression negate() const;
public:
    std::string print() {
        return name;
    };
    PauliMatrix(std::string name, int index, std::string flavor, std::initializer_list<std::initializer_list<Expression>> newElements );
    PauliMatrix(std::string name, int index, std::string flavor, ExprMatrix elements);
    PauliMatrix(const PauliMatrix& target);
    PauliMatrix& operator=(const PauliMatrix& target);
    PauliMatrix(int index, std::string flavor = "");
    ~PauliMatrix();
    friend Expression matMul(Expression left, Expression right);
    friend ExprVector getConstituentSymbols(Expression target);
    friend Expression transpose(Expression target);
    friend Expression getMatrixMatchingPauliFlavor(Expression target, Expression matrixToMatch);
    friend bool areEqual(const ExpressionObject& left, const ExpressionObject& right);
    friend Expression substitute(Expression source, Expression target, Expression value);
};

static const size_t PAULIMATRIXTYPE = typeid(PauliMatrix).hash_code();
//static const size_t PAULIVECTORTYPE = typeid(PauliVector).hash_code();
#endif /* PauliMatrices_hpp */
