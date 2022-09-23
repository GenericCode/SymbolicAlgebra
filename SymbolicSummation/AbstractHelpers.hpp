//
//  AbstractHelpers.hpp
//  SymbolicSummation
//
//  Created by Laina Stahulak on 6/22/22.
//

#ifndef AbstractHelpers_hpp
#define AbstractHelpers_hpp

#include <stdio.h>
#include "Expression.hpp"

void initializeDefaultSymbols();
//bool isSubtypeOf(size_t sub, size_t super);
bool isSubtypeOf(Expression sub, Expression super);
bool isSubtypeOf(Expression sub, size_t superType);
bool areEqual(const ExpressionObject& left, const ExpressionObject& right);
int positionOfElement(ExprVector list, Expression target, bool rightToLeft = false);
int positionOfElementIgnoringSign(ExprVector list, Expression target, bool rightToLeft = false);
int positionOfType(ExprVector list, size_t type, bool rightToLeft = false);
bool containsElement(ExprVector list, Expression target, bool rightToLeft = false);
bool containsType(ExprVector list, size_t type, bool rightToLeft = false);
bool intVectorContains(std::vector<int> container, int target);
std::string printExprMatrix(ExprMatrix target);
Expression getElementOfType(Expression source, size_t type, bool rightToLeft = false);
Expression removeElementMultiplicatively(Expression source, Expression target, bool rightToLeft = false);
Expression removeElementAdditively(Expression source, Expression target, bool rightToLeft = false);
Expression removeElementAbsolutely(Expression source, Expression target, bool rightToLeft = false);
Expression replaceElement(Expression source, Expression target, Expression value, bool rightToLeft = false);
ExprVector replaceElementInVector(ExprVector source, Expression target, Expression value, bool rightToLeft = false);
Expression replaceElementOfType(Expression source, size_t type, Expression value, bool rightToLeft = false);
ExprVector replaceElementOfTypeInVector(ExprVector source, size_t type, Expression value, bool rightToLeft = false);
ExprVector combineExprVectors(ExprVector left, ExprVector right);
ExprVector generateExprVector(std::initializer_list<Expression> elements);
ExprMatrix generateExprMatrix(std::initializer_list<std::initializer_list<Expression>> elements);

Expression parseString(std::string exprString);

Expression declareSymbol(std::string name);
Expression declareSymbol(std::string name, Expression value);
Expression declareReal(float value);
Expression declareMatrix(std::string matName, ExprMatrix elements);
Expression declareMatrix(std::string matName, std::initializer_list<std::initializer_list<Expression>> elements);
Expression declareMatrix(std::string matName, Expression value);
Expression declarePauliMatrix(int index, std::string flavor);
Expression declarePauliMatrix(Expression value);
Expression declarePauliVector(bool up, std::string flavor);
#endif /* AbstractHelpers_hpp */
