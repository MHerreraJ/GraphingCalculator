#ifndef GRAPH_CALC_PARSER_VARIABLE_H
#define GRAPH_CALC_PARSER_VARIABLE_H

#include <iostream>

using Number = float;

class Variable{
    private:
        Number _coefficient;
        char _literal;
    public:
        Variable(char lit):_coefficient(1), _literal(lit){}
        Variable(Number coeff = 0, char lit = 0):_coefficient(coeff), _literal(lit){}

        inline bool isConstant() const {
            return _literal == 0;
        }

        inline const Number& coefficient() const{
            return _coefficient;
        }

        inline const char& literal() const{
            return _literal;
        }

        inline Number operator ()(Number value) const{
            if(isConstant()) return _coefficient;
            return _coefficient*value;
        }

    public:
        friend std::ostream& operator<<(std::ostream&, const Variable&);

};

inline std::ostream& operator<<(std::ostream& os, const Variable& var){
    if(var._literal == 0){
        os << var._coefficient;
    }else if(var._coefficient == 1){
        os << var._literal;
    }else if(var._coefficient == -1){
        os << "-" << var._literal;
    }else if(var._coefficient == 0){
        os << 0;
    }else{
        os << var._coefficient << var._literal;
    }
    return os;
}





#endif //GRAPH_CALC_PARSER_VARIABLE_H