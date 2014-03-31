#include "nfa.hpp"

namespace nfa
{
  NFA::NFA() {}

  NFA::NFA(std::string regex)
  {
    nfa_api::AbstractNFA * abstractLabelsPtr = this->mkNFAFromRegEx(regex);
    this->setStartStates(abstractLabelsPtr->getStartStates());
    this->setFinalStates(abstractLabelsPtr->getFinalStates());
    this->setEdges(abstractLabelsPtr->getEdges());
  }

  nfa_api::AbstractNFA * NFA::mkNFAFromRegEx(std::string regex)
  {
    std::stack<AbstractNFA *> nfaStack;
    char16_t c;
    uint16_t pos = 0;
    while (pos < regex.length())
    {
      c = regex.at(pos); ++pos;
      if (c == '\\')
      {
        /* escape */
        if (pos == regex.length())
          throw std::invalid_argument( std::string("unknown pattern at position "
                                     + std::to_string(pos)
                                     + std::string(" ")
                                     + regex));
        else
        {
          c = regex.at(pos); ++pos;
          if (c == 'd')
            /* digit */
            nfaStack.push(mkNFAOfDigit());
          else if (c == 'D')
            /* non-digit */
            nfaStack.push(mkNFAOfNonDigit());
          else if (c == 'w')
            /* alphanumeric */
            nfaStack.push(mkNFAOfAlphaNum());
          else if (c == 'W')
            /* non-alphanumeric */
            nfaStack.push(mkNFAOfNonAlphaNum());
          else if (c == 's')
            /* whitespace */
            nfaStack.push(mkNFAOfWhite());
          else if (c == 'S')
            /* non-whitespace */
            nfaStack.push(mkNFAOfNonWhite());
          else if (c == 't')
            /* tab */
            nfaStack.push(mkNFAOfChar('\t'));
          else if (isMetaChar(c))
            /* meta-character */
            nfaStack.push(mkNFAOfChar(c));
          else
            throw std::invalid_argument( std::string("unknown meta-character \\")
                                       + std::to_string(c)
                                       + std::string(" at position ")
                                       + std::to_string(pos)
                                       + std::string(" ")
                                       + regex);
        }
      }
      else if (c == '.')
      {
        /* wildcard */
        nfaStack.push(mkNFAOfAnyChar());
      }
      else if (c == '&')
      {
        /* concatenation */
        nfa_api::AbstractNFA * nfa2 = nfaStack.top();
        nfaStack.pop();
        nfa_api::AbstractNFA * nfa1 = nfaStack.top();
        nfaStack.pop();
        nfaStack.push(concatOf(nfa1, nfa2));
      }
      else if (c == '|')
      {
        /* union */
        nfa_api::AbstractNFA * nfa2 = nfaStack.top();
        nfaStack.pop();
        nfa_api::AbstractNFA * nfa1 = nfaStack.top();
        nfaStack.pop();
        nfaStack.push(unionOf(nfa1, nfa2));
      }
      else if (c == '*')
      {
        /* kleene star */
        nfa_api::AbstractNFA * nfa = nfaStack.top();
        nfaStack.pop();
        nfaStack.push(starOf(nfa));
      }
      else if (c == '+')
      {
        /* at least once */
        nfa_api::AbstractNFA * nfa = nfaStack.top();
        nfaStack.pop();
        nfaStack.push(plusOf(nfa));
      }
      else if (c == '?')
      {
        /* at most once */
        nfa_api::AbstractNFA * nfa = nfaStack.top();
        nfaStack.pop();
        nfaStack.push(maxOnceOf(nfa));
      }
      else
      {
        /* accept such character */
        nfaStack.push(mkNFAOfChar(c));
      }
    }

    if (nfaStack.size() != 1)
      throw
        std::invalid_argument( std::string("illegal regular expression:")
                             + std::string(" the number of operators do")
                             + std::string(" not match the number of tokens")
                             );

    return nfaStack.top();
  }

