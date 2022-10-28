//
//  main.cpp
//  SymbolicSummation
//
//  Created by Laina Stahulak on 1/27/22.
//

#include <iostream>
#include "Expression.hpp"
#include "AlgebraicHelpers.hpp"
#include "AbstractHelpers.hpp"
#include "Real.hpp"
#include "Symbol.hpp"
#include "PauliMatrices.hpp"
Expression matrixElement(Expression potential,Expression initialState, Expression finalState) {
    Expression temp = transpose(initialState)*potential*finalState;
    Expression result = simplify(temp);
    return result;
}

/*
 TODO:
 Need to restructure Add object so that it no longer needs to track signage; instead, only Sign will be used to track signage.
 
 
 
 
 */

Expression insertProperQVectors(Expression potential,Expression qOne, Expression qTwo, Expression qThree = *new Expression(new NullObject("two body calc"))) {
    Expression qOneX =      declareSymbol("qOneX");
    Expression qOneY =      declareSymbol("qOneY");
    Expression qOneZ =      declareSymbol("qOneZ");
    Expression qTwoX =      declareSymbol("qTwoX");
    Expression qTwoY =      declareSymbol("qTwoY");
    Expression qTwoZ =      declareSymbol("qTwoZ");
    Expression qThreeX =    declareSymbol("qThreeX");
    Expression qThreeY =    declareSymbol("qThreeY");
    Expression qThreeZ =    declareSymbol("qThreeZ");
    
    Expression modifiedPotential = substitute(potential, qOneX, XUNITVECTOR*qOne);
    modifiedPotential = substitute(modifiedPotential, qOneY, YUNITVECTOR*qOne);
    modifiedPotential = substitute(modifiedPotential, qOneZ, ZUNITVECTOR*qOne);
    modifiedPotential = substitute(modifiedPotential, qTwoX, XUNITVECTOR*qTwo);
    modifiedPotential = substitute(modifiedPotential, qTwoY, YUNITVECTOR*qTwo);
    modifiedPotential = substitute(modifiedPotential, qTwoZ, ZUNITVECTOR*qTwo);
    if(qThree.getTypeHash() != NULLTYPE) {
        modifiedPotential = substitute(modifiedPotential, qThreeX, XUNITVECTOR*qThree);
        modifiedPotential = substitute(modifiedPotential, qThreeY, YUNITVECTOR*qThree);
        modifiedPotential = substitute(modifiedPotential, qThreeZ, ZUNITVECTOR*qThree);
    }
    
    return modifiedPotential;
}

