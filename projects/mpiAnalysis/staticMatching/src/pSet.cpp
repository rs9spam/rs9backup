#include "pSet.h"

//static int debugLevel=1;

/***********
 * PSet *
 ***********/
//=======================================================================================
PSet::PSet(bool empty, bound lb, bound hb)
{
  this->empty_ = empty;
  this->lb_ = lb;
  this->hb_ = hb;
}

//=======================================================================================
PSet::PSet(bool empty,
     bool lb_abs, bool hb_abs,
     int lnum, int hnum,
     int lden, int hden,
     int loff, int hoff)
{
  this->empty_ = empty;
  this->lb_ = bound(lb_abs, lnum, lden, loff);
  this->hb_ = bound(hb_abs, hnum, hden, hoff);
}

//=======================================================================================
PSet::PSet()
{
  this->empty_ = true;
  this->lb_ = bound();
  this->hb_ = bound();
}

//=======================================================================================
bool PSet::isEmpty() { return empty_; }

//=======================================================================================
bool PSet::isLbAbs() { return lb_.abs; }

//=======================================================================================
bool PSet::isHbAbs() { return hb_.abs; }

//=======================================================================================
bool PSet::isAbsProcNumber()
{
  return ((lb_.abs && hb_.abs) || empty_) ? true : false;
}

//=======================================================================================
int PSet::getAbsNumProc()
{
  if(empty_)
    return 0;
  if(isAbsProcNumber())
    return hb_.o - lb_.o + 1;
  return -1;
}

//=======================================================================================
bool PSet::isMinBound()
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
bool PSet::isMaxBound()
{
  if(empty_)
    return false;
  if(!hb_.abs && hb_.d != 0)
    if(hb_.n == hb_.d && hb_.o == -1)
      return true;
  return false;
}

//=======================================================================================
//Returns true if 100% sure about the decision
bool PSet::contains(bound b)
{
  return (b <= hb_ && b >= lb_);
}

//=======================================================================================
bool PSet::interleavesOrTouches(const PSet& that) const
{
  //false if
  //either p1_hb < p2_lb - 1  or p2_hb < p1_lb -1
  // compare if higher bound of p_set1 is smaller than lower bound -1 of p_set2
//  if(p_set1.compareHbSmallerLb(p_set2) ||
//     p_set2.compareHbSmallerLb(p_set1))
  if((this->hb_ >= (that.lb_ - 1)) && ((this->lb_ - 1) <= that.hb_))
    return true;

  return false;
}

//=======================================================================================
PSet PSet::combineWith(const PSet& p) const
{
  if(this->empty_)
    return p;
  if(p.empty_)
    return *this;
  if(!this->interleavesOrTouches(p))
    return *this;
  bool empty = false;
  bound lb, hb;

  if(*this < p)
    lb = this->getLBound();
  else
    lb = p.getLBound();

  if(*this > p)
    hb = this->getHBound();
  else
    hb = p.getHBound();

  return PSet(empty, lb, hb);
}

//=======================================================================================
PSet PSet::intersectWith(const PSet& p) const
{
  //TODO:
  return PSet();
}

//=======================================================================================
int PSet::getLnum() const { return lb_.n; }

//=======================================================================================
int PSet::getHnum() const { return hb_.n; }

//=======================================================================================
int PSet::getLden() const { return lb_.d; }

//=======================================================================================
int PSet::getHden() const { return hb_.d; }

//=======================================================================================
int PSet::getLoff() const { return lb_.o; }

//=======================================================================================
int PSet::getHoff() const { return hb_.o; }

//=======================================================================================
bound PSet::getLBound() const { return lb_; }

//=======================================================================================
bound PSet::getHBound() const { return hb_; }

//=======================================================================================
void PSet::setLnum( const int& lnum ) { this->lb_.n = lnum; }

//=======================================================================================
void PSet::setHnum( const int& hnum ) { this->hb_.n = hnum; }

//=======================================================================================
void PSet::setLden( const int& lden ) { this->lb_.d = lden; }

//=======================================================================================
void PSet::setHden( const int& hden ) { this->hb_.d = hden; }

//=======================================================================================
void PSet::setLoff( const int& loff ) { this->lb_.o = loff; }

