#include "ns.hpp"
#include "Dataset.hpp"

#include <sstream>

cluster::Dataset::Dataset(cluster::Dataset::index_t numVars) : numVars(numVars) {}

cluster::Dataset::Dataset(std::vector<std::string> columnNames)
: numVars(columnNames.size()) {
  // Map each string back to its original index in the vector.
  for (cluster::Dataset::index_t i = 0; i < columnNames.size(); i++) {
    this->columnNameIndex[columnNames[i]] = i;
  }
}

cluster::Dataset::index_t cluster::Dataset::nObs() const {
  return this->data.size();
}

cluster::Dataset::index_t cluster::Dataset::nVars() const {
  return this->numVars;
}

cluster::Dataset& cluster::Dataset::add(
  std::vector<cluster::Dataset::data_t> newData
) {
  if (newData.size() != numVars) {
    std::stringstream s;
    s << "Expected " << numVars << " entries in data entry; "
      << "instead found " << newData.size();
    throw s.str();
  }

  this->data.push_back(newData);
  return *this;
}

cluster::Dataset& cluster::Dataset::add(
  std::vector<std::vector<cluster::Dataset::data_t>> newData
) {
  std::stringstream errorMessage;
  errorMessage << "The following errors were encountered:\n";
  bool errors = false;

  for (auto it = newData.begin(); it != newData.end(); ++it) {
    try {
      this->add(*it);
    } catch (std::string error) {
      errors = true;
      errorMessage << "  " << error << "\n";
    }
  }

  if (errors) {
    throw errorMessage.str();
  }

  return *this;
}

cluster::Dataset& cluster::Dataset::operator += (
  std::vector<cluster::Dataset::data_t> newData
) {
  return this->add(newData);
}

cluster::Dataset& cluster::Dataset::operator += (
  std::vector<std::vector<cluster::Dataset::data_t>> newData
) {
  return this->add(newData);
}

cluster::Dataset cluster::Dataset::operator + (
  const cluster::Dataset& other
) const {
  // Make sure the two maps have the same number of variables.
  if (this->numVars != other.numVars) {
    std::stringstream s;
    s << "Can't add datasets with different numbers of variables "
      << "(encountered variables with " << this->numVars
      << " and " << other.numVars << " variables)";
    throw s.str();
  }

  // Make sure the column names are the same.
  if (
    this->columnNameIndex.size() != other.columnNameIndex.size() ||
    !std::equal(
      this->columnNameIndex.begin(),
      this->columnNameIndex.end(),
      other.columnNameIndex.begin()
    )
  ) {
    std::stringstream s;
    throw "Can't add datasets with differing column names";
  }

  // Add the maps.
  cluster::Dataset combined(this->numVars);
  combined.columnNameIndex = this->columnNameIndex;
  combined.data = this->data;
  combined += other.data;
  return combined;
}

std::vector<cluster::Dataset::data_t> cluster::Dataset::row(
  cluster::Dataset::index_t index
) const {
  if (index >= this->nObs()) {
    std::stringstream s;
    s << "Index " << index << " is out of bounds";
    throw s.str();
  }

  return this->data[index];
}

cluster::Dataset cluster::Dataset::rows(
  std::vector<cluster::Dataset::index_t> indices
) const {
  std::stringstream errorMessage;
  errorMessage << "The following errors were encountered:\n";
  bool errors = false;

  // Prepare the new dataset.
  cluster::Dataset newSet(this->numVars);
  newSet.columnNameIndex = this->columnNameIndex;

  for (auto it = indices.begin(); it != indices.end(); ++it) {
    try {
      newSet += this->row(*it);
    } catch (std::string error) {
      errors = true;
      errorMessage << "  " << error << "\n";
    }
  }

  if (errors) {
    throw errorMessage.str();
  }

  return newSet;
}

std::vector<cluster::Dataset::data_t> cluster::Dataset::operator [] (
  cluster::Dataset::index_t index
) const {
  return this->row(index);
}

cluster::Dataset cluster::Dataset::operator [] (
  std::vector<cluster::Dataset::index_t> indices
) const {
  return this->rows(indices);
}

std::vector<cluster::Dataset::data_t> cluster::Dataset::col(
  cluster::Dataset::index_t index
) const {
  if (index >= this->numVars) {
    std::stringstream s;
    s << "Index " << index << " is out of bounds";
    throw s.str();
  }

  std::vector<cluster::Dataset::data_t> column;

  for (auto it = this->data.begin(); it != this->data.end(); ++it) {
    column.push_back((*it)[index]);
  }

  return column;
}

