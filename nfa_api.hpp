#ifndef NFA_API_HPP
#define NFA_API_HPP

#include <set>
#include <vector>
#include <cstdint>
#include <string>

namespace nfa_api
{
  class AbstractLabels
  {
  public:
    static int32_t const epsilon = -1;
    static int32_t const anyChar = -2;

    AbstractLabels();
    AbstractLabels(std::string regex);
    AbstractLabels(std::vector<char16_t> chars);
    AbstractLabels(std::vector<int32_t> ints);
    virtual ~AbstractLabels();

    /**
     * There are two types of children, Labels and CoLabels.
     * Labels match given characters; while CoLabels match
     * characters other than those given.
     * @return boolean
     */
    virtual bool isLabel() = 0;

    /**
     * Add a character to labels, ignoring the duplicates
     * @param chars
     */
    void add(char16_t c);

    /**
     * Add a vector of characters to labels, ignoring the duplicates
     * @param chars
     */
    void add(std::vector<char16_t> chars);

    /**
     * Add an integer to labels, ignoring the duplicates
     * @param ints
     */
    void add(int32_t i);

    /**
     * Add a vector of integers to labels, ignoring the duplicates
     * @param ints
     */
    void add(std::vector<int32_t> ints);

    /**
     * Add labels from a character to another character, inclusively
     * @param from
     * @param to
     */
    void addFromTo(char16_t from, char16_t to);

    /**
     * Add labels from an int to another int, inclusively
     * @param from
     * @param to
     */
    void addFromTo(int32_t from, int32_t to);
    bool match(char16_t c);
    bool match(int32_t i);

  protected:
    std::set<int32_t> labels;
    // Char is a 16-bit unicode character with minimum value of 0
    // while int is a 32-bit signed two's complement integer.
    // Thus the coercion is legit.
    int32_t charToInt(char16_t c) { return (int32_t)c; }
  };

  /**
   * Labels match given characters,
   * different from CoLabels
   */
  class Labels: public AbstractLabels
  {
  public:
    bool isLabel() override { return true; }
  };

  /**
   * CoLabels match characters other than given ones,
   * different from Labels
   */
  class CoLabels: public AbstractLabels
  {
  public:
    bool isLabel() override {return false; }
  };

  /**
   * Edge class stores source node, destination node and the label set.
   * CAUTION: Though if one needs to attach a label set and a colabel set to
   * the same pair of source node and destination node, one needs to create
   * at least two Edge instances.
   */
  class Edge
  {
  public:
    Edge();
    Edge(int32_t src, int32_t dst, AbstractLabels * abstractLabelsPtr);
    ~Edge();
    void setAbstractLabels(AbstractLabels * abstractLabelsPtr);
    AbstractLabels * getAbstractLabels();
    int32_t getSrc();
    int32_t getDst();

  private:
    int32_t src;
    int32_t dst;
    AbstractLabels * abstractLabelsPtr;
  };

  /**
   * The whole NFA diagram as opposed to a node in the graph.
   * State set can be implied by edges; start states and final states
   * are stored as S, and F respectively; transition function is specified by
   * edges; the alphabet is the same set of the type char.
   */
  class AbstractNFA
  {
  public:
    AbstractNFA();
    /**
     * A constructor which given a regular expression
     * makes an NFA for it
     * @param regex
     */
    AbstractNFA(std::string regex);
    virtual ~AbstractNFA();
    void setStartStates(std::set<int32_t> startStates);
    void setFinalStates(std::set<int32_t> finalStates);
    void setEdges(std::set<Edge *> edges);
    std::set<int32_t> getStartStates();
    std::set<int32_t> getFinalStates();
    std::set<Edge *> getEdges();
    /**
     * given a string input
     * says whether or not it is accepted
     * @param input
     * @return
     */
    bool accept(std::string input);

  protected:
    virtual AbstractNFA * mkNFAFromRegEx(std::string regex) = 0;
    /**
     * makes an NFA accepting one digit
     * @return
     */
    virtual AbstractNFA * mkNFAOfDigit() = 0;
    /**
     * makes an NFA accepting one non-digit
     * @return
     */
    virtual AbstractNFA * mkNFAOfNonDigit() = 0;
    /**
     * makes an NFA accepting one alphanumeric character
     * @return
     */
    virtual AbstractNFA * mkNFAOfAlphaNum() = 0;
    /**
     * makes an NFA accepting one non-alphanumeric character
     * @return
     */
    virtual AbstractNFA * mkNFAOfNonAlphaNum() = 0;
    /**
     * makes an NFA accepting one white character
     * @return
     */
    virtual AbstractNFA * mkNFAOfWhite() = 0;
    /**
     * makes an NFA accepting one non-white character
     * @return
     */
    virtual AbstractNFA * mkNFAOfNonWhite() = 0;
    /**
     * makes an NFA accepting one character of any kind
     * @return
     */
    virtual AbstractNFA * mkNFAOfAnyChar() = 0;
    /**
     * makes an NFA accepting the given character
     * @param c
     * @return
     */
    virtual AbstractNFA * mkNFAOfChar(char c) = 0;
    /**
     * makes an NFA union construction of the given two
     * @param nfa1
     * @param nfa2
     * @return
     */
    virtual AbstractNFA * unionOf(AbstractNFA * nfa1, AbstractNFA * nfa2) = 0;

    /**
     * makes an NFA concatenation construction of the given two
     * @param nfa1
     * @param nfa2
     * @return
     */
    virtual AbstractNFA * concatOf(AbstractNFA * nfa1, AbstractNFA * nfa2) = 0;

    /**
     * makes an NFA kleene star construction of the given one
     * @param nfa
     * @return
     */
    virtual AbstractNFA * starOf(AbstractNFA * nfa) = 0;

    /**
     * makes an NFA accepting what the given one accepts at least once
     * @param nfa
     * @return
     */
    virtual AbstractNFA * plusOf(AbstractNFA * nfa) = 0;

    /**
     * makes an NFA accepting what the given one accepts at most once
     * @param nfa
     * @return
     */
    virtual AbstractNFA * maxOnceOf(AbstractNFA * nfa) = 0;
    std::set<int32_t> startStates;
    std::set<int32_t> finalStates;
    std::set<Edge *> edges;
  };

  /**
   * A class always issues unique state number
   * CAUTION: if the length of regex is too long,
   * the uniqueness of state number cannot be guaranteed.
   */
  class StateNumberKeeper
  {
  public:
    static int32_t getNewStateNumber();
  private:
    static int32_t currentStateNumber;
  };
}
#endif /* NFA_API_HPP */
