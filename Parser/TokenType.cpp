#include <iostream>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include "TokenType.h"


using FArgs = Function::Arguments&;

static Number minf(FArgs x){
    if(x.size() == 0) return nanf("");
    Number min = x[0];
    for(auto& n : x){
        if(n < min){
            min = n; 
        }
    }
    return min;
}

static Number maxf(FArgs x){
    if(x.size() == 0) return nanf("");
    Number max = x[0];
    for(auto& n : x){
        if(n > max){
            max = n; 
        }
    }
    return max;
}

static Number avgf(FArgs x){
    if(x.size() == 0) {
        return 0.0f;
    } 
    Number ret = 0; 
    for(auto& f : x){
        ret += f;
    } 
    return ret/x.size();
}

static Number signf(FArgs x){
    if(x.size() == 0){
        return nanf("");
    }
    if(x[0] == 0) return 0;
    return x[0] < 0 ? -1 : 1;
}

static const std::unordered_map<TokenType, const TokenInfo> tokenTable{
    {TokenType::NONE,       {"NONE"}},
    {TokenType::EOT,        {"EOT"}},
    {TokenType::COMMA,      {","}},
    {TokenType::CONSTANT,   {"CONSTANT"}},
    {TokenType::VARIABLE,   {"VARIABLE"}},
    {TokenType::CONST_PI,   {"pi"}},
    {TokenType::CONST_E,    {"e"}},
    {TokenType::PAR_LEFT,   {"("}},
    {TokenType::PAR_RIGHT,  {")"}},
    {TokenType::OP_EQUAL,   {"=",       {2}}},
    {TokenType::OP_ADD,     {"+",       {2, [](FArgs x){ return x[0] + x[1]; }} }},
    {TokenType::OP_SUBS,    {"-",       {2, [](FArgs x){ return x[0] - x[1]; }} }},
    {TokenType::OP_MULT,    {"*",       {2, [](FArgs x){ return x[0] * x[1]; }} }},
    {TokenType::OP_DIV,     {"/",       {2, [](FArgs x){ return x[0] / x[1]; }} }},
    {TokenType::OP_POW,     {"^",       {2, [](FArgs x){ return powf(x[0], x[1]); }} }},
    {TokenType::OP_NEG,     {"-",       {1, [](FArgs x){ return -x[0]; }} }},
    {TokenType::FUN_SINC,   {"sinc",    {1, [](FArgs x){ return sinf(x[0])/x[0]; }}}},
    {TokenType::FUN_SIN,    {"sin",     {1, [](FArgs x){ return sinf(x[0]); }} }},
    {TokenType::FUN_COS,    {"cos",     {1, [](FArgs x){ return cosf(x[0]); }}}},
    {TokenType::FUN_TAN,    {"tan",     {1, [](FArgs x){ return tanf(x[0]); }}}},
    {TokenType::FUN_ASIN,   {"asin",    {1, [](FArgs x){ return asinf(x[0]); }}}},
    {TokenType::FUN_ACOS,   {"acos",    {1, [](FArgs x){ return acosf(x[0]); }}}},
    {TokenType::FUN_ATAN,   {"atan",    {1, [](FArgs x){ return atanf(x[0]); }}}},
    {TokenType::FUN_EXP,    {"exp",     {1, [](FArgs x){ return expf(x[0]); }}}},
    {TokenType::FUN_LN,     {"ln",      {1, [](FArgs x){ return logf(x[0]); }}}},
    {TokenType::FUN_LOG2,   {"log2",    {1, [](FArgs x){ return log2f(x[0]); }}}},
    {TokenType::FUN_LOG,    {"log",     {1, [](FArgs x){ return log10f(x[0]); }}}},
    {TokenType::FUN_SQRT,   {"sqrt",    {1, [](FArgs x){ return sqrtf(x[0]); }}}},
    {TokenType::FUN_FLOOR,  {"floor",   {1, [](FArgs x){ return floorf(x[0]); }}}},
    {TokenType::FUN_CEIL,   {"ceil",    {1, [](FArgs x){ return ceilf(x[0]); }}}},
    {TokenType::FUN_SIGN,   {"sign",    {1, [](FArgs x){ return signf(x); }}}},
    {TokenType::FUN_MIN,    {"min",     {-1, [](FArgs x){ return minf(x); }}}},
    {TokenType::FUN_MAX,    {"max",     {-1, [](FArgs x){ return maxf(x); }}}},
    {TokenType::FUN_AVG,    {"avg",     {-1, [](FArgs x){ return avgf(x); }}}},
    {TokenType::FUN_MASK,   {"mask",    {2, [](FArgs x){ return 1.0f; }}}},
};

static const std::set<TokenType> tokenFunctionSet{
    TokenType::FUN_SINC,
    TokenType::FUN_SIN,
    TokenType::FUN_COS,  
    TokenType::FUN_TAN,
    TokenType::FUN_ASIN,
    TokenType::FUN_ACOS,
    TokenType::FUN_ATAN,
    TokenType::FUN_EXP,
    TokenType::FUN_LN,
    TokenType::FUN_LOG2, 
    TokenType::FUN_LOG, 
    TokenType::FUN_SQRT,  
    TokenType::FUN_FLOOR,  
    TokenType::FUN_CEIL,  
    TokenType::FUN_SIGN,
    TokenType::FUN_MIN,  
    TokenType::FUN_MAX,  
    TokenType::FUN_AVG,  
    TokenType::FUN_MASK,
};

