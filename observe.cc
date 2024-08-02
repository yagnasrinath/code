// Our program receives inputs in the form of a text string (it might come from
// a web service or something.) The maximum input size is 30 MB. The text data
// comprise lines of input rows. Each row contains some number of columns,
// separated by commas. Each column for each row contains a numeric value.  The
// goal is to calculate the average value of each column in the data.

// Printing the correct answer is important, and not spending too much
// (programmer) time on the problem is important. Low-level machine efficiency
// is not very important.

// Example input data:
// data = "1,2,3,4\n0,-1,2,-3\n11,2,4,0\n"

// the same data, but formatted nicely:
// 1,  2, 3,  4
// 0, -1, 2, -3
// 11, 2, 4,  0

// Example output (exact format or return type is unimportant):
// 4,1,3,0.3333333

////////////////////////////////

// Human beings sometimes produce hard-to-read data.  Also, conditions may
//  change over time for a data stream.

// Let's develop this to a more fault tolerant CSV column averaging utility!

// Example input data:
// data = "1,2,3\n\n0,-1,,-3\n10,2.5,1,0,8\n5,q,5,4.5\n99,,,"

// the same data, but formatted nicely:
//  1,   2,   3

//  0,  -1,    ,  -3
// 10, 2.5,   1,   0,   8
//  5,   q,   5, 4.5
// 99,    ,    ,

// avg
//                      8

// Example output is not provided as it can differ based on decisions around
// error handling. An answer should always be returned.

// If

#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

// 0 1 2 3
// 123,
std::vector<float> computeAverageImproved(std::string &&input) {
  std::stringstream inputStream(input);
  auto columnSum = std::unordered_map<int, float>();
  auto numRowsPerColumn = std::unordered_map<int, int>();
  auto line = std::string{};
  while (std::getline(inputStream, line)) {
    auto pos = 0;
    auto index = 0;
    if (line.empty()) {
      continue;
    }
    while (true) {
      auto commaPos = line.find_first_of(',', pos);
      auto substr = line.substr(pos, commaPos - pos);
      try {
        columnSum[index] += (stof(substr));
        numRowsPerColumn[index] += 1;
      } catch (const std::invalid_argument &ia) {
        //
      }
      ++index;
      pos = commaPos + 1;
      if (commaPos == std::string::npos) {
        break;
      }
    }
  }
  auto result = std::vector<float>(numRowsPerColumn.size(), 0.0);
  for (int index = 0; index < numRowsPerColumn.size(); ++index) {
    result[index] = columnSum[index] / numRowsPerColumn[index];
  }
  return result;
}

std::vector<float> computeAverage(std::string &&input) {
  std::stringstream inputStream(input);
  auto result = std::vector<float>();
  auto rowCount = std::vector<int>();
  auto line = std::string{};
  auto isFirst = true;
  auto rows = 0;
  while (std::getline(inputStream, line)) {
    auto pos = 0;
    auto index = 0;
    
    while (true) {
      auto commaPos = line.find_first_of(',', pos);
      auto substr = line.substr(pos, commaPos - pos);
      if (isFirst) {
        result.push_back(stoi(substr));
      } else {
        result[index] += stoi(substr);
        ++index;
      }
      pos = commaPos + 1;
      if (commaPos == std::string::npos) {
        break;
      }
    }
    ++rows;
    isFirst = false;
  }
  for (auto &n : result) {
    n /= rows;
  }
  return result;
}

// To execute C++, please define "int main()"
int main() {
  auto test1 = std::string{"1,2,3,4\n0,-1,2,-3\n11,2,4,0\n"};
  for (const auto e : computeAverage(std::move(test1))) {
    std::cout << e << ",";
  }
  std::cout << std::endl;

  for(const auto e: computeAverageImproved({"1,2,3\n\n0,-1,,-3\n10,2.5,1,0,8\n5,q,5,4.5\n99,,,"})) {
    std::cout << e << ",";
  }
  std::cout << std::endl;
  return 0;
}
