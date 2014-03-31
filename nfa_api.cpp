#include "nfa_api.hpp"

namespace nfa_api
{
  AbstractLabels::AbstractLabels() {}

  AbstractLabels::AbstractLabels(std::vector<char16_t> chars)
  {
    this->add(chars);
  }

  AbstractLabels::AbstractLabels(std::vector<int32_t> ints)
  {
    this->add(ints);
  }

  AbstractLabels::~AbstractLabels()
  {
    this->labels.clear();
  }

  void AbstractLabels::add(char16_t c)
  {
    this->labels.insert(charToInt(c));
  }

  void AbstractLabels::add(int32_t i)
  {
    this->labels.insert(i);
  }

  void AbstractLabels::add(std::vector<char16_t> chars)
  {
    this->labels.insert(chars.begin(), chars.end());
  }

  void AbstractLabels::add(std::vector<int32_t> ints)
  {
    this->labels.insert(ints.begin(), ints.end());
  }

  void AbstractLabels::addFromTo(char16_t from, char16_t to)
  {
    if (from <= to)
      for (int32_t i = this->charToInt(from); i <= this->charToInt(to); ++i)
        this->labels.insert(i);
    else
      for (int32_t i = this->charToInt(from); i >= this->charToInt(to); --i)
        this->labels.insert(i);
  }

  void AbstractLabels::addFromTo(int32_t from, int32_t to)
  {
    if (from <= to)
      for (int32_t i = from; i <= to; ++i)
        this->labels.insert(i);
    else
      for (int32_t i = from; i >= to; --i)
        this->labels.insert(i);
  }

  bool AbstractLabels::match(char16_t c)
  {
    // If it is a label, containing such is true;
    // Otherwise, as a co-label, not containing such is true.
    return (  this->labels.find(charToInt(c))
           != this->labels.end()
           )
      == this->isLabel();
  }

  bool AbstractLabels::match(int32_t i)
  {
    // If it is a label, containing such is true;
    // Otherwise, as a co-label, not containing such is true.
    return (  this->labels.find(i)
           != this->labels.end()
           )
      == this->isLabel();
  }

  Edge::Edge() {}

  Edge::Edge(int32_t src, int32_t dst, AbstractLabels * abstractLabelsPtr)
  {
    this->src = src;
    this->dst = dst;
    this->abstractLabelsPtr = abstractLabelsPtr;
  }

  Edge::~Edge()
  {
    delete abstractLabelsPtr;
    this->abstractLabelsPtr = nullptr;
  }

  void Edge::setAbstractLabels(AbstractLabels * abstractLabelsPtr)
  {
    this->abstractLabelsPtr = abstractLabelsPtr;
  }

  AbstractLabels * Edge::getAbstractLabels()
  {
    return this->abstractLabelsPtr;
  }

  int32_t Edge::getSrc()
  {
    return this->src;
  }

  int32_t Edge::getDst()
  {
    return this->dst;
  }

  AbstractNFA::AbstractNFA() {}

  AbstractNFA::AbstractNFA(std::string regex) {}

  AbstractNFA::~AbstractNFA()
  {
    this->startStates.clear();
    this->finalStates.clear();
    this->edges.clear();
  }

  void AbstractNFA::setStartStates(std::set<int32_t> startStates)
  {
    this->startStates = startStates;
  }

  void AbstractNFA::setFinalStates(std::set<int32_t> finalStates)
  {
    this->finalStates = finalStates;
  }

  void AbstractNFA::setEdges(std::set<Edge *> edges)
  {
    this->edges = edges;
  }

  std::set<int32_t> AbstractNFA::getStartStates()
  {
    std::set<int32_t> new_(this->startStates);
    return new_;
  }

  std::set<int32_t> AbstractNFA::getFinalStates()
  {
    std::set<int32_t> new_(this->finalStates);
    return new_;
  }

  std::set<Edge *> AbstractNFA::getEdges()
  {
    std::set<Edge *> new_(this->edges);
    return new_;
  }

  bool AbstractNFA::accept(std::string input)
  {
    // use intermediates to represent the states we have seen
    // we begin from start states
    std::set<int32_t> intermediates(this->startStates);
    uint16_t i = 0;

    while (true) {
      bool changed;
      // handle epsilon transitions
      do {
        changed = false;
        for (Edge * e : edges)
          if (  e->getAbstractLabels()->match(AbstractLabels::epsilon)
             && ( intermediates.find(e->getSrc()) != intermediates.end() )
             )
            changed = changed || intermediates.insert(e->getDst()).second;
      } while (changed);

      if (i >= input.length()) break;
      char16_t c = input.at(i);
      i += 1;

      // reachables is the set of states that one can reach eventually
      std::set<int32_t> reachables;
      for (Edge * e: edges)
        if (  e->getAbstractLabels()->match(c)
           && ( intermediates.find(e->getSrc()) != intermediates.end() )
           )
          reachables.insert(e->getDst());

      if (reachables.empty()) return false;
      intermediates = reachables;
    }

    //are any of the states we reached a final state
    for (int32_t q : intermediates)
      if (this->finalStates.find(q) != this->finalStates.end()) return true;
    return false;
  }

  int32_t StateNumberKeeper::currentStateNumber = 0;

  int32_t StateNumberKeeper::getNewStateNumber()
  {
    currentStateNumber += 1;
    return currentStateNumber;
  }
}
