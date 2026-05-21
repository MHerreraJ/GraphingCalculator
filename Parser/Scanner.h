#ifndef GRAPH_CALC_PARSER_SCANNER_H
#define GRAPH_CALC_PARSER_SCANNER_H

#include "Token.h"

class Scanner{
    private:
        size_t exprIdx;
        const std::string expression;

        size_t tknIdx;
        std::vector<Token> tokens;

        bool validExpression;
        int parenthesisCount;
        int equalCount;

        std::vector<int> functionStack;
        std::vector<int> commaStack;
        bool promiseLeftParenthesis = false;

        void tokenize();
        
    public:
        Scanner():exprIdx(0), expression(""), tknIdx(0), validExpression(false), parenthesisCount(0), equalCount(0){}
        Scanner(const std::string& str):exprIdx(0), expression(str), tknIdx(0), validExpression(true), parenthesisCount(0), equalCount(0) {tokenize();}
        ~Scanner(){}

        inline bool isValidExpression() const { return validExpression; }
        inline bool tokensAvailable() const { return tknIdx < tokens.size(); }

        void rewind() { tknIdx = 0; }
        Token& nextToken();
        Token& peekToken();

    private:
        char peek();
        char consume();
        bool match(char c);
        bool match(const std::string& str);

        void consumewhitespace();

        void addToken(const Token& tkn);
        void addToken(const TokenType& tkn);

        Token consumeOperator();
        Token consumeNumber();
        Token consumeFunction();
        Token consumeMathConst();

        bool checkFunctionArguments(int idx, const Token& tkn);


        Token& lastAddedToken();
        inline bool available() const { return exprIdx < expression.size(); }

};

#endif //GRAPH_CALC_PARSER_SCANNER_H