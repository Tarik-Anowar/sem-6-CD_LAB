#include <iostream>
#include <vector>
#include <set>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <functional>
#include <string>
using namespace std;
int final = 0;
namespace std
{
    template <>
    struct hash<set<int>>
    {
        size_t operator()(const set<int> &s) const
        {
            size_t hashVal = 0;
            for (int val : s)
            {
                hashVal ^= hash<int>()(val) + 0x9e3779b9 + (hashVal << 6) + (hashVal >> 2);
            }
            return hashVal;
        }
    };
}

class Tree;
bool isValid(char);
bool isNode(char);
bool nullable(Tree *, bool);
void setPos(Tree *, Tree *, Tree *);

class Tree
{
public:
    char data;
    bool isNullable; // NOTE:- The sign '$' denotes "****epsilon****"
    set<int> firtsPos;
    set<int> lastPos;
    Tree *left;
    Tree *right;

    Tree(char val) : data(val), left(nullptr), right(nullptr), isNullable(false) {}
    ~Tree()
    {
        delete left;
        delete right;
    }
};

class Dfa
{
public:
    set<set<int>> states;
    set<char> terminals;
    unordered_map<set<int>, vector<pair<set<int>, int>>> dfaList;

    Dfa(set<int> s, set<char> t)
    {
        states.insert(s);
        for (char c : t)
            terminals.insert(c);
    }

    void printState() const
    {
        cout << "\nTotal states:- \n";
        for (auto state : states)
        {
            cout << "{ ";
            for (int s : state)
            {
                cout << s << ", ";
            }
            cout << "}" << endl;
        }
        cout << endl;
    }

    void createDfa(unordered_map<char, vector<int>> &positions, unordered_map<int, unordered_set<int>> &followPos)
    {
        auto it = states.begin();
        set<int> &s = const_cast<set<int> &>(*it);
        while (!isMarked(s))
        {
            markState(s);
            set<int> temp;
            for (char c : terminals)
            {
                for (int i : s)
                {
                    if (find(positions[c].begin(), positions[c].end(), i) != positions[c].end())
                    {
                        temp.insert(followPos[i].begin(), followPos[i].end());
                    }
                }

                if (!temp.empty())
                {
                    for (int i : s)
                    {
                        cout << i << " ";
                    }
                    cout << endl;
                    if (!isInStates(temp))
                    {
                        states.insert(temp);
                    }
                    dfaList[s].push_back({temp, c});
                }
            }

            s = temp;
        }
    }

    void printDfa(unordered_map<char, vector<pair<char, char>>> &fianlDfa)
    {
        cout << "DFA States and Transitions:" << endl;
        int n = 0;
        char fin = 'f';
        char from, to;
        for (const auto &state : states)
        {
            if (state.find(final) != state.end())
            {
                from = fin;
            }
            else
            {
                from = (char)(n + 48);
                n++;
            }
            cout << "State: {";
            for (auto it = state.begin(); it != state.end(); it++)
            {
                cout << *it;
                if (next(it) != state.end())
                {
                    cout << ", ";
                }
            }
            cout << "} ";

            cout << "Transitions: ";
            if (dfaList.find(state) != dfaList.end())
            {
                const auto &transitions = dfaList.at(state);
                for (const auto &transition : transitions)
                {
                    cout << (char)transition.second << ": ";
                    cout << "(";
                    for (auto it = transition.first.begin(); it != transition.first.end(); it++)
                    {
                        cout << *it;
                        if (next(it) != transition.first.end())
                        {
                            cout << ", ";
                        }
                    }
                    char move = (char)transition.second;
                    cout << ") ";
                    if (transition.first.find(final) != transition.first.end())
                    {
                        to = fin;
                    }
                    else
                    {
                        to = (char)(n + 48);
                        if (state != transition.first)
                            n++;
                    }

                    fianlDfa[from].push_back({to, move});
                }
            }
            else
            {
                cout << "state not found";
            }
            cout << endl;
        }
    }

    // void createFinalDfa(unordered_map<char, vector<pair<char, char>>> &fianlDfa)
    // {
    //     int n = 0;
    //     char fin = 'f';
    //     char from;
    //     for (const auto state : states)
    //     {
    //         if (state.find(final) != state.end())
    //         {
    //             from = fin;
    //             cout << "from = " << from << endl;
    //         }
    //         else
    //         {
    //             from = (char)(n + 48);
    //             cout << "from = " << from << endl;
    //             n++;
    //         }

