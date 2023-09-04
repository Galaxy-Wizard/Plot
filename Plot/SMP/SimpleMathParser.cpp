#include "pch.h"

#include "SimpleMathParser.h"
#include "SMPExceptions.h"
#include <iostream>

using namespace smp;

ParserSettings::ParserSettings()
{
	InitializeConstants();
	InitializeFunctions();
}

//If the function is called without arguments, all constants are set by default.
void ParserSettings::InitializeConstants(std::map<char, double>* consts, bool addConstants)
{
	Constants.clear();

	if (addConstants)
	{
		if (Constants.find('p') == Constants.end())//Default constants (No forced redifinitions)
			Constants['p'] = PI;

		if (Constants.find('e') == Constants.end())
			Constants['e'] = E;
	}

	if (consts != nullptr)
	{
		Constants = *consts;
	}

	
}

void ParserSettings::InitializeFunctions(FunctionsMap *funcs, bool addFunctions)
{
	Functions.clear();

	if (funcs != nullptr)
		this->Functions = *funcs;

	if (addFunctions) //Standart functions
	{
		Functions["arcctg"] = (Function)[](double argument) { return std::atan(1 / argument); };
		Functions["ctg"] = (Function)[](double argument) { return 1 / std::tan(argument); };
		Functions["cth"] = (Function)[](double argument) { return 1 / std::tanh(argument); };

		Functions["asin"] = (Function)[](double argument)
		{
			if (argument < -1 || argument > 1)
				throw IncorrectArgument("asin`s argument must be in [-1; 1]!", "asin", std::to_string(argument));
			return std::asin(argument);
		};

		Functions["acos"] = (Function)[](double argument)
		{
			if (argument < -1 || argument > 1)
				throw IncorrectArgument("acos`s argument must be in [-1; 1]!", "acos", std::to_string(argument));
			return std::acos(argument);
		};

		Functions["lg"] = (Function)[](double argument)
		{
			if (argument <= 0)
				throw IncorrectArgument("log`s argument must be positive!", "log10", std::to_string(argument));
			return std::log10(argument);
		};

		Functions["ln"] = (Function)[](double argument)
		{ 
			if (argument <= 0)
				throw IncorrectArgument("ln`s argument must be positive!", "ln", std::to_string(argument));
			return std::log(argument) / std::log(E);
		};

		Functions["sqrt"] = (Function)[](double argument)
		{
			if (argument < 0)
				throw IncorrectArgument("sqrt`s argument can`t be less zero!", "sqrt", std::to_string(argument));
			return std::sqrt(argument);
		};

		Functions["sin"] = std::sin;
		Functions["cos"] = std::cos;
		Functions["tan"] = std::tan;
		Functions["tg"] = Functions["tan"];
		Functions["arctg"] = std::atan;
		Functions["sh"] = std::sinh;
		Functions["ch"] = std::cosh;
		Functions["th"] = std::tanh;
		Functions["log"] = Functions["ln"];
		Functions["abs"] = std::abs;
		Functions["floor"] = std::floor;
		Functions["ceil"] = std::ceil;
		Functions["erf"] = std::erf;

		Functions["exp"] = std::exp;
	}
}

double ParserSettings::getFunctionValue(std::string name, double argument)
{
	auto it = Functions.find(name);
	
	if (it == Functions.end())
		throw MathFunctionCrash("Function " + name + " was not declared! Expression isn`t correct!", name);
	
	return Functions[name].getCalculatedValue(argument);
}

double ParserSettings::getNumberFromLetter(char symb, double x_value)
{
	if (symb != x_alias)
	{
		auto it = Constants.find(symb);
		if (it != Constants.end())
			return Constants.at(symb);
		else
		{
			throw IncorrectSyntax("Unknown variable");

			return 1;
		}
	}

	else
		return x_value;

	throw IncorrectSyntax("Unknown variable");

	return 0.0;
}

//Function ParserSettings::getFunctionFromString(std::string func_name)
//{
//	auto it = Functions.find(func_name);
//
//	if (it == Functions.end())
//		throw MathFunctionCrash("Function " + func_name + " was not declared! Expression isn`t correct!", func_name);
//
//	return Functions[func_name];
//}



