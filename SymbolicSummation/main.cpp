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



Expression spinIsospinSummation(ExprVector interactions, bool exchange ) {
    //{sigA,sigB,tauA,tauB}
    //int numTerms = (int)interactions.size();
    ExprVector sigmaAStates = generateExprVector({declarePauliVector(false, "sigmaA"),declarePauliVector(true, "sigmaA")});
    ExprVector sigmaBStates = generateExprVector({declarePauliVector(false, "sigmaB"),declarePauliVector(true, "sigmaB")});
    ExprVector tauAStates   = generateExprVector({declarePauliVector(false, "tauA"),  declarePauliVector(true, "tauA")});
    ExprVector tauBStates   = generateExprVector({declarePauliVector(false, "tauB"),  declarePauliVector(true, "tauB")});
    ExprVector sigAMats = generateExprVector({declarePauliMatrix(1, "sigmaA"),declarePauliMatrix(2, "sigmaA"),declarePauliMatrix(3, "sigmaA")});
    ExprVector sigBMats = generateExprVector({declarePauliMatrix(1, "sigmaB"),declarePauliMatrix(2, "sigmaB"),declarePauliMatrix(3, "sigmaB")});
    ExprVector tauAMats = generateExprVector({declarePauliMatrix(1, "tauA"),declarePauliMatrix(2, "tauA"),declarePauliMatrix(3, "tauA")});
    ExprVector tauBMats = generateExprVector({declarePauliMatrix(1, "tauB"),declarePauliMatrix(2, "tauB"),declarePauliMatrix(3, "tauB")});
    Expression sigmaAVector = declareMatrix("sigmaAVector", {{sigAMats[0],sigAMats[1],sigAMats[2]}});
    Expression sigmaBVector = declareMatrix("sigmaBVector", {{sigBMats[0],sigBMats[1],sigBMats[2]}});
    Expression tauAVector = declareMatrix("tauAVector", {{tauAMats[0],tauAMats[1],tauAMats[2]}});
    Expression tauBVector = declareMatrix("tauBVector", {{tauBMats[0],tauBMats[1],tauBMats[2]}});
    Expression qx = declareSymbol("qx");
    Expression qy = declareSymbol("qy");
    Expression qz = declareSymbol("qz");
    //Expression qVector = declareMatrix("qVector", {{declareSymbol("qx"),declareSymbol("qy"),declareSymbol("qz")}});
    Expression qVector = declareMatrix("qVector", {{declareSymbol("qx")},{declareSymbol("qy")},{declareSymbol("qz")}});
    //Expression p1Vec = declareMatrix("p1Vec", {{declareSymbol("p1x"),declareSymbol("p1y"),declareSymbol("p1z")}});
    //Expression p3Vec = declareMatrix("p3Vec", {{declareSymbol("p3x"),declareSymbol("p3y"),declareSymbol("p3z")}});
    //Expression qExchVector = p3Vec-p1Vec;
    //Expression first = (sigmaAVector*transpose(qVector));
    //Expression second = (sigmaBVector*transpose(qVector));
    Expression potential = simplify(performActions(parseString("-(tauAVector*transpose[tauBVector])*(sigmaAVector*qVector)*(sigmaBVector*qVector)")));
    //Expression potential = simplify(parseString("(sigmaAVector*qVector)*(sigmaBVector*qVector)"));//first*second;
    Expression exchangePotential = -potential;
    Expression directPotential = substitute(potential, qx, ZERO);
    directPotential = substitute(directPotential, qy, ZERO);
    directPotential = substitute(directPotential, qz, ZERO);
    std::cout << potential.print()+"\n";
    Expression total = ZERO;
    for(int sigmaA = 1; sigmaA>=0; sigmaA--) {
        for(int sigmaB = 1; sigmaB>=0; sigmaB--) {
            for(int tauA = 1; tauA>=0; tauA--) {
                for(int tauB = 1; tauB>=0; tauB--) {
                    std::cout<<sigmaA;
                    std::cout<<sigmaB;
                    std::cout<<tauA;
                    std::cout<<tauB;
                    std::cout<<"\n";
                    Expression directContribution = matrixElement(directPotential, sigmaAStates[sigmaA], sigmaAStates[sigmaA]);
                    std::cout<<"sigmaA\n";
                    std::cout<<directContribution.print()+"\n";
                    directContribution = matrixElement(directContribution, sigmaBStates[sigmaB], sigmaBStates[sigmaB]);
                    std::cout<<"sigmaB\n";
                    std::cout<<directContribution.print()+"\n";
                    directContribution = matrixElement(directContribution, tauAStates[tauA], tauAStates[tauA]);
                    std::cout<<"tauA\n";
                    std::cout<<directContribution.print()+"\n";
                    directContribution = matrixElement(directContribution, tauBStates[tauB], tauBStates[tauB]);
                    std::cout<<"tauB\n";
                    //std::cout<<"Direct\n";
                    std::cout<<directContribution.print()+"\n";
                    Expression exchangeContribution;
                    if(exchange) {
                        exchangeContribution = matrixElement(exchangePotential, sigmaAStates[sigmaA], sigmaAStates[sigmaB]);
                        std::cout<<"exchange sigmaA\n";
                        std::cout<<exchangeContribution.print()+"\n";
                        exchangeContribution = matrixElement(exchangeContribution, sigmaBStates[sigmaB], sigmaBStates[sigmaA]);
                        std::cout<<"exchange sigmaB\n";
                        std::cout<<exchangeContribution.print()+"\n";
                        exchangeContribution = matrixElement(exchangeContribution, tauAStates[tauA], tauAStates[tauB]);
                        std::cout<<"exchange tauA\n";
                        std::cout<<exchangeContribution.print()+"\n";
                        exchangeContribution = matrixElement(exchangeContribution, tauBStates[tauB], tauBStates[tauA]);
                        std::cout<<"exchange TauB\n";
                        //std::cout<<"Exchange\n";
                        std::cout<<exchangeContribution.print()+"\n";
                    } else {
                        exchangeContribution = ZERO;
                    }
                    Expression tempTotal = total;
                    total = tempTotal + directContribution + exchangeContribution;
                    std::cout<<total.print()+"\n";
                    std::cout<<"running total:"+total.print()+"\n\n";
                }
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
    Expression result = spinIsospinSummation(interactions, true);
    //Expression result = parseString("transpose[x]");
    std::cout << result.print()+"\n";
    
    return 0;
}


