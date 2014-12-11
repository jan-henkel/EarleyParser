#include "earleyparser.h"
#include <cstring>

/*void EarleyParser::predict(State *state, int pos)
{
    int nonTerminalIndex=~NONTERMINALTAG&prod(state)->rightHandSide[state->current];
    if(!predicted[nonTerminalIndex])
    {
        for(int i=0;i<grammar->nProductions[nonTerminalIndex];++i)
            stateChart[numStates[pos]++].setState(prod(state)->rightHandSide[state->current],i,0,pos,0,0);
        predicted[nonTerminalIndex]=true;
    }
}


void EarleyParser::scan(State *state, int pos)
{
    if(pos<nLen && prod(state)->rightHandSide[state->current]==str[pos])
    {
        stateChart[numStates[pos+1]]=*state;
        ++stateChart[numStates[pos+1]].current;
        stateChart[numStates[pos+1]].antecedent1=state;
        ++numStates[pos+1];
    }
}

void EarleyParser::complete(State *state, int pos)
{
    int origin=state->origin;
    int start=origin*nLen*nTotalProductions;
    for(int i=start;i<start+numStates[origin];++i)
    {
        if(!stateChart[i].wasCompleted && !isComplete(&stateChart[i]) && prod(&stateChart[i])->rightHandSide[stateChart[i].current]==state->symbol)
        {
            stateChart[numStates[pos]]=stateChart[i];
            ++stateChart[numStates[pos]].current;
            stateChart[numStates[pos]].antecedent1=&stateChart[i];
            stateChart[numStates[pos]].antecedent2=state;
            ++numStates[pos];

            stateChart[i].wasCompleted=true;
            statesCompleted[nCompleted]=&stateChart[i];
            ++nCompleted;
        }
    }
}

void EarleyParser::initializeParser()
{

}

void EarleyParser::parse()
{
    int i;
    delete [] numStates;
    numStates=new int[nLen+1];
    memset(numStates,0,sizeof(int)*(nLen+1));
    delete [] predicted;
    predicted=new bool[grammar->nNonterminals];
    nTotalProductions=0;
    for(i=0;i<grammar->nNonterminals;++i)
        nTotalProductions+=grammar->nProductions[i];

    delete [] statesCompleted;
    statesCompleted=new State*[nLen*nTotalProductions];

    delete [] stateChart;
    stateChart=new State[nLen*nTotalProductions*(nLen+1)];
    memset(stateChart,0,sizeof(State)*nLen*nTotalProductions*(nLen+1));
    numStates[0]=1;

    int pos=0;
    int stateIndex;
    int currentSetStart;
    nCompleted=0;
    while(pos<=nLen)
    {
        currentSetStart=pos*nLen*nTotalProductions;
        stateIndex=currentSetStart;
        for(i=0;i<nCompleted;++i)
            statesCompleted[i]->wasCompleted=false;
        nCompleted=0;
        memset(predicted,0,sizeof(bool)*grammar->nNonterminals);
        while(stateIndex-currentSetStart<numStates[pos])
        {
            if(isComplete(&stateChart[stateIndex]))
            {
                complete(&stateChart[stateIndex],pos);
            }
            else
            {
                if(nextIsNonTerminal(&stateChart[stateIndex]))
                {
                    predict(&stateChart[stateIndex],pos);
                }
                else
                {
                    scan(&stateChart[stateIndex],pos);
                }
            }
        }
    }
}

void EarleyParser::generateTreeRecursion(State *state, Node *node, int *pos)
{
    node->symbol=state->symbol;
    State *tmp;
    tmp=state;
    int i;

    node->productionIndex=state->productionIndex;
    if(node->data.children==0)
        node->data.children=new Node[prod(state)->nSymbols];
    i=state->current-1;
    while(i>=0)
    {
        if(prod(tmp)->rightHandSide[i] & NONTERMINALTAG)
        {
            generateTreeRecursion(tmp->antecedent2,&node->data.children[i],pos);
            tmp=tmp->antecedent1;
        }
        else
        {
            node->data.children[i].symbol=prod(tmp)->rightHandSide[i];
            node->productionIndex=0;
            node->data.position=*pos;
            --*pos;
            tmp=tmp->antecedent1;
        }
        --i;
    }
}

void EarleyParser::generateTree()
{
    int start=nLen*nLen*nTotalProductions;
    State* state;
    for(int i=start;i<start+numStates[nLen];++i)
    {
        if(stateChart[i].symbol==NONTERMINALTAG && isComplete(&stateChart[i]))
        {
            generateTreeRecursion(&stateChart[i],parseTree,&nLen);
            return;
        }
    }
}

void parseSumOf1s()
{
    Grammar g;
    g.nNonterminals=4;
    g.productions=new Grammar::Production*[4];
}
*/

EarleyParser::EarleyParser()
{
    setToZero();
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
