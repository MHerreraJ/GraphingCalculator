#ifndef GRAPHER_EQUATION2D_H
#define GRAPHER_EQUATION2D_H

#include <vector>
#include <functional>

#include "Parser/Expression.h"

#include <Ixtli/Graphics/Point2D.h>
#include <Ixtli/Graphics/Canvas.h>
#include <Ixtli/Graphics/Color.h>
#include <Ixtli/Graphics/Paint.h>

using Ixtli::PointF;
using Ixtli::Paint;
using Ixtli::Canvas;

using PointMapper = std::function<PointF(float, float)>;

class Equation2D{
    public:
        enum Type{
            CARTESIAN,
            POLAR,
            INVERSE_CARTESIAN,
            INVERSE_POLAR,
            IMPLICIT_CARTESIAN,
        };

    private:
        std::shared_ptr<Expression> graphEquation;
        Type equationType;
        std::vector<float> graphSeries;
        Paint graphPaint;
        PointMapper pointMapper;

        bool isValidNumber(float x);

    public:
        Equation2D(std::shared_ptr<Expression> equation, Type type = CARTESIAN):
            graphEquation(equation), equationType(type), graphPaint(), pointMapper(nullptr){};

        inline std::shared_ptr<Expression> equation() const 
            { return graphEquation; }

        inline void setEquation(std::shared_ptr<Expression> expr) 
            { graphEquation = expr; }

        inline Type type() const 
            { return equationType; }

        inline Paint& paint()  
            { return graphPaint; }

        inline void setScreenPointMapper(PointMapper mapper) 
            { pointMapper = mapper; }

        
        inline bool isValid() const 
            { return graphEquation != nullptr && graphEquation->isValid(); }
        
        PointF evaluateAt(float x0, float rX1, float rX2, float rY1, float rY2, float delta, PointMapper mapper = nullptr);
        
        void implicitEvaluate(float x0, float x1, float y0, float y1, float delta, PointMapper mapper = nullptr);

        void evaluate(float x0, float x1, float y0, float y1, float dx, PointMapper mapper = nullptr);
        
        void drawTo(Canvas& canvas, float slopeThreshold);

        static char getLiteralOf(Type type);

};


#endif //GRAPHER_EQUATION2D_H