#include <vector>

class SegmentTree {
private:
  std::vector<int> data;
  int N;

public:
  SegmentTree(const std::vector<int> &input)
      : data([](const auto &input) {
          std::vector<int> data(2 * input.size(), 0);
          auto N = input.size();
          for (int i = N; i < data.size(); ++i) {
            data[i] = input[i - N];
          }
          for (int i = N - 1; i >= 1; --i) {
            data[i] = data[2 * i] + data[2 * i + 1];
          }
          return data;
        }(input)),
        N(input.size()) {}

  void updateIndex(int index, int val) {
    auto start = N + index;
    data[start] = val;
    while (start > 0) {
      start /= 2;
      data[start] = data[2 * start] + data[2 * start + 1];
    }
  }

  int queryRange(int start, int end) {
    start += N;
    end += N;
    auto sum = 0;
    while (start <= end) {
      if (start % 2 == 1) {
        sum += data[start];
        ++start;
      }
      if (end % 2 == 0) {
        sum += data[end];
        --end;
      }
      start /= 2;
      end /= 2;
    }
    return sum;
  }
};