  nfa_api::AbstractNFA * NFA::mkNFAOfDigit()
  {
    auto nfaPtr = new NFA();

    int32_t startState = nfa_api::StateNumberKeeper::getNewStateNumber();
    std::set<int32_t> S;
    S.insert(startState);
    nfaPtr->setStartStates(S);

    int32_t finalState = nfa_api::StateNumberKeeper::getNewStateNumber();
    std::set<int32_t> F;
    F.insert(finalState);
    nfaPtr->setFinalStates(F);

    nfa_api::AbstractLabels * labelsPtr = new nfa_api::Labels();
    labelsPtr->addFromTo('0', '9');
    auto edgePtr = new nfa_api::Edge(startState, finalState, labelsPtr);
    std::set<nfa_api::Edge *> edges;
    edges.insert(edgePtr);
    nfaPtr->setEdges(edges);

    return nfaPtr;
  }

  nfa_api::AbstractNFA * NFA::mkNFAOfNonDigit()
  {
    auto nfaPtr = new NFA();

    int32_t startState = nfa_api::StateNumberKeeper::getNewStateNumber();
    std::set<int32_t> S;
    S.insert(startState);
    nfaPtr->setStartStates(S);

    int32_t finalState = nfa_api::StateNumberKeeper::getNewStateNumber();
    std::set<int32_t> F;
    F.insert(finalState);
    nfaPtr->setFinalStates(F);

    nfa_api::AbstractLabels * coLabelsPtr = new nfa_api::CoLabels();
    coLabelsPtr->addFromTo('0', '9');
    auto edgePtr = new nfa_api::Edge(startState, finalState, coLabelsPtr);
    std::set<nfa_api::Edge *> edges;
    edges.insert(edgePtr);
    nfaPtr->setEdges(edges);

    return nfaPtr;
  }

  nfa_api::AbstractNFA * NFA::mkNFAOfAlphaNum()
  {
    auto nfaPtr = new NFA();

    int32_t startState = nfa_api::StateNumberKeeper::getNewStateNumber();
    std::set<int32_t> S;
    S.insert(startState);
    nfaPtr->setStartStates(S);

    int32_t finalState = nfa_api::StateNumberKeeper::getNewStateNumber();
    std::set<int32_t> F;
    F.insert(finalState);
    nfaPtr->setFinalStates(F);

    nfa_api::AbstractLabels * labelsPtr = new nfa_api::Labels();
    labelsPtr->addFromTo('a', 'z');
    labelsPtr->addFromTo('A', 'Z');
    labelsPtr->addFromTo('0', '9');
    auto edgePtr = new nfa_api::Edge(startState, finalState, labelsPtr);
    std::set<nfa_api::Edge *> edges;
    edges.insert(edgePtr);
    nfaPtr->setEdges(edges);

    return nfaPtr;
  }

  nfa_api::AbstractNFA * NFA::mkNFAOfNonAlphaNum()
  {
    auto nfaPtr = new NFA();

    int32_t startState = nfa_api::StateNumberKeeper::getNewStateNumber();
    std::set<int32_t> S;
    S.insert(startState);
    nfaPtr->setStartStates(S);

    int32_t finalState = nfa_api::StateNumberKeeper::getNewStateNumber();
    std::set<int32_t> F;
    F.insert(finalState);
    nfaPtr->setFinalStates(F);

    nfa_api::AbstractLabels * coLabelsPtr = new nfa_api::CoLabels();
    coLabelsPtr->addFromTo('a', 'z');
    coLabelsPtr->addFromTo('A', 'Z');
    coLabelsPtr->addFromTo('0', '9');
    auto edgePtr = new nfa_api::Edge(startState, finalState, coLabelsPtr);
    std::set<nfa_api::Edge *> edges;
    edges.insert(edgePtr);
    nfaPtr->setEdges(edges);

    return nfaPtr;
  }

