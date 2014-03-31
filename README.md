# Grep11 * [![Build Status](https://travis-ci.org/Thadz/Grep11.svg?branch=master)](https://travis-ci.org/Thadz/Grep11)

a Unix/Linux grep-like command remake in **C++11**. It tries to match the given
regular expression with the given input string and returns true if it succeeds,
otherwise false.

## Regular Expression Format

For simplicity, the source code only supports following characters in **Postfix
Notation**
- standard ascii characters
- . as wildcard
- \d a single digit from 0 to 9
- \w an aphanumeric character
- \s a whitespace character
- \t a tab
- escaping reserved characters like \, . etc.
- | for union
- & for concatenation
- * for kleene star
- ? for at most once
- + for at least once

## How to Run
`````````
>> ./grep "ba*&" "baaaa"
`````````

## License

Ruby on Rails is released under the [MIT License](http://www.opensource.org/licenses/MIT).