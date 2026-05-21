#include <cmath>
#include "Equation2D.h"
#include "Solver.h"

using namespace Ixtli;

char Equation2D::getLiteralOf(Equation2D::Type type){
    switch (type){
        case Type::CARTESIAN: return 'x';
        case Type::POLAR: return 't';
        case Type::INVERSE_CARTESIAN: return 'y';
        case Type::INVERSE_POLAR: return 'r';
        case Type::IMPLICIT_CARTESIAN: return 'x';
    }
    return 0;
}

static std::string lastExpr;

PointF Equation2D::evaluateAt(float x0, float rX1, float rX2, float rY1, float rY2, float delta, PointMapper mapper){
    if(!graphEquation) return PointF(nanf(""), nanf(""));

    auto x = x0;
    auto y = graphEquation->evaluateAt(x, getLiteralOf(equationType));
    
    if(equationType == Type::POLAR){
        auto r = y;
        auto theta = x;
        x = r*cosf(theta);
        y = r*sinf(theta);
    }else if(equationType == Type::INVERSE_CARTESIAN){
        std::swap(x, y);
    }else if(equationType == Type::INVERSE_POLAR){
        auto r = x;
        auto theta = y;
        x = r*cosf(theta);
        y = r*sinf(theta);
    }

    if(mapper){
        return mapper(x, y);
    }else if(pointMapper){
        return pointMapper(x, y);
    }
    return PointF(x, y);
}


void Equation2D::implicitEvaluate(float x0, float x1, float y0, float y1, float delta, PointMapper mapper){
    auto x = x0;
    while(x < x1){
        Expression expr = graphEquation->evaluate(x, 'x');
        auto solutions = Solver(&expr, 'y', y0, y1, delta).newtonRaphson(20);
        for(auto y : solutions){
            if(isValidNumber(y)){
                if(mapper){
                    auto p = mapper(x,y);
                    graphSeries.push_back(p.x);
                    graphSeries.push_back(p.y);
                }else if(pointMapper){
                    auto p = pointMapper(x,y);
                    graphSeries.push_back(p.x);
                    graphSeries.push_back(p.y);
                }else{
                    graphSeries.push_back(x);
                    graphSeries.push_back(y);
                }
            }
        }
        x += 0.5*delta;
    }

    /*if(lastExpr != graphEquation->getExpression()){
        std::cout << "PARSED EQUATION at x = " << x << std::endl;
        for(auto& tkn : expr.getRPN()){
            std::cout << "TKN " << tkn << std::endl;
        }
        std::cout << "VARIABLES EXPR: " << std::endl;
        for(auto& var : expr.getVariables()){
            std::cout << "VAR " << var << std::endl;
        }
        lastExpr = graphEquation->getExpression();
        canPrint = true;
        std::cout << "SOLVER INIT WITH " << rY1 << ", " << rY2 << ", " << delta << std::endl;
    }*/
    
}


void Equation2D::evaluate(float x0, float x1, float y0, float y1, float dx, PointMapper mapper){
    graphSeries.clear();
    if(!isValid()) return;
    auto x = x0;
    auto xend = x1;
    auto deltaX = dx;

    if(equationType == Type::IMPLICIT_CARTESIAN){
        implicitEvaluate(x0, x1, y0, y1, dx, mapper);
        return;
    }


    if(equationType == Type::POLAR){
        x = 0.0f;
        xend = (2.0f * M_PI);
        deltaX  = dx*(xend - x)/(x1-x0);
        xend += deltaX;
    }else if(equationType == Type::INVERSE_CARTESIAN){
        x = y0;
        xend = y1;
    }else if(equationType == Type::INVERSE_POLAR){
        x = (x0 > 0.0f? x0 : 0.0f);
        xend = (x1 > 0.0f? x1 : 0.0f) + 10.0f*deltaX;
    }

    while(x < xend){
        auto p = evaluateAt(x, x0, x1, y0, y1, dx, mapper);
        graphSeries.push_back(p.x);
        graphSeries.push_back(p.y);

        x += deltaX;
    }
}

bool Equation2D::isValidNumber(float x){
    return !(x == nanf("") || isinff(x));
}

static inline float signf(float f){
    return (f < 0.0f? -1.0f : 1.0f);
}

static inline float absf(float f){
    return (f < 0.0f? -f : f);
}

void Equation2D::drawTo(Canvas& canvas, float slopeThreshold){
    if(graphSeries.empty()) return;

    if(equationType == Type::IMPLICIT_CARTESIAN){
        canvas.drawPoints(graphSeries, graphPaint);
        return;
    } 

    auto x1 = graphSeries[0];
    auto y1 = graphSeries[1];
    bool lastSkipped = false;
    bool lastLastSkipped = false;
    auto skipX = graphSeries[0];
    auto skipY = graphSeries[1];

    float lastSignM = 1.0f;

    for(size_t i=2; i < graphSeries.size(); i+=2){
        auto x2 = graphSeries[i];
        auto y2 = graphSeries[i+1];
        auto m = (y2-y1)/(x2-x1);
        auto signM = signf(m);

        if(isValidNumber(y1) && isValidNumber(y2)){
            if(abs(m) <= slopeThreshold || equationType == Type::POLAR || equationType == Type::INVERSE_POLAR){
                canvas.drawLine(x1, y1, x2, y2, graphPaint);
                lastSkipped = false;
                lastLastSkipped = false;
            }else if(lastSkipped && lastSignM == signM){
                canvas.drawLine(x1, y1, x2, y2, graphPaint);
                if(!lastLastSkipped){
                    canvas.drawLine(skipX, skipY, x1, y1, graphPaint);
                    lastLastSkipped = true;
                }
                lastSkipped = true;
            }else{
                lastSkipped = true;
                lastLastSkipped = false;
                skipX = x1;
                skipY = y1;
            }
        }else{
            lastSkipped = false;
            lastLastSkipped = false;
        }
        x1 = x2;
        y1 = y2;
        lastSignM = signM;
    }
}
