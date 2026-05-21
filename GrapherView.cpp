#include <thread>
#include "GrapherView.h"

#include <Ixtli/Graphics/PathEffect.h>

using namespace Ixtli;


GrapherView::GrapherView() : View(){
    setBackgroundColor(Color::parse("#222222"));
    graphWindow = new Grapher2D(getWidth(), getHeight());
    //std::cout << "GRAPHER VIEW create window with size" << Point2D<int>(width(), height()) << std::endl;


    graphAxisPaint = Paint().setColor(Color::WHITE).setStrokeWidth(1.7f).setTextSize(10.0f);
    graphPrimaryGridLinesPaint = 
        Paint()
        .setColor(Color::GRAY)
        .setStrokeWidth(1.5)
        .setPathEffect(PathEffect().setFactor(3).setPattern(0xCCCC));

    graphSecondaryGridLinesPaint = 
        Paint()
        .setColor(Color::DARK_GRAY)
        .setStrokeWidth(1)
        .setPathEffect(PathEffect().setFactor(3).setPattern(0xAAAA));

    graphWindow->invalidate();
}

void GrapherView::onSizeChanged(int w, int h, int oldW, int oldH){ 
    //std::cout << "GraphView viewport: " << getViewport() << std::endl;
    graphWindow->onViewSizeChanged(w, h);
    invalidate();
    //std::cout << "GRAPHER VIEW resized window to " << Point2D<int>(width(), height()) << std::endl;
    //std::cout << "GRAPHER VIEW resized viewport change " <<  getViewport() << std::endl;

}

void GrapherView::setExpressionType(size_t idx, Equation2D::Type type)
{
    graphWindow->setGraphExpression(idx, graphWindow->getExpression(idx), type);
}


void GrapherView::onDraw(Canvas& canvas){
    graphWindow->drawGridTo(canvas, graphAxisPaint, graphPrimaryGridLinesPaint, graphSecondaryGridLinesPaint);
    graphWindow->drawTo(canvas);
    graphWindow->drawGridValuesTo(canvas, graphAxisPaint);
}

void GrapherView::onMouseEvent(MouseButton button, MouseAction action, int x, int y){
    static MouseAction lastAction = MouseAction::NONE;
    static Point2D<int> lastPoint(0,0);

    Point p(x, y);

    if(action == MouseAction::MOVE){
        if(lastAction == MouseAction::PRESSED_DOWN){
            lastPoint = p;
        }else if(lastAction == MouseAction::MOVE){
            if(lastPoint != p){
                auto offset = p - lastPoint;
                graphWindow->translateWindow(offset);
                invalidate();
            }
        }
    }else if(action == MouseAction::SCROLL_DOWN){
        graphWindow->scale(1.2f, p);
        invalidate();
    }else if(action == MouseAction::SCROLL_UP){
        graphWindow->scale(0.833f, p);
        invalidate();
    }
    lastPoint = p;
    lastAction = action;
}