    //         if (dfaList.find(state) != dfaList.end())
    //         {
    //             const auto &transitions = dfaList.at(state);
    //             for (const auto transition : transitions)
    //             {
    //                 char move = (char)transition.second;
    //                 char to;
    //                 if (transition.first.find(final) != transition.first.end())
    //                 {
    //                     to = fin;
    //                 }
    //                 else
    //                 {
    //                     to = (char)(n + 48);
    //                     if (state != transition.first)
    //                         n++;
    //                 }

    //                 fianlDfa[from].push_back({to, move});
    //             }
    //         }
    //     }
    // }

private:
    bool isMarked(const set<int> &s)
    {
        return markedStates.count(s) > 0;
    }

    void markState(const set<int> &s)
    {
        markedStates.insert(s);
    }

    bool isInStates(const set<int> &s)
    {
        return states.find(s) != states.end();
    }

    set<set<int>> markedStates;
};

bool nullable(Tree *root)
{
    if (root->data == '$' || root->data == '*')
    {
        return true;
    }
    else if (root->data == '|')
    {
        return root->left->isNullable | root->right->isNullable;
    }
    else if (root->data == '.')
    {
        return root->left->isNullable & root->right->isNullable;
    }
    else
    {
        return false;
    }
}

void setPos(Tree *root, Tree *leftTree, Tree *rightTree)
{
    if (root->data == '|')
    {
        // current node first setting: firstpos(n) = firstpos(c1) U firstPos(c2)
        for (int pos : leftTree->firtsPos)
        {
            root->firtsPos.insert(pos);
        }
        for (int pos : rightTree->firtsPos)
        {
            root->firtsPos.insert(pos);
        }

        // current node laspos setting: lastPos(n) = lastpos(c1) U lastpos(c2)
        for (int pos : rightTree->lastPos)
        {
            root->lastPos.insert(pos);
        }
        for (int pos : leftTree->lastPos)
        {
            root->lastPos.insert(pos);
        }
    }
    else if (root->data == '.')
    {
        // current node firstpos setting:if(nullable(c1)) then: firstpos(n) = firstpos(c1) U firstpos(c2)  else: firstpos(n) = firstpos(c1)
        if (leftTree->isNullable)
        {
            for (int pos : leftTree->firtsPos)
            {
                root->firtsPos.insert(pos);
            }
            for (int pos : rightTree->firtsPos)
            {
                root->firtsPos.insert(pos);
            }
        }
        else
        {
            for (int pos : leftTree->firtsPos)
            {
                root->firtsPos.insert(pos);
            }
        }

        // current node laspos setting:if(nullable(c1)) then: lastPos(n) = lastpos(c1) U lastpos(c2)  else: lastpos(n) = lastpos(c2)
        if (rightTree->isNullable)
        {
            for (int pos : leftTree->lastPos)
            {
                root->lastPos.insert(pos);
            }
            for (int pos : rightTree->lastPos)
            {
                root->lastPos.insert(pos);
            }
        }
        else
        {
            for (int pos : rightTree->lastPos)
            {
                root->lastPos.insert(pos);
            }
        }
    }
    else if (root->data == '*')
    {
        // current node first setting: firstpos(n) = firstpos(c1)

        for (int pos : leftTree->firtsPos)
        {
            root->firtsPos.insert(pos);
        }

        // current node first setting: lastpos(n) = lastpos(c1)

        for (int pos : leftTree->lastPos)
        {
            root->lastPos.insert(pos);
        }
    }
    else
    {
        if (isValid(root->data))
            return;
        cout << "\n*********Enter a valid expression...\n\n";
        exit(0);
    }
}
Tree *makeTree(Tree *leftTree, Tree *rightTree, char val, int pos = -1)
{
    Tree *newTree = new Tree(val);
    newTree->left = leftTree;
    newTree->right = rightTree;

    if (isValid(val))
    {
        if (val == '$')
        {
            newTree->isNullable = true;
        }
        else
        {
            newTree->firtsPos.insert(pos);
            newTree->lastPos.insert(pos);
            newTree->isNullable = false;
        }
    }
    else
    {
        if (newTree != nullptr)
        {
            newTree->isNullable = nullable(newTree);
            setPos(newTree, leftTree, rightTree);
        }
    }

    return newTree;
}

bool isValid(char c)
{
    if (c >= 'a' && c <= 'z')
        return true;
    // else if (c >= 'A' && c <= 'Z')
    //     return true;
    else if (c >= '0' && c <= '9')
        return true;
    else if (c == '@' || c == '#' || c == '&' || c == '$')
        return true;
    return false;
}

bool isNode(char c)
{
    if (c == '|' || c == '*' || c == '.')
        return true;
    return false;
}
int getPrecedence(char c)
{
    if (c == '*')
        return 3;
    if (c == '.')
        return 2;
    if (c == '|')
        return 1;
    return 0; // For parentheses
}

