#include <cmath>
#include <initializer_list>
#include "Expression.h"

Expression Expression::evaluate(Number x, char variable){
    Expression expr;
    expr.validExpression = false;
    
    std::vector<Token> newRPN;

    if(!validExpression) return expr;
    if(!variables.empty() && variables.count(variable) == 0){
        return expr;
    }

    for(auto& tkn : rpn){
        if(tkn.isEvaluator()){
            if((int)newRPN.size() < tkn.getArgumentCount()){
                return expr;
            }
            int top = newRPN.size() - 1;

            if(tkn == TokenType::FUN_MASK){
                Token t2 = newRPN[top];
                Token t1 = newRPN[top-1];
                
                if(!t1.isConstant() || !t2.isConstant()){
                    newRPN.push_back(tkn);
                    continue;
                }

                Number x1 = t1.getValue().coefficient();
                Number x2 = t2.getValue().coefficient();
                newRPN.pop_back();
                newRPN.pop_back();

                if(x1 <= x && x <= x2 && x1 < x2){
                    newRPN.push_back(Token(TokenType::CONSTANT, Variable(1.0f)));
                }else{
                    newRPN.push_back(Token(TokenType::CONSTANT, Variable(0.0f)));
                }
            }else if(tkn == TokenType::OP_EQUAL){
                newRPN.push_back(Token(TokenType::OP_SUBS));
                continue;
            }else if(tkn.getArgumentCount() == 0){
                newRPN.push_back(Token(TokenType::CONSTANT, Variable(0.0f)));
            }else if(tkn.getArgumentCount() == 1){
                Token t = newRPN[top];
                if(!t.isConstant()){
                    newRPN.push_back(tkn);
                    continue;;
                }
                Number x = t.getValue().coefficient();
                newRPN.pop_back();
                newRPN.push_back(Token(TokenType::CONSTANT, Variable(tkn.invoke(x))));
            }else if(tkn.getArgumentCount() == 2){
                Token t2 = newRPN[top];
                Token t1 = newRPN[top-1];
                if(!t1.isConstant() || !t2.isConstant()){
                    newRPN.push_back(tkn);
                    continue;
                }
                Number x1 = t1.getValue().coefficient();
                Number x2 = t2.getValue().coefficient();
                newRPN.pop_back();
                newRPN.pop_back();
                newRPN.push_back(Token(TokenType::CONSTANT, Variable(tkn.invoke(x1, x2))));
            }else{
                int i =  0;
                std::vector<Number> nums;
                for(i=0; i<tkn.getArgumentCount(); i++){
                    Token t = newRPN[top-i];
                    if(!t.isConstant()){
                        i=-1;
                        break;
                    }
                    nums.push_back(t.getValue().coefficient());
                }
                if(i <= 0){
                    newRPN.push_back(tkn);
                    continue;
                }
                auto removeIt = newRPN.end() - i;
                newRPN.erase(removeIt, newRPN.end());
                newRPN.push_back(Token(TokenType::CONSTANT, Variable(tkn.invoke(nums))));
            }
        }else if(tkn.isNumber()){
            const Variable& v = tkn.getValue();
            if(v.literal() == variable){
                Number n = tkn.getValue()(x);
                newRPN.push_back(Token(TokenType::CONSTANT, Variable(n)));
            }else{
                newRPN.push_back(tkn);
            }
        }else{
            return expr;
        }
    }
    expr.validExpression = true;
    expr.rpn = std::move(newRPN);
    expr.variables = std::unordered_set<char>(variables);
    if(expr.variables.count(variable) > 0)
        expr.variables.erase(variable);
    return expr;
}

Number Expression::operator()(Number x, char variable){
    static Number fNaN = nanf("");

    if(!validExpression) { return fNaN;}
    if(!variables.empty() && variables.count(variable) == 0){
        return fNaN;
    }

    std::vector<Number> stack;
    for(auto& tkn : rpn){
        if(tkn.isEvaluator()){
            if((int)stack.size() < tkn.getArgumentCount()){
                return fNaN;
            }
            if(tkn == TokenType::FUN_MASK){
                Number x2 = stack.back();
                stack.pop_back();
                Number x1 = stack.back();
                stack.pop_back();

                
                if(x1 <= x && x <= x2 && x1 < x2){
                    stack.push_back(1.0);
                }else{
                    stack.push_back(0.0);
                }
            }else if(tkn.getArgumentCount() == 0){
                stack.push_back(0);
            }else if(tkn.getArgumentCount() == 1){
                Number value = stack.back();
                stack.pop_back();
                stack.push_back(tkn.invoke(value));
            }else if(tkn.getArgumentCount() == 2){
                Number n2 = stack.back();
                stack.pop_back();
                Number n1 = stack.back();
                stack.pop_back();
                stack.push_back(tkn.invoke(n1, n2));
            }else{
                std::vector<Number> nums;
                for(int i=0; i<tkn.getArgumentCount(); i++){
                    nums.push_back(stack.back());
                    stack.pop_back();
                }
                stack.push_back(tkn.invoke(nums));
            }
        }else{
            const Variable& v = tkn.getValue();
            if(v.isConstant() || v.literal() == variable){
                Number n = tkn.getValue()(x);
                stack.push_back(n);
            }else{
                return fNaN;
            }
        }
    }

    if(stack.size() != 1){
        return fNaN;
    }

    

    return stack[0];
}

