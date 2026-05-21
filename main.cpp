#include <iostream>
#include <chrono>
#include <thread>
#include <memory>
#include <stdexcept>

#include <unistd.h>

#include <GL/freeglut.h>

#include <Ixtli/Uuid.h>
#include <Ixtli/Graphics/Color.h>
#include <Ixtli/View/ContextProvider.h>
#include <Ixtli/View/MarginLayoutParams.h>
#include <Ixtli/Widget/Toast.h>
#include <Ixtli/Widget/EditText.h>
#include <Ixtli/View/View.h>
#include <Ixtli/View/Window.h>
#include <Ixtli/Events/KeyboardEvent.h>

#include "GrapherView.h"
#include <Ixtli/Widget/LinearLayout.h>
#include <Ixtli/Graphics/Paint.h>
#include <Ixtli/Graphics/Font.h>


//sin(3t)-cos(4t)+3

using namespace std;
using namespace Ixtli;

class GraphWindow : public Window, OnTextChangedListener, OnKeyEventListener {
private:
    std::shared_ptr<GrapherView> grapher = nullptr;
    
    UUID input1{};
    UUID input2{};
public:
    GraphWindow() : Window(){};
    ~GraphWindow(){};

    void onCreate() override{
        auto rootLayout = setRootView<View>();
        rootLayout->setLayoutParams(MarginLayoutParams(LayoutParams::MATCH_PARENT));
        rootLayout->setBackgroundColor(Color::BLACK);
        std::cout << "CREATE ROOT LAYOUT " << *rootLayout << std::endl;
        

        grapher = std::make_shared<GrapherView>();
        grapher->setLayoutParams(MarginLayoutParams(LayoutParams::MATCH_PARENT, 0, 0, 50, 0));
        rootLayout->addView(grapher);

        auto separator = std::make_shared<View>();
        separator->setLayoutParams(MarginLayoutParams(LayoutParams::MATCH_PARENT, LayoutParams::WRAP_CONTENT, 0, 0, 50, 0));
        separator->setHeight(1);
        separator->setBackgroundColor(Color::RED);
        rootLayout->addView(separator);

        auto exprListView = std::make_shared<LinearLayout>(false);
        exprListView->setLayoutParams(MarginLayoutParams(LayoutParams::MATCH_PARENT, LayoutParams::WRAP_CONTENT, 0, 0, 51, 0));
        exprListView->setHeight(100);
        exprListView->setBackgroundColor(Color(0x121212).setAlpha(80));
        rootLayout->addView(exprListView);

        auto inputText = std::make_shared<EditText>();
        inputText->setLayoutParams(MarginLayoutParams(LayoutParams::MATCH_PARENT, LayoutParams::WRAP_CONTENT));
        inputText->setHeight(50);
        inputText->setBackgroundColor(Color::BLACK);
        inputText->setTextColor(Color::WHITE);
        inputText->setTextSize(20);
        inputText->setOnTextChangedListener(this);
        rootLayout->addView(inputText);
        input1 = inputText->getID();

        //Load font
        auto font = std::make_shared<Font>();
        // if(!font->loadFromFile("BrownieStencil.ttf")){
        if(!font->loadFromFile("NeoEuler.otf")){
            std::cerr << "Unable to load font" << std::endl;
        }else{
            std::cout << "Font loaded successfully" << std::endl;
            inputText->getTextPaint().setFont(font);
        }

        inputText->registerOnKeyEventListener(this);

        // inputText = std::make_shared<EditText>();
        // inputText->setLayoutParams(MarginLayoutParams(LayoutParams::MATCH_PARENT, LayoutParams::WRAP_CONTENT, 50, 50, 51, 0));
        // inputText->setHeight(50);
        // inputText->setBackgroundColor(Color(0x121212).setAlpha(80));
        // inputText->setTextColor(Color::WHITE);
        // inputText->setTextSize(20);
        // //inputText->setText("avg(5,6,7)x+1=y+1+avg(x,1,2)");
        // inputText->setText("sin(x)=y");
        // inputText->setOnTextChangedListener(this);
        // rootLayout->addView(inputText);
        // input2 = inputText->getID();

        

        auto expr1 = Expression::parseObj("(8/(pi*pi))(sin(pix)-1/9sin(3pix)+1/25sin(5pix)-1/49sin(7pix))");
        // auto expr2 = Expression::parseObj(inputText->getText());
        // std::cout << "txt: " << expr2->getExpression() << std::endl;
        grapher->add(expr1, Color::parse("#9A63EC"), Equation2D::CARTESIAN);
        inputText->setText(expr1->getExpression());
        // grapher->add(expr2, Color::parse("#62EF6B"), Equation2D::CARTESIAN);
    }

