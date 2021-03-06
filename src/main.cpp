#include <iostream>
#include <queue>
#include <stack>
#include <stdio.h>
#include "base.h"
#include "command.h"
#include "orop.h"
#include "andop.h"
#include "semicolon.h"
#include <vector>
#include <cstring>
#include <string>

#include <algorithm>
#include <sstream>
#include <unistd.h>

using namespace std;

/*
 * Note:
 * So I have the classes available to construct the tree for you
 * Here is the hierarchy for the classes i made
 *
 * class: Base -> subclasses: command and connector
 * class: connector -> subclasses: AndOp (&&), OrOp(||), SemiColon
 *
 * Therefore, all the commands of the input are put into the command object (look at the constructor)
 * Therefore, all the connectors of the input are put into the connector object (look at the constructor too)
 *
 * So your goal: Infix to post
 *
 * 1,2,3,4 -> commands -> i.e. ls -a, echo b, ls,
 *
 * process:
 * 1 && 2 || 3; 4;  //infix
 * 1 && 2 || 3 ; 4 ; //separating the ; connector from the 3 command AND ; at the end to make it easier to process
 * 1 2 && 3 || 4 ; //postfix
 *
 * Afterwards: you tokenize the char string
 * [ [1], [2], [&&], [3], [||], [4], [;] ] //postfix tokenized string
 * make a queue<char*> then use the push(char*) function through each index from left to right
 *(look up the push function on c++ reference)
 *
 * To make the tree
 * check the contents within the index of the iteration
 * if its a command -> construct a command object (look at constructor)
 * if its a connector -> construct a andOP, orOP, or SemiColon object based on what the connector is (look at constructor)
 *
 * DONE!
 *
 */


Base* generateTree(vector<string> v);
Base* exitorCommand(string s);

void readCommandLine(string &);
bool isBalancedParentheses(string &, char);
void separateConnectors(string &);
void addHashtag(string &);
void tokenizeInOrder(string &, queue<string> &);
queue<string> tokenizePostOrder(queue<string>);
bool isOperator(string);
bool isParenthesis(string);


int main () {

    while(1)
    {
        string input;
        queue<string> inOrder;
        queue<string> postOrder;
	vector<string> v;
        Base* head;
        //char **args;
        //int argsSize = 0;
        readCommandLine(input);
 	if(isBalancedParentheses(input,'(')) {
     	
        
		separateConnectors(input);

		tokenizeInOrder(input,inOrder);
		postOrder = tokenizePostOrder(inOrder);
		while(!postOrder.empty())
		{
		     v.push_back(postOrder.front());
		     postOrder.pop();
		}

	
		head = generateTree(v);

		head->execute();
		delete head;
		head =NULL;
	}
    }

    //tokenize(input);
    //args = tokenize_input(input);

    //char *execArgs[] = { "ls", "-a", NULL };
    //execvp(args[0], args);

    return 0;
}

/*
 * Read set of commands from the user. Cut all characters that proceed
 * after the first hashtag, including the hashtag itself. Store rest of
 * commands into the string reference.
 *
 */
void readCommandLine(string& input) {
    cout << "$ ";
    // Read the line of command from user
    getline(cin,input);

    // Truncate Bash comment
    if (input.find("#") != string::npos) {
        input.erase(input.find("#"),input.length());
    }
}

/*
 * Check to to see if the number of left parentheses matches the number of
 * right parentheses. Return an error message if the numbers do not match.
 * Also return an error message if a right parenthesis proceeds before a
 * left parenthesis. Also perform same check for brackets.
 * 
 * e.g. $ (echo a || echo b) && (echo a || echo b) ==> valid output
 * 	$ (echo a || echo b && (echo a || echo b) ==> invalid output
 *	$ )echo a || echo b( && (echo a || echo b) ==> invalid output
 */
