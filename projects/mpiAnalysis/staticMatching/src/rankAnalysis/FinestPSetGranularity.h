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
struct directed_bound
{
  bound b_;        //!< Absolute or relative bound.
  bool is_lb_;     //!< Lower / upper bound flag.

  //! default + parameterized constructor
  directed_bound(bound b = bound(), bool lb = true) : b_(b), is_lb_(lb) {}

  //! assignment operator
  directed_bound& operator=(const directed_bound& that)
  {
      this->b_ = that.b_;
      this->is_lb_ = that.is_lb_;
      return *this;
  }

  //! equality comparison. doesn't modify object. therefore const.
  bool operator==(const directed_bound& that) const
  {
      return (this->b_ == that.b_ && this->is_lb_ == that.is_lb_);
  }

  //! equality comparison. doesn't modify object. therefore const.
  bool operator!=(const directed_bound& that) const
  {
      return !(*this == that);
  }

  //! less comparison.
  //
  //! Returns only true if 100% sure about the less fact.
  bool operator<(const directed_bound& that) const
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
  bool operator>(const directed_bound& that) const
  {
    return !(*this < that || *this == that);
  }

  //! greater equal comparison.
  //
  //! Returns only true if 100% sure about the greater or equal fact.
  bool operator>=(const directed_bound& that) const
  {
    return (*this > that || *this == that);
  }

  //! less or equal comparison.
  //
  //! Returns only true if 100% sure about the less or equal fact.
  bool operator<=(const directed_bound& that) const
  {
    return (*this < that || *this == that);
  }

  //! copy constructor
  directed_bound& operator()(const directed_bound& that)
  {
    this->b_ = that.b_;
    this->is_lb_ = that.is_lb_;
    return *this;
  }

  //! copy constructor
  directed_bound& copy(const directed_bound& that)
  {
    this->b_ = that.b_;
    this->is_lb_ = that.is_lb_;
    return *this;
  }

  //! escape directed_bound to String
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
  //!
  RankAnalysis* ra_;
  //!
  std::set<directed_bound> all_bounds_;
  //!
  std::vector<PSet> all_psets_;

public:
  //!
  FinestPSetGranularity(RankAnalysis* that_ra) : ra_(that_ra)
  {
    all_bounds_.clear();
    all_psets_.clear();
  }

  //!
  void visit(const Function& func, const DataflowNode& n, NodeState& state);

  //!
  void buildPSets();

  //!
  std::vector<PSet> getPSets();

  //!
  string toStr() const;

};
#endif
