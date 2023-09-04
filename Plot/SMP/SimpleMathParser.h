#pragma once
#include <vector>
#include <string>
#include <map>
#include <vector>
#include <math.h>
#include <memory>
#include <istream>


//Created by grub_666 (Alexander Furmanov)
//Check README file on github: https://github.com/GRUB666/SimpleMathParser
//My contact email: aleksandrfurmanoa@gmail.com

/*Structure:
1. Definitions of default constants (PI and E)
2. Strcut ParserSettings
3. Abstract class Oper
4. Interface IActionsMap
5. Class Expression (To use)
6. Class Multiplication_Oper (Service class)
7. Class Number_Oper (Service class)
8. Class Power_Oper (Service class)
9. Class Function_Oper (Service class)
*/

namespace smp //Simple Math Parser namespace
{
	constexpr auto PI = 3.141592653589793; //Just constants;
	constexpr auto E = 2.718281828459045;

	struct FunctionWrapper;
	class Expression;

	using Function = double(*)(double argument);

	struct MapFunctionCompare
	{
		bool operator()(const std::string &lhs, const std::string &rhs) const;
	};

	typedef std::map<std::string, FunctionWrapper, MapFunctionCompare> FunctionsMap;
	typedef std::map<char, double> ConstantsMap;

	struct ParserSettings
	{
		ConstantsMap Constants;
		FunctionsMap Functions;
		char x_alias = 'x';

		ParserSettings();

		void InitializeConstants(std::map<char, double> *consts = nullptr, bool addConstants = true);
		void InitializeFunctions(FunctionsMap *funcs = nullptr, bool addFunctions = true);

		double getFunctionValue(std::string name, double argument);
		double getNumberFromLetter(char symb, double x_value);
	};

	class Oper //Base abstract class
	{
	private:
		void CutUnnecessary();
		void FunctionsMarker();
		
	protected:
		std::vector<std::shared_ptr<Oper>> sub_opers;
		std::string value;
		std::shared_ptr<ParserSettings> ps;
		double x_value;
		std::string origin_string;

		Oper(std::string value = "", std::shared_ptr<ParserSettings> ps = nullptr);
		void prepareString();
		void clearSubOpers();
		void checkBracketsCorrect();
		void replaceIncorrectSymbols();
		static bool isLetter(char symb);
		static bool isDigit(char symb);
		static bool isServiceSymbol(char symb);

		virtual ~Oper();
		virtual void updateSubOpers() = 0;

	public:
		virtual double getResult(double x) = 0;
		virtual double getResult() { return 1; }
		virtual void setExpression(std::string str = "") = 0;

		std::string getExpression() const { return this->value; } //returns pasred string
		std::string getOriginalExpression() const { return this->origin_string; } //returns inputed string
		void setXValue(double x) { x_value = x; } //set current default x value
		double getXValue() const { return x_value; } //returns current default x value
		void setConstants(ConstantsMap *consts = nullptr, bool addConstants = true); //set Constants with ConstantsMap
        ConstantsMap& getConstants() const; //Returns ConstantsMap
		void addConstant(char symb, double value); //Adds a current constant
		void deleteConstant(char name, bool isThrow = false); //Delete a current constants
		void resetConstants(bool addDefault = true); //Reset all constants (if true - it adds default constants)

		void setFunctions(FunctionsMap *funcs = nullptr, bool addFunctions = true); //Set functions with functions Map
        FunctionsMap& getFunctions() const; //Returns FunctionsMap
		static void checkFunctionNameCorrectness(std::string func_name); //Check if function name is not correct
		void addFunction(std::string name, Function function); //Adds a current function (with function pointer)
		void addFunction(std::string name, Expression &exp); //Add function with Expression instance
		void addFunction(std::string name, std::string function_str); //Add function with string of expression
		void deleteFunction(std::string name, bool isThrow = false); //It throws exception if isThrow = true and there is no target function
		void resetFunctions(bool addDefault = true); //Reset functions (default or not)
		void setParserSettings(ParserSettings& se); 
	    void setNewXAlias(char symb); //Set alias of variable (as default - x)
		char getXAlias() const; //Returns current variable alias
	};


	class IActionsMap //Map of mathematical actions (multipling, dividing e.t.c)
	{
	protected:
		std::vector<char> actions;

		virtual ~IActionsMap() {};
	};


	//Main class. To be used in external space
	class Expression : public Oper, public IActionsMap
	{
	private:
		void updateSubOpers() override;
	public:
		friend FunctionWrapper;

		explicit Expression(std::string value = "", bool toBePrepared = true, std::shared_ptr<ParserSettings> ps = nullptr);
		Expression(Expression& exp);

		double getResult(double x) override;
		double getResult() override { return getResult(x_value); }
		void setExpression(std::string str = "") override;

        void operator=(const std::string& exp);
        double operator()(double value);
        double operator()();

		std::string operator+(const Oper& obj) const;
		std::string operator-(const Oper& obj) const;
		std::string operator*(const Oper& obj) const;
		std::string operator/(const Oper& obj) const;
		std::string operator^(const Oper& obj) const;
	};

	struct FunctionWrapper
	{
	private:
		Function function_ptr;
		std::shared_ptr<Expression> exp_ptr;

	public:

		FunctionWrapper(Function func) : function_ptr(func), exp_ptr(nullptr) {}

		FunctionWrapper() {}

		FunctionWrapper(std::shared_ptr<Expression> ptr) : exp_ptr(ptr) {}

		std::string getFunctionNotation();

		FunctionWrapper(const FunctionWrapper& copy);

		double getCalculatedValue(double argument);

		bool isExpressionObject() { return exp_ptr != nullptr; }

		std::shared_ptr<ParserSettings> getParserSettingsFromExpressionObject();

		Expression* getExpPtr() { return exp_ptr.get(); }
	};
	



	class Multiplication_Oper : public Oper, public IActionsMap
	{
	private:
		void updateSubOpers() override;
	public:
		Multiplication_Oper(std::string value, std::shared_ptr<ParserSettings> ps);

		double getResult(double x) override;
		void setExpression(std::string str) override;

		~Multiplication_Oper() { };
	};



	class Number : public Oper 
	{
	private:
		void updateSubOpers() override {}
	public:
		Number(std::string value, std::shared_ptr<ParserSettings> ps);

		double getResult(double x) override;
		void setExpression(std::string str) override;

		~Number() { };
	};



	class Power_Oper : public Oper
	{
	private:
		void updateSubOpers() override;
	public:
		Power_Oper(std::string value, std::shared_ptr<ParserSettings> ps);

		double getResult(double x) override;
		void setExpression(std::string str) override;
	};


	class Function_Oper : public Oper
	{
	private:
		void updateSubOpers() override;
	public:
		Function_Oper(std::string value, std::shared_ptr<ParserSettings> ps);

		double getResult(double x) override;
		void setExpression(std::string str) override;
	};
}

std::istream& operator>>(std::istream& in, smp::Expression& exp);
std::ostream& operator<<(std::ostream& out, smp::Expression& exp);
