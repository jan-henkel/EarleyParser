#include "earleyparser.h"
#include <cstring>

EarleyParser::EarleyParser()
{
    setToZero();
}

EarleyParser::EarleyParser()
{
    delOutput();
}

void EarleyParser::setToZero()
{
    numStates=0;
    tmpPredicted=0;
    tmpCompleted=0;
    tmpNumCompleted=0;
    tmpIndexCompleted=0;
    nTotalProductions=0;
    grammar=0;
    nLen=0;
    str=0;
    stateChart=0;
    parseTree=0;
}

void EarleyParser::setGrammar(Grammar *parserGrammar)
{
    grammar=parserGrammar;
    nTotalProductions=0;
    for(int i=0;i<grammar->nNonterminals;++i)
        nTotalProductions+=grammar->nProductions[i];
}

void EarleyParser::parse()
{
    parseInit();
    parseToChart();
    parseToTree();
    parseCleanup();
}

void EarleyParser::writeParseTree(Node *out)
{
    writeParseTreeRecursion(parseTree,out);
}

void EarleyParser::writeParseTreeRecursion(Node *node, Node *out)
{
    *out=*node;
    if(out->productionIndex!=-1) //no leaf
    {
        out->data.children=new Node[prod(out)->nSymbols];
        for(int i=0;i<prod(out)->nSymbols;++i)
            writeParseTreeRecursion(&node->data.children[i],&out->data.children[i]);
    }
}

void EarleyParser::parseInit()
{
    numStates=new int[nLen+1];                              //numStates[parsePosition]
    memset(numStates,0,sizeof(int)*(nLen+1));
    tmpPredicted=new bool[grammar->nNonterminals];          //tmpPredicted[nonterminal]
    memset(tmpPredicted,0,sizeof(bool)*grammar->nNonterminals);
    tmpCompleted=new bool[nLen*(grammar->nNonterminals)];   //tmpCompleted[origin*nNonterminals+nonterminal]
    memset(tmpCompleted,0,sizeof(bool)*nLen*(grammar->nNonterminals));
    tmpNumCompleted=0;
    tmpIndexCompleted=new int[nLen*(grammar->nNonterminals)];
    memset(tmpIndexCompleted,0,sizeof(int)*nLen*(grammar->nNonterminals));
    delOutput();
    stateChart=new State[(nLen+1)*nLen*(grammar->nNonterminals)];
    addState(FIRSTNONTERMINAL,0,0,0,0);
}

void EarleyParser::parseCleanup()
{
    delete[] numStates;
    delete[] tmpPredicted;
    delete[] tmpCompleted;
    delete[] tmpIndexCompleted;
    tmpNumCompleted=0;
}

void EarleyParser::delOutput()
{
    delete [] stateChart;
    delTree(parseTree);
}

void EarleyParser::delTree(Node *node)
{
    if(node && node->productionIndex!=-1) //no leaf
    {
        for(int i=0;i<prod(node)->nSymbols;++i)
            delTree(&node->data.children[i]);
    }
    delete [] node;
}

void EarleyParser::addState(int symb, int productionRule, int nextPos, int originPos, int parsePos)
{
    stateChart[numStates[parsePos]++].setState(symb,productionRule,nextPos,originPos,0,0);
}

void EarleyParser::addState(int symb, int productionRule, int nextPos, int originPos, State* ant1, State *ant2, int parsePos)
{
    stateChart[numStates[parsePos]++].setState(symb,productionRule,nextPos,originPos,ant1,ant2);
}

EarleyParser::State* EarleyParser::getState(int stateIndex, int parsePos)
{
    return &stateChart[parsePos*(nLen+1)+stateIndex];
}

void EarleyParser::parseToChart()
{
    int pos=0;
    State *state;
    int stateIndex;
    while(pos<=nLen)
    {
        stateIndex=0;
        while(stateIndex<numStates[pos])
        {
            state=getState(stateIndex,pos);
            if(isComplete(state))
            {
                complete(state,pos);
            }
            else
            {
                if(nextIsNonTerminal(state))
                {
                    scan(state,pos);
                }
                else
                {
                    predict(state,pos);
                }
            }
            ++stateIndex;
        }
        parseTmpCleanup();
        ++pos;
    }
}

void EarleyParser::complete(State *state, int pos)
{
    int lookupIndex=state->origin*grammar->nNonterminals+nonTerminal(state->symbol);
    if(!tmpCompleted[lookupIndex])
    {
        tmpCompleted[lookupIndex]=true;
        tmpIndexCompleted[tmpNumCompleted++]=lookupIndex;
        int posBefore=state->origin;
        State *tmpState;
        for(int i=0;i<numStates[posBefore];++i)
        {
            tmpState=getState(i,posBefore);
            if(tmpState->next==state->symbol)
            {
                addState(tmpState->symbol,tmpState->productionIndex,tmpState->next+1,tmpState->origin,state,tmpState,pos);
                //first antecedent is state with finished production,
                //second antecedent is state before nonterminal "obstacle" was parsed
            }
        }
    }
}

void EarleyParser::scan(State *state, int pos)
{
    if(nextSymbol(state)==str[pos])
        addState(state->symbol,state->productionIndex,state->next+1,state->origin,state,0,pos+1);
        //first and only antecedent is state pre-scan, state is added to set for next parsing position
}

void EarleyParser::predict(State *state, int pos)
{
    int symbol=nextSymbol(state);
    int nonTerminalSymbol=nonTerminal(symbol);
    if(!tmpPredicted[nonTerminalSymbol])
    {
        tmpPredicted[nonTerminalSymbol]=true;
        for(int i=0;i<grammar->nProductions[nonTerminalSymbol];++i)
            addState(symbol,i,0,pos,0,0,pos);
            //nothing was parsed, hence no antecedents. every production is added, origin is current parsing position
    }
}

void EarleyParser::parseTmpCleanup()
{
    memset(tmpPredicted,0,sizeof(bool)*grammar->nNonterminals);
    for(int i=0;i<tmpNumCompleted;++i)
        tmpCompleted[tmpIndexCompleted[i]]=false;
    tmpNumCompleted=0;
}

void EarleyParser::parseToTree()
{
    State *state;
    for(int i=0;i<numStates[nLen+1];++i)
    {
        state=getState(i,nLen+1);
        if(state->symbol==FIRSTNONTERMINAL)
        {
            parseTree=new Node;
            int pos=nLen+1;
            parseTreeRecursion(state,parseTree,&pos);
        }
    }
}

void EarleyParser::parseTreeRecursion(State *state, Node *node, int *pos)
{
    State* tmp=state;
    int origin=state->origin;
    int nSymbols=prod(state)->nSymbols;
    node->symbol=state->symbol;
    node->productionIndex=state->productionIndex;
    node->data.children=new Node[nSymbols];
    while(*pos>origin)
    {
        if(prevIsNonTerminal(tmp))
        {
            parseTreeRecursion(tmp->antecedent1,&node->data.children[tmp->next-1],pos);
            tmp=tmp->antecedent2;
        }
        else
        {
            node->data.children[tmp->next-1].productionIndex=-1;
            node->data.children[tmp->next-1].symbol=prevSymbol(tmp);
            node->data.children[tmp->next-1].data.position=--*pos;
            tmp=tmp->antecedent1;
        }
    }
}