std::vector<cluster::Dataset::data_t> cluster::Dataset::col(
  std::string name
) const {
  auto it = this->columnNameIndex.find(name);

  if (it == this->columnNameIndex.end()) {
    std::stringstream s;
    s << "No column with name '" << name << "' exists";
    throw s.str();
  }

  return this->col(this->columnNameIndex.at(name));
}

std::vector<cluster::Dataset::data_t> cluster::Dataset::col(
  const char* name
) const {
  return this->col(std::string(name));
}

cluster::Dataset cluster::Dataset::cols(
  std::vector<cluster::Dataset::index_t> indices
) const {
  std::stringstream errorMessage;
  errorMessage << "The following errors were encountered:\n";
  bool errors = false;
  auto colNameMap = cluster::Dataset::reverse(this->columnNameIndex);
  std::vector<std::string> colNames;

  for (auto it = indices.begin(); it != indices.end(); ++it) {
    if (*it >= this->numVars) {
      errors = true;
      errorMessage << "  Index " << *it << " is out of bounds\n";
    } else {
      colNames.push_back(colNameMap[*it]);
    }
  }

  if (errors) {
    throw errorMessage.str();
  }

  cluster::Dataset newSet(colNames);

  for (auto it = this->data.begin(); it != this->data.end(); ++it) {
    std::vector<cluster::Dataset::data_t> newRow;
    auto row = *it;

    for (auto iIt = indices.begin(); iIt != indices.end(); ++iIt) {
      newRow.push_back(row[*iIt]);
    }

    newSet += newRow;
  }

  return newSet;
}

cluster::Dataset cluster::Dataset::cols(std::vector<std::string> names) const {
  std::stringstream errorMessage;
  errorMessage << "The following errors were encountered:\n";
  bool errors = false;

  for (auto it = names.begin(); it == names.end(); ++it) {
    if (this->columnNameIndex.find(*it) == this->columnNameIndex.end()) {
      errors = true;
      errorMessage << "  No column with name '" << *it << "' exists\n";
    }
  }

  cluster::Dataset newSet(names);
  for (auto it = this->data.begin(); it != this->data.end(); ++it) {
    auto row = *it;
    std::vector<cluster::Dataset::data_t> newRow;

    for (auto nIt = names.begin(); nIt != names.end(); nIt++) {
      newRow.push_back(row[this->columnNameIndex.at(*nIt)]);
    }
  }

  return newSet;
}

cluster::Dataset cluster::Dataset::cols(std::vector<const char*> names) const {
  // Convert char* to std::string.
  std::vector<std::string> strings;

  for (auto it = names.begin(); it != names.end(); ++it) {
    strings.push_back(std::string(*it));
  }

  // Then call cols() with those strings.
  return this->cols(strings);
}

std::vector<cluster::Dataset::data_t> cluster::Dataset::operator () (
  cluster::Dataset::index_t index
) const {
  return this->col(index);
}

std::vector<cluster::Dataset::data_t> cluster::Dataset::operator () (
  std::string name
) const {
  return this->col(name);
}

std::vector<cluster::Dataset::data_t> cluster::Dataset::operator () (
  const char* name
) const {
  return this->col(name);
}

cluster::Dataset cluster::Dataset::operator () (
  std::vector<cluster::Dataset::index_t> indices
) const {
  return this->cols(indices);
}

cluster::Dataset cluster::Dataset::operator () (
  std::vector<std::string> names
) const {
  return this->cols(names);
}

cluster::Dataset cluster::Dataset::operator () (
  std::vector<const char*> names
) const {
  return this->cols(names);
}

std::vector<cluster::Dataset::data_t> cluster::Dataset::applyRow(
  cluster::Dataset::Aggregator a
) const {
  std::vector<cluster::Dataset::data_t> result;

  for (auto row : this->data) {
    result.push_back(a(row));
  }

  return result;
}

std::vector<cluster::Dataset::data_t> cluster::Dataset::applyCol(
  cluster::Dataset::Aggregator a
) const {
  std::vector<cluster::Dataset::data_t> result;

  for (cluster::Dataset::index_t i = 0; i < this->numVars; i++) {
    result.push_back(a(this->col(i)));
  }

  return result;
}

cluster::Dataset cluster::Dataset::standardize() {
  auto means = this->applyCol(cluster::stat::mean);
  auto sds = this->applyCol(cluster::stat::sd);
  cluster::Dataset d(this->numVars);
  d.columnNameIndex = this->columnNameIndex;

  for (cluster::Dataset::index_t i = 0; i < this->nObs(); i++) {
    std::vector<cluster::Dataset::data_t> row;

    for (cluster::Dataset::index_t j = 0; j < this->numVars; j++) {
      row.push_back((data[i][j] - means[j]) / sds[j]);
    }

    d.add(row);
  }

  return d;
}