//Oper definintions-------------
Oper::Oper(std::string value, std::shared_ptr<ParserSettings> ps)
{
	origin_string = value;
	this->value = value;
	
	if (ps == nullptr)
	{
		std::shared_ptr<ParserSettings> ps_tmp(new ParserSettings);
		this->ps = ps_tmp;
	}
		
	else
		this->ps = ps;
}

void Oper::setNewXAlias(char symb)
{
	if (!isLetter(symb))
		throw IncorrectXAliasName("Incorrect x alias: " + symb, std::to_string(symb));
	
	ps->x_alias = symb;
}

char Oper::getXAlias() const
{
	return ps->x_alias;
}



bool Oper::isLetter(char symb)
{
	if (symb >= 'a' && symb <= 'z' || symb >= 'A' && symb <= 'Z')
		return true;

	return false;
}

bool Oper::isDigit(char symb)
{
	if (symb >= '0' && symb <= '9')
		return true;

	return false;
}

bool Oper::isServiceSymbol(char symb)
{
	if (symb == '(' || symb == ')' || symb == '+' || symb == '-' || symb == '*' || symb == '/' || symb == '^' || symb == '.' || symb == ':')
		return true;
	return false;
}


void Oper::CutUnnecessary() //Whitespaces, special symbols and unreadable symbols like '?'
{
	if (value[0] == '+')
		value.erase(0);

	for (int i = 0; i < value.size(); i++)
	{
		if (!isLetter(value[i]) && !isDigit(value[i]) && !isServiceSymbol(value[i]))
		{
			value.erase(i, 1);
			i--;
		}
	}
}

void Oper::FunctionsMarker()
{
	size_t pos;

	for (auto &var : ps->Functions)
	{
		pos = value.find(var.first, size_t(0));

		std::string new_string = "({" + var.first + "}";

		while (pos != std::string::npos)
		{
			if ((pos == 0 || value[pos - 1] != '{') && value[pos + var.first.size()] != '}')
			{
				value.replace(pos, var.first.length(), new_string);

				size_t j = pos + var.first.size() + size_t(4);
				int opened_brackets = 1;

				while (j < value.size() && opened_brackets != 0)
				{
					if (value[j] == '(')
						opened_brackets++;
					else if (value[j] == ')')
						opened_brackets--;
					j++;
				}

				if (j < value.size() - 1)
					value.insert(j, 1, ')');
				else
					value.push_back(')');

				pos = value.find(var.first, pos + size_t(3));
			}

			else
				pos = value.find(var.first, pos + size_t(1));
		}
	}
}


void Oper::prepareString()
{
	replaceIncorrectSymbols();
	CutUnnecessary();
	FunctionsMarker();

	bool isFunction = false;

	for (int i = 0; i < value.size(); i++)
	{
		if (value[i] == '{')
			isFunction = true;
		else if (value[i] == '}')
			isFunction = false;

		if (!isFunction)
		{
			if (i > 0)
			{
				if (isLetter(value[i]))
				{
					if (value[i - 1] != '*')
					{
						if (value[i - 1] != '+' && value[i - 1] != '-' && value[i - 1] != '^' && value[i - 1] != '/' && value[i - 1] != '(')
						{
							value.insert(i, 1, '*');
							i++;
						}
					}
				}

				if (value[i - 1] != '*')
				{
					if (value[i] == '(')
					{
						if (value[i - 1] != '^' && value[i - 1] != '+' && value[i - 1] != '-' && value[i - 1] != 'x' && value[i - 1] != '/' && value[i - 1] != '(' && value[i - 1] != '}')
						{
							value.insert(i, 1, '*');
							i++;
						}
					}
				}
			}

			if (i < value.size() - 1)
			{
				if (isLetter(value[i]))
				{
					if (value[i + 1] != '*')
					{
						if (value[i + 1] != '+' && value[i + 1] != '-' && value[i + 1] != '^' && value[i + 1] != '/' && value[i + 1] != ')')
						{
							value.insert(i + 1, 1, '*');
						}
					}
				}

				if (value[i] == ')')
				{
					if (value[i + 1] != '*')
					{
						if (value[i + 1] != '^' && value[i + 1] != '+' && value[i + 1] != '-' && value[i + 1] != 'x' && value[i + 1] != '/' && value[i + 1] != ')')
						{
							value.insert(i + 1, 1, '*');
						}
					}
				}
			}
		}
	}
}

