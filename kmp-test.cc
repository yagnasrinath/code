#include <iostream>
#include <string>
#include <vector>

std::vector<int> buildLPS(const std::string &pattern) {
  auto lps = std::vector<int>(pattern.size(), 0);
  auto i = 0;
  for (int j = 1; j < pattern.size(); ++j) {
    while (i > 0 && pattern[i] != pattern[j]) {
      i = lps[i - 1];
    }
    if (pattern[i] == pattern[j]) {
      ++i;
    }
    lps[j] = i;
  }
  return lps;
}

bool search(const std::string &text, const std::string &pattern) {
  const auto lps = buildLPS(pattern);

  auto i = 0;
  auto j = 0;

  while (i < text.size()) {
    if (text[i] == pattern[j]) {
      ++i;
      ++j;
      if (j == pattern.size()) {
        return true;
      }
    } else {
      if (j > 0) {
        j = lps[j - 1];
      } else {
        ++i;
      }
    }
  }
  return false;
}

int main() { std::cout << search("asbabcdef", "abc") << std::endl; }