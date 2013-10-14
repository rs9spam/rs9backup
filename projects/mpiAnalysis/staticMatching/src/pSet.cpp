#include <set>
#include "pSet.h"

//===================================================================================
std::set<unsigned int> PSet::getPSet()
{
  return pset_;
}

//===================================================================================
bool PSet::setPSet(std::set<unsigned int> pset)
{
  if(pset_ == pset)
    return false;
  return true;
}

//===================================================================================
bool PSet::pSetUnion(std::set<unsigned int> pset)
{
  if(pset_ == pset)
    return false;

  bool x = true;
  std::set<unsigned int>::iterator it;
  for(it = pset.begin(); it != pset.end(); ++it) {
    pset_.insert(*it);
  }

  return true;
}

//===================================================================================
bool PSet::pSetIntersection(std::set<unsigned int> pset)
{
//  if(pset_ == pset)
//    return false;
//
//  std::set<unsigned int> tset = pset_;
//  std::vector<unsigned int> hello(pset_.size());
//  std::vector<unsigned int>::iterator temp_it;
//  std::set<unsigned int>::iterator it;
//
//  it=std::set_intersection (pset.begin(), pset.end(),
//                            tset.begin(), tset.end(), hello.begin());
//
//
//  pset_.empty();
//  for(temp_it = hello.begin(); temp_it < hello.end(); temp_it++)
//    pset_.insert(temp_it);

  return true;
}

//===================================================================================
std::set<unsigned int> PSet::merge(PSet* ps1, PSet* ps2)
{
  std::set<unsigned int> ret_set;
//  std::set<unsigned int> tset;
//  std::set<unsigned int>::iterator it;
//  tset = ps1->getPSet;
//  for(it = tset.begin(); it < tset.end(); it++) {
//    ret_set.insert(it);
//  }
//  tset = ps2->getPSet;
//  for(it = tset.begin(); it < tset.end(); it++) {
//    ret_set.insert(it);
//  }
  return ret_set;
}





