void Oper::clearSubOpers()
{
	sub_opers.clear();
}

void Oper::checkBracketsCorrect()
{
	int opened_count = 0;

	for (auto &var : value)
	{
		if (var == '(')
			opened_count++;
		else if (var == ')')
			opened_count--;
	}

	if (opened_count != 0)
		throw InvalidBracketsCount("Brackets count is invalid. Expression isn`t correct!");
}


void Oper::replaceIncorrectSymbols()
{
	std::string to_change_syms[3]{ ":", ",", "pi" };

	for (auto var : to_change_syms)
	{
		auto to_change = value.find(var); //Find all ':'
		while (to_change != std::string::npos)
		{
			if(var == ":")
				value[to_change] = '/';
			else if(var == ",")
				value[to_change] = '.';
			else if(var == "pi" && ps->Constants.find('p') != ps->Constants.end())
				value.replace(to_change, var.length(), "p");

			to_change = value.find(var, to_change + size_t(1));
		}
	}
}

void Oper::setConstants(ConstantsMap* consts, bool addConstants)
{
	ps->InitializeConstants(consts, addConstants);
	setExpression(origin_string);
}

ConstantsMap& Oper::getConstants() const
{
	return ps->Constants;
}

FunctionsMap& Oper::getFunctions() const
{
	return this->ps->Functions;
}

void Oper::addConstant(char symb, double value)
{
	std::string tmp_str{ symb };
	if (!isLetter(symb))
		throw IncorrectConstantName("Unresolved character (" + tmp_str + ") in name of constant: " + symb, std::to_string(symb));

	ps->Constants[symb] = value;
	setExpression(origin_string);
}

void Oper::deleteConstant(char name, bool isThrow)
{
	auto it = ps->Constants.find(name);
	if (it == ps->Constants.end() && isThrow)
		throw IncorrectConstantName("There is no constant to delete with name: " + name, std::to_string(name));

	ps->Constants.erase(it);
	setExpression(origin_string);
}

void Oper::setFunctions(FunctionsMap *funcs, bool addFunctions)
{
	ps->InitializeFunctions(funcs, addFunctions);
	setExpression(origin_string);
}

void Oper::addFunction(std::string name, Function function)
{
	checkFunctionNameCorrectness(name);
	
	ps->Functions[name] = function;
	setExpression(origin_string);
}

void Oper::checkFunctionNameCorrectness(std::string func_name)
{
	for (auto var : func_name)
	{
		std::string tmp_str{ var };
		if (!isLetter(var))
			throw IncorrectFunctionName("Unresolved character (" + tmp_str + ") in name of function: " + func_name, std::to_string(var));
	}
}

void Oper::addFunction(std::string name, Expression & exp)
{
	if (this == &exp)
		throw RecursionException("It is about to be a recursive function " + name + "(" + this->ps->x_alias + ")! addFunction can`t get an instance equals this", name, this, &exp);

	for (auto &var : exp.ps->Functions)
	{
		if(var.second.isExpressionObject())
			if (var.second.getParserSettingsFromExpressionObject() == this->ps)
				throw RecursionException("It is about to be a recursive function " + name + "(" + this->ps->x_alias + ")! Expression has reference to this object", name, this, var.second.getExpPtr());
	}

	std::shared_ptr<Expression> internal_exp = std::shared_ptr<Expression>(new Expression);

	*internal_exp = Expression(exp.getExpression(), false, exp.ps); 

	ps->Functions[name] = internal_exp;
	setExpression(origin_string);
}

void Oper::addFunction(std::string name, std::string function_str)
{
	Expression func(function_str);
	func.setFunctions(&this->getFunctions()); //Create new exp object with same of this parameters 
	func.setConstants(&this->getConstants());
	addFunction(name, func);
}

void Oper::deleteFunction(std::string name, bool isThrow)
{
	auto it = ps->Functions.find(name);
	if (it == ps->Functions.end() && isThrow)
		throw IncorrectFunctionName("There is no function to delete with name: " + name, name);

	ps->Functions.erase(it);
	setExpression(origin_string);
}

void Oper::resetConstants(bool addDefault)
{
	ps->InitializeConstants(nullptr, addDefault);

	setExpression(origin_string);
}

