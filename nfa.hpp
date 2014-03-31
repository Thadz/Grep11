#ifndef NFA_HPP
#define NFA_HPP

#include <string>
#include <set>
#include <stack>
#include <stdexcept>
#include "nfa_api.hpp"

namespace nfa
{
  /**
   * A possible solution provided by TAs
   * Created by Honglin Zhang 2/4/14.
   */
  class NFA : public nfa_api::AbstractNFA
  {
  public:
    NFA();
    NFA(std::string regex);
  protected:
    nfa_api::AbstractNFA * mkNFAFromRegEx(std::string regex) override;
    nfa_api::AbstractNFA * mkNFAOfDigit() override;
    nfa_api::AbstractNFA * mkNFAOfNonDigit() override;
    nfa_api::AbstractNFA * mkNFAOfAlphaNum() override;
    nfa_api::AbstractNFA * mkNFAOfNonAlphaNum() override;
    nfa_api::AbstractNFA * mkNFAOfWhite() override;
    nfa_api::AbstractNFA * mkNFAOfNonWhite() override;
    nfa_api::AbstractNFA * mkNFAOfAnyChar() override;
    nfa_api::AbstractNFA * mkNFAOfChar(char c) override;
    nfa_api::AbstractNFA *
      unionOf(nfa_api::AbstractNFA * nfa1, nfa_api::AbstractNFA * nfa2) override;
    nfa_api::AbstractNFA *
      concatOf(nfa_api::AbstractNFA * nfa1, nfa_api::AbstractNFA * nfa2) override;
    nfa_api::AbstractNFA * starOf(nfa_api::AbstractNFA * nfa) override;
    nfa_api::AbstractNFA * plusOf(nfa_api::AbstractNFA * nfa) override;
    nfa_api::AbstractNFA * maxOnceOf(nfa_api::AbstractNFA * nfa) override;
 private:
    bool isMetaChar(char c)
    {
      return c == '\\' || c == '.' || c == '&' ||
        c == '|' || c == '*' || c == '+' || c == '?';
    }
  };
}

#endif /* NFA_HPP */
