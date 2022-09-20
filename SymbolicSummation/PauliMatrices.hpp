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
    Expression add(ExpressionObject* other);
    Expression subtract(ExpressionObject* other);
    Expression multiply(ExpressionObject* other);
    Expression negate();
public:
    std::string print() {
        return name;
    };
    PauliMatrix(std::string name, int index, std::string flavor, std::initializer_list<std::initializer_list<Expression>> newElements );
    PauliMatrix(std::string name, int index, std::string flavor, ExprMatrix elements);
    PauliMatrix(const PauliMatrix& target);
    PauliMatrix(int index, std::string flavor = "");
    ~PauliMatrix();
    friend Expression matMul(ExpressionObject* left, ExpressionObject* right);
    friend ExprVector getConstituentSymbols(ExpressionObject* target);
    friend Expression transpose(ExpressionObject* target);
    friend Expression getMatrixMatchingPauliFlavor(ExpressionObject* target, ExpressionObject* matrixToMatch);
    friend bool areEqual(ExpressionObject& left, ExpressionObject& right);
    friend Expression substitute(ExpressionObject* source, ExpressionObject* target, ExpressionObject* value);
};

static const size_t PAULIMATRIXTYPE = typeid(PauliMatrix).hash_code();
//static const size_t PAULIVECTORTYPE = typeid(PauliVector).hash_code();
#endif /* PauliMatrices_hpp */
