//
//  main.cpp
//  SymbolicSummation
//
//  Created by Robert Stahulak on 1/27/22.
//

#include <iostream>
#include "Expression.hpp"
#include "AlgebraicHelpers.hpp"
#include "AbstractHelpers.hpp"
#include "Real.hpp"
#include "Symbol.hpp"
#include "PauliMatrices.hpp"
Expression matrixElement(Expression potential,Expression initialState, Expression finalState) {
    Expression temp = transpose(initialState.get())*potential*finalState;
    Expression result = simplify(temp.get());
    //result = cancelTerms(result.get());
    //result = result*finalState;
    //result = simplify(result);
    //std::cout<<result.print()+"\n";
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
    //Expression first = (sigmaAVector*transpose(qVector.get()));
    //Expression second = (sigmaBVector*transpose(qVector.get()));
    Expression potential = simplify(parseString("(sigmaAVector*qVector)*(sigmaBVector*qVector)").get());//first*second;
    Expression exchangePotential = substitute(potential.get(), qx.get(), ZERO.get());
    exchangePotential = substitute(exchangePotential.get(), qy.get(), ZERO.get());
    exchangePotential = substitute(exchangePotential.get(), qz.get(), ZERO.get());
    std::cout << potential.print()+"\n";
    Expression total = ZERO;
    for(int i = 1; i>=0; i--) {
        for(int j = 1; j>=0; j--) {
            for(int k = 1; k>=0; k--) {
                for(int l = 1; l>=0; l--) {
                    std::cout<<i;
                    std::cout<<j;
                    std::cout<<k;
                    std::cout<<l;
                    std::cout<<"\n";
                    Expression dirPot = potential;//substitute(potential.get(), qVector.get(), ZERO.get());
                    Expression contribution = matrixElement(dirPot, sigmaAStates[i], sigmaAStates[i]);
                    std::cout<<"sigmaA\n";
                    std::cout<<contribution.print()+"\n";
                    contribution = matrixElement(contribution, sigmaBStates[j], sigmaBStates[j]);
                    std::cout<<"sigmaB\n";
                    std::cout<<contribution.print()+"\n";
                    contribution = matrixElement(contribution, tauAStates[k], tauAStates[k]);
                    std::cout<<"tauA\n";
                    std::cout<<contribution.print()+"\n";
                    contribution = matrixElement(contribution, tauBStates[l], tauBStates[l]);
                    std::cout<<"tauB\n";
                    std::cout<<contribution.print()+"\n";
                    //Expression negativeOfPotential = -first;
                    Expression exchangeContribution;
                    if(exchange) {
                        exchangeContribution = matrixElement(exchangePotential, sigmaAStates[j], sigmaAStates[j]);
                        std::cout<<"exchange sigmaA\n";
                        std::cout<<exchangeContribution.print()+"\n";
                        exchangeContribution = matrixElement(exchangeContribution, sigmaBStates[i], sigmaBStates[i]);
                        std::cout<<"exchange sigmaB\n";
                        std::cout<<exchangeContribution.print()+"\n";
                        exchangeContribution = matrixElement(exchangeContribution, tauAStates[l], tauAStates[l]);
                        std::cout<<"exchange tauA\n";
                        std::cout<<exchangeContribution.print()+"\n";
                        exchangeContribution = matrixElement(exchangeContribution, tauBStates[k], tauBStates[k]);
                        std::cout<<"exchange TauB\n";
                        std::cout<<exchangeContribution.print()+"\n";
                    } else {
                        exchangeContribution = ZERO;
                    }
                    Expression tempTotal = total;
                    total = tempTotal + contribution + exchangeContribution;
                    std::cout<<total.print()+"\n";
                    //total = cancelTerms(total.get());
                    std::cout<<"running total:"+total.print()+"\n\n";
                }
            }
        }
    }
    Expression result = simplify(total.get());
    result = cancelTerms(result.get());
    //result = simplify(result.get());
    //result = cancelTerms(result.get());
    return result;
}

Expression spinIsospinSummation(std::vector<std::string> interactions, bool exchange ) {
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
    /*ExprVector upVectors = {declarePauliVector(true, "sigmaA"),declarePauliVector(true, "sigmaB"),declarePauliVector(true, "tauA"),declarePauliVector(true, "tauB")};
    ExprVector downVectors = {declarePauliVector(false, "sigmaA"),declarePauliVector(false, "sigmaB"),declarePauliVector(false, "tauA"),declarePauliVector(false, "tauB")};
    ExprVector sigAMats = {declarePauliMatrix(1, "sigmaA"),declarePauliMatrix(2, "sigmaA"),declarePauliMatrix(3, "sigmaA")};
    ExprVector sigBMats = {declarePauliMatrix(1, "sigmaB"),declarePauliMatrix(2, "sigmaB"),declarePauliMatrix(3, "sigmaB")};
    ExprVector tauAMats = {declarePauliMatrix(1, "tauA"),declarePauliMatrix(2, "tauA"),declarePauliMatrix(3, "tauA")};
    ExprVector tauBMats = {declarePauliMatrix(1, "tauB"),declarePauliMatrix(2, "tauB"),declarePauliMatrix(3, "tauB")};
    Expression sigmaAVector = declareMatrix("sigmaAVector", {{sigAMats[0],sigAMats[1],sigAMats[2]}});
    Expression sigmaBVector = declareMatrix("sigmaBVector", {{sigAMats[0],sigAMats[1],sigAMats[2]}});
    Expression tauAVector = declareMatrix("tauAVector", {{sigAMats[0],sigAMats[1],sigAMats[2]}});
    Expression tauBVector = declareMatrix("tauBVector", {{sigAMats[0],sigAMats[1],sigAMats[2]}});
    Expression kVector = declareMatrix("kVector", {{declareSymbol("kx"),declareSymbol("ky"),declareSymbol("kz")}});*/
    //Expression result = sigmaAVector*transpose(kVector);
    //std::vector<std::string> interactions = {"-f^2/m^2*tauAVector*tauBVector*sigmaAVector*kVector*sigmaBVector*kVector/(k^2+m^2)"};
    ExprVector interactions = {};
    //Expression result = parseString("(sigmaX*qx+sigmaY*qy+sigmaZ*qz)/(q^2+m^2)+1/(q^2+m^2)");
    Expression result = spinIsospinSummation(interactions, true);
    //result = simplify(result);
    //Expression result = -ONE*-ONE;
    //result = simplify(result);
    //Expression result = parseString("-A/B^2*C/D");
    //Expression result = 2-ONE;
    //result = simplify(result);
    std::cout << result.print()+"\n";
    
    return 0;
}