Expression Expression::fakeEvaluate(char variable){
    Expression expr{};
    expr.validExpression = false;
    
    std::vector<Token> newRPN{};

    if(!validExpression) return expr;
    if(!variables.empty() && variables.count(variable) == 0){
        return expr;
    }

    for(auto& tkn : rpn){
        if(tkn.isEvaluator()){
            if((int)newRPN.size() < tkn.getArgumentCount()){
                return expr;
            }
            int top = newRPN.size() - 1;

            if(tkn == TokenType::FUN_MASK){
                Token t2 = newRPN[top];
                Token t1 = newRPN[top-1];
                
                if(!t1.isConstant() || !t2.isConstant()){
                    newRPN.push_back(tkn);
                    continue;
                }


                newRPN.pop_back();
                newRPN.pop_back();

                newRPN.push_back(Token(TokenType::CONSTANT, Variable(1.0f)));
            }else if(tkn == TokenType::OP_EQUAL){
                newRPN.push_back(Token(TokenType::OP_SUBS));
                continue;
            }else if(tkn.getArgumentCount() == 0){
                newRPN.push_back(Token(TokenType::CONSTANT, Variable(1.0f)));
            }else if(tkn.getArgumentCount() == 1){
                Token t = newRPN[top];
                if(!t.isConstant()){
                    newRPN.push_back(tkn);
                    continue;;
                }
                newRPN.pop_back();
                newRPN.push_back(Token(TokenType::CONSTANT, Variable(1.0f)));
            }else if(tkn.getArgumentCount() == 2){
                Token t2 = newRPN[top];
                Token t1 = newRPN[top-1];
                if(!t1.isConstant() || !t2.isConstant()){
                    newRPN.push_back(tkn);
                    continue;
                }
                newRPN.pop_back();
                newRPN.pop_back();
                newRPN.push_back(Token(TokenType::CONSTANT, Variable(1.0f)));
            }else{
                int i =  0;
                std::vector<Number> nums;
                for(i=0; i<tkn.getArgumentCount(); i++){
                    Token t = newRPN[top-i];
                    if(!t.isConstant()){
                        i=-1;
                        break;
                    }
                    nums.push_back(t.getValue().coefficient());
                }
                if(i <= 0){
                    newRPN.push_back(tkn);
                    continue;
                }
                auto removeIt = newRPN.end() - i;
                newRPN.erase(removeIt, newRPN.end());
                newRPN.push_back(Token(TokenType::CONSTANT, Variable(1.0f)));
            }
        }else if(tkn.isNumber()){
            const Variable& v = tkn.getValue();
            if(v.literal() == variable){
                Number n = tkn.getValue()(1.0f);
                newRPN.push_back(Token(TokenType::CONSTANT, Variable(n)));
            }else{
                newRPN.push_back(tkn);
            }
        }else{
            return expr;
        }
    }

    // std::cout << "Fake evaluated expression, RPN: " << std::endl;
    // for(auto& tkn : newRPN){
    //     std::cout << tkn << std::endl;
    // }
    // std::cout << std::endl;
    expr.validExpression = true;
    expr.rpn = std::move(newRPN);
    expr.variables = std::unordered_set<char>(variables);
    if(expr.variables.count(variable) > 0)
        expr.variables.erase(variable);
    return expr;
}

Expression Expression::fakeEvaluate(){
    if(variables.empty()){
        return *this;
    }
    Expression expr = *this;
    // std::cout << "RPN: " << expr << std::endl;

    for(char variable : variables){
        expr = expr.fakeEvaluate(variable);
        // std::cout <<"RPN('" << variable << "'): " << expr << std::endl;
    }
    return expr;
}

bool Expression::isValid(const std::unordered_set<char>& variables) const {
    if(!validExpression) {
        return false;
    }
    for(char v : this->variables){
        if(variables.count(v) == 0){
            return false;
        }
    }
    return true;
}