////=======================================================================================
//PSet::PSet(bool empty_range,
//     bool lb_abs, bool hb_abs,
//     int lnum, int hnum,
//     int lden, int hden,
//     int loff, int hoff)
//{
//  this->empty_range = empty_range;
//  this->hb_abs = hb_abs;
//  this->lb_abs = lb_abs;
//  this->lnum = lnum;
//  this->hnum = hnum;
//  this->lden = lden;
//  this->hden = hden;
//  this->loff = loff;
//  this->hoff = hoff;
//}
//
////=======================================================================================
//PSet::PSet()
//{
//  this->empty_range = true;
//  this->hb_abs = true;
//  this->lb_abs = true;
//  this->lnum = 0;
//  this->hnum = 0;
//  this->lden = 1;
//  this->hden = 1;
//  this->loff = 0;
//  this->hoff = 0;
//}
//
////=======================================================================================
//bool PSet::isEmpty() { return empty_range; }
////=======================================================================================
//bool PSet::isLbAbs() { return lb_abs; }
////=======================================================================================
//bool PSet::isHbAbs() { return hb_abs; }
//
////=======================================================================================
//bool PSet::isAbsProcNumber()
//{
//  return ((lb_abs && hb_abs) || empty_range)?true:false;
//}
//
////=======================================================================================
//int PSet::getNumProc()
//{
//  if(empty_range)
//    return 0;
//  if(isAbsProcNumber())
//    return hoff-loff+1;
//  return -1;
//}
//
////=======================================================================================
//int PSet::getLnum() { return lnum; }
////=======================================================================================
//int PSet::getHnum() { return hnum; }
////=======================================================================================
//int PSet::getLden() { return lden; }
////=======================================================================================
//int PSet::getHden() { return hden; }
////=======================================================================================
//int PSet::getLoff() { return loff; }
////=======================================================================================
//int PSet::getHoff() { return hoff; }
////=======================================================================================
//void PSet::setLnum( int lnum ) { this->lnum = lnum; }
////=======================================================================================
//void PSet::setHnum( int hnum ) { this->hnum = hnum; }
////=======================================================================================
//void PSet::setLden( int lden ) { this->lden = lden; }
////=======================================================================================
//void PSet::setHden( int hden ) { this->hden = hden; }
////=======================================================================================
//void PSet::setLoff( int loff ) { this->loff = loff; }
////=======================================================================================
//void PSet::setHoff( int hoff ) { this->hoff = hoff; }
////=======================================================================================
//void PSet::setLow(bool abs, int num, int den, int off)
//{
//  this->lb_abs = abs;
//  this->lnum = num;
//  this->lden = den;
//  this->loff = off;
//}
////=======================================================================================
//void PSet::setHigh(bool abs, int num, int den, int off)
//{
//  this->hb_abs = abs;
//  this->hnum = num;
//  this->hden = den;
//  this->hoff = off;
//}
//
//
//
//
////=======================================================================================
//bool PSet::contains(bool abs, int num, int den, int off)
//{
//  //not smaller than lb
//  if(lb_abs && abs)
//    if(off<loff)
//      return false;
//  if(!lb_abs && abs)
//    return false;
//  if(!lb_abs && !abs)
//  {
//    if( ((float)this->lnum/(float)this->lden) >
//        ((float)num/(float)den) )
//      return false;
//    if( ((float)this->lnum/(float)this->lden) ==
//        ((float)num/(float)den) )
//      if(off<loff)
//        return false;
//  }
//
//  //not bigger than higher bound
//  if(hb_abs && abs)
//    if(off>hoff)
//      return false;
//  if(hb_abs && !abs)
//    return false;
//  if(!hb_abs && !abs)
//  {
//    if( ((float)this->hnum/(float)this->hden) <
//        ((float)num/(float)den) )
//      return false;
//    if( ((float)this->hnum/(float)this->hden) ==
//        ((float)num/(float)den) )
//      if(off>hoff)
//        return false;
//  }
//
//  return true;
//}
//
////=======================================================================================
//bool PSet::isUnder(bool abs, int num, int den, int off)
//{
//  //if smaller than lower bound
//  if(lb_abs && abs)
//    if(off<loff)
//      return true;
//  if(!lb_abs && abs)
//    return true;
//  if(!lb_abs && !abs)
//  {
//    if( ((float)this->lnum/(float)this->lden) >
//        ((float)num/(float)den) )
//      return true;
//    if( ((float)this->lnum/(float)this->lden) ==
//        ((float)num/(float)den) )
//      if(off<loff)
//        return true;
//  }
//  return false;
//}
//
////=======================================================================================
//bool PSet::isOver(bool abs, int num, int den, int off)
//{
//  //true if it is bigger than upper bound
//  if(hb_abs && abs)
//    if(off>hoff)
//      return true;
//  if(hb_abs && !abs)
//    return true;
//  if(!hb_abs && !abs)
//  {
//    if( ((float)this->hnum/(float)this->hden) <
//        ((float)num/(float)den) )
//      return true;
//    if( ((float)this->hnum/(float)this->hden) ==
//        ((float)num/(float)den) )
//      if(off>hoff)
//        return true;
//  }
//  return false;
//}
//
//
////=======================================================================================
//bool PSet::compareHbSmallerLb(const PSet& that) const
//{
//  if(this->hb_abs && that.lb_abs)
//    return ((this->hoff) < (that.loff - 1));
//  if(this->hb_abs)
//    return true;
//  if(that.lb_abs)
//    return false;
//
//  if( ((float)this->hnum/(float)this->hden) <
//      ((float)that.lnum/(float)that.lnum) )
//    return true;
//  if( ((float)this->hnum/(float)this->hden) >
//      ((float)that.lnum/(float)that.lnum) )
//    return false;
//  if(this->hoff < (that.loff - 1))
//    return true;
//
//  return false;
//}
//
////=======================================================================================
////consider, if no bound is Abs, return the offset from the biggest
//bool PSet::operator<(const PSet& that) const
//{
//  if(this->lb_abs && that.lb_abs)
//    return ((this->loff) < (that.loff));
//  if(this->lb_abs)
//    return true;
//  if(that.lb_abs)
//    return false;
//
//  if( ((float)this->lnum/(float)this->lden) <
//      ((float)that.lnum/(float)that.lnum) )
//    return true;
//  if( ((float)this->lnum/(float)this->lden) >
//      ((float)that.lnum/(float)that.lnum) )
//    return false;
//  if(this->loff < that.loff)
//    return true;
//
//  return false;
//}
//
////=======================================================================================
//bool PSet::operator>(const PSet& that) const
//{
//  if(this->hb_abs && that.hb_abs)
//    return ((this->hoff) > (that.hoff));
//  if(this->hb_abs)
//    return false;
//  if(that.hb_abs)
//    return true;
//
//  if( ((float)this->hnum/(float)this->hden) >
//      ((float)that.hnum/(float)that.hnum) )
//    return true;
//  if( ((float)this->hnum/(float)this->hden) <
//      ((float)that.hnum/(float)that.hnum) )
//    return false;
//  if(this->hoff > that.hoff)
//    return true;
//
//  return false;
//}
//
////=======================================================================================
//bool PSet::copy(const PSet& that)
//{
//  bool modified = this->empty_range != that.empty_range ||
//                  this->lb_abs != that.lb_abs ||
//                  this->lnum != that.lnum ||
//                  this->lden != that.lden ||
//                  this->loff != that.loff ||
//                  this->hb_abs != that.hb_abs ||
//                  this->hnum != that.hnum ||
//                  this->hden != that.hden ||
//                  this->hoff != that.hoff;
//
//  this->empty_range = that.empty_range;
//
//  this->lb_abs = that.lb_abs;
//  this->lnum = that.lnum;
//  this->lden = that.lden;
//  this->loff = that.loff;
//
//  this->hb_abs = that.hb_abs;
//  this->hnum = that.hnum;
//  this->hden = that.hden;
//  this->hoff = that.hoff;
//
//  return modified;
//}
//
//
//// ************************
//// *** NodeFact methods ***
//// ************************
//
////=======================================================================================
//string PSet::str(string indent) const
//{
//  ostringstream outs;
//  outs << "[";
//
//  // empty set
//  if(this->empty_range)
//    outs << "empty";
//
//  // one element in set
//  else if(lb_abs == hb_abs && lnum == hnum && lden == hden && loff == hoff)
//  {
//    if(this->lb_abs)
//      outs << loff;
//    else
//      outs << lnum << "/" << lden << "*size+" << loff;
//  }
//
//  // continuous set
//  else
//  {
//    if(this->lb_abs)
//      outs << loff << "..";
//    else
//      outs << lnum << "/" << lden << "*size+" << loff << "..";
//    if(this->hb_abs)
//      outs << hoff;
//    else
//      outs << hnum << "/" << hden << "*size+" << hoff;
//  }
//
//  outs << "] ";
//  return Dbg::escape(outs.str());
//}
//
////=======================================================================================
//string PSet::str(string indent)
//{
//  ostringstream outs;
//  outs << "[";
//
//  // empty set
//  if(this->isEmpty())
//    outs << "empty";
//
//  // one element in set
//  else if(lb_abs == hb_abs && lnum == hnum && lden == hden && loff == hoff)
//  {
//    if(this->lb_abs)
//      outs << loff;
//    else
//      outs << lnum << "/" << lden << "*size+" << loff;
//  }
//
//  // continuous set
//  else
//  {
//    if(this->lb_abs)
//      outs << loff << "..";
//    else
//      outs << lnum << "/" << lden << "*size+" << loff << "..";
//    if(this->hb_abs)
//      outs << hoff;
//    else
//      outs << hnum << "/" << hden << "*size+" << hoff;
//  }
//
//  outs << "] ";
//  return Dbg::escape(outs.str());
//}
//
////=======================================================================================
//// returns a copy of this node fact
//NodeFact* PSet::copy() const
//{
//        return new PSet();
//}
//


