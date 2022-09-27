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
    String flavor = "";
    Expression add(Expression other) const;
    Expression subtract(Expression other) const;
    Expression multiply(Expression other) const;
    Expression negate() const;
public:
    String print() {
        return name;
    };
    PauliMatrix(String name, int index, String flavor, std::initializer_list<std::initializer_list<Expression>> newElements );
    PauliMatrix(String name, int index, String flavor, ExprMatrix elements);
    PauliMatrix(const PauliMatrix& target);
    PauliMatrix& operator=(const PauliMatrix& target);
    PauliMatrix(int index, String flavor = "");
    ~PauliMatrix();
    friend Expression matMul(Expression left, Expression right);
    friend ExprVector getConstituentSymbols(Expression target);
    friend Expression transpose(Expression target);
    friend Expression getMatrixMatchingPauliFlavor(Expression target, Expression matrixToMatch);
    friend bool areEqual(const ExpressionObject& left, const ExpressionObject& right);
    friend Expression substitute(Expression source, Expression target, Expression value);
    friend Expression performActions(Expression target);
    friend Expression performActionsOn(Expression target, Expression var);
    friend Expression insertAsVariable(Expression target, Expression var);
};

static const size_t PAULIMATRIXTYPE = typeid(PauliMatrix).hash_code();
//static const size_t PAULIVECTORTYPE = typeid(PauliVector).hash_code();
#endif /* PauliMatrices_hpp */
