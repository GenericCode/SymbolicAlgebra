//
//  ExpressionHelpers.hpp
//  SymbolicSummation
//
//  Created by Laina Stahulak on 2/1/22.
//

#ifndef ExpressionHelpers_hpp
#define ExpressionHelpers_hpp
#pragma once
#include <stdio.h>
#include <unordered_map>
#include "Expression.hpp"
//typedef std::vector<Expression> ExprVector;
/*

#include "Real.hpp"
#include "Operator.hpp"
#include "Symbol.hpp"
 */

bool commutesWith(Expression left, Expression right);
Expression performActions(Expression target);
Expression performActionsOn(Expression target, Expression var);
Expression insertAsVariable(Expression target, Expression var);
Expression reciprocal(Expression self);
Expression matMul(Expression left, Expression right);
Expression determinant(Expression target);
Expression transpose(Expression target);
Expression combineProducts(Expression left, Expression right);
Expression combineSums(Expression left, Expression right);
Expression cancelTerms(Expression target);
Expression simplify(Expression target);
Expression distribute(Expression left, Expression right);
ExprVector getFactors(Expression factee);
ExprVector getCommonFactors(ExprVector terms);
ExprVector getSimpleCommonFactors(ExprVector terms);
Expression combineTermsDifferingByCoefficientsAdditively(Expression left, Expression right);
Expression factor(Expression factee);
Expression substitute(Expression source, Expression target, Expression value);
Expression substitute(Expression source, std::vector<std::pair<Expression,Expression>> substitutions);

#endif /* ExpressionHelpers_hpp */
