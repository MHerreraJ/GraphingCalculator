#include <iostream>
#include <cmath>
#include <cstring>

#include <Ixtli/Math.h>
#include <Ixtli/Graphics/Rectangle.h>

#include "Grapher2D.h"

using namespace Ixtli;

static int iLog10Abs(float value){
    if (value == 0.0f) return -1;
    if (value == 1.0f || value == -1.0f) return  0;
    if (isinff(value)) return -2;
    return (int)floorf(log10f(Ixtli::abs(value)));
}


Grapher2D::Grapher2D(float w, float h, int samples, float x0, float x1){
    auto xRange = x1 - x0;
    scaleFactor =  w / xRange;
    invScaleFactor = 1.0f / scaleFactor;

    screenSize = PointF(w, h);
    range = PointF(xRange, (h*xRange)/w);

    auto y0 = (screenOffsetY - 0.5f*h) * invScaleFactor;
    auto y1 = y0 + range.y;

    leftBottomCorner = PointF(x0, y0);
    rightUpCorner = PointF(x1, y1);

    graphSamples = samples;
    dx = xRange / static_cast<float>(samples);
}

float Grapher2D::mapX(float x){
    return (x - leftBottomCorner.x)*scaleFactor;
}

float Grapher2D::mapY(float y){
    return 0.5f*screenSize.y + screenOffsetY - (y*scaleFactor);
}

float Grapher2D::invMapX(float screenX){
    return leftBottomCorner.x + (screenX * invScaleFactor);
}

float Grapher2D::invMapY(float screenY){
    return (screenOffsetY + 0.5f*screenSize.y - screenY) * invScaleFactor;
}

PointMapper Grapher2D::getMapper() const{
    return [&](float x, float y){
        return PointF((x - leftBottomCorner.x)*scaleFactor, 0.5f*screenSize.y + screenOffsetY - (y*scaleFactor));
    };
}


void Grapher2D::setGraphExpression(size_t index, std::shared_ptr<Expression> equation, Equation2D::Type type){
    if(index < grapher2DEquations.size()){
        auto currentPaint = grapher2DEquations[index].paint();
        grapher2DEquations[index] = Equation2D(equation, type);
        grapher2DEquations[index].paint() = currentPaint;
        grapher2DEquations[index].setScreenPointMapper(getMapper());
    }else if(index == grapher2DEquations.size()){
        grapher2DEquations.push_back(Equation2D(equation, type));
        grapher2DEquations[index].setScreenPointMapper(getMapper());
    }else{
        return;
    }
    recompute(index);
}

void Grapher2D::setGraphLinePaint(size_t index, const Paint& paint){
    if(index < grapher2DEquations.size()){
        grapher2DEquations[index].paint() = paint;
    }
}

std::shared_ptr<Expression> Grapher2D::getExpression(size_t index){
    if(index < grapher2DEquations.size()){
        return grapher2DEquations[index].equation();
    }
    return nullptr;
}


Equation2D::Type Grapher2D::getExpressionType(size_t index){
    if(index < grapher2DEquations.size()){
        return grapher2DEquations[index].type();
    }
    return Equation2D::Type::CARTESIAN;
}

void Grapher2D::removeAt(size_t idx){
    grapher2DEquations.erase(grapher2DEquations.begin() + idx);
}

PointF Grapher2D::getRelativeScreenPosition(float screenX, float screenY){
    return PointF(screenX/screenSize.x, screenY/screenSize.y);
}


void Grapher2D::drawTo(Canvas& canvas){
    float threshold = 1.5f*dx;
    for(auto& equation : grapher2DEquations){
        equation.drawTo(canvas, threshold);
    }
}

void Grapher2D::drawGridTo(Canvas& canvas, const Paint& axisPaint, 
        const Paint& gridPrimaryPaint, const Paint& gridSecondaryPaint){
    auto origin = map(0.0f, 0.0f);
    for(auto& vLine : verticalGridLines){
        auto& vLinePos = vLine.first;
        auto& isMajor = vLine.second.first;
        if(vLine.second.second != "0"){
            canvas.drawLine(vLinePos, 0.0f, vLinePos, screenSize.y, isMajor ? gridPrimaryPaint :gridSecondaryPaint);
        }
    }
    for(auto& hLine : horizontalGridLines){
        auto& hLinePos = hLine.first;
        auto& isMajor = hLine.second.first;
        if(hLine.second.second != "0")
            canvas.drawLine(0.0f, hLinePos, screenSize.x, hLinePos, isMajor ? gridPrimaryPaint :gridSecondaryPaint);
    }

    // Main Axis
    auto xAxisPos = min(max(2.0f, origin.y), screenSize.y - 2.0f);
    canvas.drawLine(0.0f, xAxisPos, screenSize.x, xAxisPos, axisPaint);

    auto yAxisPos = min(max(2.0f, origin.x), screenSize.x - 2.0f);
    canvas.drawLine(yAxisPos, 0.0f, yAxisPos, screenSize.y, axisPaint);
}