bool isBalancedParentheses(string& input, char c) {
    //Iterate through input to keep track of parentheses
    int checkBalance = 0;

  if(c == '('){
    for (unsigned i = 0; i < input.size(); i++) {
        if (input.at(i) == '(') {
            checkBalance++;
        }
        else if (input.at(i) == ')') {
            checkBalance--;
            if (checkBalance < 0) {
                cout << "ERROR: Left parenthesis must come "
                     << "before right parenthesis." << endl;
                return 0;
            }
        }
    }
    if (checkBalance) {
        cout << "ERROR: Number of left and right parentheses "
             << "must be equal." << endl;
        return 0;
    }
    return 1;
  }
  if(c == '['){
    for (unsigned i = 0; i < input.size(); i++) {
        if (input.at(i) == '[') {
            checkBalance++;
        }
        else if (input.at(i) == ']') {
            checkBalance--;
            if (checkBalance < 0) {
                cout << "ERROR: Left bracket must come "
                     << "before right brackets." << endl;
                return 0;
            }
        }
    }
    if (checkBalance) {
        cout << "ERROR: Number of left and right brackets "
             << "must be equal." << endl;
        return 0;
    }
    return 1;

  }

}

/*
 * Read user's string of commands into a vector of substrings. Add hashtag
 * to the left and right of all parentheses and connectors for tokenizing.
 * Combine substrings together back into the input after all parentheses
 * and connectors have surrounding hashtags.
 *
 * e.g. Before: $ (echo a || echo b) && echo c; (echo d || echo e)
 * 	After:  $#(#echo a#||# echo b#)# #&&# echo c#;# #(#echo d#||# echo#)#
 *
 */
void separateConnectors(string& input) {
    //Read input to a stream so split the commands
    stringstream stream(input);
    string splitInput;
    vector<string> vectorSubCMD;

    //Store the commands into a vector
    while(getline(stream,splitInput, ' ')) {
        vectorSubCMD.push_back(splitInput);
    }

    //Add space right before semicolon for all cmd;
    for (unsigned i = 0; i < vectorSubCMD.size(); i++) {
        //Do not include space commands that are size 0
        if(vectorSubCMD.at(i).size() > 0) {
            //Add hashtag if parentheses or semicolon exist
            addHashtag(vectorSubCMD.at(i));
            //Add hashtag if connector && exists
            if (vectorSubCMD.at(i) == "&&") {
                    vectorSubCMD.at(i).pop_back();
                    vectorSubCMD.at(i).pop_back();

                    vectorSubCMD.at(i).push_back('#');
                    vectorSubCMD.at(i).push_back('&');
                    vectorSubCMD.at(i).push_back('&');
                    vectorSubCMD.at(i).push_back('#');
            }
            //Add hashtag if connector || exists
            else if (vectorSubCMD.at(i) == "||") {
                    vectorSubCMD.at(i).pop_back();
                    vectorSubCMD.at(i).pop_back();

                    vectorSubCMD.at(i).push_back('#');
                    vectorSubCMD.at(i).push_back('|');
                    vectorSubCMD.at(i).push_back('|');
                    vectorSubCMD.at(i).push_back('#');
            }
        }
    }

    //Store subcommands back into string reference
    input = "";
    for (unsigned i = 0; i < vectorSubCMD.size(); i++) {

            input += vectorSubCMD.at(i);
            input += " ";
    }
}

/*
 * For subCommand strings containing parentheses or semicolon,
 * surround with hashtags.
 *
 * e.g. Before: $ echo a; (echo b)
 * 	After:  $ echo a#;## #(#echo b#)#
 */
void addHashtag(string& subCommand) {
    for (unsigned i = 0; i < subCommand.size(); i++) {
        if (subCommand.at(i) == '(') {
            subCommand.insert(i,"#");
            subCommand.insert(i+2,"#");
            i += 2;
        }
        else if (subCommand.at(i) == ')') {
            subCommand.insert(i,"#");
            subCommand.insert(i+2,"#");
            i += 2;
        }
        else if (subCommand.at(i) == ';') {
            subCommand.insert(i,"#");
            subCommand.insert(i+2,"#");
            i += 2;
        }
    }
}

/*
 * Tokenize string input into inOrder notation by separating connectors and
 * command arguments. Store inOrder notation into a queue called inOrder.
 *
 */
void tokenizeInOrder(string& input, queue<string>& inOrder) {
    //Read input to a stream so split the commands
    stringstream stream(input);
    string splitInput;

    //Store the commands into the inorder queue
    while (getline(stream,splitInput,'#')) {
        //Exclude whitespaces and empty string from inOrder queue
        if (splitInput != "" && splitInput != " ") {
            inOrder.push(splitInput);
        }
    }
}

/*
 * Read inOrder queue and convert to postOrder notation. Use stacks to keep
 * track of where connectors should be placed.
 *
 */
