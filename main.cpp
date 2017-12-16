#include "ns.hpp"
#include "Dataset.hpp"
using namespace cluster;

#include <iostream>
#include <vector>
#include <sstream>

void tests();
void testDataset();
void testDistMeasures();
void testClustering(
  dist::DistanceMeasure dist,
  agg::Linkage linkage,
  agg::StopCriteria stop
);

template<class T>
std::string vectorToString(std::vector<T> vec, std::string sep = " ");

int main(int argc, char** argv) {
  // Run tests.
  tests();

  return 0;
}

void tests() {
  testDataset();

  testDistMeasures();

  testClustering(
    dist::euclidean,
    agg::lSingle,
    agg::nClusters<3>
  );

  testClustering(
    dist::maximum,
    agg::lComplete,
    agg::nClusters<4>
  );

  testClustering(
    dist::manhattan,
    agg::lAverage,
    agg::nClusters<4>
  );

  testClustering(
    dist::minkowski,
    agg::lCentroid,
    agg::nClusters<3>
  );

  testClustering(
    dist::canberra, // This is ignored when using Ward's method
    agg::lWards,
    agg::nClusters<4>
  );
}

void testDataset() {
  Dataset d1(3);
  Dataset d2({"Age", "Height", "Weight"});

  d1.add({1, 2, 3});
  d1 += {6, 2, 3};

  try {
    d2.add({4, 5});
  } catch (std::string e) {
    std::cout << e << std::endl;
  }

  d2 += {
    {20, 50, 60},
    {37, 70, 80},
    {56, 80, 100}
  };

  std::cout << d1.nObs() << " " << d2.nObs() << std::endl;

  Dataset d3 = d2(std::vector<unsigned>{1, 0});

  std::cout << d3.nObs() << " " << d3.nVars() << "\n"
            << vectorToString(d3("Height")) << "\n"
            << vectorToString(d3("Age")) << "\n"
            << vectorToString(d3.applyCol(stat::mean)) << "\n"
            << vectorToString(d3.applyCol(stat::sd)) << std::endl;

  Dataset d4 = d3.standardize();
  std::cout << d4.nObs() << " " << d4.nVars() << "\n"
            << vectorToString(d4("Height")) << "\n"
            << vectorToString(d4("Age")) << "\n"
            << vectorToString(d4.applyCol(stat::mean)) << "\n"
            << vectorToString(d4.applyCol(stat::sd)) << std::endl;
}

void testDistMeasures() {
  std::cout << dist::euclidean({1, 2, 3}, {4, 3, 2}) << std::endl;
  std::cout << dist::manhattan({1, 2, 3}, {4, 3, 2}) << std::endl;
}

void testClustering(
  dist::DistanceMeasure dist,
  agg::Linkage linkage,
  agg::StopCriteria stop
) {
  Dataset d1({"dogs", "cats", "turtles", "fish"});

  d1 += {
    {3, 2, 0, 0},
    {1, 1, 0, 1},
    {1, 4, 1, 0},
    {1, 0, 1, 3},
    {0, 0, 2, 2},
    {1, 1, 0, 3},
    {2, 1, 0, 1},
    {1, 0, 1, 2},
    {3, 3, 3, 3},
    {4, 0, 0, 0}
  };

  auto result = agg::agglomerativeClustering(
    d1,
    dist,
    linkage,
    stop
  );

  for (auto cluster : result) {
    std::cout << "Cluster:" << std::endl;
    for (auto i = 0; i < cluster.nObs(); i++) {
      std::cout << "  " << vectorToString(cluster[i]) << std::endl;
    }
  }

  std::cout << std::endl;
}

template<class T>
std::string vectorToString(std::vector<T> vec, std::string sep) {
  std::stringstream ss;
  bool first = true;

  for (T t : vec) {
    if (first) {
      first = false;
    } else {
      ss << sep;
    }

    ss << t;
  }

  return ss.str();
}