  nfa_api::AbstractNFA * NFA::mkNFAOfWhite()
  {
    auto nfaPtr = new NFA();

    int32_t startState = nfa_api::StateNumberKeeper::getNewStateNumber();
    std::set<int32_t> S;
    S.insert(startState);
    nfaPtr->setStartStates(S);

    int32_t finalState = nfa_api::StateNumberKeeper::getNewStateNumber();
    std::set<int32_t> F;
    F.insert(finalState);
    nfaPtr->setFinalStates(F);

    nfa_api::AbstractLabels * labelsPtr = new nfa_api::Labels();
    labelsPtr->add(' ');
    labelsPtr->add('\t');
    labelsPtr->add('\r');
    labelsPtr->add('\n');
    labelsPtr->add('\f');
    auto edgePtr = new nfa_api::Edge(startState, finalState, labelsPtr);
    std::set<nfa_api::Edge *> edges;
    edges.insert(edgePtr);
    nfaPtr->setEdges(edges);

    return nfaPtr;
  }

  nfa_api::AbstractNFA * NFA::mkNFAOfNonWhite()
  {
    auto nfaPtr = new NFA();

    int32_t startState = nfa_api::StateNumberKeeper::getNewStateNumber();
    std::set<int32_t> S;
    S.insert(startState);
    nfaPtr->setStartStates(S);

    int32_t finalState = nfa_api::StateNumberKeeper::getNewStateNumber();
    std::set<int32_t> F;
    F.insert(finalState);
    nfaPtr->setFinalStates(F);

    nfa_api::AbstractLabels * coLabelsPtr = new nfa_api::CoLabels();
    coLabelsPtr->add(' ');
    coLabelsPtr->add('\t');
    coLabelsPtr->add('\r');
    coLabelsPtr->add('\n');
    coLabelsPtr->add('\f');
    auto edgePtr = new nfa_api::Edge(startState, finalState, coLabelsPtr);
    std::set<nfa_api::Edge *> edges;
    edges.insert(edgePtr);
    nfaPtr->setEdges(edges);

    return nfaPtr;
  }

  nfa_api::AbstractNFA * NFA::mkNFAOfAnyChar()
  {
    auto nfaPtr = new NFA();

    int32_t startState = nfa_api::StateNumberKeeper::getNewStateNumber();
    std::set<int32_t> S;
    S.insert(startState);
    nfaPtr->setStartStates(S);

    int32_t finalState = nfa_api::StateNumberKeeper::getNewStateNumber();
    std::set<int32_t> F;
    F.insert(finalState);
    nfaPtr->setFinalStates(F);

    nfa_api::AbstractLabels * coLabelsPtr = new nfa_api::CoLabels();
    coLabelsPtr->add(nfa_api::AbstractLabels::anyChar);
    auto edgePtr = new nfa_api::Edge(startState, finalState, coLabelsPtr);
    std::set<nfa_api::Edge *> edges;
    edges.insert(edgePtr);
    nfaPtr->setEdges(edges);

    return nfaPtr;
  }

  nfa_api::AbstractNFA * NFA::mkNFAOfChar(char c)
  {
    auto nfaPtr = new NFA();

    int32_t startState = nfa_api::StateNumberKeeper::getNewStateNumber();
    std::set<int32_t> S;
    S.insert(startState);
    nfaPtr->setStartStates(S);

    int32_t finalState = nfa_api::StateNumberKeeper::getNewStateNumber();
    std::set<int32_t> F;
    F.insert(finalState);
    nfaPtr->setFinalStates(F);

    nfa_api::AbstractLabels * labelsPtr = new nfa_api::Labels();
    labelsPtr->add(c);
    auto edgePtr = new nfa_api::Edge(startState, finalState, labelsPtr);
    std::set<nfa_api::Edge *> edges;
    edges.insert(edgePtr);
    nfaPtr->setEdges(edges);

    return nfaPtr;
  }