queue<string> tokenizePostOrder(queue<string> inOrder) {
    //Store connectors into connector stack
    stack<string> connector;
    //Store subcommands into postOrder queue
    queue<string> postOrder;
	
    //Read tokens from inOrder queue one at a time until queue becomes empty
    while (!inOrder.empty()) {
	//Push token to postOrder queue if token is a command argument
        if (!isOperator(inOrder.front()) && !isParenthesis(inOrder.front())) {
            postOrder.push(inOrder.front());
        }
	//Check if token is an operator that is not a parenthesis
        else if(isOperator(inOrder.front())) {
	    //Add to empty stack if token is an operator that is not a parenthesis
            if(connector.empty()) {
                goto addToStack;
            }
	    //Pop stack onto postOrder as long as front of stack has same precedence 
	    //as token. Then add token to top of stack.
            while(!isParenthesis(connector.top())) {
                postOrder.push(connector.top());
                connector.pop();
                if(connector.empty()) {
                    goto addToStack;
                }
            }
            addToStack:
            connector.push(inOrder.front());
        }
	//Push left parenthesis to top of stack if token is left parenthesis
        else if(inOrder.front() == "(") {
            connector.push(inOrder.front());
        }
	//Check if token is right parenthesis
        else if(inOrder.front() == ")") {
	    //Pop all operators in stack into postOrder until top of stack is 
	    //left parenthesis
            while(connector.top() != "(") {
                postOrder.push(connector.top());
                connector.pop();
            }
	    //Pop left parenthesis
            connector.pop();
        }
	//Pop token from inOrder queue to check for next token
        inOrder.pop();
    }
    //Empty the stack into postOrder
    while (!connector.empty()) {
        postOrder.push(connector.top());
        connector.pop();
    }

    return postOrder;
}

/*
 * Return true if string is a connector. Otherwise, return false.
 *
 */
bool isOperator(string token){
    //Return true if string is a connector
    if(token == "&&" || token == "||" || token == ";") {
        return true;
    }
    return false;
}
/*
 * Return true if string is a token. Otherwise, return false.
 *
 */
bool isParenthesis(string token) {
    //Return false if string is a parenthesis
    if(token == "(" || token == ")") {
        return true;
    }
    return false;
}

Base * exitorCommand(string s)
{
  //variables
  Base * temp;
  char * val;
  char * split, *store;
  store = split = new char[500];
  strcpy(split,s.c_str());
  val = strtok(split," ");
  //create an exit node
  //
  //

  string p = string(val);
  while(p.substr(0,1) == " ")
	p = p.substr(1, p.length());

  if(p == "test" || p == "[")
  {
	//create a test object
 	temp = new TestCmd(s);	
  }
  else if(p == "exit" || p  == "Exit")
  {
	temp = new ExitCmd();
  }
  //create a command node
  else
  {
    temp = new Cmd(s);
  }
  if(store != NULL)
  {
  delete[] store;
  }
  return temp;

}

Base* generateTree(vector<string> v)
{
  //this stack will create our tree
  stack<Base *> shell;
  unsigned index = 0;
  while(index < v.size() )
  {
    if(v.at(index) == "||")
    {
      if(shell.size()>1)
      {
        Base * r = shell.top();
        shell.pop();
        Base * l = shell.top();
        shell.pop();
        Base * temp = new orOp(l,r);
        shell.push(temp);
      }
    }
    else if( v.at(index) == "&&")
    {
      if(shell.size()>1)
      {
        Base * r = shell.top();
        shell.pop();
        Base * l = shell.top();
        shell.pop();
        Base * temp = new AndOp(l,r);
        shell.push(temp);
      }
    }
    else if( v.at(index) == ";")
    {
      if(shell.size()>1)
      {
        Base * r = shell.top();
        shell.pop();
        Base * l = shell.top();
        shell.pop();
        Base * temp = new SemiColon(l,r);
        shell.push(temp);
      }
    }
    else
    {
      //will create a cmd node, exit node, or test node
      Base * temp = exitorCommand(v.at(index));

      shell.push(temp);
    }
    index++;
  }
  //if there is a node, return it
  if(shell.size())
  {
  return shell.top();
  }
  //if there is not, then create an empty node to execute
  else
  {
    cout<<"creating empty node"<<endl;
    Base * p = new Cmd();
    return p;
  }
}
