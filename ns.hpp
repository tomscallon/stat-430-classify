#ifndef NS_H
#define NS_H

#include <vector>

namespace cluster {
  using data_t = long double;
  class Dataset;

  namespace stat {
    data_t mean(std::vector<data_t> data);
    data_t cov(std::vector<data_t> x, std::vector<data_t> y);
    data_t var(std::vector<data_t> data);
    data_t sd(std::vector<data_t> data);
  }

  namespace dist {
    using DistanceMeasure = long double (
      std::vector<data_t> x,
      std::vector<data_t> y
    );

    DistanceMeasure euclidean;
    DistanceMeasure manhattan;
    DistanceMeasure minkowski;
    DistanceMeasure maximum;
    DistanceMeasure canberra;
  };

  namespace agg {
    using Linkage = long double (
      dist::DistanceMeasure dist,
      const Dataset& cluster1,
      const Dataset& cluster2
    );

    Linkage lSingle;
    Linkage lComplete;
    Linkage lAverage;
    Linkage lCentroid;
    Linkage lWards;

    using StopCriteria = bool (std::vector<Dataset> clusters);

    template <unsigned int n>
    StopCriteria nClusters;

    std::vector<Dataset> agglomerativeClustering(
      const Dataset& data,
      dist::DistanceMeasure dist,
      Linkage linkage,
      StopCriteria stop
    );
  };
};

template <unsigned int n>
bool cluster::agg::nClusters(std::vector<cluster::Dataset> clusters) {
  return clusters.size() == n;
}

#endif