void Oper::resetFunctions(bool addDefault)
{
	ps->InitializeFunctions(nullptr, addDefault);

	setExpression(origin_string);
}

void Oper::setParserSettings(ParserSettings & se)
{
	ps->Constants = se.Constants;
	ps->Functions = se.Functions;
	setExpression(origin_string);
}

Oper::~Oper()
{
	clearSubOpers();
}
//--------------------------------



//Expression defenitions----
Expression::Expression(std::string value, bool toBePrepared, std::shared_ptr<ParserSettings> ps) : Oper(value, ps)
{ 	
	origin_string = value;
	checkBracketsCorrect();

	if (toBePrepared)
		prepareString();

	updateSubOpers();
}

Expression::Expression(Expression & exp)
{
	*this = Expression(exp.getExpression(), false, exp.ps);
}

double Expression::getResult(double x)
{
	double result = 0;
	int counter = 0;
	int k = 1;


	for (auto& var : sub_opers)
	{
		if (actions[counter] == '+')
			k = 1;
		else
			k = -1;

		result += var->getResult(x) * k;
		counter++;
	}

	

	return result;
}

void Expression::setExpression(std::string str)
{
	origin_string = str;
	this->value = str;
	checkBracketsCorrect();
	prepareString();
	clearSubOpers();
	updateSubOpers();
}

void Expression::operator=(const std::string& exp)
{
	setExpression(exp);
}

std::istream& operator>>(std::istream& in, Expression& exp)
{
    std::string inputed;
    std::getline(in, inputed);
    exp.setExpression(inputed);
    return in;
}

std::ostream& operator<<(std::ostream& out, smp::Expression& exp)
{
    return out << exp.getExpression();
}

double Expression::operator()(double value)
{
    return getResult(value);
}

double Expression::operator()()
{
    return getResult(x_value);
}

std::string Expression::operator+(const Oper & obj) const
{
	
	return this->getOriginalExpression() + " + " + obj.getOriginalExpression();
}

std::string Expression::operator-(const Oper & obj) const
{
	return this->getOriginalExpression() + " - " + obj.getOriginalExpression();
}

std::string Expression::operator*(const Oper & obj) const
{
	return "(" + this->getOriginalExpression() + ")(" + obj.getOriginalExpression() + ")";
}

std::string Expression::operator/(const Oper & obj) const
{
	return "(" + this->getOriginalExpression() + ")/(" + obj.getOriginalExpression() + ")";
}

std::string Expression::operator^(const Oper & obj) const
{
	return "(" + this->getOriginalExpression() + ")^(" + obj.getOriginalExpression() + ")";
}


void Expression::updateSubOpers()
{
	actions.clear();

	int i = 0;
	if (value[i] != '-')
		actions.push_back('+');

	while (true)
	{
		while (value[i] == '-' || value[i] == '+')
		{
			actions.push_back(value[i]);
			i++;
		}

		if (i >= value.size())
			break;

		int j = 0;
		std::string new_string = "";
		bool isBracketOpened = false;
		int openedBrackets = 0;
		for (i; i < value.size(); i++, j++)
		{
			if ((value[i] == '+' || value[i] == '-') && !isBracketOpened)
				break;

			if (value[i] == '(')
			{
				isBracketOpened = true;
				openedBrackets++;
			}
			else if (value[i] == ')')
			{
				openedBrackets--;
				isBracketOpened = openedBrackets != 0;
			}

			new_string.push_back(value[i]);
		}

		sub_opers.push_back(std::shared_ptr<Oper>(new Multiplication_Oper(new_string, ps)));
	}
}

//------------------------



//Multiplication_Oper defenitions------------------------------------------
Multiplication_Oper::Multiplication_Oper(std::string value, std::shared_ptr<ParserSettings> ps) : Oper(value, ps) { setExpression(value); }

double Multiplication_Oper::getResult(double x)
{
	double result = 1;
	for (int i = 0; i < sub_opers.size(); i++)
	{
		if (actions[i] == '*')
			result *= sub_opers[i]->getResult(x);
		else
		{
			double to_divide = sub_opers[i]->getResult(x);
			if (to_divide == 0)
				throw MathError("There is dividing by zero! Expression isn`t correct!");

			result /= to_divide;
		}
	}

	return result;
}

