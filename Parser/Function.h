#ifndef GRAPH_CALC_PARSER_FUNCTION_H
#define GRAPH_CALC_PARSER_FUNCTION_H

#include <vector>
#include <exception>

class EvaluateException : public std::runtime_error{
    public:
        enum Reason{
            OTHER,
            ARGUMENT_COUNT
        };

        const Reason reason;
        EvaluateException(const std::string what, Reason reason=OTHER):std::runtime_error(what), reason(reason){}
};

class Function{
    public:
        using Arguments = std::vector<Number>;
        using Evaluator = Number (*)(Arguments&);
    private:
        int argsCount;
        bool noThrow = false;
        Evaluator evaluator;
        Number noThrowValue{};
    private:
        inline bool canEvaluate(int n) const{
            if(!evaluator || (argsCount != n && argsCount + 1 > 0)){
                if(!noThrow){
                    throw EvaluateException("Function expected " + std::to_string(argsCount) + " arguments, got " + std::to_string(n) + ".", EvaluateException::ARGUMENT_COUNT);
                }
                return false;
            }
            return true;
        }

    public:
        Function(int args = 0, Evaluator evaluator = nullptr): argsCount(args), evaluator(evaluator){};

        inline int argumentCount() const {
            return argsCount;
        }

        inline void setNoThrow(Number defaultValue){
            noThrow = true;
            noThrowValue = defaultValue;
        }

        inline Number operator()(Arguments& x) const{
            if(!canEvaluate(x.size()))
                return noThrowValue;
            return evaluator(x);
        }
};


#endif // GRAPH_CALC_PARSER_FUNCTION_H