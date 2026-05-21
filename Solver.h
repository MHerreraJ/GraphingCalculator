#ifndef SOLVER_H
#define SOLVER_H

#include <unordered_map>
#include "Parser/Expression.h"


class Solver{
    Expression* expr;
    char x;
    float xStart;
    float xEnd;
    float dx;

    public:
        Solver(Expression* expr, char xVariable, float xStart, float xEnd, float dx);
        ~Solver(){};

        std::vector<float> newtonRaphson(int maxIterations = 20);

        std::vector<float> bisectionMethod(int maxIterations = 30);

        std::vector<float> secantMethod(int maxIterations = 30);
    

};


#endif //SOLVER_H