void Multiplication_Oper::setExpression(std::string str)
{
	this->value = str;
	clearSubOpers();
	updateSubOpers();
}

void Multiplication_Oper::updateSubOpers()
{
	actions.clear();
	actions.push_back('*');

	int i = 0;
	while (true)
	{
		int j = 0;
		std::string new_string = "";

		while (i < value.size() && value[i] == '*' || value[i] == '/' || value[i] == ')') //Filling the actions map
		{
			if (value[i] == '*' || value[i] == '/')
				actions.push_back(value[i]);

			i++;
		}


		if (i >= value.size())
			break;

		//An expression
		if (value[i] == '(')
		{
			int isComplexPowerMode = 0; //0 - Not defined, 1 - Complex, 2 - Not complex
			int opened_count = 0;
			for (i; i < value.size(); i++, j++)
			{
				if (value[i] == ')')
				{
					opened_count--;
					if (i < value.size() - 2)
					{
						if (value[i + 1] == '^' && opened_count == 0)
						{
							if (value[i + 2] == '(')
								isComplexPowerMode = 1;
							else
								isComplexPowerMode = 2;
						}

						else if (opened_count == 0)
						{
							if (isComplexPowerMode == 0)
							{
								new_string.erase(0, 1); 
								sub_opers.push_back(std::shared_ptr<Oper>(new Expression(new_string, false, ps)));
								break;
							}

							else
							{
								new_string.push_back(value[i]);
								sub_opers.push_back(std::shared_ptr<Oper>(new Power_Oper(new_string, ps)));
								break;
							}

						}
					}

					else if (opened_count == 0)
					{
						if (isComplexPowerMode == 0)
						{
							new_string.erase(0, 1);
							sub_opers.push_back(std::shared_ptr<Oper>(new Expression(new_string, false, ps)));
							break;
						}

						else
						{
							new_string.push_back(value[i]);
							sub_opers.push_back(std::shared_ptr<Oper>(new Power_Oper(new_string, ps)));
							break;
						}
					}
				}

				if (value[i] == '(')
				{
					opened_count++;
				}

				if (isComplexPowerMode == 2)
				{
					if ((value[i] == '*' || value[i] == '/' || isLetter(value[i])) && i < value.size() - 1 && value[i + 1] != '^')
					{
						sub_opers.push_back(std::shared_ptr<Oper>(new Power_Oper(new_string, ps)));
						break;
					}
				}

				new_string.push_back(value[i]);
			}

			if (i > value.size() - 1)
			{
				if (isComplexPowerMode == 0)
					sub_opers.push_back(std::shared_ptr<Oper>(new Expression(new_string, false, ps)));
				else
					sub_opers.push_back(std::shared_ptr<Oper>(new Power_Oper(new_string, ps)));
			}

		}

		//A function
		else if (value[i] == '{')
		{
			int opened_count = 0;
			for (i; i < value.size(); i++)
			{
				if (value[i] == '(')
					opened_count++;
				else if (value[i] == ')')
					opened_count--;

				new_string.push_back(value[i]);
			}

			sub_opers.push_back(std::shared_ptr<Oper>(new Function_Oper(new_string, ps)));
		}

		//A number
		else if (!isLetter(value[i]))
		{
			int isComplexPowerMode = 0; //0 - Not defined, 1 - Complex, 2 - Not complex
			int opened_count = 0;
			for (i; i < value.size(); i++, j++)
			{
				if (isComplexPowerMode == 0)
				{
					if (value[i] == '(' || isLetter(value[i]) || value[i] == '*' || value[i] == '/')
					{
						sub_opers.push_back(std::shared_ptr<Oper>(new Number(new_string, ps)));
						break;
					}
				}

				if (i < value.size() - 2)
				{
					if (value[i + 1] == '^')
					{
						if (value[i + 2] == '(')
							isComplexPowerMode = 1;
						else
							isComplexPowerMode = 2;
					}
				}

				if (isComplexPowerMode == 1)
				{
					if (value[i] == ')')
					{
						opened_count--;
						if (i < value.size() - 2)
						{
							if (value[i + 1] == '^' && opened_count == 0)
							{
								if (value[i + 2] == '(')
									isComplexPowerMode = 1;
								else
									isComplexPowerMode = 2;
							}

							else if(opened_count == 0)
							{
								new_string.push_back(value[i]);
								sub_opers.push_back(std::shared_ptr<Oper>(new Power_Oper(new_string, ps)));
								break;
							}
						}

						else if (opened_count == 0)
						{
							new_string.push_back(value[i]);
							sub_opers.push_back(std::shared_ptr<Oper>(new Power_Oper(new_string, ps)));
							break;
						}
					}
				}

				if (value[i] == '(')
				{
					opened_count++;
				}

				if (isComplexPowerMode == 2)
				{
					if ((value[i] == '*' || value[i] == '/' || isLetter(value[i])) && i < value.size() - 1 && value[i + 1] != '^')
					{
						if (isLetter(value[i]))
						{
							new_string.push_back(value[i]);
							i++;
						}

						new_string.push_back(value[i]);
						sub_opers.push_back(std::shared_ptr<Oper>(new Power_Oper(new_string, ps)));
						break;
					}
				}
				new_string.push_back(value[i]);
			}


			if (i > value.size() - 1)
			{
				if(isComplexPowerMode == 0)
					sub_opers.push_back(std::shared_ptr<Oper>(new Number(new_string, ps)));
				else 
					sub_opers.push_back(std::shared_ptr<Oper>(new Power_Oper(new_string, ps)));
			}
		}

		//x in n power
		else if (i < value.size() - 2 && value[i + 1] == '^')
		{
			int isComplexPowerMode; //0 - Not defined, 1 - Complex, 2 - Not complex
			int opened_count = 0;

			if (value[i + 2] == '(')
				isComplexPowerMode = 1;
			else
				isComplexPowerMode = 2;

			new_string.push_back(value[i]);
			i++;

			for (i; i < value.size(); i++, j++)
			{
				if (isComplexPowerMode == 1)
				{
					if (value[i] == ')')
					{
						opened_count--;
						if (i < value.size() - 2)
						{
							if (value[i + 1] == '^' && opened_count == 0)
							{
								if (value[i + 2] == '(')
									isComplexPowerMode = 1;
								else
									isComplexPowerMode = 2;
							}

							else if (opened_count == 0)
							{
								new_string.push_back(value[i]);
								sub_opers.push_back(std::shared_ptr<Oper>(new Power_Oper(new_string, ps)));
								break;
							}
						}

						else if (opened_count == 0)
						{
							new_string.push_back(value[i]);
							sub_opers.push_back(std::shared_ptr<Oper>(new Power_Oper(new_string, ps)));
							break;
						}
					}
				}

				if (value[i] == '(')
				{
					opened_count++;
				}

				if (isComplexPowerMode == 2)
				{
					if (value[i] == '^')
					{
						if (i < value.size() - 1 && value[i + 1] == '(')
							isComplexPowerMode = 1;
					}

					else
					{
						if ((value[i] == '*' || value[i] == '/' || isLetter(value[i])) && i < value.size() - 1 && value[i + 1] != '^')
						{
							if (isLetter(value[i]))
							{
								new_string.push_back(value[i]);
								i++;
							}
							
							sub_opers.push_back(std::shared_ptr<Oper>(new Power_Oper(new_string, ps)));
							
							break;
						}
					}
				}

				new_string.push_back(value[i]);
			}

			if (i > value.size() - 1)
				sub_opers.push_back(std::shared_ptr<Oper>(new Power_Oper(new_string, ps)));
		}

		//Just x
		else
		{
			new_string.push_back(value[i]);
			sub_opers.push_back(std::shared_ptr<Oper>(new Number(new_string, ps)));
			i++;
		}
	}
}
//------------------------