void Grapher2D::drawGridValuesTo(Canvas& canvas, const Paint& paint){
    auto origin = map(0.0f, 0.0f);
    auto crossOrigin = false;

    for(auto& hLine : horizontalGridLines){
        auto pos = hLine.first;
        auto value = hLine.second.second;
        if(value == "0"){
            crossOrigin = true;
        }

        Rect bounds{};
        paint.getTextBounds(value, 0, value.length(), bounds);
        auto xPos = max(0.0f, min(origin.x, screenSize.x - bounds.width() - 20.0f)) + 5.0f;
        auto yPos = pos - 0.5f*bounds.height() - 15.0f;
        canvas.drawText(value, xPos, yPos, paint);
    }

    for(auto& vLine : verticalGridLines){
        auto pos = vLine.first;
        auto value = vLine.second.second;
        if(value == "0" && crossOrigin){
            continue;
        }

        Rect bounds{};
        paint.getTextBounds(value, 0, value.length(), bounds);
        auto xPos = pos - 0.5f*bounds.width();
        auto yPos = max(0.0f, min(origin.y, screenSize.y - bounds.height() -30.0f)) + bounds.height() + 10.0f;
        canvas.drawText(value, xPos, yPos, paint);
    }
}



void Grapher2D::invalidate(){
    recompute();
}

void Grapher2D::setWindowParameters(float x0, float x1, float yOffset){
    auto xRange = x1 - x0;
    screenOffsetY = yOffset;
    scaleFactor = screenSize.x / xRange;
    invScaleFactor = 1.0f / scaleFactor;

    range = PointF(xRange, (screenSize.y*xRange)/screenSize.x);

    auto y0 = (screenOffsetY - 0.5f*screenSize.y) * invScaleFactor;
    auto y1 = y0 + range.y;
    leftBottomCorner = PointF(x0, y0);
    rightUpCorner = PointF(x1, y1);

    dx = xRange/static_cast<float>(graphSamples);
    recompute();
}

void Grapher2D::translateWindow(PointF offset){
    auto xOffset = - invScaleFactor * offset.x;
    auto yOffset = offset.y;

    auto limit = 8.0f * grapherLimitMax;

    if(xOffset > 0 && rightUpCorner.x > limit){
        return;
    }
    if(xOffset < 0 && leftBottomCorner.x < -limit){
        return;
    }
    if(yOffset < 0 && leftBottomCorner.y < -limit){
        return;
    }
    if(yOffset > 0 && rightUpCorner.y > limit){
        return;
    }

    leftBottomCorner.x +=  xOffset;
    rightUpCorner.x += xOffset;

    screenOffsetY += yOffset;
    leftBottomCorner.y = invMapY(screenSize.y);
    rightUpCorner.y = leftBottomCorner.y + range.y;

    recompute();
    return;
}

void Grapher2D::scale(float factor, const PointF& screenFocus){
    if(factor < 1.0 && (abs(range.x) < grapherLimitMin || abs(range.y) < grapherLimitMin))
        return;

    if(factor >= 1.0 &&(range.x > grapherLimitMax || range.y > grapherLimitMax))
        return;

    auto focus = invMap(screenFocus.x, screenFocus.y);
    auto relativeFocus = getRelativeScreenPosition(screenFocus.x, screenFocus.y);

    auto xRange = range.x * factor;
    auto yRange = range.y * factor;

    auto x0 = focus.x - (xRange * relativeFocus.x);
    auto x1 = x0 + xRange;

    scaleFactor = screenSize.x / xRange;
    invScaleFactor = 1.0f / scaleFactor;

    screenOffsetY = focus.y * scaleFactor + screenFocus.y - 0.5f*screenSize.y;
    auto y0 = invMapY(screenSize.y);
    auto y1 = y0 + yRange;


    range = PointF(xRange, yRange);
    leftBottomCorner = PointF(x0, y0);
    rightUpCorner = PointF(x1, y1);

    dx = xRange/static_cast<float>(graphSamples);
    recompute();
}

