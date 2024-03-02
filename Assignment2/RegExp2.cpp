#include <iostream>
#include <vector>
#include <set>
#include <stack>
#include <string>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <functional>


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

class State{
    public:
    char data;
    bool isFinal;
    State(){}
    State(char a,bool f =false)
    {
        this->data = a;
        this->isFinal = f;
    }
    bool operator==(const State& other) const {
        return (data == other.data) && (isFinal == other.isFinal);
    }
};

namespace std {
    template <>
    struct hash<State> {
        std::size_t operator()(const State& s) const {
            return hash<char>()(s.data) ^ (hash<bool>()(s.isFinal) << 1);
        }
    };
}

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
        for (const auto &state : states)
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
        queue<set<int>> stateQueue;
        stateQueue.push(*states.begin());
        while (!stateQueue.empty())
        {
            set<int> currentState = stateQueue.front();
            stateQueue.pop();

            for (char c : terminals)
            {
                set<int> nextState;

                for (int i : currentState)
                {
                    if (find(positions[c].begin(), positions[c].end(), i) != positions[c].end())
                    {
                        nextState.insert(followPos[i].begin(), followPos[i].end());
                    }
                }

                if (!nextState.empty())
                {
                    cout << "move " << c << " from { ";
                    for (int i : currentState)
                    {
                        cout << i << " ";
                    }
                    cout << "} to { ";
                    for (int i : nextState)
                    {
                        cout << i << " ";
                    }
                    cout << "}" << endl;

                    if (!isInStates(nextState))
                    {
                        states.insert(nextState);
                        stateQueue.push(nextState);
                    }
                    dfaList[currentState].push_back({nextState, c});
                }
            }
        }
    }

    void printDfa() const
    {
        
        cout << "DFA States and Transitions:" << endl;
        for (const auto &state : states)
        {
            cout << "\nState: {";
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
                    cout << ") ";
                }
            }
            else
            {
                cout << "state not found";
            }
            cout << endl;
        }
    }
    
    void createFinalDfa(unordered_map<State, vector<pair<State, char>>> &fianlDfa,unordered_map<set<int>, State>& store)
    {
        int n = 0;
        State from;
        
        bool isFin = false;
        for (const auto state : states)
        {
            if (store.find(state) != store.end())
            {
                from = store.at(state);
                cout << "from = " << from.data << ": ";
            }
            else if (state.find(final) != state.end())
            {
                from = State((char)(n + 48),true);
                cout << "from = " << from.data << ": ";
                n++;
            }
            else
            {
                from = State((char)(n + 48),false);
                cout << "from = " << from.data << ": ";
                n++;
            }

            store[state] = from;

            if (dfaList.find(state) != dfaList.end())
            {
                const auto &transitions = dfaList.at(state);
                for (const auto transition : transitions)
                {
                    char move = (char)transition.second;
                    State to;
                    if (store.find(transition.first) != store.end())
                    {
                        to = store.at(transition.first);
                         cout << "to: " << to.data << " ";
                    }
                    else if (transition.first.find(final) != transition.first.end())
                    {
                        to = State((char)(n + 48),true);
                        cout << "to: " << to.data << " ";
                    }
                    else
                    {
                        to = State((char)(n + 48),false);
                        cout << "to: " << to.data << " ";
                        n++;
                    }
                    store[transition.first] = to;
                    cout << "via: " << move << endl;
                    fianlDfa[from].push_back({to, move});
                }
            }
        }
    }

   void acceptString(const std::unordered_map<State, std::vector<std::pair<State, char>>>& finalDfa, State u, const std::string& str, int i) {
    if (i == str.length()) {
        if (u.isFinal)
            std::cout << "The string is accepted by the dfa\n";
        else
            std::cout << "The string is not accepted by the dfa\n";
        return;
    }

    char trans = str[i];
    std::cout << "char = " << u.data << ", trans = " << trans << std::endl;

    auto it = finalDfa.find(u);
    if (it != finalDfa.end()) {
        for (const auto& element : it->second) {
            char c = element.second;
            std::cout << "via : " << c << " ";
            if (c == trans) {
                State s = element.first;
                std::cout << "to: " << s.data << std::endl;
                acceptString(finalDfa, s, str, i + 1);
                return;
            }
        }
    }
    std::cout << "\nThe string is not accepted by the dfa\n";
   
}


private:
    bool isMarked(const set<int> &s)
    {
        return markedStates.find(s) != markedStates.end();
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
    else if (c >= 'A' && c <= 'Z')
        return true;
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
    // final = i - 1;
}
void makeString(string &str)
{
    unordered_map<string, string> strmap;
    strmap["a-z"] = "a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z";
    strmap["A-Z"] = "A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z";
    strmap["0-9"] = "0|1|2|3|4|5|6|7|8|9";

    for (const auto &entry : strmap)
    {
        const string &range = entry.first;
        const string &replacement = entry.second;
        size_t found = str.find(range);
        
        while (found != string::npos)
        {
            str.replace(found, range.length(), replacement);
            found = str.find(range, found + replacement.length());
        }
    }
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

    string regex = "(a-z).(a-z|0-9)*.@.(a-z).(a-z|0-9)*.d.o.t.c.o.m.#";
    //string regex = "(a|b|d.e)*.#";
    makeString(regex);
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

    for (auto &entry : positions)
    {
        char c = entry.first;
        cout << "char = " << c << ":- ";
        for (int v : positions[c])
        {
            cout << v << " ";
        }
        cout << endl;
    }

    final = pos - 1;

    unordered_map<int, unordered_set<int>> followPos;

    setFollowPos(root, followPos);
    cout << endl
         << endl;
    cout << "Follow-Pos table: \n";
    printFollowPos(followPos);

    // final = root->data;
    cout<<"final == "<<final<<endl;

    Dfa dfa = Dfa(root->firtsPos, terminals);
    dfa.createDfa(positions, followPos);
    cout << endl;

    dfa.printState();
    unordered_map<State,vector<pair<State,char>>> finalDfa;
    unordered_map<set<int>, State> store;
    dfa.printDfa();
    dfa.createFinalDfa(finalDfa,store);


    string str;

    State starting;
    auto it = store.find(root->firtsPos);
    if(it!=store.end())
    {
        starting = store.at(root->firtsPos);
    }

    while (str != "end")
    {
        cout << "\nEnter a regular expression: ";
        cin >> str;
        dfa.acceptString(finalDfa, starting, str, 0);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "Execution time: " << duration.count() << " microseconds" << std::endl;

    return 0;
}