//=======================================================================================
void PSet::setHoff( const int& hoff ) { this->hb_.o = hoff; }

//=======================================================================================
void PSet::setLow(bool abs, int num, int den, int off)
{
  this->lb_ = bound(abs, num, den, off);
}

//=======================================================================================
void PSet::setHigh(bool abs, int num, int den, int off)
{
  this->hb_ = bound(abs, num, den, off);
}

//=======================================================================================
void PSet::setLBound(const bound& b) { this->lb_(b); }

//=======================================================================================
void PSet::setHBound(const bound& b) { this->hb_(b); }

#if 0
////=======================================================================================
//bool PSet::isUnder(bound b)
//{
//  return isUnder(b.abs, b.n, b.d, b.o);
//}
//
////=======================================================================================
//bool PSet::isUnder(bool abs, int num, int den, int off)
//{
//  //if smaller than lower bound
//  if(lb_.abs && abs)
//    if(off < lb_.o)
//      return true;
//  if(!lb_.abs && abs)
//    return true;
//  if(!lb_.abs && !abs)
//  {
//    if( ((float)this->lb_.n/(float)this->lb_.d) >
//        ((float)num/(float)den) )
//      return true;
//    if( ((float)this->lb_.n/(float)this->lb_.d) ==
//        ((float)num/(float)den) )
//      if(off<lb_.o)
//        return true;
//  }
//  return false;
//}
//
////=======================================================================================
//bool PSet::isOver(bound b)
//{
//  return isOver(b.abs, b.n, b.d, b.o);
//}
//
////=======================================================================================
//bool PSet::isOver(bool abs, int num, int den, int off)
//{
//  //true if it is bigger than upper bound
//  if(hb_.abs && abs)
//    if(off>hb_.o)
//      return true;
//  if(hb_.abs && !abs)
//    return true;
//  if(!hb_.abs && !abs)
//  {
//    if( ((float)this->hb_.n/(float)this->hb_.d) <
//        ((float)num/(float)den) )
//      return true;
//    if( ((float)this->hb_.n/(float)this->hb_.d) ==
//        ((float)num/(float)den) )
//      if(off>hb_.o)
//        return true;
//  }
//  return false;
//}
////=======================================================================================
//bool PSet::compareHbSmallerLb(const PSet& that) const
//{
//  return (this->hb_ < that.lb_);
//}
#endif

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
//consider, if no bound is Abs, return the offset from the biggest
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
string PSet::str(string indent) const
{
  ostringstream outs;
  outs << "[";

  // empty set
  if(this->empty_)
    outs << "empty";

  // one element in set
  else if(lb_ == hb_)
  {
    if(this->lb_.abs)
      outs << lb_.o;
    else
      outs << lb_.n << "/" << lb_.d << "*size+" << lb_.o;
  }

  // continuous set
  else
  {
    if(this->lb_.abs)
      outs << lb_.o << "..";
    else
      outs << lb_.n << "/" << lb_.d << "*size+" << lb_.o << "..";
    if(this->hb_.abs)
      outs << hb_.o;
    else
      outs << hb_.n << "/" << hb_.d << "*size+" << hb_.o;
  }

  outs << "] ";
  return Dbg::escape(outs.str());
}

//=======================================================================================
string PSet::str(string indent)
{
  ostringstream outs;
  outs << "[";

  // empty set
  if(this->isEmpty())
    outs << "empty";

  // one element in set
  else if(lb_ == hb_)
  {
    if(this->lb_.abs)
      outs << lb_.o;
    else
      outs << lb_.n << "/" << lb_.d << "*size+" << lb_.o;
  }

  // continuous set
  else
  {
    if(this->lb_.abs)
      outs << lb_.o << "..";
    else
      outs << lb_.n << "/" << lb_.d << "*size+" << lb_.o << "..";
    if(this->hb_.abs)
      outs << hb_.o;
    else
      outs << hb_.n << "/" << hb_.d << "*size+" << hb_.o;
  }

  outs << "] ";
  return Dbg::escape(outs.str());
}

//=======================================================================================
// returns a copy of this node fact
// TODO verify usage of this function ....
NodeFact* PSet::copy() const
{
  return new PSet(*this);
}