void Grapher2D::onViewSizeChanged(float width, float height){
    graphSamples = (int)(0.3f*max(width, height));
    dx = range.x/static_cast<float>(graphSamples);

    screenSize = PointF(width, height);
    scaleFactor = width/range.x;
    invScaleFactor = 1.0f / scaleFactor;
    screenOrigin = map(0.0f, 0.0f);

    //if(leftBottomCorner.y.isNaN() || rightUpCorner.y.isNaN() || range.y.isNaN()){
        range.y = (height*range.x)/width;
        auto y0 = (screenOffsetY - 0.5f*screenSize.y) * invScaleFactor;
        auto y1 = y0 + range.y;
        leftBottomCorner.y = y0;
        rightUpCorner.y = y1;
    //}


    recompute();
}

std::string Grapher2D::formattedGridValue(float f, float magnitude){
     if(f == 0.0f)
        return "0";

    static char buffer[50];
    std::string numStr = "";
    std::memset(buffer, 0, sizeof(buffer));
    
    if(magnitude >= 0.099 && magnitude < 1){
        snprintf(buffer, sizeof(buffer) -1, "%.1f", f);
        numStr = buffer;
    } else if(magnitude < 0.01){
        //gridValueFormatter.format(f)
        //numStr = std::to_string(f);
        snprintf(buffer, sizeof(buffer) -1, "%.02e", f);
        numStr=buffer;
    } else if(magnitude < 0.1){
        snprintf(buffer, sizeof(buffer) -1, "%.2f", f);
        numStr = buffer;
    } else if (magnitude < 999){
        snprintf(buffer, sizeof(buffer) -1, "%.0f", f);
        numStr = buffer;
    }else{
        //gridValueFormatter.format(f)
        snprintf(buffer, sizeof(buffer) -1, "%.1e", f);
        numStr=buffer;
        //numStr = std::to_string(f);
    }
    return numStr;
}


void Grapher2D::buildGraphGrid(){
    verticalGridLines.clear();
    horizontalGridLines.clear();

    // Get cell size = the closest 10-multiple where x-range is
    float cellSize = powf(10, iLog10Abs(range.x/subGridDivision));
    float screenCellSize = cellSize * scaleFactor;

    float numOfVerticalLines = screenSize.x / screenCellSize;
    if(numOfVerticalLines >= 4.5f*subGridDivision){
        cellSize *= 2.0f;
        screenCellSize *= 2.0f;
        //numOfVerticalLines *= 0.05f
    }

    // Get the first vertical line 'x' position of the cell
    float firstVerticalLinePos = ceilf(leftBottomCorner.x / cellSize);

    // Get the index
    int vLineIndex = (int)firstVerticalLinePos;
    float vLineValue = cellSize * vLineIndex;
    float vLineScreenPos = mapX(firstVerticalLinePos * cellSize);

    while(vLineValue <= rightUpCorner.x){
        bool isMajorLine = (vLineIndex % subGridDivision) == 0;
        vLineValue = cellSize * vLineIndex;
        verticalGridLines[vLineScreenPos] = std::pair<bool, std::string>(isMajorLine, formattedGridValue(vLineValue, cellSize));

        vLineScreenPos += screenCellSize;
        vLineIndex ++;
    }

    float firstHorizontalLinePos = ceilf(leftBottomCorner.y / cellSize);
    // Get the index
    int hLineIndex = (int)firstHorizontalLinePos;
    float hLineValue = cellSize * hLineIndex;
    float hLineScreenPos = mapY(hLineIndex * cellSize);

    while(hLineValue <= rightUpCorner.y){
        bool isMajorLine = (hLineIndex % subGridDivision) == 0;
        hLineValue = cellSize * hLineIndex;
        horizontalGridLines[hLineScreenPos] = std::pair<bool, std::string>(isMajorLine, formattedGridValue(hLineValue, cellSize));

        hLineScreenPos -= screenCellSize;
        hLineIndex ++;
    }
}

void Grapher2D::evaluateExpression(int exprIndex){
    if(exprIndex < 0) {
        for (auto& equation : grapher2DEquations) {
            equation.evaluate(leftBottomCorner.x, rightUpCorner.x, leftBottomCorner.y, rightUpCorner.y, dx, getMapper());
        }
    }else{
        grapher2DEquations[exprIndex].evaluate(leftBottomCorner.x, rightUpCorner.x, leftBottomCorner.y, rightUpCorner.y, dx, getMapper());
    }
}

void Grapher2D::recompute(int index){
    evaluateExpression(index);
    buildGraphGrid();
}