/*
 * Title: Lab3(Challenge) - expression.cpp
 * Purpose: An Expression class that can store an expression both as infix and postfix.
 * The class also has a conversion function from infix to postfix as well as a postfix expression evaluator
 * Author: J. Chris Marek (.h file provided by Dr. Arias)
 * Date: May 15, 2025
 */
#include "expression.h"
#include <stack>
#include <sstream>
using namespace std;

/**
 * Constructor
 * initializes both infix and postfix data members to empty strings
 */
Expression::Expression() {
    _infix = "";
    _postfix = "";
}
/**
 * Constructor
 * initializes the infix data member to the parameter infix
 * initializes the postfix data member to an empty string
 * @param infix the infix expression to be stored in the infix data member
 */
Expression::Expression(const string& infix) {
    _infix = infix;
    _postfix = "";
}

/**
 * Precedence
 * Tests the operational precedence of two operators against each other.
 * @param op1 first operator to be compared
 * @param op2 second operator to be compared
 * @return boolean returns true if operator 1's precedence is equal or greater to operator 2's
 */
bool Precedence(char op1, char op2) {
    int op1Pre; //operator 1's Precedence value
    int op2Pre; //operator 2's Precedence value
    if (op1 == '*' || op1 == '/') {
        op1Pre = 2;
    } else {
        op1Pre = 1;
    }
    if (op2 == '*' || op2 == '/') {
        op2Pre = 2;
    } else {
        op2Pre = 1;
    }
    if (op1Pre >= op2Pre) { //compares the precedence of the two operators
        return true;
    }
    return false;
}

/**
 * Convert to Postfix
 * takes the infix data member of the expression and changes it to a postfix expression stored in the postfix data member
 */
void Expression::ConvertToPostfix() {
    stack<char> stack;
    string infix = trim(_infix);
    for(size_t i = 0; i < infix.length(); i++) {
        char sym = infix[i];
        if (sym == '(') { //pushes open parenthesis to the stack
            stack.push(sym);
        }
        else if (sym == ')') { // if sym is a close parenthesis it appends (to postfix) and pops all items on the stack between the close parenthesis and the open one then pops the open one
            while (!stack.empty() && stack.top() != '(') {
                _postfix += stack.top();
                _postfix += " ";
                stack.pop();
            }
            if (!stack.empty() && stack.top() == '(') {
                stack.pop();
            }
        }
        else if (sym == '+' || sym == '-' || sym == '*' || sym == '/' || sym == '^') { // if sym is an operator it appends all operators with lower precedence to postfix and pops them
            while (!stack.empty() && stack.top() != '(' && Precedence(stack.top(), sym)) {
                _postfix += stack.top();
                _postfix += " ";
                stack.pop();
            }
            stack.push(sym);
        }
        else if (isalnum(sym)) { // if sym is a letter or number it appends all alphanumeric characters before the next non-alphanumeric character to postfix
            _postfix += sym;
            if (i+1 < infix.length() && isalnum(infix[i+1])) {
                int j = 1;
                while (i+j < infix.length() && isalnum(infix[i+j])) {
                    _postfix += infix[i+j];
                    j++;
                }
                i += j - 1; //shifts the index forward by the number of characters appended -1
            }
            _postfix += " ";
        }
    }
    while (!stack.empty()) { //appends to postfix any operators that are still on the stack
        _postfix += stack.top();
        if (stack.size() > 1) {
            _postfix += " ";
        }
        stack.pop();
    }
}

/**
 * Get Infix
 * a function to return the value of the private data member infix
 * @return the expression stored in the infix data member is returned as a string
 */
string Expression::GetInfix()const {
    return _infix;
}
/**
 * Get Postfix
 * a function to return the value of the private data member postfix
 * @return the expression stored in the postfix data member is returned as a string
 */
string Expression::GetPostfix()const {
    return _postfix;
}

/**
 * To JSON
 * provides a string in JSON format that contains the infix and postfix versions of the expression
 * @return a string with the JSON expression of both the infix and postfix expressions
 */
string Expression::ToJSON()const {
    return "{\"infix\":\"" + _infix + "\", ""\"postfix\":\"" + _postfix + "\"}";
}
/**
 * Is Valid Double
 * checks if a string is a valid value for the double class containing only numbers or a single period
 * @param val is a string to be checked for conversion into a double object
 * @return boolean value for if the string is a valuable double. Returns true if the value is a valid double false otherwise
 */
bool IsValidDouble(const string& val){
    bool retVal = false;
    bool secondDot = false;
    for(size_t i = 0; i < val.length(); i++){ //checks each character of val if it is a number, period or other character
        if(isdigit(val[i])){
            retVal = true;
        }
        else if(val[i] == '.'){
            retVal = true;
            if(secondDot){ // makes sure there is no second period in the string
                retVal = false;
            }
            secondDot = true;
        } else {
            return false;
        }
    }
    return retVal;
}
/**
 * Evaluate
 * Evaluates the postfix expression from the postfix data member if possible
 * if the expression is unable to be evaluated the function sets the error variable to true and return -1
 * @param error initialized to false, becomes true if an expression is unable to be evaluated
 * @return returns a double with the value of the evaluated expression or -1 if unable to evaluate
 */
double Expression::Evaluate(bool &error) const {
  stack<double> vals;
  error = false;
  string postfix = _postfix;
  istringstream iss(postfix); // turns the postfix parameter into tokens to remove the white spaces
  string splits;
  while(iss >> splits) { // while loop that runs as long as there are still tokens to process
    if(IsValidDouble(splits)){ // checks if the tokens are valid doubles
      vals.push(stod(splits)); //turns valid tokens into doubles
    }
    else if (splits == "+" || splits == "-" || splits == "*" || splits == "/"){ //checks if the token is an operator
      if(vals.size() < 2){ //checks that there are a minimum of 2 values on the stack so a and b do not go out of bounds
        error = true;
        return -1;
      }
      double b = vals.top();
      vals.pop();
      double a = vals.top();
      vals.pop();
      if(splits == "+"){ // separates the operators and completes them individually
        vals.push(a + b);
      }
      else if(splits == "-"){
        vals.push(a - b);
      }
      else if(splits == "*"){
        vals.push(a * b);
      }
      else if(splits == "/"){
        if(b==0){ //prevents division by 0
          error = true;
          return -1;
        }
        vals.push(a / b);
      }
    }
    else { // returns -1 and sets error to true if the token is not an operator or valid double
      error = true;
      return -1;
    }
  }
  if(vals.size() != 1){
    error = true;
    return -1;
  }
  return vals.top();
}


/**
 * Got this from: https://stackoverflow.com/questions/1798112/removing-leading-and-trailing-spaces-from-a-string
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * W A R N I N G        W A R N I N G        W A R N I N G        W A R N I N G    * *
 * Even when I am giving you code that I found on the Internet, you are still      * *
 * not allowed to copy ANY code from the Internet or from any AI tool.             * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * @param str the string that we want to trim from trailing and leading spaces
 * @param whitespace the whitespaces we want to remove, by default it will remove space and tab
 * @return a string without leading or trailing spaces.
 */
string trim(const string& str, const string& whitespace){
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == string::npos)
        return ""; // no content
    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;
    return str.substr(strBegin, strRange);
}