Expression spinIsospinSummation(ExprVector interactions, bool threeBody = false ) {
    //{sigA,sigB,tauOne,tauTwo}
    //int numTerms = (int)interactions.size();
    ExprVector sigmaOneStates = generateExprVector({declarePauliVector(false, "sigmaOne"),declarePauliVector(true, "sigmaOne")});
    ExprVector sigmaTwoStates = generateExprVector({declarePauliVector(false, "sigmaTwo"),declarePauliVector(true, "sigmaTwo")});
    ExprVector sigmaThreeStates = generateExprVector({declarePauliVector(false, "sigmaThree"),declarePauliVector(true, "sigmaThree")});
    ExprVector tauOneStates   = generateExprVector({declarePauliVector(false, "tauOne"),  declarePauliVector(true, "tauOne")});
    ExprVector tauTwoStates   = generateExprVector({declarePauliVector(false, "tauTwo"),  declarePauliVector(true, "tauTwo")});
    ExprVector tauThreeStates = generateExprVector({declarePauliVector(false, "tauTwo"),  declarePauliVector(true, "tauThree")});
    ExprVector sigmaOneMatrices = generateExprVector({declarePauliMatrix(1, "sigmaOne"),declarePauliMatrix(2, "sigmaOne"),declarePauliMatrix(3, "sigmaOne")});
    ExprVector sigmaTwoMatrices = generateExprVector({declarePauliMatrix(1, "sigmaTwo"),declarePauliMatrix(2, "sigmaTwo"),declarePauliMatrix(3, "sigmaTwo")});
    ExprVector sigmaThreeMatrices = generateExprVector({declarePauliMatrix(1, "sigmaThree"),declarePauliMatrix(2, "sigmaThree"),declarePauliMatrix(3, "sigmaThree")});
    ExprVector tauOneMatrices = generateExprVector({declarePauliMatrix(1, "tauOne"),declarePauliMatrix(2, "tauOne"),declarePauliMatrix(3, "tauOne")});
    ExprVector tauTwoMatrices = generateExprVector({declarePauliMatrix(1, "tauTwo"),declarePauliMatrix(2, "tauTwo"),declarePauliMatrix(3, "tauTwo")});
    ExprVector tauThreeMatrices = generateExprVector({declarePauliMatrix(1, "tauThree"),declarePauliMatrix(2, "tauThree"),declarePauliMatrix(3, "tauThree")});
    Expression sigmaOneVector = declareEuclidVector("sigmaOneVector", {sigmaOneMatrices[0],sigmaOneMatrices[1],sigmaOneMatrices[2]});
    Expression sigmaTwoVector = declareEuclidVector("sigmaTwoVector", {sigmaTwoMatrices[0],sigmaTwoMatrices[1],sigmaTwoMatrices[2]});
    Expression sigmaThreeVector = declareEuclidVector("sigmaThreeVector", {sigmaThreeMatrices[0],sigmaThreeMatrices[1],sigmaThreeMatrices[2]});
    Expression tauOneVector = declareEuclidVector("tauOneVector", {tauOneMatrices[0],tauOneMatrices[1],tauOneMatrices[2]});
    Expression tauTwoVector = declareEuclidVector("tauTwoVector", {tauTwoMatrices[0],tauTwoMatrices[1],tauTwoMatrices[2]});
    Expression tauThreeVector = declareEuclidVector("tauThreeVector", {tauThreeMatrices[0],tauThreeMatrices[1],tauThreeMatrices[2]});
    Expression qOneX = declareSymbol("qOneX");
    Expression qOneY = declareSymbol("qOneY");
    Expression qOneZ = declareSymbol("qOneZ");
    Expression qOneVector = declareEuclidVector("qOneVector", {qOneX,qOneY,qOneZ});
    Expression qTwoX = declareSymbol("qTwoX");
    Expression qTwoY = declareSymbol("qTwoY");
    Expression qTwoZ = declareSymbol("qTwoZ");
    Expression qTwoVector = declareEuclidVector("qTwoVector", {qTwoX,qTwoY,qTwoZ});
    Expression qThreeX = declareSymbol("qThreeX");
    Expression qThreeY = declareSymbol("qThreeY");
    Expression qThreeZ = declareSymbol("qThreeZ");
    Expression qThreeVector = declareEuclidVector("qThreeVector", {qThreeX,qThreeY,qThreeZ});
    
    Expression pOneX = declareSymbol("pOneX");
    Expression pOneY = declareSymbol("pOneY");
    Expression pOneZ = declareSymbol("pOneZ");
    Expression pOneVector = declareEuclidVector("pOneVector", {pOneX,pOneY,pOneZ});
    Expression pTwoX = declareSymbol("pTwoX");
    Expression pTwoY = declareSymbol("pTwoY");
    Expression pTwoZ = declareSymbol("pTwoZ");
    Expression pTwoVector = declareEuclidVector("pTwoVector", {pTwoX,pTwoY,pTwoZ});
    Expression pThreeX = declareSymbol("pThreeX");
    Expression pThreeY = declareSymbol("pThreeY");
    Expression pThreeZ = declareSymbol("pThreeZ");
    Expression pThreeVector = declareEuclidVector("pThreeVector", {pThreeX,pThreeY,pThreeZ});
    Expression test = qOneVector*qTwoVector;
    Expression potential = parseString("-(tauOneVector*tauTwoVector)*(sigmaOneVector*qOneVector)*(sigmaTwoVector*qOneVector)");
    potential = simplify(potential);
    //Expression potential = simplify(parseString("(sigmaOneVector*qVector)*(sigmaTwoVector*qVector)"));//first*second;
    /*Expression exchangePotential = -potential;
    Expression directPotential = substitute(potential, qOneX, ZERO);
    directPotential = substitute(directPotential, qOneY, ZERO);
    directPotential = substitute(directPotential, qOneZ, ZERO);*/
    std::cout << potential.print()+"\n";
    Expression total = ZERO;
    for(int sigmaOne = 1; sigmaOne>=0; sigmaOne--) {
        for(int sigmaTwo = 1; sigmaTwo>=0; sigmaTwo--) {
            for(int sigmaThree = 1; sigmaThree>=0; sigmaThree--) {
                for(int tauOne = 1; tauOne>=0; tauOne--) {
                    for(int tauTwo = 1; tauTwo>=0; tauTwo--) {
                        for(int tauThree = 1; tauThree>=0; tauThree--) {
                            std::cout<<sigmaOne;
                            std::cout<<sigmaTwo;
                            std::cout<<tauOne;
                            std::cout<<tauTwo;
                            std::cout<<"\n";
                            Expression totalForCurrentStates = ZERO;
                            std::vector<std::pair<int, int>> finalStates = {{sigmaOne,tauOne},{sigmaTwo,tauTwo},{sigmaThree,tauThree}};
                            ExprVector momentumVectors = {pOneVector,pTwoVector,pThreeVector};
                            int sigmaOneFinal, tauOneFinal, sigmaTwoFinal, tauTwoFinal, sigmaThreeFinal, tauThreeFinal;
                            Expression qOneVectorActual, qTwoVectorActual, qThreeVectorActual;
                            int numParticles = threeBody? 3 : 2;
                            for(int first = 0; first<numParticles; first++) {
                                for(int second = 0; second<numParticles; second++) {
                                    if(first==second)
                                        continue;
                                    sigmaOneFinal = finalStates[first].first;
                                    tauOneFinal = finalStates[first].second;
                                    sigmaTwoFinal = finalStates[second].first;
                                    tauTwoFinal = finalStates[second].second;
                                    qOneVectorActual = pOneVector - momentumVectors[first];
                                    qTwoVectorActual = pTwoVector - momentumVectors[second];
                                    if(threeBody) {
                                        for(int third = 0; third<numParticles; third++) {
                                            if(first==third||second==third)
                                                continue;
                                            sigmaThreeFinal = finalStates[third].first;
                                            tauThreeFinal = finalStates[third].second;
                                            qThreeVectorActual = pThreeVector - momentumVectors[third];
                                            
                                            Expression currentPotential = insertProperQVectors(potential, qOneVectorActual, qTwoVectorActual, qThreeVectorActual);
                                            Expression contribution = matrixElement(currentPotential, sigmaOneStates[sigmaOne], sigmaOneStates[sigmaOneFinal]);
                                            contribution = matrixElement(contribution, sigmaTwoStates[sigmaTwo], sigmaTwoStates[sigmaTwoFinal]);
                                            contribution = matrixElement(contribution, sigmaThreeStates[sigmaThree], sigmaThreeStates[sigmaThreeFinal]);
                                            contribution = matrixElement(contribution, tauOneStates[tauOne], tauOneStates[tauOneFinal]);
                                            contribution = matrixElement(contribution, tauTwoStates[tauTwo], tauTwoStates[tauTwoFinal]);
                                            contribution = matrixElement(contribution, tauThreeStates[tauThree], tauThreeStates[tauThreeFinal]);
                                            totalForCurrentStates = totalForCurrentStates + contribution;
                                        }
                                    }
                                    else {
                                        Expression currentPotential = insertProperQVectors(potential, qOneVectorActual, qTwoVectorActual);
                                        Expression contribution = matrixElement(currentPotential, sigmaOneStates[sigmaOne], sigmaOneStates[sigmaOneFinal]);
                                        contribution = matrixElement(contribution, sigmaTwoStates[sigmaTwo], sigmaTwoStates[sigmaTwoFinal]);
                                        contribution = matrixElement(contribution, tauOneStates[tauOne], tauOneStates[tauOneFinal]);
                                        contribution = matrixElement(contribution, tauTwoStates[tauTwo], tauTwoStates[tauTwoFinal]);
                                        totalForCurrentStates = totalForCurrentStates + contribution;
                                    }
                                }
                            }
                            
                            
                            
                            
                            
                            /*Expression directContribution = matrixElement(directPotential, sigmaOneStates[sigmaOne], sigmaOneStates[sigmaOne]);
                            std::cout<<"sigmaOne\n";
                            std::cout<<directContribution.print()+"\n";
                            directContribution = matrixElement(directContribution, sigmaTwoStates[sigmaTwo], sigmaTwoStates[sigmaTwo]);
                            std::cout<<"sigmaTwo\n";
                            std::cout<<directContribution.print()+"\n";
                            directContribution = matrixElement(directContribution, tauOneStates[tauOne], tauOneStates[tauOne]);
                            std::cout<<"tauOne\n";
                            std::cout<<directContribution.print()+"\n";
                            directContribution = matrixElement(directContribution, tauTwoStates[tauTwo], tauTwoStates[tauTwo]);
                            std::cout<<"tauTwo\n";
                            //std::cout<<"Direct\n";
                            std::cout<<directContribution.print()+"\n";
                            Expression exchangeContribution;
                            exchangeContribution = matrixElement(exchangePotential, sigmaOneStates[sigmaOne], sigmaOneStates[sigmaTwo]);
                            std::cout<<"exchange sigmaOne\n";
                            std::cout<<exchangeContribution.print()+"\n";
                            exchangeContribution = matrixElement(exchangeContribution, sigmaTwoStates[sigmaTwo], sigmaTwoStates[sigmaOne]);
                            std::cout<<"exchange sigmaTwo\n";
                            std::cout<<exchangeContribution.print()+"\n";
                            exchangeContribution = matrixElement(exchangeContribution, tauOneStates[tauOne], tauOneStates[tauTwo]);
                            std::cout<<"exchange tauOne\n";
                            std::cout<<exchangeContribution.print()+"\n";
                            exchangeContribution = matrixElement(exchangeContribution, tauTwoStates[tauTwo], tauTwoStates[tauOne]);
                            std::cout<<"exchange TauB\n";
                            //std::cout<<"Exchange\n";
                            std::cout<<exchangeContribution.print()+"\n";*/
                            Expression tempTotal = total;
                            //total = tempTotal + directContribution + exchangeContribution;
                            total = tempTotal + totalForCurrentStates;
                            std::cout<<total.print()+"\n";
                            std::cout<<"running total:"+total.print()+"\n\n";
                            
                            if(!threeBody)
                                break;
                        }
                    }
                }
            if(!threeBody)
                break;
            }
        }
    }
    Expression result = cancelTerms(total);
    result = simplify(result);
    //result = simplify(result);
    //result = cancelTerms(result);
    return result;
}

Expression spinIsospinSummation(std::vector<String> interactions, bool exchange ) {
    int numTerms = (int)interactions.size();
    ExprVector interactionExpressions = *new ExprVector();
    for(int i = 0; i<numTerms; i++) {
        Expression nextTerm = parseString(interactions[i]);
        interactionExpressions.push_back(nextTerm);
    }
    return spinIsospinSummation(interactionExpressions, exchange);
}

int main(int argc, const char * argv[]) {
    initializeDefaultSymbols();
    initializeDefaultFunctions();
    std::vector<String> interactions = {};
    Expression result = spinIsospinSummation(interactions, false);
    //Expression result = parseString("transpose[x]");
    std::cout << result.print()+"\n";
    
    return 0;
}


