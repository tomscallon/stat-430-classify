#include "ns.hpp"

#include <vector>
#include <cmath>

cluster::data_t cluster::stat::mean(std::vector<cluster::data_t> data) {
  cluster::data_t sum = 0;

  for (auto d : data) sum += d;

  return sum / data.size();
}

cluster::data_t cluster::stat::cov(
  std::vector<cluster::data_t> x,
  std::vector<cluster::data_t> y
) {
  cluster::data_t mx = cluster::stat::mean(x);
  cluster::data_t my = cluster::stat::mean(y);
  cluster::data_t sum = 0;

  for (
    auto ix = x.begin(), iy = y.begin();
    ix != x.end();
    ++ix, ++iy
  ) {
    sum += (*ix - mx)*(*iy - my);
  }

  return sum / (x.size() - 1);
}

cluster::data_t cluster::stat::var(std::vector<cluster::data_t> data) {
  return cluster::stat::cov(data, data);
}

cluster::data_t cluster::stat::sd(std::vector<cluster::data_t> data) {
  return sqrt(cluster::stat::var(data));
}
