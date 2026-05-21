#include "Parser.h"


void Parser::inTimeSimplify(){
    Token& tkn = queue.back();
    if(!(tkn.isFunction() || tkn.isOperator()) || tkn == TokenType::FUN_MASK || (int)queue.size() < tkn.getArgumentCount() + 1){
        return;
    }

    if(tkn.getArgumentCount() == 1){
        if(queue[queue.size()-2] == TokenType::CONSTANT){
            Variable var = queue[queue.size()-2].getValue();
            Token newVar = Token(TokenType::CONSTANT, Variable(tkn.invoke(var.coefficient())));
            queue.pop_back();
            queue.pop_back();
            queue.push_back(newVar);
        }else if(tkn == TokenType::OP_NEG && queue[queue.size()-2] == TokenType::OP_NEG){
            queue.pop_back();
            queue.pop_back();
        }else if(tkn == TokenType::OP_NEG && queue[queue.size()-2] == TokenType::VARIABLE){
            Variable var = queue[queue.size()-2].getValue();
            Token newVar = Token(TokenType::VARIABLE, Variable(-var.coefficient(), var.literal()));
            queue.pop_back();
            queue.pop_back();
            queue.push_back(newVar);
        }
        return;
    }

    if(tkn.getArgumentCount() != 2 || !queue[queue.size()-3].isNumber() || !queue[queue.size()-2].isNumber()){
        return;
    }

    Token& t1 = queue[queue.size()-3];
    Token& t2 = queue[queue.size()-2];
    const Variable& v1 = t1.getValue();
    const Variable& v2 = t2.getValue();
    Token newVar = TokenType::NONE;

    if(t1 == TokenType::CONSTANT && t2 == TokenType::CONSTANT){
        Number value = tkn.invoke(v1.coefficient(), v2.coefficient());
        newVar = Token(TokenType::CONSTANT, Variable(value));
    }else if(t1 == TokenType::VARIABLE && t2 == TokenType::VARIABLE && v1.literal() == v2.literal() && tkn.precedence() == Precedence::ADDITION){
        Number value = tkn.invoke(v1.coefficient(), v2.coefficient());
        newVar = Token(TokenType::VARIABLE, Variable(value, v1.literal()));
    }else if(t1 == TokenType::CONSTANT && t2 == TokenType::VARIABLE && tkn == TokenType::OP_MULT){
        Number value = tkn.invoke(v1.coefficient(), v2.coefficient());
        newVar = Token(TokenType::VARIABLE, Variable(value, v2.literal()));        
    }else if(t1 == TokenType::VARIABLE && t2 == TokenType::CONSTANT && (tkn == TokenType::OP_MULT || tkn == TokenType::OP_DIV)){
        Number value = tkn.invoke(v1.coefficient(), v2.coefficient());
        newVar = Token(TokenType::VARIABLE, Variable(value, v1.literal()));
    }

    if(newVar != TokenType::NONE){
        queue.pop_back();
        queue.pop_back();
        queue.pop_back();
        queue.push_back(newVar);
    }
}

void Parser::matchComma(){
    while(!emptyStack() && top() != TokenType::PAR_LEFT){
        enqueue(pop());
    }
    if(emptyStack()){
        validExpression = false;
        throw ParserException("Unbalanced parenthesis", ParserException::UNBALANCED_PARENTHESIS);
    }
}

void Parser::matchOperator(const Token& tkn){
    while(!emptyStack() && top().isOperator() &&
        ((tkn.associates(Associativity::LEFT) && tkn.precedence() <= top().precedence()) ||
            (tkn.associates(Associativity::RIGHT) && tkn.precedence() < top().precedence()))){
        
        enqueue(pop());
    }
    push(tkn);
}

void Parser::matchRightParenthesis(){
    while(!emptyStack() && top() != TokenType::PAR_LEFT){
        enqueue(pop());
    }
    if(emptyStack()){
        validExpression = false;
        throw ParserException("Unbalanced parenthesis", ParserException::UNBALANCED_PARENTHESIS);
    }
    pop();
    if(!emptyStack() && top().isFunction()){
        enqueue(pop());
    }
}

void Parser::parse(){
    while(lexer.tokensAvailable()){
        Token& tkn = lexer.nextToken();
        //std::cout << "Parsing TOKEN " << tkn << std::endl;
        if(tkn.isNumber()){
            enqueue(tkn);
            if(tkn == TokenType::VARIABLE){
                if(!tkn.getValue().isConstant()){
                    variables.insert(tkn.getValue().literal());
                }
            }
        }else if(tkn.isFunction()){
            if(tkn.getArgumentCount() < 2 && lexer.peekToken() != TokenType::PAR_LEFT){
                tkn.setFunctionAsOperator();
            }
            push(tkn);
        }else if(tkn == TokenType::COMMA){
            matchComma();
        }else if(tkn.isOperator()){
            matchOperator(tkn);
        }else if(tkn == TokenType::PAR_LEFT){
            push(tkn);
        }else if(tkn == TokenType::PAR_RIGHT){
            matchRightParenthesis();
        }else if(tkn != TokenType::EOT){
            throw ParserException("Unexpected token found in expression", ParserException::UNEXPECTED_TOKEN);
        }else if(tkn == TokenType::EOT){
            break;
        }
    }

    while(!emptyStack()){
        if(top() == TokenType::PAR_LEFT){
            validExpression = false;
            throw ParserException("Unbalanced parenthesis", ParserException::UNBALANCED_PARENTHESIS);
        }
        enqueue(pop());
    }
    // std::cout << "Parsed expression successfully, RPN: ";
    // for(const Token& tkn : queue){
    //     std::cout << tkn << std::endl;
    // }
    // std::cout << std::endl;
}
