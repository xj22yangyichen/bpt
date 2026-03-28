#ifndef MYSTRING_HPP
#define MYSTRING_HPP

#include <string>
#include <string.h>
#include <iostream>

struct my_string {
  char a[65];

  my_string() {
    memset(a, 0, sizeof(a));
  }
  my_string(const char* str) {
    memset(a, 0, sizeof(a));
    strncpy(a, str, sizeof(a) - 1);
  }
  my_string(const std::string &str) {
    memset(a, 0, sizeof(a));
    strncpy(a, str.c_str(), sizeof(a) - 1);
  }

  inline bool operator<(const my_string &other) const {
    return strcmp(a, other.a) < 0;
  }
  inline bool operator>(const my_string &other) const {
    return strcmp(a, other.a) > 0;
  }
  inline bool operator==(const my_string &other) const {
    return strcmp(a, other.a) == 0;
  }
  inline bool operator!=(const my_string &other) const {
    return strcmp(a, other.a) != 0;
  }
  inline bool operator<=(const my_string &other) const {
    return strcmp(a, other.a) <= 0;
  }
  inline bool operator>=(const my_string &other) const {
    return strcmp(a, other.a) >= 0;
  }

  friend std::ostream& operator<<(std::ostream &os, const my_string &str) {
    os << str.a;
    return os;
  }
};

#endif // MYSTRING_HPP