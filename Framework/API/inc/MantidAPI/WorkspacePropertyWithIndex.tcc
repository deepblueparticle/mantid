#include "MantidAPI/IndexTypeProperty.h"
#include "MantidAPI/WorkspacePropertyWithIndex.h"
#include <MantidAPI/MatrixWorkspace.h>
#include <MantidIndexing/IndexInfo.h>
#include <MantidKernel/Property.h>
#include <MantidKernel/make_unique.h>

#include "MantidAPI/WorkspaceProperty.tcc"

using namespace Mantid::Kernel;
using namespace Mantid::Indexing;

namespace Mantid {
namespace API {

template <typename TYPE>
WorkspacePropertyWithIndex<TYPE>::WorkspacePropertyWithIndex(
    const std::string &name, int indexTypes, const std::string &wsName,
    IValidator_sptr validator)
    : WorkspaceProperty<TYPE>(name, wsName, Direction::Input),
      m_indexListProp(
          make_unique<ArrayProperty<int>>("Indices", Direction::Output)),
      m_indexTypeProp(make_unique<IndexTypeProperty>(indexTypes)) {}

template <typename TYPE>
WorkspacePropertyWithIndex<TYPE>::WorkspacePropertyWithIndex(
    const std::string &name, const int indexTypes, const std::string &wsName,
    API::PropertyMode::Type optional, Kernel::IValidator_sptr validator)
    : WorkspaceProperty<TYPE>(name, wsName, Direction::Input, optional),
      m_indexListProp(
          make_unique<ArrayProperty<int>>("Indices", Direction::Output)),
      m_indexTypeProp(make_unique<IndexTypeProperty>(indexTypes)) {}

template <typename TYPE>
WorkspacePropertyWithIndex<TYPE>::WorkspacePropertyWithIndex(
    const std::string &name, const int indexTypes, const std::string &wsName,
    API::PropertyMode::Type optional, API::LockMode::Type locking,
    Kernel::IValidator_sptr validator)
    : WorkspaceProperty<TYPE>(name, wsName, Direction::Input, optional,
                              locking),
      m_indexListProp(
          make_unique<ArrayProperty<int>>("Indices", Direction::Output)),
      m_indexTypeProp(make_unique<IndexTypeProperty>(indexTypes)) {}

template <typename TYPE>
WorkspacePropertyWithIndex<TYPE>::WorkspacePropertyWithIndex(
    const WorkspacePropertyWithIndex<TYPE> &right)
    : WorkspaceProperty<TYPE>(right),
      m_indexListProp(make_unique<ArrayProperty<int>>(*right.m_indexListProp)),
      m_indexTypeProp(make_unique<IndexTypeProperty>(*right.m_indexTypeProp)) {}

template <typename TYPE>
std::string WorkspacePropertyWithIndex<TYPE>::isValid() const {
  std::string error = "";
  std::string valid;

  if ((valid = WorkspaceProperty<TYPE>::isValid()) != "")
    error += "\n" + valid;
  if ((valid = m_indexTypeProp->isValid()) != "")
    error += "\n" + valid;
  if ((valid = m_indexListProp->setValue(m_indexListValue)) != "")
    error += "\n" + valid;

  if (error.empty()) {
    try {
      getIndices();
    } catch (std::out_of_range &) {
      error += m_indexTypeProp->value() + "s provided out of range.";
    } catch (std::logic_error &) {
      error += "Invalid " + m_indexTypeProp->value() +
               "s is invalid. May contain duplicate indices.";
    }
  }

  return error;
}

template <typename TYPE>
std::string
WorkspacePropertyWithIndex<TYPE>::setValue(const std::string &value) {
  *this = value;
  return isValid();
}

template <typename TYPE>
bool WorkspacePropertyWithIndex<TYPE>::
operator==(const WorkspacePropertyWithIndex<TYPE> &rhs) {
  return WorkspaceProperty<TYPE>::operator==(rhs) &&
         m_indexListProp == rhs.m_indexListProp &&
         m_indexTypeProp == rhs.m_indexTypeProp;
}

template <typename TYPE>
WorkspacePropertyWithIndex<TYPE> &WorkspacePropertyWithIndex<TYPE>::
operator=(const std::string &rhs) {
  std::string del = ";";
  if (std::count(rhs.cbegin(), rhs.cend(), del[0]) == 2) {
    std::string s = rhs;
    size_t pos;
    std::vector<std::string> values;
    while ((pos = s.find(del)) != std::string::npos) {
      auto token = s.substr(0, pos);
      values.push_back(token);
      s.erase(0, pos + del.length());
    }

    values.push_back(s);

    WorkspaceProperty<TYPE>::setValue(values[0]);
    *m_indexTypeProp.get() = values[1];
    m_indexListValue = values[2];
    m_indexListProp->setValue(m_indexListValue);

  } else {
    WorkspaceProperty<TYPE>::setValue(rhs);
  }

  return *this;
}

template <typename TYPE>
WorkspacePropertyWithIndex<TYPE> &WorkspacePropertyWithIndex<TYPE>::
operator=(const std::tuple<boost::shared_ptr<TYPE>, API::IndexType,
                           std::vector<int>> &rhs) {
  boost::shared_ptr<TYPE> ws;
  API::IndexType type;
  std::vector<int> list;

  std::tie(ws, type, list) = rhs;

  *this = ws;
  *this->m_indexTypeProp.get() = type;
  *this->m_indexListProp.get() = list;

  m_indexListValue = this->m_indexListProp->value();
  return *this;
}

template <typename TYPE>
WorkspacePropertyWithIndex<TYPE> &WorkspacePropertyWithIndex<TYPE>::
operator=(const std::tuple<boost::shared_ptr<TYPE>, API::IndexType, std::string>
              &rhs) {
  boost::shared_ptr<TYPE> ws;
  API::IndexType type;
  std::string list;

  std::tie(ws, type, list) = rhs;

  *this = ws;
  *this->m_indexTypeProp.get() = type;
  m_indexListValue = list;
  m_indexListProp->setValue(m_indexListValue);

  return *this;
}

template <typename TYPE>
WorkspacePropertyWithIndex<TYPE> &WorkspacePropertyWithIndex<TYPE>::
operator=(const WorkspacePropertyWithIndex<TYPE> &rhs) {
  if (&rhs == this)
    return *this;
  API::WorkspaceProperty<TYPE>::operator=(rhs);
  *m_indexListProp = *rhs.m_indexListProp;
  m_indexListValue = m_indexListProp->value();
  *m_indexTypeProp = rhs.m_indexTypeProp->selectedType();

  return *this;
}

template <typename TYPE>
WorkspacePropertyWithIndex<TYPE> &WorkspacePropertyWithIndex<TYPE>::
operator+=(WorkspacePropertyWithIndex const *rhs) {
  throw Kernel::Exception::NotImplementedError(
      "+= operator is not implemented for WorkspacePropertyWithIndex.");
  return *this;
}

template <typename TYPE>
WorkspacePropertyWithIndex<TYPE> *
WorkspacePropertyWithIndex<TYPE>::clone() const {
  return new WorkspacePropertyWithIndex<TYPE>(*this);
}

template <typename TYPE>
std::string WorkspacePropertyWithIndex<TYPE>::value() const {
  auto wksp = getWorkspace();
  std::string wkspName = "";
  if (wksp)
    wkspName = wksp->getName();

  return wkspName + ";" + m_indexTypeProp->value() + ";" +
         m_indexListProp->value();
}

template <typename TYPE>
WorkspacePropertyWithIndex<TYPE>::
operator const std::tuple<boost::shared_ptr<TYPE>, SpectrumIndexSet>() const {
  return std::make_pair(operator()(), getIndices());
}

template <typename TYPE>
WorkspacePropertyWithIndex<TYPE>::
operator const std::tuple<boost::shared_ptr<const TYPE>, SpectrumIndexSet>()
    const {
  return std::make_pair(operator()(), getIndices());
}

template <typename TYPE>
const SpectrumIndexSet WorkspacePropertyWithIndex<TYPE>::getIndices() const {
  // TODO handle DetectorID->SpectrumNumber conversion
  // This will need to be an extra IndexType
  const auto &list = this->m_indexListProp->operator()();

  const auto &indexInfo =
      boost::dynamic_pointer_cast<API::MatrixWorkspace>(getWorkspace())
          ->indexInfo();

  // If no indices provided, then assume all
  if (list.size() == 0)
    return indexInfo.makeIndexSet();

  // Determine if input is a pure range or just a list of numbers
  auto res = std::minmax_element(list.begin(), list.end());
  auto minIndex = res.first - list.begin();
  auto maxIndex = res.second - list.begin();
  bool isPureRange = (list[maxIndex] - list[minIndex] + 1) == list.size();

  if (isPureRange) {
    auto min = list[minIndex];
    auto max = list[maxIndex];
    switch (m_indexTypeProp->selectedType()) {
    case IndexType::SpectrumNumber:
      return indexInfo.makeIndexSet(Indexing::SpectrumNumber(min),
                                    Indexing::SpectrumNumber(max));
      break;
    case IndexType::WorkspaceIndex:
      return indexInfo.makeIndexSet(Indexing::GlobalSpectrumIndex(min),
                                    Indexing::GlobalSpectrumIndex(max));
      break;
    }
  } else { // Only make a vector if we don't have a pure range
    switch (m_indexTypeProp->selectedType()) {
    case IndexType::SpectrumNumber:
      return indexInfo.makeIndexSet(
          std::vector<Indexing::SpectrumNumber>(list.begin(), list.end()));
      break;
    case IndexType::WorkspaceIndex:
      return indexInfo.makeIndexSet(
          std::vector<Indexing::GlobalSpectrumIndex>(list.begin(), list.end()));
      break;
    }
  }

  return Mantid::Indexing::SpectrumIndexSet(0);
}
} // namespace API
} // namespace Mantid