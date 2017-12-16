#ifndef DATASET_H
#define DATASET_H

#include "ns.hpp"

#include <vector>
#include <map>
#include <string>

class cluster::Dataset {
public:
  using index_t = unsigned int;
  using data_t = cluster::data_t;
  using Aggregator = data_t (std::vector<data_t>);

private:
  index_t numVars;
  std::vector<std::vector<data_t>> data;
  std::map<std::string, index_t> columnNameIndex;

  template<class K, class V>
  static std::map<V, K> reverse(std::map<K, V> map);

  template<class T>
  static std::vector<std::vector<T>> transpose(
    std::vector<std::vector<T>> data,
    unsigned int nRows,
    unsigned int nCols
  );

public:
  // Constructors.
  Dataset(index_t numVars);
  Dataset(std::vector<std::string> columnNames);

  // Default destructor is fine.

  // Basic information.
  index_t nObs() const;
  index_t nVars() const;

  // Add data.
  cluster::Dataset& add(std::vector<data_t> newData);
  cluster::Dataset& add(std::vector<std::vector<data_t>> newData);
  cluster::Dataset& operator += (std::vector<data_t> newData);
  cluster::Dataset& operator += (std::vector<std::vector<data_t>> newData);

  // Combine two maps into a new map.
  cluster::Dataset operator + (const cluster::Dataset& other) const;

  // Access rows.
  std::vector<data_t> row(index_t index) const;
  cluster::Dataset rows(std::vector<index_t> indices) const;
  std::vector<data_t> operator [] (index_t index) const;
  cluster::Dataset operator [] (std::vector<index_t> indices) const;

  // Access cols.
  std::vector<data_t> col(index_t index) const;
  std::vector<data_t> col(std::string name) const;
  std::vector<data_t> col(const char* name) const;
  cluster::Dataset cols(std::vector<index_t> indices) const;
  cluster::Dataset cols(std::vector<std::string> names) const;
  cluster::Dataset cols(std::vector<const char*> names) const;
  std::vector<data_t> operator () (index_t index) const;
  std::vector<data_t> operator () (std::string name) const;
  std::vector<data_t> operator () (const char* name) const;
  cluster::Dataset operator () (std::vector<index_t> indices) const;
  cluster::Dataset operator () (std::vector<std::string> names) const;
  cluster::Dataset operator () (std::vector<const char*> names) const;

  // Computation.
  std::vector<data_t> applyRow(Aggregator a) const;
  std::vector<data_t> applyCol(Aggregator a) const;
  cluster::Dataset standardize();
};

template<class K, class V>
std::map<V, K> cluster::Dataset::reverse(std::map<K, V> map) {
  std::map<V, K> rev;

  for (auto it = map.begin(); it != map.end(); it++) {
    rev[it->second] = it->first;
  }

  return rev;
}

template<class T>
std::vector<std::vector<T>> cluster::Dataset::transpose(
  std::vector<std::vector<T>> data,
  unsigned int nRows,
  unsigned int nCols
) {
  std::vector<std::vector<T>> trans;

  for (unsigned int i = 0; i < nCols; i++) {
    trans.push_back(std::vector<T>());
  }

  for (unsigned int i = 0; i < data.size(); i++) {
    for (unsigned int j = 0; j < data.size(); j++) {
      trans[j].push_back(data[i][j]);
    }
  }

  return trans;
}

#endif
