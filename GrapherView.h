#ifndef GRAPHER_CALC_VIEW_H
#define GRAPHER_CALC_VIEW_H

#include <mutex>
#include <memory>

#include "Grapher2D.h"
#include "Equation2D.h"
#include "Expression.h"

#include <Ixtli/Graphics/Color.h>
#include <Ixtli/Graphics/Paint.h>
#include <Ixtli/Graphics/Canvas.h>
#include <Ixtli/View/View.h>

#include <Ixtli/Events/MouseEvent.h>

using Ixtli::Color;
using Ixtli::View;
using Ixtli::Canvas;
using Ixtli::Paint;
using Ixtli::MouseButton;
using Ixtli::MouseAction;

class GrapherView : public View {
    private:
        Grapher2D* graphWindow;
        Paint graphAxisPaint{};
        Paint graphPrimaryGridLinesPaint{};
        Paint graphSecondaryGridLinesPaint{};

    public:
        GrapherView();

        void onSizeChanged(int w, int h, int oldW, int oldH) override;
        void onDraw(Canvas& canvas) override;

        inline size_t size() const 
            { return graphWindow->size(); }

        inline std::shared_ptr<Expression> getExpression(size_t idx) 
            { return graphWindow->getExpression(idx); }

        inline Equation2D::Type getExpressionType(size_t idx) 
            { return graphWindow->getExpressionType(idx); }

        void setExpressionType(size_t idx, Equation2D::Type type);

        inline void setExpression(size_t idx, std::shared_ptr<Expression> expr, Equation2D::Type type = Equation2D::Type::CARTESIAN)
            { graphWindow->setGraphExpression(idx, expr, type); }

        inline void setPathColor(size_t idx, Color graphColor)
            { graphWindow->setGraphLinePaint(idx, Paint().setStrokeWidth(3).setColor(graphColor)); invalidate(); }

        inline void add(std::shared_ptr<Expression> expr, Color graphColor = Color::WHITE, Equation2D::Type type = Equation2D::Type::CARTESIAN){
            graphWindow->setGraphExpression(graphWindow->size(), expr, type);
            graphWindow->setGraphLinePaint(graphWindow->size()-1, Paint().setStrokeWidth(2.5).setColor(graphColor));
        }

        inline void removeAt(size_t idx)
            { graphWindow->removeAt(idx); }

        void onMouseEvent(MouseButton button, MouseAction action, int x, int y) override;
};


#endif