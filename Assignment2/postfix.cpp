#include <iostream>
#include <stack>
#include <string>

// bool isNode(char c) {
//     return (c == '|' || c == '.' || c == '*');
// }

int getPrecedence(char c) {
    if (c == '*') return 3;
    if (c == '.') return 2;
    if (c == '|') return 1;
    return 0; // For parentheses
}

std::string infixToPostfix(const std::string& infix) {
    std::stack<char> operators;
    std::string postfix;

    for (char c : infix) {
        if (isalnum(c)) {
            postfix += c;
        } else if (c == '(') {
            operators.push(c);
        } else if (c == ')') {
            while (!operators.empty() && operators.top() != '(') {
                postfix += operators.top();
                operators.pop();
            }
            operators.pop(); // Pop '('
        } else {
            while (!operators.empty() && getPrecedence(operators.top()) >= getPrecedence(c)) {
                postfix += operators.top();
                operators.pop();
            }
            operators.push(c);
        }
    }

    while (!operators.empty()) {
        postfix += operators.top();
        operators.pop();
    }

    return postfix;
}

int main() {
    std::string regex = "((a|b)*.(c|d)*)*.e*bc";
    std::string postfix = infixToPostfix(regex);

    std::cout << "Regular Expression: " << regex << std::endl;
    std::cout << "Postfix Expression: " << postfix << std::endl;

    return 0;
}