//Power_Oper defenitions------------------------------------------
Power_Oper::Power_Oper(std::string value, std::shared_ptr<ParserSettings> ps) : Oper(value, ps) { setExpression(value); }

double Power_Oper::getResult(double x)
{
	double base = sub_opers.at(0)->getResult(x);
	double power = sub_opers.at(1)->getResult(x);
	double result = pow(base, power);

	if (std::isnan(result))
	{
		double cel;
		if (modf(1 / power, &cel) != 0)
			throw MathError("Incorrect Exponentiation! Base: " + std::to_string(base) + " | Power: " + std::to_string(power));

		if((int)(1 / power) % 2 == 0)
			throw MathError("Incorrect Exponentiation! Base: " + std::to_string(base) + " | Power: " + std::to_string(power));

		result = -pow(-base, power);
	}
	
	return result;
}

void Power_Oper::setExpression(std::string str)
{
	this->value = str;
	clearSubOpers();
	updateSubOpers();
}

void Power_Oper::updateSubOpers()
{
	int i = 0;
	std::string power_base = "";
	std::string power_factor = "";

	//Base of power
	{
		if (value[0] == '(')
		{
			int opened_count = 0;
			for (i; i < value.size(); i++)
			{
				if (value[i] == '(')
					opened_count++;
				else if (value[i] == ')')
					opened_count--;

				power_base.push_back(value[i]);

				if (opened_count == 0)
					break;
			}
			i += 2;

			sub_opers.push_back(std::shared_ptr<Oper>(new Expression(power_base, false, ps)));
		}

		else
		{
			for (i; i < value.size(); i++)
			{
				if (value[i] == '^')
					break;

				power_base.push_back(value[i]);
			}
			i++;

			sub_opers.push_back(std::shared_ptr<Oper>(new Number(power_base, ps)));
		}
	}

	//Factor of power
	for (i; i < value.size(); i++)
	{
		power_factor.push_back(value[i]);
	}

	sub_opers.push_back(std::shared_ptr<Oper>(new Expression(power_factor, false, ps)));
}
//------------------------



