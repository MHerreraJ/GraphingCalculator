#ifndef GRAPH_CALC_PARSER_TOKEN_TYPE_H
#define GRAPH_CALC_PARSER_TOKEN_TYPE_H

#include "Variable.h"
#include "Function.h"

#include <set>
#include <unordered_set>

enum class TokenType{
    NONE,       EOT,
    COMMA,
    CONSTANT,   VARIABLE,
    CONST_PI,   CONST_E,
    PAR_LEFT,   PAR_RIGHT,
    OP_EQUAL,
    OP_ADD,     OP_SUBS,
    OP_MULT,    OP_DIV,
    OP_POW,     OP_NEG,
    FUN_SINC,
    FUN_SIN,    FUN_COS,    FUN_TAN,
    FUN_ASIN,   FUN_ACOS,   FUN_ATAN,
    FUN_EXP,    FUN_LN,     FUN_LOG2,    FUN_LOG,
    FUN_SQRT,
    FUN_FLOOR,  FUN_CEIL, FUN_SIGN,
    FUN_MIN,    FUN_MAX,
    FUN_AVG, FUN_MASK,
};

enum class Associativity{
    LEFT, RIGHT
};

enum class Precedence{
    NONE,
    EQUAL,
    ADDITION,
    MULTIPLICATION,
    FUNCTION,
    EXPONENTIATION,
    PARENTHESIS
};

using TokenTypeSet = std::unordered_set<TokenType>;
using TokenTypeOrderedSet = std::set<TokenType>;


class TokenInfo{
    public:
        const std::string lexeme;
        const Function evaluator;

        TokenInfo(const char* lexeme, const Function& fun=Function{}):
            lexeme(lexeme), evaluator(fun) {}


        static const TokenInfo& get(const TokenType& t);

        static Associativity getAssociativity(const TokenType& t);
        static Precedence getPrecedence(const TokenType& t);
        static Number getMathConst(const TokenType& t);

        static TokenType parse(char c);
        static TokenType parse(const std::string& str);
        static TokenType parse(const char* str) {return parse(std::string(str));};
        static const TokenTypeOrderedSet& getSetOfFunctions();
        static const TokenTypeSet& getSetOfMathConsts();

        static bool isMathConst(const TokenType& t);
        static inline bool isVariable(const TokenType& t) { return t == TokenType::VARIABLE; }
        static inline bool isConstant(const TokenType& t) { return t == TokenType::CONSTANT; }
        static inline bool isNumber(const TokenType& t) { return isVariable(t) || isConstant(t) || isMathConst(t); }

        static bool isFunction(const TokenType& t);
        static bool isOperator(const TokenType& t);

        static inline bool isOperator(char c) { return c == '+' || c == '-' || c == '*' || c == '/' || c == '^'; }
};



#endif //GRAPH_CALC_PARSER_TOKEN_TYPE_H