#pragma once
#include <vector>
#include <string>
namespace yunyuncc{

inline std::vector<std::string> split(const std::string &s, char c) {
  std::string buff;
  std::vector<std::string> v;

  for (auto const &n : s) {
    if (n != c) {
      buff += n;
    } else if (n == c && !buff.empty()) {
      v.push_back(buff);
      buff.clear();
    }
  }
  if (!buff.empty()) {
    v.push_back(buff);
  }
  return v;
}


}

