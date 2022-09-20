//
//  ExpressionHelpers.hpp
//  SymbolicSummation
//
//  Created by Robert Stahulak on 2/1/22.
//

#ifndef ExpressionHelpers_hpp
#define ExpressionHelpers_hpp
#pragma once
#include <stdio.h>
#include <unordered_map>
#include "Expression.h"
//typedef std::vector<Expression> ExprVector;
/*

#include "Real.hpp"
#include "Operator.hpp"
#include "Symbol.hpp"
 */

bool commutesWith(ExpressionObject* left, ExpressionObject* right);
Expression reciprocal(ExpressionObject* self);
Expression matMul(ExpressionObject* left, ExpressionObject* right);
Expression determinant(ExpressionObject* target);
Expression transpose(ExpressionObject* target);
Expression combineProducts(ExpressionObject* left, ExpressionObject* right);
Expression combineSums(ExpressionObject* left, ExpressionObject* right);
Expression cancelTerms(ExpressionObject* target);
Expression simplify(ExpressionObject* target);
Expression distribute(ExpressionObject* left, ExpressionObject* right);
ExprVector getFactors(ExpressionObject* factee);
ExprVector getCommonFactors(ExprVector terms);
ExprVector getSimpleCommonFactors(ExprVector terms);
Expression combineTermsDifferingByCoefficientsAdditively(ExpressionObject* left, ExpressionObject* right);
Expression factor(ExpressionObject* factee);
Expression substitute(ExpressionObject* source, ExpressionObject* target, ExpressionObject* value);
Expression substitute(ExpressionObject* source, std::vector<std::pair<Expression,Expression>> substitutions);

#endif /* ExpressionHelpers_hpp */
