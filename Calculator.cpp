#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <set>

// Calculator program
// By Hunwoo Oh

using namespace std;

// constants for controlling the program
const char number = '8'; // we use '8' to represent a number
const char quit = 'Q';
const char space = ' ';
const char print = ';';
const char assign = '=';
const char name = 'a';
const char root = 'S';
const char pows = 'P';
const char help = 'H';
const char let = 'L';
const char new_line = '\n';

const string declkey = "let";
const string prompt = "> ";
const string result = "= ";

const double PI = 3.14592653589793238463;
const double K = 1000;


// error() simply disguises throws:
inline void error(const string& s) {
		throw runtime_error(s);
}

class Variable {
public:
		string name;
		double value;
		Variable(string var, double val);
};

class Symbol_table {
public:
		double get(string s);
		void set(string s, double d);
		bool is_declared(string var);
		double define_name(string var, double val);

private:
		vector<Variable> var_table;
};

class Token {
public:
		char kind;
		double value;
		string name;
		Token(char c)
				:kind(c), value(0) {}
		Token(char c, double val)
				:kind(c), value(val) {}
		Token(char c, string s)
				:kind(c), name(s) {}
};


class Token_stream {
public:
		Token_stream();
		Token get();
		void putback(Token t);
		void ignore(char c);

private:
		bool is_full;
		Token buffer;
};

Variable::Variable(string var, double val)

		:name(var), value(val) {}

Token_stream::Token_stream()

		:is_full(false), buffer(0) {}


void Token_stream::putback(Token t) {

		if(is_full) error("already full");
		buffer = t;
		is_full = true;
}

void Token_stream::ignore(char c) {
		// c represents the kind of Token
		if (is_full && c==buffer.kind) { // check buffer
				is_full = false;
				return;
		}
		is_full = false;

		// search input
		char ch = 0;
		while(cin >> ch)
				if(ch == c) return;

}

Token Token_stream::get() {
		// read characters from cin and compose a Token
		if(is_full) {
				is_full = false;
				return buffer;
		}

		char ch = cin.get();

		switch(ch){
		case print:
		case assign:
		case space:
		case '!':
		case ',':
		case '_':
		case '(': 
		case ')': 
		case '{': 
		case '}': 
		case '+': 
		case '-': 
		case '*': 
		case '/': 
		case '%': 
					if(isspace(ch)) error("no spaces please");
	    		return Token(ch);        // let each character represent itself
	  case '.':
	  case '0': case '1': case '2': case '3': case '4':
	  case '5': case '6': case '7': case '8': case '9':
	  {    
		      cin.putback(ch);         // put digit back into the input stream
		      double val;
		      cin >> val;              // read a floating-point number
		      return Token(number,val);
	  }
	  case new_line:
	  			return Token(print);
	  default:
	  			if(isalpha(ch)) {
	  					string s;
	  					s += ch;
	  					while(cin.get(ch) && (isalpha(ch) || isdigit(ch))) s+=ch;

	  					cin.putback(ch);
	  					if(s == declkey) return Token(let);
	  					if(s == "sqrt") return Token(root);
	  					if(s == "pow") return Token(pows);
	  					if(s == "exit") return Token(quit);
	  					if(s == "help") return Token(help);
	  					return Token(name, s);
  				}
	     		error("Bad token");
		}
		return Token('0', 0);
}

double Symbol_table::get(string s) {

		for(int i = 0; i < var_table.size(); i++) 
				if(var_table[i].name == s) return var_table[i].value;
		error("get: undefined variable ");
		return 0;
}

void Symbol_table::set(string s, double d) {

		for(int i = 0; i < var_table.size(); i++)
				if(var_table[i].name == s) {
						var_table[i].value = d;
						return;
				}
		error("set: undefined variable ");
}

bool Symbol_table::is_declared(string var) {
		// is var already in the table?
		for(int i = 0; i < var_table.size(); i++)
				if(var_table[i].name == var) return true;
		return false;
}

double Symbol_table::define_name(string var, double val) {
		// add var to var_table
		if(is_declared(var)) error("var declared twice");
		var_table.push_back(Variable(var, val));
		return val;
}

double factorial(int num) {

	if(num < 0) error("only non-negative factorials are allowed");
	if(num < 2) 
		return 1.0;
	else
		return (double)num * factorial(num - 1);

}

Token_stream ts;
Symbol_table st;

double expression();

