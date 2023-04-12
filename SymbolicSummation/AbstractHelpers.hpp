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
#include "Container.hpp"

void initializeDefaultSymbols(bool force = false);
void initializeDefaultFunctions(bool force = false);
//bool isSubtypeOf(size_t sub, size_t super);
bool areSimilarTypes(Expression sub, Expression super);
bool isTypeSimilarTo(Expression sub, size_t superType);
bool areEqual(const ExpressionObject& left, const ExpressionObject& right);
int positionOfElement(ExprVector list, Expression target, bool rightToLeft = false);
int positionOfElementIgnoringSign(ExprVector list, Expression target, bool rightToLeft = false);
int positionOfType(ExprVector list, size_t type, bool rightToLeft = false);
bool exprVectorContains(ExprVector list, Expression target, bool rightToLeft = false);
bool exprVectorContainsType(ExprVector list, size_t type, bool rightToLeft = false);
bool intVectorContains(std::vector<int> container, int target);
String printExprMatrix(ExprMatrix target);
Expression getElementOfType(Expression source, size_t type, bool rightToLeft = false);
Expression getElementMatchingCondition(Expression source, std::function<bool(Expression)>, bool rightToLeft = false);
Expression getMatrixMatchingPauliFlavor(Expression target, Expression matrixToMatch);
Expression removeElementMultiplicatively(Expression source, Expression target, bool rightToLeft = false);
Expression removeElementAdditively(Expression source, Expression target, bool rightToLeft = false);
Expression removeElementAbsolutely(Expression source, Expression target, bool rightToLeft = false);
Expression replaceElement(Expression source, Expression target, Expression value, bool rightToLeft = false);
ExprVector replaceElementInVector(ExprVector source, Expression target, Expression value, bool rightToLeft = false);
Expression replaceElementOfType(Expression source, size_t type, Expression value, bool rightToLeft = false);
ExprVector replaceElementOfTypeInVector(ExprVector source, size_t type, Expression value, bool rightToLeft = false);
ExprVector setUnion(ExprVector setA, ExprVector setB);
ExprVector setIntersect(ExprVector setA, ExprVector setB);
ExprVector setDifference(ExprVector setA, ExprVector setB);
ExprVector combineExprVectors(ExprVector left, ExprVector right);
ExprVector generateExprVector(std::initializer_list<Expression> elements);
ExprMatrix generateExprMatrix(std::initializer_list<std::initializer_list<Expression>> elements);

Expression parseString(String exprString);

bool assignActionToFunction(String name, ExprAction action);
Expression declareFunction(String name);
Expression declareFunction(String name, ExprAction action);
Expression declareSymbol(String name);
Expression declareSymbol(String name, Expression value);
Expression declareReal(double value);
Expression declareEuclidVector(std::string name, ExprVector components);
Expression declareEuclidVector(std::string name, std::initializer_list<Expression> components);
Expression declareMatrix(String matName, ExprMatrix elements);
Expression declareMatrix(String matName, std::initializer_list<std::initializer_list<Expression>> elements);
Expression declareMatrix(String matName, Expression value);
Expression declarePauliMatrix(int index, String flavor);
Expression declarePauliMatrix(Expression value);
Expression declarePauliVector(bool up, String flavor);
#endif /* AbstractHelpers_hpp */
