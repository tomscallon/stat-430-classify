#include "ns.hpp"
#include "Dataset.hpp"

#include <cmath>
#include <iostream>

long double cluster::dist::euclidean(
  std::vector<cluster::data_t> x,
  std::vector<cluster::data_t> y
) {
  long double sum = 0;

  for (
    auto ix = x.begin(), iy = y.begin();
    ix != x.end(); // Should both reach end() at the same time
    ++ix, ++iy
  ) {
    sum += pow(*ix - *iy, 2);
  }

  return sqrt(sum);
}

long double cluster::dist::manhattan(
  std::vector<cluster::data_t> x,
  std::vector<cluster::data_t> y
) {
  long double sum = 0;

  for (
    auto ix = x.begin(), iy = y.begin();
    ix != x.end(); // Should both reach end() at the same time
    ++ix, ++iy
  ) {
    sum += std::abs(*ix - *iy);
  }

  return sum;
}

long double cluster::dist::minkowski(
  std::vector<cluster::data_t> x,
  std::vector<cluster::data_t> y
) {
  long double sum = 0;

  for (
    auto ix = x.begin(), iy = y.begin();
    ix != x.end(); // Should both reach end() at the same time
    ++ix, ++iy
  ) {
    sum += pow(*ix - *iy, x.size());
  }

  return pow(sum, 1.0 / x.size());
}

long double cluster::dist::maximum(
  std::vector<cluster::data_t> x,
  std::vector<cluster::data_t> y
) {
  long double max = 0;

  for (
    auto ix = x.begin(), iy = y.begin();
    ix != x.end(); // Should both reach end() at the same time
    ++ix, ++iy
  ) {
    max = std::max(max, std::abs(*ix - *iy));
  }

  return max;
}

long double cluster::dist::canberra(
  std::vector<cluster::data_t> x,
  std::vector<cluster::data_t> y
) {
  long double sum = 0;

  for (
    auto ix = x.begin(), iy = y.begin();
    ix != x.end(); // Should both reach end() at the same time
    ++ix, ++iy
  ) {
    if (abs(*ix) + abs(*iy) > 0) {
      sum += abs(*ix - *iy) / (abs(*ix) + abs(*iy));
    }
  }

  return sum;
}
