#include "pSet.h"

//static int debugLevel=1;

/***********
 * PSet *
 ***********/
//=======================================================================================
PSet::PSet(bool empty, _Bound_ lb, _Bound_ hb)
{
  this->empty_ = empty;
  this->lb_ = lb;
  this->hb_ = hb;
}

//=======================================================================================
PSet::PSet()
{
  this->empty_ = true;
  this->lb_ = _Bound_();
  this->hb_ = _Bound_();
}

//=======================================================================================
PSet::PSet(bool empty, int value)
{
  this->empty_ = empty;
  lb_ = _Bound_(true, 1, 1, value);
  hb_ = _Bound_(true, 1, 1, value);
}

//=======================================================================================
PSet::PSet(const PSet& that)
{
  this->empty_ = that.empty_;
  this->lb_ = that.lb_;
  this->hb_ = that.hb_;
}

//=======================================================================================
bool PSet::isEmpty() const { return empty_; }

//=======================================================================================
bool PSet::isLbAbs() const { return lb_.abs; }

//=======================================================================================
bool PSet::isHbAbs() const { return hb_.abs; }

//=======================================================================================
bool PSet::isAbsProcNumber() const
{
  return ((lb_.abs && hb_.abs) || empty_) ? true : false;
}

//=======================================================================================
int PSet::getAbsNumProc() const
{
  if(empty_)
    return 0;
  if(isAbsProcNumber())
    return hb_.o - lb_.o + 1;
  return -1;
}

//=======================================================================================
bool PSet::isMinBound() const
{
  if(empty_)
    return false;
  if(lb_.abs)
    if(lb_.o == 0)
      return true;
  if(!lb_.abs)
    if(lb_.n == 0 && lb_.o == 0)
      return true;
  return false;
}

//=======================================================================================
bool PSet::isMaxBound() const
{
  if(empty_)
    return false;
  if(!hb_.abs && hb_.d != 0)
    if(hb_.n == hb_.d && hb_.o == -1)
      return true;
  return false;
}

//=======================================================================================
bool PSet::contains(const _Bound_& b) const
{
  return (b <= hb_ && b >= lb_);
}

//=======================================================================================
bool PSet::contains(const PSet& that) const
{
  return (lb_ <= that.lb_ && hb_ >= that.hb_);
}

//=======================================================================================
bool PSet::interleavesOrTouches(const PSet& that) const
{
  //false if
  //either p1_hb < p2_lb - 1  or p2_hb < p1_lb -1
  // compare if higher _Bound_ of p_set1 is smaller than lower _Bound_ -1 of p_set2
//  if(p_set1.compareHbSmallerLb(p_set2) ||
//     p_set2.compareHbSmallerLb(p_set1))
  if((this->hb_ >= (that.lb_ - 1)) && ((this->lb_ - 1) <= that.hb_))
    return true;

  return false;
}

//=======================================================================================
PSet PSet::combineWith(const PSet& that) const
{
  if(this->empty_)
    return that;
  if(that.empty_)
    return *this;
  if(!(this->interleavesOrTouches(that)))
    return *this;
  bool empty = false;
  _Bound_ lb, hb;

  lb = (*this < that) ? this->getLBound() : that.getLBound();
  hb = (*this > that) ? this->getHBound() : that.getHBound();

  return PSet(empty, lb, hb);
}

//=======================================================================================
PSet PSet::intersectWith(const PSet& that) const
{
  if(this->empty_ || that.empty_)
    return PSet();
  _Bound_ lb = (this->lb_ < that.lb_) ? that.lb_ : this->lb_;
  _Bound_ hb = (this->hb_ < that.hb_) ? this->hb_ : that.hb_;

  return (lb <= hb) ? PSet(false, lb, hb) : PSet();
}

//=======================================================================================
std::vector<PSet> PSet::remove(const PSet& that) const
{
  std::vector<PSet> ps_vec;
  if(this->empty_ || that.empty_ || this->lb_ > that.hb_ || this->hb_ < that.lb_)
    ps_vec.push_back(PSet(*this));
  else if(this->lb_ >= that.lb_ && this->hb_ <= that.hb_)
    ps_vec.push_back(PSet());
  else
  {
    if(this->lb_ < that.lb_)
      ps_vec.push_back(PSet(false, this->lb_, that.lb_-1 ));
    if(this->hb_ > that.hb_)
      ps_vec.push_back(PSet(false, that.hb_+1, this->hb_));
  }

  return ps_vec;
}

//=======================================================================================
_Bound_ PSet::getLBound() const { return lb_; }

//=======================================================================================
_Bound_ PSet::getHBound() const { return hb_; }

//=======================================================================================
void PSet::setLBound(const _Bound_& b) { this->lb_(b); }

//=======================================================================================
void PSet::setHBound(const _Bound_& b) { this->hb_(b); }

//=======================================================================================
bool PSet::operator==(const PSet& that) const
{
  if(this->empty_ == true && that.empty_ == true)
    return true;
  return (this->empty_ == that.empty_
          && this->lb_ == that.lb_
          && this->hb_ == that.hb_);
}

//=======================================================================================
bool PSet::operator!=(const PSet& that) const
{
  return !(*this == that);
}

//=======================================================================================
//consider, if no _Bound_ is Abs, return the offset from the biggest
bool PSet::operator<(const PSet& that) const
{
  return lb_ < that.lb_;
}

//=======================================================================================
bool PSet::operator>(const PSet& that) const
{
  return hb_ > that.hb_;
}

//=======================================================================================
bool PSet::operator<=(const PSet& that) const
{
  return (*this < that || *this == that);
}

//=======================================================================================
bool PSet::operator>=(const PSet& that) const
{
  return (*this > that || *this == that);
}

//=======================================================================================
bool PSet::operator=(const PSet& that)
{
  return this->copy(that);
}

//=======================================================================================
bool PSet::copy(const PSet& that)
{
  bool modified = *this != that;
  this->empty_ = that.empty_;
  this->lb_ = that.lb_;
  this->hb_ = that.hb_;
  return modified;
}

// ************************
// *** NodeFact methods ***
// ************************
//=======================================================================================
string PSet::toString() const
{
  ostringstream outs;
  outs << "[";

  // empty set
  if(this->empty_)
    outs << "empty";

  // one element in set
  else if(lb_ == hb_)
    outs << lb_.toStr();

  // continuous set
  else
    outs << lb_.toStr() << ".." << hb_.toStr();

  outs << "] ";
  return Dbg::escape(outs.str());
}

//=======================================================================================
string PSet::str(string indent)
{
  ostringstream outs;
  outs << indent;
  outs << this->toString();
  return Dbg::escape(outs.str());
}

//=======================================================================================
// returns a copy of this node fact
// TODO verify usage of this function ....
NodeFact* PSet::copy() const
{
  return new PSet(*this);
}
