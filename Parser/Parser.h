#ifndef GRAPH_CALC_PARSER_PARSER_H
#define GRAPH_CALC_PARSER_PARSER_H

#include <stack>
#include <queue>
#include <unordered_set>
#include <exception>
#include "Scanner.h"


class ParserException : public std::runtime_error{
    public:
        enum Reason{
            OTHER,
            UNEXPECTED_TOKEN,
            UNBALANCED_PARENTHESIS,
        };
        const Reason reason;
        ParserException(const std::string& what, Reason reason = OTHER): std::runtime_error(what), reason(reason){}
};

class Parser{
    private:
        Scanner lexer;
        bool validExpression = true;
        std::unordered_set<char> variables;

        std::vector<Token> stack;
        std::vector<Token> queue;

        void parse();

    public:
        Parser():validExpression(false){};
        Parser(const std::string& expression) : lexer(expression){
            validExpression = lexer.isValidExpression();
            if(validExpression){
                lexer.rewind();
                parse();
            }
        }
        ~Parser(){}

        inline bool isValidExpression() const { return validExpression; }
        inline std::vector<Token>& getRPN() { return queue; }
        inline std::unordered_set<char>& getVariables() { return variables; }

    private:
        void inTimeSimplify();
        inline bool emptyStack() { return stack.empty(); }
        inline void enqueue(const Token& tkn){ queue.push_back(tkn); inTimeSimplify();}
        inline void push(const Token& tkn){ stack.push_back(tkn);}
        inline Token pop() { Token retVal = stack.back(); stack.pop_back(); return retVal; }
        inline Token& top() { return stack.back(); }
        

        void matchComma();
        void matchOperator(const Token& tkn);
        void matchRightParenthesis();
};

#endif //GRAPH_CALC_PARSER_PARSER_H