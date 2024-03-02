#include <iostream>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <sstream>

using namespace std;

class State
{
public:
    set<int> parameter;
    bool isFinal;
    bool isStart;
    bool isMarked;
    State()
    {
        this->isFinal = false;
        this->isStart = false;
        this->isMarked = false;
    }
    State(set<int> prams)
    {
        this->parameter = prams;
        this->isFinal = false;
        this->isStart = false;
        this->isMarked = false;
    }

    // Equality operator is needed for using State in set
    bool operator==(const State &other) const
    {
        return this->parameter == other.parameter;
    }

    // Comparison operator is needed for using State in set
    bool operator<(const State &other) const
    {
        return this->parameter < other.parameter;
    }
};

class Dfa
{
public:
    set<State> states;
    set<char> terminals;
    unordered_map<pair<State, char>, State> dfaList;
    Dfa(State s,set<char> t)
    {
        states.insert(s);
        for(char c: t)
            terminals.insert(c);
    }
};

//     unordered_map<set<int>, vector<pair<set<int>, int>>> dfaList;


void createDfa(Dfa &dfa, unordered_map<char, set<int>> &positions, unordered_map<int, unordered_set<int>> &followPos)
{
    for (auto it = dfa.states.begin(); it != dfa.states.end(); ++it)
    {
        State &s = const_cast<State &>(*it); // Use const_cast to remove const qualifier

        if (!s.isMarked)
        {
            s.isMarked = true;

            for (char c : dfa.terminals)
            {
                set<int> temp;
                for (int i : s.parameter)
                {
                    if (positions[c].find(i) == positions[c].end())
                    {
                        temp.insert(followPos[i].begin(), followPos[i].end());
                    }
                }

                if (!temp.empty())
                {
                    State newState = State(temp);
                    if (dfa.states.find(newState) == dfa.states.end())
                    {
                        dfa.states.insert(newState);
                        dfa.terminals.insert(c);
                    }
                    dfa.dfaList[{s, c}] = newState;
                }
            }
        }
    }
}

int main()
{
    // Example usage of the createDfa function
    Dfa dfa(State({1}), 'a');
    unordered_map<char, set<int>> positions;
    unordered_map<int, unordered_set<int>> followPos;

    // Populate positions and followPos

    createDfa(dfa, positions, followPos);

    return 0;
}