//Number definitions------
Number::Number(std::string value, std::shared_ptr<ParserSettings> ps) : Oper(value, ps) {}

double Number::getResult(double x)
{
	if (value.size() != 0)
	{
		if (isLetter(value[0]))
			return ps->getNumberFromLetter(value[0], x);
		else
		{
			try
			{
				return std::stod(value);
			}

			catch (std::invalid_argument&)
			{
				throw ConversionError("stod() couldn`t convert " + value + " to double. Expression isn`t correct!", value);
			}
		}
			
	}

	else
	{
		throw IncorrectSyntax("String was empty. Expression isn`t correct!");
	}
}

void Number::setExpression(std::string str)
{
	this->value = str;
}
//-------------------------




//Function definitions
Function_Oper::Function_Oper(std::string value, std::shared_ptr<ParserSettings> ps) : Oper(value, ps) { setExpression(value); }

void Function_Oper::setExpression(std::string str)
{
	this->value = str;
	clearSubOpers();
	updateSubOpers();
}

double Function_Oper::getResult(double x) //You can add your functions there (Check documentation)
{
	std::string func_string;
	for (auto & var : ps->Functions) //Definding the the type of function
	{
		if (value.find(var.first, 0) != std::string::npos)
		{
			func_string = var.first;
			break;
		}
			
	}

	return ps->getFunctionValue(func_string, sub_opers[0]->getResult(x));
}

void Function_Oper::updateSubOpers()
{
	std::string argument;
	int i = 0;

	while (value[i] != '(' && i < value.size())
		i++;

	i++; //We don`t read '('

	for (i; i < value.size() - 1; i++)
		argument.push_back(value[i]);

	sub_opers.push_back(std::shared_ptr<Oper>(new Expression(argument, false, ps)));
}

bool MapFunctionCompare::operator()(const std::string & lhs, const std::string & rhs) const
{
	return lhs.length() > rhs.length() || (lhs.length() == rhs.length() && lhs < rhs);
}

std::string FunctionWrapper::getFunctionNotation()
{
	if (isExpressionObject())
		return exp_ptr->getOriginalExpression();
	else
		return "Impossible to demonstrate (Implemented via code)";
}

FunctionWrapper::FunctionWrapper(const FunctionWrapper & copy)
{
	if (copy.exp_ptr != nullptr)
		this->exp_ptr = std::shared_ptr<Expression>(new Expression(*copy.exp_ptr));
	else
		this->function_ptr = copy.function_ptr;
}

double FunctionWrapper::getCalculatedValue(double argument)
{
	if (isExpressionObject())
		return exp_ptr->getResult(argument);

	return function_ptr(argument);
}

std::shared_ptr<ParserSettings> FunctionWrapper::getParserSettingsFromExpressionObject()
{
	if (isExpressionObject())
		return exp_ptr->ps;
	else
		return nullptr;
}
