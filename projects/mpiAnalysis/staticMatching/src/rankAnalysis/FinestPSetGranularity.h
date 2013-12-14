#ifndef FINEST_PSET_GRANULARITY_H
#define FINEST_PSET_GRANULARITY_H

#include "rankAnalysis/pSet.h"
#include "rankAnalysis/rankAnalysis.h"
#include "nodeState.h"
#include <string>

//! Bound structure to represent absolute and relative bounds and the information if it
//! is a lower bound or an upper bound.
//
//! Bound is represented either as an absolute bound. In this case the value for
//! abs is true and the absolute bound value is stored in o.
//! In the case of a relative bound, the bound is represented as a fraction of
//! the maximum number of processes plus a value for an offset.
//! bound = number_of_processes * n / d + o.
//! The lower / upper bound information is represented with the is_lb_ flag.
//! It is true if it is a lower bound and fals if bound is an upper bound.
struct _Directed_Bound_
{
  _Bound_ b_;        //!< Absolute or relative bound.
  bool is_lb_;     //!< Lower / upper bound flag.

  //! default + parameterized constructor
  _Directed_Bound_(_Bound_ b = _Bound_(), bool lb = true) : b_(b), is_lb_(lb) {}

  /**
   *
   */
  _Directed_Bound_& operator=(const _Directed_Bound_& that)
  {
      this->b_ = that.b_;
      this->is_lb_ = that.is_lb_;
      return *this;
  }

  /**
   * @brief Equality comparison. Doesn't modify object.
   */
  bool operator==(const _Directed_Bound_& that) const
  {
      return (this->b_ == that.b_ && this->is_lb_ == that.is_lb_);
  }

  /**
   *
   */
  bool operator!=(const _Directed_Bound_& that) const
  {
      return !(*this == that);
  }

  //! less comparison.
  //
  //! Returns only true if 100% sure about the less fact.
  bool operator<(const _Directed_Bound_& that) const
  {
    if(this->b_ < that.b_)
      return true;
    if(this->b_ == that.b_ && this->is_lb_ && !that.is_lb_)
      return true;
    return false;
  }

  //! greater comparison.
  //
  //! Returns only true if 100% sure about the greater fact.
  bool operator>(const _Directed_Bound_& that) const
  {
    return !(*this < that || *this == that);
  }

  //! greater equal comparison.
  //
  //! Returns only true if 100% sure about the greater or equal fact.
  bool operator>=(const _Directed_Bound_& that) const
  {
    return (*this > that || *this == that);
  }

  //! less or equal comparison.
  //
  //! Returns only true if 100% sure about the less or equal fact.
  bool operator<=(const _Directed_Bound_& that) const
  {
    return (*this < that || *this == that);
  }

  //! copy constructor
  _Directed_Bound_& operator()(const _Directed_Bound_& that)
  {
    this->b_ = that.b_;
    this->is_lb_ = that.is_lb_;
    return *this;
  }

  //! copy constructor
  _Directed_Bound_& copy(const _Directed_Bound_& that)
  {
    this->b_ = that.b_;
    this->is_lb_ = that.is_lb_;
    return *this;
  }

  //! escape _Directed_Bound_ to String
  //
  //! [bound| in case of a lower bound and
  //! |bound] in case of an upper bound.
  string toStr() const
  {
    ostringstream out;
    (is_lb_) ? out << "|" : out << ">";
    out << b_.toStr();
    (is_lb_) ? out << "<" : out << "|";
    return out.str();
  }
};

//=======================================================================================
class FinestPSetGranularity: public UnstructuredPassIntraAnalysis
{
protected:
  /**
   * @brief Mandatory RankAnalysis with information about process sets at every node.
   */
  RankAnalysis* ra_;

  /**
   * @brief Set of all bounds which occur in ra_.
   */
  std::set<_Directed_Bound_> all_bounds_;

  /**
   * @brief Final vector containing all processes in minimum granularity process sets.
   */
  std::vector<PSet> all_psets_;

public:
  /**
   * @brief Constructor setting the required rank analysis and clearing the bounds and
   *        process sets vector.
   */
  FinestPSetGranularity(RankAnalysis* that_ra) : ra_(that_ra)
  {
    all_bounds_.clear();
    all_psets_.clear();
  }

  /**
   * @brief Iterate over all process sets of the data flow node and insert the bounds
   *        into the all_bounds_ vector.
   * @param func is the function that contains the DataflwoNodes n.
   * @param n DataflowNode to be visited.
   * @param state NodeState of the DataflowNode n.
   */
  void visit(const Function& func, const DataflowNode& n, NodeState& state);

  /**
   * @brief
   */
  void buildPSets();

  /**
   * @brief
   */
  std::vector<PSet> getPSets() { return all_psets_; };

  /**
   * @brief
   */
  string toStr() const;

protected:
  /**
   * @brief iterates over the PSet and fills possible gaps with the missing processes
   *        combined into consecutive process sets.
   */
  void fillWithMissingProcesses();
};
#endif
