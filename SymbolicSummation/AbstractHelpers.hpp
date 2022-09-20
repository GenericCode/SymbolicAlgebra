//
//  AbstractHelpers.hpp
//  SymbolicSummation
//
//  Created by Robert Stahulak on 6/22/22.
//

#ifndef AbstractHelpers_hpp
#define AbstractHelpers_hpp

#include <stdio.h>
#include "Expression.h"

void initializeDefaultSymbols();
//bool isSubtypeOf(size_t sub, size_t super);
bool isSubtypeOf(ExpressionObject* sub, ExpressionObject* super);
bool isSubtypeOf(ExpressionObject* sub, size_t superType);
bool areEqual(ExpressionObject& left, ExpressionObject& right);
int positionOfElement(ExprVector list, ExpressionObject* target, bool rightToLeft = false);
int positionOfElementIgnoringSign(ExprVector list, ExpressionObject* target, bool rightToLeft = false);
int positionOfType(ExprVector list, size_t type, bool rightToLeft = false);
bool containsElement(ExprVector list, ExpressionObject* target, bool rightToLeft = false);
bool containsType(ExprVector list, size_t type, bool rightToLeft = false);
bool intVectorContains(std::vector<int> container, int target);
std::string printExprMatrix(ExprMatrix target);
Expression getElementOfType(ExpressionObject* source, size_t type, bool rightToLeft = false);
Expression removeElementMultiplicatively(ExpressionObject* source, ExpressionObject* target, bool rightToLeft = false);
Expression removeElementAdditively(ExpressionObject* source, ExpressionObject* target, bool rightToLeft = false);
Expression removeElementAbsolutely(ExpressionObject* source, ExpressionObject* target, bool rightToLeft = false);
Expression replaceElement(ExpressionObject* source, ExpressionObject* target, ExpressionObject* value, bool rightToLeft = false);
ExprVector replaceElementInVector(ExprVector source, ExpressionObject* target, ExpressionObject* value, bool rightToLeft = false);
Expression replaceElementOfType(ExpressionObject* source, size_t type, ExpressionObject* value, bool rightToLeft = false);
ExprVector replaceElementOfTypeInVector(ExprVector source, size_t type, ExpressionObject* value, bool rightToLeft = false);
ExprVector combineExprVectors(ExprVector left, ExprVector right);
ExprVector generateExprVector(std::initializer_list<Expression> elements);
ExprMatrix generateExprMatrix(std::initializer_list<std::initializer_list<Expression>> elements);

Expression parseString(std::string exprString);

Expression declareSymbol(std::string name);
Expression declareSymbol(std::string name, ExpressionObject* value);
Expression declareReal(float value);
Expression declareMatrix(std::string matName, ExprMatrix elements);
Expression declareMatrix(std::string matName, std::initializer_list<std::initializer_list<Expression>> elements);
Expression declareMatrix(std::string matName, ExpressionObject* value);
Expression declarePauliMatrix(int index, std::string flavor);
Expression declarePauliMatrix(ExpressionObject* value);
Expression declarePauliVector(bool up, std::string flavor);
#endif /* AbstractHelpers_hpp */