  nfa_api::AbstractNFA * NFA::unionOf(nfa_api::AbstractNFA * nfa1, nfa_api::AbstractNFA * nfa2)
  {
    auto resNFAPtr = new NFA();

    int32_t startState = nfa_api::StateNumberKeeper::getNewStateNumber();
    std::set<int32_t> S;
    S.insert(startState);
    resNFAPtr->setStartStates(S);

    int32_t finalState = nfa_api::StateNumberKeeper::getNewStateNumber();
    std::set<int32_t> F;
    F.insert(finalState);
    resNFAPtr->setFinalStates(F);

    nfa_api::AbstractLabels * labelsPtr = new nfa_api::Labels();
    labelsPtr->add(nfa_api::AbstractLabels::epsilon);

    std::set<nfa_api::Edge *> edges;

    std::set<int32_t> startStates1 = nfa1->getStartStates();
    std::set<int32_t> finalStates1 = nfa1->getFinalStates();
    std::set<int32_t> startStates2 = nfa2->getStartStates();
    std::set<int32_t> finalStates2 = nfa2->getFinalStates();

    for (int32_t i : startStates1)
      edges.insert(new nfa_api::Edge(startState, i, labelsPtr));

    for (int32_t i : finalStates1)
      edges.insert(new nfa_api::Edge(i, finalState, labelsPtr));

    for (int32_t i : startStates2)
      edges.insert(new nfa_api::Edge(startState, i, labelsPtr));

    for (int32_t i : finalStates2)
      edges.insert(new nfa_api::Edge(i, finalState, labelsPtr));

    {
      std::set<nfa_api::Edge *> edges1 = nfa1->getEdges();
      edges.insert(edges1.begin(), edges1.end());
    }
    {
      std::set<nfa_api::Edge *> edges2 = nfa2->getEdges();
      edges.insert(edges2.begin(), edges2.end());
    }
    resNFAPtr->setEdges(edges);

    return resNFAPtr;
  }

  nfa_api::AbstractNFA * NFA::concatOf(nfa_api::AbstractNFA * nfa1, nfa_api::AbstractNFA * nfa2)
  {
    auto resNFAPtr = new NFA();

    int32_t startState = nfa_api::StateNumberKeeper::getNewStateNumber();
    std::set<int32_t> S;
    S.insert(startState);
    resNFAPtr->setStartStates(S);

    int32_t finalState = nfa_api::StateNumberKeeper::getNewStateNumber();
    std::set<int32_t> F;
    F.insert(finalState);
    resNFAPtr->setFinalStates(F);

    nfa_api::AbstractLabels * labelsPtr = new nfa_api::Labels();
    labelsPtr->add(nfa_api::AbstractLabels::epsilon);

    std::set<nfa_api::Edge *> edges;

    std::set<int32_t> startStates1 = nfa1->getStartStates();
    std::set<int32_t> finalStates1 = nfa1->getFinalStates();
    std::set<int32_t> startStates2 = nfa2->getStartStates();
    std::set<int32_t> finalStates2 = nfa2->getFinalStates();

    for (int32_t i : startStates1)
      edges.insert(new nfa_api::Edge(startState, i, labelsPtr));

    for (int32_t i : startStates2)
      for (int32_t j : finalStates1)
        edges.insert(new nfa_api::Edge(j, i, labelsPtr));

    for (int32_t i : finalStates2)
      edges.insert(new nfa_api::Edge(i, finalState, labelsPtr));

    {
      std::set<nfa_api::Edge *> edges1 = nfa1->getEdges();
      edges.insert(edges1.begin(), edges1.end());
    }
    {
      std::set<nfa_api::Edge *> edges2 = nfa2->getEdges();
      edges.insert(edges2.begin(), edges2.end());
    }
    resNFAPtr->setEdges(edges);

    return resNFAPtr;
  }

