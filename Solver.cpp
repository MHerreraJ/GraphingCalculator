/*
    int leftSideEndIndex = -1;
    int balance = 2;
    int idx = 1;

    for(auto node = rpn.rbegin()+1; node != rpn.rend(); node++){
        auto& tkn = *node;
        balance-=1;
        if(balance == 0){
            leftSideEndIndex = rpn.size() - 1 - idx;
            break;
        }
        if(tkn.isEvaluator()){
            balance += tkn.getArgumentCount();
        }
        idx++;
    }

    std::cout << "FOUND INDEX AT " << leftSideEndIndex << ": " << rpn[leftSideEndIndex] << std::endl;*/

#include <cmath>
#include "Solver.h"


inline float fabs(float x){
    return x<0?-x:x;
}

inline float minf(float x, float y){
    return x<y? x : y;
}

inline float maxf(float x, float y){
    return x>y?x:y;
}

Solver::Solver(Expression* expr, char xVariable, float xStart, float xEnd, float dx) 
: expr(expr), x(xVariable), xStart(xStart), xEnd(xEnd), dx(dx) {
    /*if(epxr->getExpression() != exprLast){
        solve('x', 3.0);
        exprLast = equation->getExpression();
    }*/
}


static float findRootNewtonRaphson(Expression* expr, char var, float xStart, float xEnd, float dx, int maxIterations){
    static int count = 0;

    float lastFx = 0;
    float fx = 0;
    float dfx = 0;
    float xn = (xEnd + xStart)/2;
    //std::cout << std::endl << "Start algo at " << xn << "; " << dx << std::endl;
    //for(auto& tkn : expr->getRPN()){
        //std::cout << "TKN " << tkn << std::endl;
    //}

    float epsilon = 0.25*dx;

    int i = 0;
    do{
        if(i++ > maxIterations){
            //std::cout << "Max Iter " << i << std::endl;
            return nanf("");
        }
        //if(xn < xStart || xn> xEnd ){
            //std::cout << "Out of range " << xn << " (" << xStart << "," << xEnd << ")" << std::endl;
        //    return nanf("");
        //}
        
        fx = expr->evaluateAt(xn, var);
        //dfx = (expr->evaluateAt(xn+dx, var) - fx)/dx;
        dfx = (fx - expr->evaluateAt(xn-epsilon, var))/epsilon;
        xn = xn - fx/dfx;
        //std::cout << "fx = " << fx << "; dfx = " << dfx << "; xn = " << xn << std::endl;

        lastFx = fx;
    }while(fabs(fx) > epsilon);

    //if(xn < xStart || xn > xEnd){
    //    return nanf("");
    //}

    return xn;
}

#include <thread>

std::vector<float> Solver::newtonRaphson(int maxIterations){
    /*static int count = 0;

    float fx = 0;
    float dfx = 0;
    float xn = (xEnd + xStart)/2;

    std::vector<float> solutions;



    int i = 0;
    do{
        if(i++ > maxIterations){
            return std::vector<float>();
        } 
        fx = expr->evaluateAt(xn, x);
        dfx = expr->evaluateAt(xn+dx, x);

        xn -= (dx*fx)/(dfx - dx);
        
    }while(fabs(fx) > dx);
    solutions.push_back(xn);
    count++;*/

    std::vector<float> solutions;
    float root = 0;
    float lastRoot = 0;
    float a = xStart;
    float b = xEnd;

    float epsilon = 0.1*dx;

    do{
        if(a > b) break;

        root = findRootNewtonRaphson(expr, x, a, b, dx, maxIterations);
        if(isnanf(root) || isinff(root)){
            break;
        }

        float mid = 0.5*(a+b);

        if(root < a){
            a = mid + dx;
        }else if(root > b){
            b = mid - dx;
        }else{
            if(fabs(lastRoot - root) < 0.5*dx){
                if(root - mid > 0){
                    b = 0.5*(mid+root) -dx;
                }else{
                    a = 0.5*(mid + root) + dx;
                }
            }else{
                if(root - mid > 0){
                    b = root - dx;
                }else{
                    a = root + dx;
                }
                solutions.push_back(root);
            }
        }
        //std::cout << std::endl << "Found solution " << root << std::endl;
        
    }while(fabs(a-b)>epsilon);

    return solutions;

    /*std::vector<float> solutions;
    std::vector<std::thread> threadPool;

    float y = xStart;
    while(y < xEnd){
        float f = expr->evaluateAt(y, x);
        if(fabs(f) < 0.25*dx){
            //std::cout << "ADDING for f: " << f << " y:" << y << std::endl;
            solutions.push_back(y);
        }
        y+= 0.5*dx;
    }

    return solutions;*/
}


std::vector<float> Solver::bisectionMethod(int maxIterations){
    float a = xStart;
    float b = xEnd;

    float c = 0;
    float fc = 0;

    int i = 0;
    std::vector<float> solutions;
    
    if(expr->evaluateAt(a) * expr->evaluateAt(b) >= 0){
        return solutions;
    }

    do{
        if(i++ > maxIterations){
            return std::vector<float>();
        }

        c = 0.5*(a + b);
        fc = expr->evaluateAt(c, x);
        if(fabs(fc) < 0.5*dx){
            solutions.push_back(c);
            break;
        }
        if(fabs(b-a) < 0.5*dx){
            return std::vector<float>();
            break;
        }
        float fa = expr->evaluateAt(a, x);
        if(fa*fc < 0){
            b = c;
        }else{
            a = c;
        }
    }while(1);


    return solutions;


}

std::vector<float> Solver::secantMethod(int maxIterations){
    int i = 0;
    std::vector<float> solutions;

    float x1 = xEnd;
    float x2 = xStart;

    float xn = 0;
    do{
        if(i++ > maxIterations){
            break;
        }
        float fx1 = expr->evaluateAt(x1, 'y');
        float fx2 = expr->evaluateAt(x2, 'y');

        if(fabs(fx2) < 0.5*dx){
            solutions.push_back(xn);
            break;
        }
        xn = (x2*fx1 - x1*fx2)/(fx1 - fx2);

        x2 = x1;
        x1 = xn;
    }while(1);


    return solutions;
}