    Color randomColor(){
        return Color::fromARGB(255, rand()%256, rand()%256, rand()%256);
    }

    bool onKeyEvent(View* v, int key, KeyAction action) override{
        if(v->getID() != input1){return false;}

        if(key == Key::SHIFT && grapher->size() > 0){
            grapher->setPathColor(grapher->size()-1, randomColor());
            return true;
        }
        if(action != KeyAction::KEY_UP){ return false;}

        if(key != Key::CARRY_RETURN) {
            return false;
        }
        if(grapher->size() == 0){
            return true;
        }

        auto expr = grapher->getExpression(grapher->size()-1);
        if(!expr || !expr->isValid()){
            return true;
        }

        grapher->add(nullptr, randomColor(), Equation2D::CARTESIAN);

        auto editText = findViewByID<EditText>(input1);
        if(!editText){
            return true;
        }
        editText->setText("");
        // grapher->add(Expression::parseObj("0"), Color::parse("#62EF6B"), Equation2D::CARTESIAN);

        return true;
    }

    void onTextChanged(View* v, const std::string& before, const std::string& after) override{
        if(v->getID() != input1) return;
        if(grapher->size() == 0){
            grapher->add(Expression::parseObj("0"), randomColor(), Equation2D::CARTESIAN);
        }

        auto expr = Expression::parseObj(after);
        if(expr->isValid({'x'})){
            grapher->setExpression(grapher->size()-1 , expr, Equation2D::CARTESIAN);
            // std::cout << "Invalid expression: " << expr->getExpression() << std::endl;
            // grapher->setExpression(grapher->size()-1 , nullptr);
            // return;
        }else if(expr->isValid({'t'})){
            grapher->setExpression(grapher->size()-1 , expr, Equation2D::POLAR);
        }else if(expr->isValid({'y'})){
            grapher->setExpression(grapher->size()-1 , expr, Equation2D::INVERSE_CARTESIAN);
        }else if(expr->isValid({'r'})){
            grapher->setExpression(grapher->size()-1 , expr, Equation2D::INVERSE_POLAR);
        }else{
            // std::cout << "Invalid expression: " << expr->getExpression() << std::endl;
            grapher->setExpression(grapher->size()-1 , nullptr);
        }
        

        // try{

            // if(!expr->isValid({'x'})){
                // std::cout << "InvE: " << expr->getExpression() << std::endl;
            // }else{
                // auto expr2= expr->fakeEvaluate();
                // std::cout << "Fake eval (size="<<expr2.getRPN().size()<<"): " << expr2 << std::endl;


            // }
            
        // }catch(std::runtime_error& e){
            // std::cerr << "Unable to set new equation, what: " << e.what() << std::endl;
        // }catch(...){
            // std::cerr << "Unable to set new equation, unknown error" << std::endl;
        // }
    }
};

int main(int argc, char** argv){
    /*if(fork() != 0){
        exit(0);
    }*/
    ContextProvider* context = ContextProvider::getProvider();
    auto grapher = context->createWindow<GraphWindow>(1024, 720, Point(800, 300), "Math Equation Grapher");
    //auto grapher = context->createWindow<GraphWindow>();
    context->transferMainControl();

    return 0;
}