static const std::unordered_set<TokenType> tokenOperatorSet{
    TokenType::OP_EQUAL,
    TokenType::OP_ADD,
    TokenType::OP_SUBS,
    TokenType::OP_MULT,
    TokenType::OP_DIV,
    TokenType::OP_POW,
    TokenType::OP_NEG,
};

static const TokenTypeSet tokenMathConstSet{
    TokenType::CONST_E, 
    TokenType::CONST_PI,
};

static const std::unordered_map<TokenType, Number> tokenMathConstTable{
    {TokenType::NONE, 0},
    {TokenType::CONST_PI, 3.14159265359},
    {TokenType::CONST_E, 2.71828182846}
};

static const std::unordered_map<char, TokenType> charToTokenTypeTable{
    {'\0', TokenType::EOT},
    {',', TokenType::COMMA},
    {'(', TokenType::PAR_LEFT},
    {')', TokenType::PAR_RIGHT},
    {'=', TokenType::OP_EQUAL},
    {'+', TokenType::OP_ADD},
    {'-', TokenType::OP_SUBS},
    {'*', TokenType::OP_MULT},
    {'/', TokenType::OP_DIV},
    {'^', TokenType::OP_POW},
};

static const std::unordered_map<std::string, TokenType> strToTokenTypeTable{
    {"\0", TokenType::EOT},
    {",", TokenType::COMMA},
    {"pi", TokenType::CONST_PI},
    {"e", TokenType::CONST_E},
    {"(", TokenType::PAR_LEFT},
    {")", TokenType::PAR_RIGHT},
    {"=", TokenType::OP_EQUAL},
    {"+", TokenType::OP_ADD},
    {"-", TokenType::OP_SUBS},
    {"*", TokenType::OP_MULT},
    {"/", TokenType::OP_DIV},
    {"^", TokenType::OP_POW},
    {"sinc", TokenType::FUN_SINC},
    {"sin", TokenType::FUN_SIN},
    {"asin", TokenType::FUN_ASIN},
    {"cos", TokenType::FUN_COS},
    {"acos", TokenType::FUN_ACOS},
    {"tan", TokenType::FUN_TAN},
    {"atan", TokenType::FUN_ATAN},
    {"exp", TokenType::FUN_EXP},
    {"ln", TokenType::FUN_LN},
    {"log2", TokenType::FUN_LOG2},
    {"log", TokenType::FUN_LOG},
    {"sqrt", TokenType::FUN_SQRT},
    {"floor", TokenType::FUN_FLOOR},
    {"ceil", TokenType::FUN_CEIL},
    {"sign", TokenType::FUN_SIGN},
    {"min", TokenType::FUN_MIN},
    {"max", TokenType::FUN_MAX},
    {"avg", TokenType::FUN_AVG},
    {"mask", TokenType::FUN_MASK}
};


const TokenInfo& TokenInfo::get(const TokenType& t){
    //std::cerr << "SEARCHING FOR TKN " << (int)t << std::endl;
    if(tokenTable.count(t) > 0){
        return tokenTable.at(t);
    }
    return tokenTable.at(TokenType::NONE);
}

Associativity TokenInfo::getAssociativity(const TokenType& t){
    if(t == TokenType::OP_POW || t == TokenType::OP_NEG || isFunction(t)){
        return Associativity::RIGHT;
    }
    return Associativity::LEFT;
}

Precedence TokenInfo::getPrecedence(const TokenType& t){
    switch (t){
        case TokenType::PAR_LEFT:
        case TokenType::PAR_RIGHT:
            return Precedence::PARENTHESIS;
        case TokenType::OP_POW:
            return Precedence::EXPONENTIATION;
        case TokenType::OP_NEG:
            return Precedence::EXPONENTIATION;
        case TokenType::OP_DIV:
        case TokenType::OP_MULT:
            return Precedence::MULTIPLICATION;
        case TokenType::OP_ADD:
        case TokenType::OP_SUBS:
            return Precedence::ADDITION;
        
        case TokenType::OP_EQUAL:
            return Precedence::EQUAL;
    
        default:
            if(isFunction(t)){
                return Precedence::FUNCTION;
            }
            return Precedence::NONE;
    }
}

Number TokenInfo::getMathConst(const TokenType& t){
    if(tokenMathConstTable.count(t) > 0){
        return tokenMathConstTable.at(t);
    }
    return tokenMathConstTable.at(TokenType::NONE);
}

TokenType TokenInfo::parse(char c){
    if(charToTokenTypeTable.count(c) > 0){
        return charToTokenTypeTable.at(c);
    }
    return TokenType::NONE;
}

TokenType TokenInfo::parse(const std::string& str){
    if(strToTokenTypeTable.count(str) > 0){
        return strToTokenTypeTable.at(str);
    }
    return TokenType::NONE;
}

const TokenTypeOrderedSet& TokenInfo::getSetOfFunctions(){
    return tokenFunctionSet;
}

const TokenTypeSet& TokenInfo::getSetOfMathConsts(){
    return tokenMathConstSet;
}

bool TokenInfo::isMathConst(const TokenType& t){
    return tokenMathConstSet.count(t) > 0;
}


bool TokenInfo::isFunction(const TokenType& t){
    return tokenFunctionSet.count(t) > 0;
}

bool TokenInfo::isOperator(const TokenType& t){
    return tokenOperatorSet.count(t) > 0;
}