string infixToPostfix(const std::string &infix)
{
    stack<char> operators;
    string postfix;

    for (char c : infix)
    {
        if (isValid(c))
        {
            postfix += c;
        }
        else if (c == '(')
        {
            operators.push(c);
        }
        else if (c == ')')
        {
            while (!operators.empty() && operators.top() != '(')
            {
                postfix += operators.top();
                operators.pop();
            }
            operators.pop(); // Pop '('
        }
        else
        {
            while (!operators.empty() && getPrecedence(operators.top()) >= getPrecedence(c))
            {
                postfix += operators.top();
                operators.pop();
            }
            operators.push(c);
        }
    }

    while (!operators.empty())
    {
        postfix += operators.top();
        operators.pop();
    }

    return postfix;
}

void setFollowPos(Tree *root, unordered_map<int, unordered_set<int>> &followPos)
{
    if (root == nullptr)
        return;
    if (root->left == nullptr && root->right == nullptr)
        return;
    if (root->data == '.')
    {
        for (int posc1 : root->left->lastPos) // If n is concatenation-node with left child c1 and right child c2, and i is a position in lastpos (c1), then all positions in firstpos(c2) are in followpos(i).
        {
            for (int posc2 : root->right->firtsPos)
            {
                followPos[posc1].insert(posc2);
            }
        }
    }
    else if (root->data == '*') // If n is a star-node, and i is a position in lastpos(n), then all positions in firstpos(n) are in followpos(i).
    {
        for (int posc1 : root->lastPos)
        {
            for (int posc2 : root->firtsPos)
            {
                followPos[posc1].insert(posc2);
            }
        }
    }
    else if (root->data == '#')
        return;
    setFollowPos(root->left, followPos);
    if (root->data == '.' || root->data == '|')
        setFollowPos(root->right, followPos);
}

void printFollowPos(unordered_map<int, std::unordered_set<int>> &followpos)
{
    for (const auto &entry : followpos)
    {
        cout << "Position " << entry.first << ": { ";
        for (int pos : entry.second)
        {
            cout << pos << " ";
        }
        cout << "}\n";
    }
}

void setPosition(string exp, unordered_map<char, vector<int>> &positions)
{
    int i = 1;
    for (auto c : exp)
    {
        if (isValid(c))
        {
            positions[c].push_back(i);
            i++;
        }
    }
    final = i - 1;
}

int main()
{
    // string expression;
    // cout << "Enter the expression: ";
    // cin >> expression;
    auto start = std::chrono::high_resolution_clock::now();

    unordered_map<char, vector<int>> positions;
    set<char> terminals;

    Tree *root;
    Tree *leftTree = nullptr;
    Tree *rightTree = nullptr;

    int pos = 1;
    char lastchar = '?';

    std::string regex = "(a.b)*.a.b.#";
    std::string postfix = infixToPostfix(regex);

    // setPosition(postfix, positions);

    std::cout << "Regular Expression: " << regex << std::endl;
    std::cout << "Postfix Expression: " << postfix << std::endl;

    stack<Tree *> treeStack;

    for (char c : postfix)
    {

        if (isValid(c))
        {
            if (isNode(lastchar) || lastchar == '?')
            {
                root = makeTree(nullptr, nullptr, c, pos);
                treeStack.push(root);
                terminals.insert(c);
            }
            else if (isValid(lastchar))
            {
                root = makeTree(nullptr, nullptr, c, pos);
                treeStack.push(root);
            }
            positions[c].push_back(pos);
            pos++;
            terminals.insert(c);
        }
        else if (isNode(c))
        {
            if (c == '*')
            {
                leftTree = treeStack.top();
                treeStack.pop();
                rightTree = nullptr;
            }
            else
            {
                rightTree = treeStack.top();
                treeStack.pop();
                leftTree = treeStack.top();
                treeStack.pop();
            }
            root = makeTree(leftTree, rightTree, c, pos);
            treeStack.push(root);
        }

        lastchar = c;
    }
    final = pos - 1;

    unordered_map<int, unordered_set<int>> followPos;

    setFollowPos(root, followPos);
    cout << endl
         << endl;
    cout << "Follow-Pos table: \n";
    printFollowPos(followPos);

    Dfa dfa = Dfa(root->firtsPos, terminals);
    vector<string> states;

    unordered_map<char, vector<pair<char, char>>> finalDfa;

    dfa.createDfa(positions, followPos);
    cout << "states: ";
    for (string s : states)
    {
        cout << s << " ";
    }
    cout << endl;

    dfa.printState();
    dfa.printDfa(finalDfa);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "Execution time: " << duration.count() << " microseconds" << std::endl;

    return 0;
}