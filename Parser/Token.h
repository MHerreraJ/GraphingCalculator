#ifndef GRAPH_CALC_PARSER_TOKEN_H
#define GRAPH_CALC_PARSER_TOKEN_H

#include <vector>

#include "Variable.h"
#include "Function.h"
#include "TokenType.h"
#include "Function.h"

class Token{
    public:
    private:
        TokenType type;
        Variable value;
        int funArgs;
        bool funAsOperator;

    public:
        Token(TokenType type = TokenType::NONE, Variable value = 0.0f): type(type), value(value), funArgs(-1), funAsOperator(false){}
        ~Token(){};

        inline void setArgumentCount(int count) { funArgs = count; }
        inline void setFunctionAsOperator() { funAsOperator = true; }
        inline int getArgumentCount() const { return funArgs < 0? TokenInfo::get(type).evaluator.argumentCount() : funArgs; }
        inline const std::string& lexeme() const { return TokenInfo::get(type).lexeme; }

        inline Number invoke(const Number& x) const{ Function::Arguments args={x}; return invoke(args); }
        inline Number invoke(const Number& x, const Number& y) const{ Function::Arguments args={x,y}; return invoke(args); }
        inline Number invoke(Function::Arguments& x) const{ return TokenInfo::get(type).evaluator(x); }
        
        inline const TokenType& getType() const { return type; }
        inline const Variable& getValue() const { return value; }

        inline Precedence precedence() const { return TokenInfo::getPrecedence(type); }
        inline bool associates(Associativity associativity) const { return TokenInfo::getAssociativity(type) == associativity; } 

        inline bool isVariable() const { return TokenInfo::isVariable(type); }
        inline bool isConstant() const { return TokenInfo::isConstant(type); }
        inline bool isMathConst() const { return TokenInfo::isMathConst(type); }
        inline bool isNumber() const { return TokenInfo::isNumber(type); }
        inline bool isFunction() const { return TokenInfo::isFunction(type); }
        inline bool isOperator() const { return TokenInfo::isOperator(type) || funAsOperator; }
        inline bool isEvaluator() const { return isOperator() || isFunction(); }

        inline bool operator == (const TokenType& t) const { return t == type; }
        inline bool operator != (const TokenType& t) const { return t != type; }


    public:
        friend std::ostream& operator << (std::ostream&, const Token& );

};

inline std::ostream& operator << (std::ostream& os, const Token& token){
    if(token.isNumber()){
        os << '\'' << token.lexeme() << '\'' << " '" << token.value << "'";
    }else if(token == TokenType::OP_SUBS){
        os << '\'' << token.lexeme() << "' (substract)";

    }else if(token == TokenType::OP_NEG){
        os << '\'' << token.lexeme() << "' (negate)";

    }else if(token.isFunction()){
        os << '\'' << token.lexeme() << "' (" << token.getArgumentCount() << " params)";
    }else{
        os << '\'' << token.lexeme() << "'";
    }
    return os;
}


#endif //GRAPH_CALC_PARSER_TOKEN_H