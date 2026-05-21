#ifndef GRAPH_CALC_GRAPHER2D_H
#define GRAPH_CALC_GRAPHER2D_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <cfloat>
#include <mutex>

#include "Equation2D.h"

#include <Ixtli/Graphics/Point2D.h>

using Ixtli::PointF;

class Grapher2D{
    protected:
        float grapherLimitMin = 2.0e-4f;
        float grapherLimitMax = FLT_MAX / 10.0f;
        std::vector<Equation2D> grapher2DEquations{};

        float dx = 1.0/800.0f;
        int graphSamples = 800;

        PointF range{};
        PointF leftBottomCorner{};
        PointF rightUpCorner{};

        float scaleFactor = 1.0f;
        float invScaleFactor = 1.0f;
        float screenOffsetY = 0.0f;
        PointF screenSize = PointF();
        PointF screenOrigin = PointF();

        int subGridDivision = 4;
        std::unordered_map<float, std::pair<bool, std::string>> verticalGridLines{};
        std::unordered_map<float, std::pair<bool, std::string>> horizontalGridLines{};

    private:
        std::string formattedGridValue(float f, float magnitude);
        void buildGraphGrid();
        void recompute(int index=-1);

    public:
        Grapher2D(float w, float h, int samples=500, float x0=-8.0f, float x1=8.0f);

        size_t size() const { return grapher2DEquations.size(); }

        float mapX(float x);
        float mapY(float y);
        inline PointF map(float x, float y) { return PointF(mapX(x), mapY(y)); }

        float invMapX(float screenX);
        float invMapY(float screenY);
        inline PointF invMap(float x, float y) { return PointF(invMapX(x), invMapY(y)); }



        PointMapper getMapper() const;


        void setGraphExpression(size_t index, std::shared_ptr<Expression> equation, Equation2D::Type type);
        void setGraphLinePaint(size_t index, const Paint& paint);

        std::shared_ptr<Expression> getExpression(size_t index);
        Equation2D::Type getExpressionType(size_t index);

        void removeAt(size_t idx);

        PointF getRelativeScreenPosition(float screenX, float screenY);

        void drawTo(Canvas& canvas);
        void drawGridTo(Canvas& canvas, const Paint& axisPaint, const Paint& gridPrimaryPaint, const Paint& gridSecondaryPaint);
        void drawGridValuesTo(Canvas& canvas, const Paint& paint);

        void invalidate();
        void setWindowParameters(float x0, float x1, float yOffset);

        void translateWindow(PointF offset);
        void scale(float factor, const PointF& screenFocus);

        void onViewSizeChanged(float width, float height);

        void evaluateExpression(int exprIdx);


};

#endif //GRAPH_CALC_GRAPHER2D_H