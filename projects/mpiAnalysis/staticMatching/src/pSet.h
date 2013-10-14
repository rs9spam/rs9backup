#ifndef P_SET_H
#define P_SET_H
// Author: stoero
// Date: 28/09/2013

#include "nodeState.h"
#include <string>

#define PMAX = 8;


class PSet
{
private:

protected:
  std::set<unsigned int> pset_;

public:
  PSet() {};
  PSet(PSet* that) { *this = *that; };
  ~PSet() {};

  std::set<unsigned int> getPSet();
  bool setPSet(std::set<unsigned int> pset);
  bool pSetUnion(std::set<unsigned int> pset);
  bool pSetIntersection(std::set<unsigned int> pset);
  std::set<unsigned int> merge(PSet* ps1, PSet* ps2);
};


//===================================================================================
//class PSet : public NodeFact
//{
//private:
//  bool empty_range;  // true if the set is empty
//  bool lb_abs;
//  bool hb_abs;   // true if it is an absolute higher bound on the set
//  int lnum;
//  int hnum;     // absolute value for high bound numerator
//  int lden;
//  int hden;   // denominator of relative higher bound
//  int loff;
//  int hoff;   // offset of relative higher bound  (hb = hnum * rank / hden + hoff)
//
//public:
//
//  PSet(bool empty_range,
//       bool lb_abs, bool hb_abs,
//       int lnum, int hnum,
//       int lden, int hden,
//       int loff, int hoff);
//
//  PSet();
//  bool isEmpty();
//  bool isLbAbs();
//  bool isHbAbs();
//  bool isAbsProcNumber();
//  int getNumProc();
//
//  int getLnum();
//  int getHnum();
//  int getLden();
//  int getHden();
//  int getLoff();
//  int getHoff();
//
//  void setLnum( int lnum);
//  void setHnum( int hnum);
//  void setLden( int lden);
//  void setHden( int hden);
//  void setLoff( int loff);
//  void setHoff( int hoff);
//  void setLow(bool abs, int num, int den, int off);
//  void setHigh(bool abs, int num, int den, int off);
//
//  bool compareHbSmallerLb(const PSet& that ) const;
//
//  bool contains(bool abs, int num, int den, int off);
//  bool isUnder(bool abs, int num, int den, int off);
//  bool isOver(bool abs, int num, int den, int off);
//
//  //checks if this->lb < that->lb
//  bool operator<(const PSet& that) const;
//  //checks if this->hb > that->hb
//  bool operator>(const PSet& that) const;
//
//  bool copy(const PSet& that);
//
//  // ************************
//  // *** NodeFact methods ***
//  // ************************
//  NodeFact* copy() const;
//
//  // Returns a string representation of this set
//  string str(string indent="") const;
//
//  // Returns a string representation of this set
//  string str(string indent="");
//};
#endif
