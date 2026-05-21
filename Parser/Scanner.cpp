#include <iostream>
#include <cctype>
#include "Scanner.h"


void Scanner::addToken(const Token& tkn){
    auto& lastTkn = lastAddedToken();

    if(lastTkn == TokenType::PAR_RIGHT || lastTkn.isNumber()){
        if(lastTkn == TokenType::PAR_RIGHT && tkn.isConstant()){
            tokens.push_back(Token(TokenType::OP_MULT));
        }else if(tkn == TokenType::PAR_LEFT || tkn.isVariable() || tkn.isFunction() || tkn.isMathConst()){
            tokens.push_back(Token(TokenType::OP_MULT));
        }
    }

    if(tkn.isMathConst()){
        float value = TokenInfo::getMathConst(tkn.getType());
        tokens.push_back(Token(TokenType::CONSTANT, Variable(value)));
    }else{
        tokens.push_back(tkn);
    }

    if(!checkFunctionArguments(tokens.size()-1, tokens.back())){
        validExpression = false;
    }
}

void Scanner::addToken(const TokenType& tkn){
    addToken(Token(tkn));
}

bool Scanner::checkFunctionArguments(int idx, const Token& tkn){
    //std::cout << __LINE__ << " Checking token " << tkn.lexeme() << std::endl;
    if(tkn.isFunction()){
        if(peek() == '('){
            //std::cout << "INFO: Pushing function " << tkn.lexeme() << " to the top of the stack" << std::endl;
            functionStack.push_back(idx);

            commaStack.push_back(0);
            promiseLeftParenthesis = true;
        }else if(tkn.getArgumentCount() != 1){
            std::cerr << "Error: Function " << tkn.lexeme() << " expecting > 1 parameter need parenthesis"<< std::endl;
            return false;
        }
        return true;
    }

    if(tkn == TokenType::PAR_LEFT && !promiseLeftParenthesis){
        //std::cout << "INFO: Pushing function " << tkn.lexeme() << " to the top of the stack" << std::endl;
        functionStack.push_back(idx);
        commaStack.push_back(0);
    }

    promiseLeftParenthesis = false;

    if(tkn == TokenType::COMMA){
        if(functionStack.empty()){
            std::cerr << "Error: Comma found out of function context"<< std::endl;
            return false;
        }
        Token& fxn = tokens[functionStack.back()];
        int args = (++ commaStack.back());
        int expectedArgs = fxn.getArgumentCount();

        if(expectedArgs >=0 && args >= expectedArgs){
            std::cerr << "Error: Function " << fxn.lexeme() << " expecting " << fxn.getArgumentCount() << " parameters, got more (" << (args + 1) << ")" << std::endl;
            return false;
        }
    }else if(tkn == TokenType::PAR_RIGHT && !functionStack.empty()){
        int fxnIdx = functionStack.back();
        Token& fxn = tokens[fxnIdx];
        int args = commaStack.back();
        int expectedArgs = fxn.getArgumentCount();

        functionStack.pop_back();
        commaStack.pop_back();


        if(fxn == TokenType::PAR_LEFT){
            if(args != 0){
                std::cerr << "Error: Comma found out of function context (parenthesis)"<< std::endl;
                return false;
            }
        } else if(expectedArgs < 0){
            if(args == 0){
                for(int tokenIdx = fxnIdx+1; tokenIdx < idx; tokenIdx++){
                    if(tokens[tokenIdx].isNumber()){
                        //std::cout << "Info: Found " << 1 << " parameters for " << fxn.lexeme() << ". Updating" << std::endl;
                        fxn.setArgumentCount(1);
                        return true;
                    }
                }
                //std::cout << "Info: Found " << 0 << " parameters for " << fxn.lexeme() << ". Updating" << std::endl;
                fxn.setArgumentCount(0);
                return true;
            }else{
                //std::cout << "Info: Found " << (args+1) << " parameters for " << fxn.lexeme() << ". Updating" << std::endl;
                fxn.setArgumentCount(args+1);
                return true;
            }
        } else if(expectedArgs != args + 1){
            std::cerr << "Error: Function " << fxn.lexeme() << " expecting " << expectedArgs << " parameters, got " << (args + 1) << std::endl;
            return false;
        } else{
            if(args == 0){
                for(int tokenIdx = fxnIdx+1; tokenIdx < idx; tokenIdx++){
                    if(tokens[tokenIdx].isNumber()){
                        //std::cout << "Info: Setting expected " << 1 << " parameters for " << fxn.lexeme() << ". Updating" << std::endl;
                        fxn.setArgumentCount(1);
                        return true;
                    }
                }
                std::cerr << "Error: Function " << fxn.lexeme() << " expecting " << expectedArgs << " parameters, got " << 0 << std::endl;
                fxn.setArgumentCount(0);
                return false;

            }else{
                //std::cout << "Info: Setting expected " << expectedArgs << " parameters for " << fxn.lexeme() << ". Updating" << std::endl;
                fxn.setArgumentCount(expectedArgs);
                return true;
            }
        }
        return true;
    }
    return true;
}


