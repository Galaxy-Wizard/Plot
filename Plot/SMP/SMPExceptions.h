#ifndef SMPEXCEPTIONS_H
#define SMPEXCEPTIONS_H

#include "SimpleMathParser.h"

namespace smp {
//Exception-classes----------------------------
enum  ExceptionType //Views of exceptions
{
    E_UnexpectedError,
    E_IncorrectSyntax,
    E_InvalidBracketsCount,
    E_MathFunctionCrash,
    E_ConversionError,
    E_IncorrectArgument,
    E_MathError,
    E_NameError,
};

class InvalidExpression : public std::exception //Main exception class
{
protected:
    std::string m_error;
    ExceptionType type;

public:
    InvalidExpression(std::string error, ExceptionType tp = E_UnexpectedError) : m_error(error), type(tp) {}

    ExceptionType getExceptionType() { return type; }

    const char* what() const noexcept { return m_error.c_str(); }
};

class IncorrectSyntax : public InvalidExpression
{
public:
    IncorrectSyntax(std::string error) : InvalidExpression(error, E_IncorrectSyntax) {}
};

class MathError : public InvalidExpression
{
public:
    MathError(std::string error) : InvalidExpression(error, E_MathError) {}
};

class InvalidBracketsCount : public InvalidExpression
{
public:
    InvalidBracketsCount(std::string error) : InvalidExpression(error, E_InvalidBracketsCount) {}
};

class ConversionError : public InvalidExpression
{
private:
    std::string invalid_value;
public:
    ConversionError(std::string error, std::string invalid_value) : InvalidExpression(error, E_ConversionError), invalid_value(invalid_value) {}

    std::string getInvalidValue() { return invalid_value; }
};

class MathFunctionCrash : public InvalidExpression
{
private:
    std::string target_function;
public:
    MathFunctionCrash(std::string error, std::string targ_func) : InvalidExpression(error, E_MathFunctionCrash), target_function(targ_func) {}

    std::string getTargetFunction() { return target_function; }
};

class RecursionException : public MathFunctionCrash
{
private:
    Oper* exp1;
    Expression* exp2;
public:
    RecursionException(std::string error, std::string targ_func, Oper* exp1, Expression* exp2) : MathFunctionCrash(error, targ_func), exp1(exp1), exp2(exp2) {}
    Oper* getExp1() { return exp1; }
    Expression* getExp2() { return exp2; }
};

class IncorrectArgument : public MathFunctionCrash
{
private:
    std::string invalid_argument;
public:
    IncorrectArgument(std::string error, std::string targ_func, std::string arg) : MathFunctionCrash(error, targ_func), invalid_argument(arg) {}

    std::string getIncorrectArgument() { return invalid_argument; }
};

class NameErrorException : public InvalidExpression
{
private:
    std::string invalid_character;
public:
    NameErrorException(std::string error, std::string letter) : InvalidExpression(error, E_NameError), invalid_character(letter) {}

    std::string getInvalidCharacter() { return invalid_character; }
};

class IncorrectFunctionName : public NameErrorException
{
public:
    IncorrectFunctionName(std::string error, std::string letter) : NameErrorException(error, letter) {}
};

class IncorrectConstantName : public NameErrorException
{
public:
    IncorrectConstantName(std::string error, std::string letter) : NameErrorException(error, letter) {}
};

class IncorrectXAliasName : public NameErrorException
{
public:
    IncorrectXAliasName(std::string error, std::string letter) : NameErrorException(error, letter) {}
};

class RangeException : public std::exception
{
private:
    std::string m_error;

public:
    RangeException(std::string error) : m_error(error) {}

    const char* what() const noexcept { return m_error.c_str(); }
};

}
#endif // SMPEXCEPTIONS_H
