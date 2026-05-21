#ifndef GRAPH_CALC_PARSER_EXPRESSION_H
#define GRAPH_CALC_PARSER_EXPRESSION_H

#include <memory>
#include <unordered_map>

#include "Parser.h"

class Expression{
    private:
        std::string expression;
        std::vector<Token> rpn;
        std::unordered_set<char> variables;
        bool validExpression;
    private:
        Expression(): validExpression(false) {};
        Expression(const std::string& expression, const std::vector<Token>& rpn, const std::unordered_set<char>& variables, bool validExpression) : 
            expression(expression), rpn(rpn), variables(variables), validExpression(validExpression){
                if(validExpression){
                    if(fakeEvaluate().rpn.size()!=1){
                        this->validExpression = false;
                    }
                }
            };

    public:
        Expression(std::vector<Token>& rpnList, const std::unordered_set<char>& variablesList){
            rpn = std::move(rpnList);
            variables = variablesList;
            validExpression = fakeEvaluate().rpn.size() == 1;
            expression = "";            
        }
        ~Expression(){}
        static Expression parse(const std::string& strExpr) {
            try{
                Parser parser(strExpr);
                return Expression(strExpr, std::move(parser.getRPN()), std::move(parser.getVariables()), parser.isValidExpression()); 
            } catch (...) {
                return Expression();
            }
        }

        static std::shared_ptr<Expression> parseObj(const std::string& strExpr) { 
            try{
                Parser parser(strExpr);
                return std::shared_ptr<Expression>(new Expression(strExpr, std::move(parser.getRPN()), std::move(parser.getVariables()), parser.isValidExpression()));         
            } catch (...) {
                return std::shared_ptr<Expression>(new Expression());
            }
        }

        inline const std::string& getExpression() const
            { return expression; }

        inline const std::vector<Token>& getRPN() const
            { return rpn; }
        
        inline bool isValid() const
            { return validExpression; }

        bool isValid(const std::unordered_set<char>& variables) const;
        
        inline const std::unordered_set<char> getVariables() const
            { return variables; }

        Number operator()(Number x, char variable = 'x');

        Expression evaluate(Number x, char variable = 'x');

        Expression fakeEvaluate();
        Expression fakeEvaluate(char variable);


        inline Number evaluateAt(Number x, char variable = 'x') { return (*this)(x, variable); }

        friend std::ostream& operator << (std::ostream& os, const Expression& expr){
            for(const Token& tkn : expr.rpn){
                os << '\'';
                switch(tkn.getType()){
                    case TokenType::VARIABLE:
                        os << tkn.getValue().literal();
                        break;
                    case TokenType::CONSTANT:
                        os << tkn.getValue().coefficient();
                        break;                    
                    default:
                        if(tkn.isFunction()){
                            os << tkn.lexeme() << "()";
                        }else if(tkn.isOperator()){
                            os << tkn.lexeme();
                        }else{
                            os << tkn.lexeme() << "(?)";
                        }
                        break;
                }
                os << '\'';
                if(&tkn != &expr.rpn.back()){
                    os << ", ";
                }
            }
            return os;
        }
        
};





#endif //GRAPH_CALC_PARSER_EXPRESSION_H