  nfa_api::AbstractNFA * NFA::starOf(nfa_api::AbstractNFA * nfa)
  {
    auto resNFAPtr = new NFA();

    int32_t startState = nfa_api::StateNumberKeeper::getNewStateNumber();
    std::set<int32_t> S;
    S.insert(startState);
    resNFAPtr->setStartStates(S);

    int32_t finalState = nfa_api::StateNumberKeeper::getNewStateNumber();
    std::set<int32_t> F;
    F.insert(finalState);
    resNFAPtr->setFinalStates(F);

    nfa_api::AbstractLabels * labelsPtr = new nfa_api::Labels();
    labelsPtr->add(nfa_api::AbstractLabels::epsilon);

    std::set<nfa_api::Edge *> edges;

    std::set<int32_t> startStates = nfa->getStartStates();
    std::set<int32_t> finalStates = nfa->getFinalStates();

    for (int32_t i : finalStates)
    {
      edges.insert(new nfa_api::Edge(startState, i, labelsPtr));
      edges.insert(new nfa_api::Edge(i, finalState, labelsPtr));
    }

    for (int32_t i : startStates)
      for (int32_t j : finalStates)
        edges.insert(new nfa_api::Edge(j, i, labelsPtr));

    {
      std::set<nfa_api::Edge *> edges1 = nfa->getEdges();
      edges.insert(edges1.begin(), edges1.end());
    }
    resNFAPtr->setEdges(edges);

    return resNFAPtr;
  }

  nfa_api::AbstractNFA * NFA::plusOf(nfa_api::AbstractNFA * nfa)
  {
    auto resNFAPtr = new NFA();

    int32_t startState = nfa_api::StateNumberKeeper::getNewStateNumber();
    std::set<int32_t> S;
    S.insert(startState);
    resNFAPtr->setStartStates(S);

    int32_t finalState = nfa_api::StateNumberKeeper::getNewStateNumber();
    std::set<int32_t> F;
    F.insert(finalState);
    resNFAPtr->setFinalStates(F);

    nfa_api::AbstractLabels * labelsPtr = new nfa_api::Labels();
    labelsPtr->add(nfa_api::AbstractLabels::epsilon);

    std::set<nfa_api::Edge *> edges;

    std::set<int32_t> startStates = nfa->getStartStates();
    std::set<int32_t> finalStates = nfa->getFinalStates();

    for (int32_t i : startStates)
    {
      edges.insert(new nfa_api::Edge(startState, i, labelsPtr));
      for (int32_t j : finalStates)
        edges.insert(new nfa_api::Edge(j, i, labelsPtr));
    }

    for (int32_t i : finalStates)
      edges.insert(new nfa_api::Edge(i, finalState, labelsPtr));

    {
      std::set<nfa_api::Edge *> edges1 = nfa->getEdges();
      edges.insert(edges1.begin(), edges1.end());
    }
    resNFAPtr->setEdges(edges);

    return resNFAPtr;
  }

  nfa_api::AbstractNFA * NFA::maxOnceOf(nfa_api::AbstractNFA* nfa)
  {
    auto resNFAPtr = new NFA();

    int32_t startState = nfa_api::StateNumberKeeper::getNewStateNumber();
    std::set<int32_t> S;
    S.insert(startState);
    resNFAPtr->setStartStates(S);

    int32_t finalState = nfa_api::StateNumberKeeper::getNewStateNumber();
    std::set<int32_t> F;
    F.insert(finalState);
    resNFAPtr->setFinalStates(F);

    nfa_api::AbstractLabels * labelsPtr = new nfa_api::Labels();
    labelsPtr->add(nfa_api::AbstractLabels::epsilon);

    std::set<nfa_api::Edge *> edges;

    std::set<int32_t> startStates = nfa->getStartStates();
    std::set<int32_t> finalStates = nfa->getFinalStates();

    for (int32_t i : startStates)
      edges.insert(new nfa_api::Edge(startState, i, labelsPtr));

    for (int32_t i : finalStates)
    {
      edges.insert(new nfa_api::Edge(i, finalState, labelsPtr));
      edges.insert(new nfa_api::Edge(startState, i, labelsPtr));
    }

    {
      std::set<nfa_api::Edge *> edges1 = nfa->getEdges();
      edges.insert(edges1.begin(), edges1.end());
    }
    resNFAPtr->setEdges(edges);

    return resNFAPtr;
  }
}
