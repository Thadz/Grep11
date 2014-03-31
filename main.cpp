#include "nfa.hpp"
#include <iostream>
#include <string>

static int printTest(std::string pattern, std::string input, bool expected);

static int mainTests();

int main(int argc, char* argv[])
{
  if (std::string(argv[1]) == "unit-tests")
  {
    std::cout << "\nFailed: " << mainTests() << '\n';
  }
  else if (argc < 3)
  {
    std::cout << "usage: grep arg1 arg2\n"
              << "arg1: the pattern to match\n"
              << "arg2: the input string\n\n"
              << "run unit tests: grep \"unit-tests\"\n";
  }
  else
  {
    auto nfaPtr = new nfa::NFA(argv[1]);
    std::cout << std::boolalpha << nfaPtr->accept(argv[2]) << '\n';
    delete nfaPtr;
  }
}

static int printTest(std::string pattern, std::string input, bool expected)
{
  auto nfaPtr = new nfa::NFA(pattern);
  bool b = nfaPtr->accept(input);
  delete nfaPtr;
  std::cout << "PATTERN: " << pattern << '\n';
  std::cout << "INPUT: " << input << '\n';
  std::cout << "STATUS: " << ((expected == b) ? "[O]" : "[X]") << '\n';
  std::cout << "VALUE: " << std::boolalpha << b << '\n';
  return expected != b;
}

static int mainTests()
{
  uint16_t counter = 0;
  counter += printTest("\\d", "0", true);
  counter += printTest("\\d", "1", true);
  counter += printTest("\\d", "2", true);
  counter += printTest("\\d", "3", true);
  counter += printTest("\\d", "4", true);
  counter += printTest("\\d", "5", true);
  counter += printTest("\\d", "6", true);
  counter += printTest("\\d", "7", true);
  counter += printTest("\\d", "8", true);
  counter += printTest("\\d", "9", true);
  counter += printTest("\\d", "a", false);
  counter += printTest("\\d", "10", false);
  counter += printTest("\\d", "\t", false);
  counter += printTest("\\d", "\\t", false);

  counter += printTest("\\D", "0", false);
  counter += printTest("\\D", "1", false);
  counter += printTest("\\D", "2", false);
  counter += printTest("\\D", "3", false);
  counter += printTest("\\D", "4", false);
  counter += printTest("\\D", "5", false);
  counter += printTest("\\D", "6", false);
  counter += printTest("\\D", "7", false);
  counter += printTest("\\D", "8", false);
  counter += printTest("\\D", "9", false);
  counter += printTest("\\D", "10", false);
  counter += printTest("\\D", "\\t", false);
  counter += printTest("\\D", "a", true);
  counter += printTest("\\D", "z", true);
  counter += printTest("\\D", "@", true);
  counter += printTest("\\D", "\t", true);

  counter += printTest("\\w", "a", true);
  counter += printTest("\\w", "z", true);
  counter += printTest("\\w", "A", true);
  counter += printTest("\\w", "Z", true);
  counter += printTest("\\w", "0", true);
  counter += printTest("\\w", "5", true);
  counter += printTest("\\w", "9", true);

  counter += printTest("\\w", "\\", false);
  counter += printTest("\\w", "\t", false);
  counter += printTest("\\w", "\r", false);
  counter += printTest("\\w", "\n", false);
  counter += printTest("\\w", "?", false);
  counter += printTest("\\w", "*", false);
  counter += printTest("\\w", "^", false);
  counter += printTest("\\w", "10", false);
  counter += printTest("\\w", "abc", false);

  counter += printTest("\\W", "a", false);
  counter += printTest("\\W", "z", false);
  counter += printTest("\\W", "A", false);
  counter += printTest("\\W", "Z", false);
  counter += printTest("\\W", "0", false);
  counter += printTest("\\W", "5", false);
  counter += printTest("\\W", "9", false);
  counter += printTest("\\W", "10", false);
  counter += printTest("\\W", "abc", false);

  counter += printTest("\\W", "\\", true);
  counter += printTest("\\W", "\t", true);
  counter += printTest("\\W", "\r", true);
  counter += printTest("\\W", "\n", true);
  counter += printTest("\\W", "?", true);
  counter += printTest("\\W", "*", true);
  counter += printTest("\\W", "^", true);

  counter += printTest("\\s", " ", true);
  counter += printTest("\\s", "\t", true);
  counter += printTest("\\s", "\r", true);
  counter += printTest("\\s", "\n", true);
  counter += printTest("\\s", "\f", true);
  counter += printTest("\\s", "0", false);
  counter += printTest("\\s", "9", false);
  counter += printTest("\\s", ",", false);
  counter += printTest("\\s", ".", false);
  counter += printTest("\\s", "@", false);
  counter += printTest("\\s", "1x", false);

  counter += printTest("\\S", " ", false);
  counter += printTest("\\S", "\t", false);
  counter += printTest("\\S", "\r", false);
  counter += printTest("\\S", "\n", false);
  counter += printTest("\\S", "\f", false);
  counter += printTest("\\S", "0", true);
  counter += printTest("\\S", "9", true);
  counter += printTest("\\S", ",", true);
  counter += printTest("\\S", ".", true);
  counter += printTest("\\S", "@", true);

  counter += printTest(".", " ", true);
  counter += printTest(".", "\t", true);
  counter += printTest(".", "\r", true);
  counter += printTest(".", "\n", true);
  counter += printTest(".", "\f", true);
  counter += printTest(".", "0", true);
  counter += printTest(".", "9", true);
  counter += printTest(".", ",", true);
  counter += printTest(".", ".", true);
  counter += printTest(".", "@", true);

  counter += printTest("a", "a", true);
  counter += printTest("9", "9", true);
  counter += printTest("\\.", ".", true);
  counter += printTest("@", "@", true);

  counter += printTest("a1|", "a", true);
  counter += printTest("a1|", "1", true);
  counter += printTest("a1|", "b", false);
  counter += printTest("a1|", "2", false);
  counter += printTest("a\\D|", "a", true);
  counter += printTest("a\\D|", "b", true);
  counter += printTest("a\\D|", "1", false);
  counter += printTest("a\\D|", "2", false);

  counter += printTest("a1&", "a1", true);
  counter += printTest("a1&", "a2", false);
  counter += printTest("a1&", "b1", false);
  counter += printTest("a1&", "b2", false);
  counter += printTest("a\\S&", "ab", true);
  counter += printTest("a\\S&", "a ", false);
  counter += printTest("a\\S&", "a\t", false);

  counter += printTest("a*", "", true);
  counter += printTest("a*", "a", true);
  counter += printTest("a*", "aa", true);
  counter += printTest("a*", "aaaa", true);
  counter += printTest("a*", "b", false);
  counter += printTest("a*", "bb", false);
  counter += printTest("a*", "1", false);

  counter += printTest("a+", "a", true);
  counter += printTest("a+", "aa", true);
  counter += printTest("a+", "aaaa", true);
  counter += printTest("a+", "", false);
  counter += printTest("a+", "b", false);
  counter += printTest("a+", "bb", false);

  counter += printTest("a?", "", true);
  counter += printTest("a?", "a", true);
  counter += printTest("a?", "aa", false);

  return counter;
}