Token& Scanner::lastAddedToken(){
    static Token tknEOT{TokenType::EOT};
    if(tokens.size() == 0)
        return tknEOT;
    return tokens.back();
}

char Scanner::consume(){
    if(available()){
        return expression[exprIdx++];
    }
    return 0;
}

char Scanner::peek(){
    if(available()){
        return expression[exprIdx];
    }
    return 0;
}

bool Scanner::match(char c){
    if(available()){
        if(expression[exprIdx] == c){
            exprIdx++;
            return true;
        }
    }
    return false;
}

bool Scanner::match(const std::string& str){
    size_t idx = exprIdx;
    if(!available() || (idx + str.length()) > expression.length()) 
        return false;
    
    for(const char& c : str){
        if(c == expression[idx]){
            idx++;
        }else{
            return false;
        }
    }
    exprIdx += str.length();
    return true;
}

void Scanner::consumewhitespace(){
    while(match(' ') || match('\t') || match('\n'));
}

Token Scanner::consumeOperator(){
    Token& lastToken = lastAddedToken();
    if(peek() == '-'){
        consume();
        if(lastToken == TokenType::PAR_LEFT || lastToken == TokenType::EOT || lastToken == TokenType::COMMA || lastToken.isOperator() || lastToken.isFunction()){
            return Token(TokenType::OP_NEG);
        }else{
            return Token(TokenType::OP_SUBS);
        }
    }else{
        return Token(TokenInfo::parse(consume()));
    }
}

Token Scanner::consumeNumber(){
    float numInt = 0.0F;
    float mult = 0.1F;

    while(isdigit(peek())){
        numInt = 10.0F*numInt + (int)((consume() - '0'));
    }

    if(match('.')){
        if(!isdigit(peek())){
            validExpression = false;
            return Token(TokenType::NONE);
        }
        while(isdigit(peek())){
            numInt += mult*(int)(consume() - '0');
            mult *= 0.1F;
        }
    }

    return Token(TokenType::CONSTANT, Variable(numInt));

}

Token Scanner::consumeFunction(){
    for(auto& function : TokenInfo::getSetOfFunctions()){
        auto& lexeme  = TokenInfo::get(function).lexeme;
        if(match(lexeme)){
            return Token(function);
        }
    }
    return Token(TokenType::NONE);
}

Token Scanner::consumeMathConst(){
    for(auto& mathConst : TokenInfo::getSetOfMathConsts()){
        auto& lexeme  = TokenInfo::get(mathConst).lexeme;
        if(match(lexeme)){
            return Token(mathConst);
        }
    }
    return Token(TokenType::NONE);
}

void Scanner::tokenize(){
     while(available()){
        if(!validExpression) break;
        consumewhitespace();
        if(match('(')){
            parenthesisCount += 1;
            addToken(TokenType::PAR_LEFT);
        }else if(match(')')){
            parenthesisCount -= 1;
            addToken(TokenType::PAR_RIGHT);
        }else if(match('=')){
            addToken(TokenType::OP_EQUAL);
            if(++equalCount > 1){
                validExpression = false;
            }
        } else if(match(',')){
            addToken(TokenType::COMMA);
        }else if(TokenInfo::isOperator(peek())){
            addToken(consumeOperator());
        }else if(isdigit(peek()) || peek() == '.'){
            addToken(consumeNumber());
        }else if(peek() == 'x' || peek() == 'y'){
            addToken(Token(TokenType::VARIABLE, Variable(1.0, consume())));
        }else{
            Token tkn = consumeFunction();
            if(tkn == TokenType::NONE){
                // Check if math const
                tkn = consumeMathConst();
                if(tkn == TokenType::NONE){
                    if(isalpha(peek())){
                        tkn = Token(TokenType::VARIABLE, Variable(1.0f, consume()));
                    }else{
                        validExpression = false;
                        consume();
                    }
                }
            }
            addToken(tkn);
        }
    }
    if(validExpression){
        while(parenthesisCount > 0){
            addToken(TokenType::PAR_RIGHT);
            parenthesisCount--;
        }
    }
    addToken(TokenType::EOT);
}

Token& Scanner::nextToken(){
    static Token tknEOT{TokenType::EOT};
    if(tokensAvailable()){
        return tokens[tknIdx++];
    }
    return tknEOT;
}

Token& Scanner::peekToken(){
    static Token tknEOT{TokenType::EOT};
    if(tknIdx < tokens.size() - 1){
        return tokens[tknIdx + 1];
    }
    return tknEOT;
}
