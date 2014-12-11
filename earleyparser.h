#ifndef EARLEYPARSER_H
#define EARLEYPARSER_H

class Grammar
{
public:
    struct Production
    {
        int nSymbols;
        int *rightHandSide;
    };
    int nNonterminals;
    int *nProductions;
    Production **productions; //first production is always (pre start symbol) ->  (start symbol)
};

class EarleyParser
{
public:
    struct State
    {
        void setState(int symb, int productionRule,int nextPos,int originPos,State* ant1,State* ant2)\
        {symbol=symb; productionIndex=productionRule;next=nextPos;origin=originPos;antecedent1=ant1;antecedent2=ant2;}
        int symbol;
        int productionIndex; //index of production
        int next; //index of next symbol to be read within production
        int origin; //origin position
        State* antecedent1;
        State* antecedent2;
    };
    struct Node
    {
        int symbol;
        int productionIndex; //leaves are marked by production index -1
        union
        {
            Node* children;
            int position;
        } data;
    };
    EarleyParser();
    ~EarleyParser();

    //input
    void setGrammar(Grammar* parserGrammar);
    void setString(int parseStringLength,int* parseString){nLen=parseStringLength; str=parseString;}

    //processing
    void parse();

    //output
    void writeParseTree(Node* out);

private:
    //main parsing functions
    void parseInit();
    void parseToChart();
    void parseToTree();
    void parseCleanup();

    //earley parsing steps
    void predict(State* state,int pos);
    void scan(State* state,int pos);
    void complete(State* state,int pos);

    //auxiliary parsing functions
    void parseTmpCleanup();
    void parseTreeRecursion(State* state,Node* node,int *pos);
    void writeParseTreeRecursion(Node* node, Node* out);
    void addState(int symb, int productionRule,int nextPos,int originPos, int parsePos);
    void addState(int symb, int productionRule, int nextPos, int originPos, State* ant1, State *ant2, int parsePos);
    State *getState(int stateIndex,int parsePos);

    //memory management functions
    void setToZero();
    void delOutput();
    void delTree(Node* node);

    //parser input
    Grammar* grammar;
    int nLen;
    int *str; //string to parse

    //parser output
    State* stateChart;
    Node* parseTree;

    //auxiliary functions for states and nodes
    Grammar::Production *prod(State* state){return &grammar->productions[nonTerminal(state->symbol)][state->productionIndex];}
    Grammar::Production *prod(Node* node){return &grammar->productions[nonTerminal(node->symbol)][node->productionIndex];}
    bool isComplete(State* state) {return state->next==prod(state)->nSymbols;}
    bool nextIsNonTerminal(State *state) {return isNonTerminal(nextSymbol(state));}
    bool prevIsNonTerminal(State *state) {return isNonTerminal(prevSymbol(state));}
    int nextSymbol(State *state){return prod(state)->rightHandSide[state->next];}
    int prevSymbol(State *state){return prod(state)->rightHandSide[state->next-1];}

    //general auxiliary functions
    bool isNonTerminal(int symbol){return (symbol & NONTERMINALTAG);}
    int nonTerminal(int symbol){return (symbol & ~NONTERMINALTAG);}

    //auxiliary variables for parsing
    int nTotalProductions;  //total number of productions in grammar
    int *numStates;         //current total number of states for each character read
    bool *tmpPredicted;     //set of nonterminals for which prediction step is already done
    bool *tmpCompleted;     //set of (nonterminal,origin) pairs for which completion step is already done
    //to wipe the above array without accessing every member, the indices
    int tmpNumCompleted;    //number of bools set to true in tmpCompleted
    int *tmpIndexCompleted; //indices of bools set to true in tmpCompleted

    //constants
    const int NONTERMINALTAG=1<<(sizeof(int)*8-1);
    const int FIRSTNONTERMINAL=NONTERMINALTAG;
};

#endif // EARLEYPARSER_H
