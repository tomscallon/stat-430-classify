#include "ns.hpp"
#include "Dataset.hpp"

#include <vector>
#include <limits>
#include <iostream>
#include <algorithm>

long double cluster::agg::lSingle(
  cluster::dist::DistanceMeasure dist,
  const cluster::Dataset& cluster1,
  const cluster::Dataset& cluster2
) {
  // Find the closest two items between the clusters.
  long double minDist = std::numeric_limits<long double>::max();
  cluster::Dataset::index_t n1 = cluster1.nObs();
  cluster::Dataset::index_t n2 = cluster2.nObs();

  for (cluster::Dataset::index_t i = 0; i < n1; i++) {
    for (cluster::Dataset::index_t j = 0; j < n2; j++) {
      minDist = std::min(minDist, dist(cluster1[i], cluster2[j]));
    }
  }

  return minDist;
}

long double cluster::agg::lComplete(
  cluster::dist::DistanceMeasure dist,
  const cluster::Dataset& cluster1,
  const cluster::Dataset& cluster2
) {
  // Find the closest two items between the clusters.
  long double maxDist = std::numeric_limits<long double>::lowest();
  cluster::Dataset::index_t n1 = cluster1.nObs();
  cluster::Dataset::index_t n2 = cluster2.nObs();

  for (cluster::Dataset::index_t i = 0; i < n1; i++) {
    for (cluster::Dataset::index_t j = 0; j < n2; j++) {
      maxDist = std::max(maxDist, dist(cluster1[i], cluster2[j]));
    }
  }

  return maxDist;
}

long double cluster::agg::lAverage(
  cluster::dist::DistanceMeasure dist,
  const cluster::Dataset& cluster1,
  const cluster::Dataset& cluster2
) {
  // Find the closest two items between the clusters.
  long double sum = 0;
  cluster::Dataset::index_t n1 = cluster1.nObs();
  cluster::Dataset::index_t n2 = cluster2.nObs();

  for (cluster::Dataset::index_t i = 0; i < n1; i++) {
    for (cluster::Dataset::index_t j = 0; j < n2; j++) {
      sum += dist(cluster1[i], cluster2[j]);
    }
  }

  return sum / (n1 * n2);
}

long double cluster::agg::lCentroid(
  cluster::dist::DistanceMeasure dist,
  const cluster::Dataset& cluster1,
  const cluster::Dataset& cluster2
) {
  return dist(
    cluster1.applyCol(cluster::stat::mean),
    cluster2.applyCol(cluster::stat::mean)
  );
}

template <class T>
static std::vector<T> difference(std::vector<T> x, std::vector<T> y) {
  std::vector<T> diff;

  for (int i = 0; i < x.size(); i++) {
    diff.push_back(x[i] - y[i]);
  }

  return diff;
}

static long double sumOfSquares(std::vector<long double> x) {
  long double sum = 0;

  for (auto v : x) {
    sum += v * v;
  }

  return sum;
}

long double cluster::agg::lWards(
  cluster::dist::DistanceMeasure dist,
  const cluster::Dataset& cluster1,
  const cluster::Dataset& cluster2
) {
  auto n1 = cluster1.nObs();
  auto n2 = cluster2.nObs();
  auto m1 = cluster1.applyCol(cluster::stat::mean);
  auto m2 = cluster2.applyCol(cluster::stat::mean);

  return (double)n1 * n2 / (n1 + n2) * sumOfSquares(difference(m1, m2));
}

std::vector<cluster::Dataset> cluster::agg::agglomerativeClustering(
  const cluster::Dataset& data,
  cluster::dist::DistanceMeasure dist,
  cluster::agg::Linkage linkage,
  cluster::agg::StopCriteria stop
) {
  std::vector<cluster::Dataset> clusters;

  // Initially, put each observation in its own cluster.
  for (cluster::Dataset::index_t i = 0; i < data.nObs(); i++) {
    clusters.push_back(
      data[std::vector<cluster::Dataset::index_t>({i})]
    );
  }

  // While the stop criterion isn't satisfied...
  while (!stop(clusters) && clusters.size() > 1) {
    // Determine which two clusters are closest.
    int c1 = -1, c2 = -1;
    long double minDist = std::numeric_limits<long double>::max();

    for (int i = 1; i < clusters.size(); i++) {
      for (int j = 0; j < i; j++) {
        cluster::data_t d = linkage(dist, clusters[i], clusters[j]);

        if (d < minDist) {
          // std::cout << "New min dist" << std::endl;
          minDist = d;
          c1 = i;
          c2 = j;
        } else if (d == minDist) {
          // std::cout << "Duplicate min dist" << std::endl;
        }
      }
    }

    // Merge those two clusters.
    cluster::Dataset merged = clusters[c1] + clusters[c2];
    clusters[c1] = merged;
    clusters.erase(clusters.begin() + c2);
  }

  return clusters;
}