double primary() {

		Token t = ts.get();

		while(true){
				switch(t.kind){
				case '(':
				{
							double d = expression();
							t = ts.get();
							if(t.kind != ')') error("')' expected");
							return d;
				}
				case '{':
				{
							double d = expression();
							t = ts.get();
							if(t.kind != '}') error("'}' expected");
							return d;
				}
				case root: 
				{
							double d = expression();
							if(d<0) error("can't be negative");
							else {
									double final = sqrt(d);
									return final;
							}
				}
				case pows:
				{
							t = ts.get();
							if(t.kind != '(') error("'(' expected");
							double d = expression();
							t = ts.get();
							if(t.kind != ',') error("',' expected");
							double i = expression();
							t = ts.get();
							if(t.kind != ')') error("')' expected");
							return pow(d, i);
				}
				// number is '8'
				case number:
				{
							double d = t.value;
							t = ts.get(); // check '!' for factorial
							if(t.kind == '!')
								return factorial(int(d));

							ts.putback(t); // not '!'
							return d;
				}
				// name is 'a'
				case name:
				{			
							Token next = ts.get();
							if(next.kind == assign) {
								double d = expression();
								st.set(t.name, d);
								return d;
							} else {
								ts.putback(next);
								return st.get(t.name);
							}
				}
				case '+':
							return primary();
				case '-':
							return - primary();
				default:
							error("primary expected");
				}
		}
}


double term() {

		double left = primary();
		Token t = ts.get();

		while(true)
		{
				switch(t.kind){
				case '*':
							left *= primary();
							t = ts.get();
							break;
				case '/':
				{
							double d = primary();
							if(d == 0) error("divide by zero");
							left /= d;
							t = ts.get();
							break;
				}
				case '%':
				{
							double d = primary();
							if(d == 0) error("divide by zero");
							left = fmod(left, d);
							t = ts.get();
							break;
				}
				default:
							ts.putback(t);
							return left;
				}
		}
}

double expression() {

		double left = term();
		Token t = ts.get();

		while(true)
		{
				switch(t.kind){
				case '+':
							left += term();
							t = ts.get();
							break;
				case '-':
							left -= term();
							t = ts.get();
							break;
				default:
							ts.putback(t);
							return left;
				}
		}
}

double declaration() {
		// "let" Name = Expression
		cin.ignore(256, space); // ignore whitespaces after 'let'
		Token t = ts.get();
		if(t.kind != name) error ("name expected in declaration");
		string var_name = t.name;

		Token t2 = ts.get();
		
		if(t2.kind != assign) error("= missing declaration");

		double d = expression();
		if(st.is_declared(var_name)) {
				st.set(var_name, d);
		} else {
				st.define_name(var_name, d);
		}
		return d;
}

double statement() {
	Token t = ts.get();
	switch(t.kind) {
	case let:
				return declaration();
	default:
				ts.putback(t);
				return expression();
	}
}

void clean_up_mess() {

		ts.ignore(print);
}

void calculate() {

		while(cin) {
			try {
				cout << prompt;
				Token t = ts.get();
				// help				
				if(t.kind == help) {
					cout << "\nYou are allowed to use +, -, *, /, %, !, sqrt(), pow(), variables.\n";
					cout << "\nPress the 'Enter' key once you've entered a statement.\n";
					cout << "\npow() is used in the following syntax: pow(number, power).\n";
					cout << "\nDeclare variables with the following syntax: let <variable_name>=<number>.\n\n";
					continue;
				}
				// print = ';'
				while(t.kind == print) t = ts.get();
				// quit = 'Q'
				if(t.kind == quit) return;
				ts.putback(t);
				cout << result << statement() << '\n';
			} catch (exception& e) {
				cerr << e.what() << '\n';
				clean_up_mess();
			}
		}
}

void error_exit() {

		cout << "Please enter 'x' to exit\n";
		for(char ch; cin>>ch;)
			if(ch == 'x') return;
}


int main() {

		try {
				st.define_name("PI", PI);
				st.define_name("k", K);
				cout << "Welcome to my simple calculator.\n"
						<< "Please enter expressions using floating-point numbers.\n"
						<< "Type 'help' for further instructions\n"
						<< "Type 'exit' to quit the program\n";

				calculate();

				return 0;

		} catch (runtime_error& e) {
				cerr << e.what() << endl;
				error_exit();
				return 1;

		} catch (...) {
				cerr << "Unknown exception!\n";
				return 2;
		}
}