#include "pSet.h"

static int debugLevel=1;

/***********
 * PSet *
 ***********/

//=======================================================================================
PSet::PSet(bool empty_range,
     bool lb_abs, bool hb_abs,
     int lnum, int hnum,
     int lden, int hden,
     int loff, int hoff)
{
  this->empty_range = empty_range;
  this->hb_abs = hb_abs;
  this->lb_abs = lb_abs;
  this->lnum = lnum;
  this->hnum = hnum;
  this->lden = lden;
  this->hden = hden;
  this->loff = loff;
  this->hoff = hoff;
}

//=======================================================================================
PSet::PSet()
{
  this->empty_range = true;
  this->hb_abs = true;
  this->lb_abs = true;
  this->lnum = 0;
  this->hnum = 0;
  this->lden = 1;
  this->hden = 1;
  this->loff = 0;
  this->hoff = 0;
}

//=======================================================================================
bool PSet::isEmpty() { return empty_range; }
//=======================================================================================
bool PSet::isLbAbs() { return lb_abs; }
//=======================================================================================
bool PSet::isHbAbs() { return hb_abs; }

//=======================================================================================
bool PSet::isAbsProcNumber()
{
  return ((lb_abs && hb_abs) || empty_range)?true:false;
}

//=======================================================================================
int PSet::getNumProc()
{
  if(empty_range)
    return 0;
  if(isAbsProcNumber())
    return hoff-loff+1;
  return -1;
}

//=======================================================================================
int PSet::getLnum() { return lnum; }
//=======================================================================================
int PSet::getHnum() { return hnum; }
//=======================================================================================
int PSet::getLden() { return lden; }
//=======================================================================================
int PSet::getHden() { return hden; }
//=======================================================================================
int PSet::getLoff() { return loff; }
//=======================================================================================
int PSet::getHoff() { return hoff; }
//=======================================================================================
void PSet::setLnum( int lnum ) { this->lnum = lnum; }
//=======================================================================================
void PSet::setHnum( int hnum ) { this->hnum = hnum; }
//=======================================================================================
void PSet::setLden( int lden ) { this->lden = lden; }
//=======================================================================================
void PSet::setHden( int hden ) { this->hden = hden; }
//=======================================================================================
void PSet::setLoff( int loff ) { this->loff = loff; }
//=======================================================================================
void PSet::setHoff( int hoff ) { this->hoff = hoff; }
//=======================================================================================
void PSet::setLow(bool abs, int num, int den, int off)
{
  this->lb_abs = abs;
  this->lnum = num;
  this->lden = den;
  this->loff = off;
}
//=======================================================================================
void PSet::setHigh(bool abs, int num, int den, int off)
{
  this->hb_abs = abs;
  this->hnum = num;
  this->hden = den;
  this->hoff = off;
}




//=======================================================================================
bool PSet::contains(bool abs, int num, int den, int off)
{
  //not smaller than lb
  if(lb_abs && abs)
    if(off<loff)
      return false;
  if(!lb_abs && abs)
    return false;
  if(!lb_abs && !abs)
  {
    if( ((float)this->lnum/(float)this->lden) >
        ((float)num/(float)den) )
      return false;
    if( ((float)this->lnum/(float)this->lden) ==
        ((float)num/(float)den) )
      if(off<loff)
        return false;
  }

  //not bigger than higher bound
  if(hb_abs && abs)
    if(off>hoff)
      return false;
  if(hb_abs && !abs)
    return false;
  if(!hb_abs && !abs)
  {
    if( ((float)this->hnum/(float)this->hden) <
        ((float)num/(float)den) )
      return false;
    if( ((float)this->hnum/(float)this->hden) ==
        ((float)num/(float)den) )
      if(off>hoff)
        return false;
  }

  return true;
}

//=======================================================================================
bool PSet::isUnder(bool abs, int num, int den, int off)
{
  //if smaller than lower bound
  if(lb_abs && abs)
    if(off<loff)
      return true;
  if(!lb_abs && abs)
    return true;
  if(!lb_abs && !abs)
  {
    if( ((float)this->lnum/(float)this->lden) >
        ((float)num/(float)den) )
      return true;
    if( ((float)this->lnum/(float)this->lden) ==
        ((float)num/(float)den) )
      if(off<loff)
        return true;
  }
  return false;
}

//=======================================================================================
bool PSet::isOver(bool abs, int num, int den, int off)
{
  //true if it is bigger than upper bound
  if(hb_abs && abs)
    if(off>hoff)
      return true;
  if(hb_abs && !abs)
    return true;
  if(!hb_abs && !abs)
  {
    if( ((float)this->hnum/(float)this->hden) <
        ((float)num/(float)den) )
      return true;
    if( ((float)this->hnum/(float)this->hden) ==
        ((float)num/(float)den) )
      if(off>hoff)
        return true;
  }
  return false;
}


//=======================================================================================
bool PSet::compareHbSmallerLb(const PSet& that) const
{
  if(this->hb_abs && that.lb_abs)
    return ((this->hoff) < (that.loff - 1));
  if(this->hb_abs)
    return true;
  if(that.lb_abs)
    return false;

  if( ((float)this->hnum/(float)this->hden) <
      ((float)that.lnum/(float)that.lnum) )
    return true;
  if( ((float)this->hnum/(float)this->hden) >
      ((float)that.lnum/(float)that.lnum) )
    return false;
  if(this->hoff < (that.loff - 1))
    return true;

  return false;
}

//=======================================================================================
//consider, if no bound is Abs, return the offset from the biggest
bool PSet::operator<(const PSet& that) const
{
  if(this->lb_abs && that.lb_abs)
    return ((this->loff) < (that.loff));
  if(this->lb_abs)
    return true;
  if(that.lb_abs)
    return false;

  if( ((float)this->lnum/(float)this->lden) <
      ((float)that.lnum/(float)that.lnum) )
    return true;
  if( ((float)this->lnum/(float)this->lden) >
      ((float)that.lnum/(float)that.lnum) )
    return false;
  if(this->loff < that.loff)
    return true;

  return false;
}

//=======================================================================================
bool PSet::operator>(const PSet& that) const
{
  if(this->hb_abs && that.hb_abs)
    return ((this->hoff) > (that.hoff));
  if(this->hb_abs)
    return false;
  if(that.hb_abs)
    return true;

  if( ((float)this->hnum/(float)this->hden) >
      ((float)that.hnum/(float)that.hnum) )
    return true;
  if( ((float)this->hnum/(float)this->hden) <
      ((float)that.hnum/(float)that.hnum) )
    return false;
  if(this->hoff > that.hoff)
    return true;

  return false;
}

//=======================================================================================
bool PSet::copy(const PSet& that)
{
  bool modified = this->empty_range != that.empty_range ||
                  this->lb_abs != that.lb_abs ||
                  this->lnum != that.lnum ||
                  this->lden != that.lden ||
                  this->loff != that.loff ||
                  this->hb_abs != that.hb_abs ||
                  this->hnum != that.hnum ||
                  this->hden != that.hden ||
                  this->hoff != that.hoff;

  this->empty_range = that.empty_range;

  this->lb_abs = that.lb_abs;
  this->lnum = that.lnum;
  this->lden = that.lden;
  this->loff = that.loff;

  this->hb_abs = that.hb_abs;
  this->hnum = that.hnum;
  this->hden = that.hden;
  this->hoff = that.hoff;

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
  if(this->empty_range)
    outs << "empty";

  // one element in set
  else if(lb_abs == hb_abs && lnum == hnum && lden == hden && loff == hoff)
  {
    if(this->lb_abs)
      outs << loff;
    else
      outs << lnum << "/" << lden << "*size+" << loff;
  }

  // continuous set
  else
  {
    if(this->lb_abs)
      outs << loff << "..";
    else
      outs << lnum << "/" << lden << "*size+" << loff << "..";
    if(this->hb_abs)
      outs << hoff;
    else
      outs << hnum << "/" << hden << "*size+" << hoff;
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
  else if(lb_abs == hb_abs && lnum == hnum && lden == hden && loff == hoff)
  {
    if(this->lb_abs)
      outs << loff;
    else
      outs << lnum << "/" << lden << "*size+" << loff;
  }

  // continuous set
  else
  {
    if(this->lb_abs)
      outs << loff << "..";
    else
      outs << lnum << "/" << lden << "*size+" << loff << "..";
    if(this->hb_abs)
      outs << hoff;
    else
      outs << hnum << "/" << hden << "*size+" << hoff;
  }

  outs << "] ";
  return Dbg::escape(outs.str());
}

//=======================================================================================
// returns a copy of this node fact
NodeFact* PSet::copy() const
{
        return new PSet();
}
























//// The number of variables that have been generated as range lower and upper bounds
//int contRangePSet::varCount=0;
//
//varID contRangePSet::genFreshVar(string prefix)
//{
//        ostringstream outs;
//        if(prefix=="") outs << "boundVar_"<< varCount;
//        else           outs << prefix<<"_" << varCount;
//        varCount++;
//        varID var(outs.str());
//        //Dbg::dbg << outs.str()<<" : "<<var.str()<<endl;;
//        return var;
//}
//
//contRangePSet::contRangePSet(bool emptyRange)
//{
//        lb = genFreshVar("LB");
//        ub = genFreshVar("UB");
//        cg = NULL;
//        emptyRange = emptyRange;
//        valid = true;
//}
//
//contRangePSet::contRangePSet(varID lb, varID ub, ConstrGraph* cg)
//{
//        this->cg = cg;
//        this->lb = lb;
//        this->ub = ub;
//        genFreshBounds();
//        emptyRange = false;
//        valid = true;
//        //ROSE_ASSERT(cg->lteVars(this->lb, this->ub));
//}
//
//// this->lb*lbA = lb*lbB + lbC
//// this->ub*ubA = ub*ubB + ubC
//contRangePSet::contRangePSet(varID lb, int lbA, int lbB, int lbC,
//                                   varID ub, int ubA, int ubB, int ubC,
//                                   ConstrGraph* cg)
//{
//        this->cg = cg;
//        this->lb = lb;
//        this->ub = ub;
//        genFreshBounds(lbA, lbB, lbC, ubA, ubB, ubC);
//        emptyRange = false;
//        valid = true;
//        //ROSE_ASSERT(cg->lteVars(this->lb, this->ub));
//}
//
//contRangePSet::contRangePSet(varID lb, int lbA, int lbB, int lbC,
//                                   varID ub, int ubA, int ubB, int ubC,
//                                   string annotName, void* annot,
//                                   ConstrGraph* cg)
//{
//        this->cg = cg;
//        this->lb = lb;
//        this->ub = ub;
//        genFreshBounds(lbA, lbB, lbC, ubA, ubB, ubC, annotName, annot);
//        emptyRange = false;
//        valid = true;
//        //ROSE_ASSERT(cg->lteVars(this->lb, this->ub));
//}
//
//// if freshenVars==true, calls genFreshBounds() to make this set use different lower and upper
//// bound variables from that set, while ensuring that the bound variables themselves are
//// equal to each other in cg
//contRangePSet::contRangePSet(const contRangePSet& that, bool freshenVars)
//{
//        lb = that.lb;
//        ub = that.ub;
//        cg = that.cg;
//
//        emptyRange = that.emptyRange;
//        valid = that.valid;
//        if(freshenVars) genFreshBounds();
//        if(cg) {
//                if(!(cg->lteVars(lb, ub, 1, 1, 0, "    "))) {
//                        Dbg::dbg << "contRangePSet::contRangePSet() ASSERT FAILURE: ROSE_ASSERT(cg-&lt;lteVars("<<lb<<", "<<ub<<", 1, 1, 0)) cg="<<endl;
//                        Dbg::dbg << cg->str() << endl;
//                }
//                ROSE_ASSERT(cg->lteVars(lb, ub, 1, 1, 0, "    "));
//        }
//}
//
//// copies that to this, returning true if this is not changed and false otherwise
//bool contRangePSet::copy(const contRangePSet& that)
//{
//        bool modified =
//                lb != that.lb ||
//                ub != that.ub ||
//                cg != that.cg ||
//                emptyRange != that.emptyRange ||
//                valid != that.valid;
//        lb = that.lb;
//        ub = that.ub;
//        cg = that.cg;
//        emptyRange = that.emptyRange;
//        valid = that.valid;
//
//        return modified;
//}
//
//// Returns the current value of lb
//const varID& contRangePSet::getLB() const
//{
//        return lb;
//}
//
//// Returns the current value of ub
//const varID& contRangePSet::getUB() const
//{
//        return ub;
//}
//
//// Returns the current value of cg
//ConstrGraph* contRangePSet::getConstr() const
//{
//        return cg;
//}
//
//// Sets lb, returning true if this causes this process set to change
//// and false otherwise
//bool contRangePSet::setLB(const varID& lb)
//{
//        bool modified = cg->eqVars(this->lb, lb, "    ");
//        this->lb = lb;
//        return modified;
//}
//
//// Sets ub, returning true if this causes this process set to change
//// and false otherwise
//bool contRangePSet::setUB(const varID& ub)
//{
//        bool modified = cg->eqVars(this->ub, ub, "    ");
//        this->ub = ub;
//        return modified;
//}
//
//// Do not modify the lower bound but add the constraint that it is equal lb:
//// lb*a = newLB*b + c
//bool contRangePSet::assignLB(const varID& newLB, int a, int b, int c)
//{
//        bool modified=false;
//        //Dbg::dbg << "assignLB("<<lb<<" => "<<newLB<<")"<<endl;
//        cg->eraseVarConstr(lb);
//        //Dbg::dbg << "erased LB "<<lb<<" cg = "<<cg<<" = "<<cg->str()<<endl;
//        modified = cg->assertCond(lb, newLB, a, b, c) || modified;
//        modified = cg->assertCond(newLB, lb, b, a, 0-c) || modified;
//        return modified;
//}
//
//// Do not modify the upper bound but add the constraint that it is equal ub:
//// ub*a = newUB*b + c
//bool contRangePSet::assignUB(const varID& newUB, int a, int b, int c)
//{
//        bool modified=false;
//        //Dbg::dbg << "assignUB("<<ub<<" => "<<newUB<<")"<<endl;
//        cg->eraseVarConstr(ub);
//        //Dbg::dbg << "erased UB "<<ub<<" cg = "<<cg<<" = "<<cg->str()<<endl;
//        modified = cg->assertCond(ub, newUB, a, b, c) || modified;
//        modified = cg->assertCond(newUB, ub, b, a, 0-c) || modified;
//        return modified;
//}
//
//// Sets cg, returning true if this causes this process set to change
//// and false otherwise
//bool contRangePSet::setConstr(ConstrGraph* cg)
//{
//        bool modified = this->cg != cg;
//        this->cg = cg;
//        return modified;
//}
//
//// Asserts all the set-specific invariants in this set's constraint graph
//void contRangePSet::refreshInvariants()
//{
//        ROSE_ASSERT(cg);
//
//        cg->addVar(lb);
//        cg->addVar(ub);
//        // lb <= ub
//        /*GB 2011-06-01 NO-LB-UB-rankVar : cg->assertCond(lb, ub, 1, 1, 0);
//
//        cg->localTransClosure(lb);
//        cg->localTransClosure(ub);*/
//}
//
//// Generates new lower and upper bound variables that are set to be equal to the original
//// lower and upper bound variables in this pSet's constraint graph as:
//// newLB*lbA = oldLB*lbB + lbC and newUB*ubA = oldUB*ubB + ubC
//// If annotName!="", the bounds variables will have the annotation annotName->annot
//void contRangePSet::genFreshBounds(int lbA, int lbB, int lbC,
//                                      int ubA, int ubB, int ubC,
//                                      string annotName, void* annot)
//{
//        const varID oldLB = lb;
//        lb = genFreshVar("LB");
//        if(annotName != "") lb.addAnnotation(annotName, annot);
//        cg->addVar(lb);
//        Dbg::dbg << "    Assigning="<<lbA<<"*"<<lb<<" = "<<lbB<<"*"<<oldLB<<" + "<<lbC<<endl;
//        //cg->assertCond(lb, oldLB, lbA, lbB, lbC);
//        //cg->assertCond(oldLB, lb, lbB, lbA, 0-lbC);
//        cg->assign(lb, oldLB, lbA, lbB, lbC);
//
//        const varID oldUB = ub;
//        ub = genFreshVar("UB");
//        if(annotName != "") ub.addAnnotation(annotName, annot);
//        cg->addVar(ub);
//        Dbg::dbg << "    Assigning="<<ubA<<"*"<<ub<<" = "<<ubB<<"*"<<oldUB<<" + "<<ubC<<endl;
//        //cg->assertCond(ub, oldUB, ubA, ubB, ubC);
//        //cg->assertCond(oldUB, ub, ubB, ubA, 0-ubC);
//        cg->assign(ub, oldUB, ubA, ubB, ubC);
//
//        // lb <= ub
//        cg->assertCond(lb, ub, 1, 1, 0);
//        cg->localTransClosure(lb);
//        cg->localTransClosure(ub);
//}
//
//// Transition from using the current constraint graph to using newCG, while annotating
//// the lower and upper bound variables with the given annotation annotName->annot.
//// Return true if this causes this set to change, false otherwise.
//bool contRangePSet::setConstrAnnot(ConstrGraph* newCG, string annotName, void* annot)
//{
//        bool modified = cg!=newCG;
//        cg = newCG;
//        modified = lb.addAnnotation(annotName, annot) || modified;
//        modified = ub.addAnnotation(annotName, annot) || modified;
//        return modified;
//}
//
//// Add the given annotation to the lower and upper bound variables
//// Return true if this causes this set to change, false otherwise.
//bool contRangePSet::addAnnotation(string annotName, void* annot)
//{
//        bool modified = false;
//        modified = lb.addAnnotation(annotName, annot) || modified;
//        modified = ub.addAnnotation(annotName, annot) || modified;
//        return modified;
//}
//
//// Remove the given annotation from this set's lower and upper bound variables
//// Return true if this causes this set to change, false otherwise.
//bool contRangePSet::removeVarAnnot(string annotName)
//{
//        bool modified = false;
//        modified = lb.remAnnotation(annotName) || modified;
//        modified = ub.remAnnotation(annotName) || modified;
//        return modified;
//}
//
//// Make this set invalid, return true if this causes the set to change, false otherwise
//bool contRangePSet::invalidate()
//{
//        bool modified = (valid == true);
//
//        emptyRange = false;
//        valid = false;
//
//        return modified;
//}
//
//// Make this set valid, return true if this causes the set to change, false otherwise
//bool contRangePSet::makeValid()
//{
//        bool modified = (valid == false);
//
//        emptyRange = false;
//        valid = true;
//
//        return modified;
//}
//
//// Make this set empty, return true if this causes the set to change, false otherwise.
//// Also, remove the set's lower and upper bounds from its associated constraint graph (if any).
//bool contRangePSet::emptify()
//{
//        bool modified = (emptyRange == false || valid == false);
//
//        // Disconnect the lower and upper bounds of this process set from its constraint
//        // graph since the process set is now empty and thus its lower and upper bounds don't exist.
//        modified = cgDisconnect() || modified;
//
//        emptyRange = true;
//        valid = true;
//
//        return modified;
//}
//
//// Make this set non-empty, return true if this causes the set to change, false otherwise
//bool contRangePSet::makeNonEmpty()
//{
//        bool modified = (emptyRange == true || valid == false);
//
//        emptyRange = false;
//        valid = true;
//
//        return modified;
//}
//
//// Returns true if this and that represent the equal ranges
//bool contRangePSet::rangeEq(const contRangePSet& that) const
//{
//        ROSE_ASSERT(cg == that.cg);
//        //Dbg::dbg << "contRangePSet::rangeEq: cg->eqVars("<<lb.str()<<", "<<that.lb.str()<<")="<<cg->eqVars(lb, that.lb)<<endl;
//        //Dbg::dbg << "contRangePSet::rangeEq: cg->eqVars("<<ub.str()<<", "<<that.ub.str()<<")="<<cg->eqVars(ub, that.ub)<<endl;
//        return (cg->eqVars(lb, that.lb, "    ") && cg->eqVars(ub, that.ub, "    "));
//}
//
//// Returns true if that is at the top of but not equal to this's range
//bool contRangePSet::rangeTop(const contRangePSet& that) const
//{
//        ROSE_ASSERT(cg == that.cg);
//             // [lb<=that.lb <= (ub=that.ub)]
//        //Dbg::dbg << "cg->lteVars("<<lb.str()<<", "<<that.lb.str()<<")="<<cg->lteVars(lb, that.lb)<<endl;
//        //Dbg::dbg << "cg->eqVars("<<ub.str()<<", "<<that.ub.str()<<")="<<cg->eqVars(ub, that.ub)<<endl;
//        return (cg->lteVars(lb, that.lb, 1, 1, 0, "    ") && cg->eqVars(ub, that.ub, "    "));
//}
//
//// Returns true if that is at the bottom of but not equal to this's range
//bool contRangePSet::rangeBottom(const contRangePSet& that) const
//{
//        //Dbg::dbg << "rangeBottom: cg->eqVars("<<lb.str()<<", "<<that.lb.str()<<")="<<cg->eqVars(lb, that.lb)<<endl;
//        //Dbg::dbg << "rangeBottom: cg->lteVars("<<that.ub.str()<<", "<<ub.str()<<")="<<cg->lteVars(that.ub, ub)<<endl;
//        ROSE_ASSERT(cg == that.cg);
//            // [(lb=that.lb) <= that.ub<=ub]
//        return (cg->eqVars(lb, that.lb, "    ") && cg->lteVars(that.ub, ub, 1, 1, 0, "    "));
//}
//
//// Returns true if the ranges of this and that must be disjoint
//bool contRangePSet::rangeDisjoint(const contRangePSet& that) const
//{
//            // [lb, ub] < [that.lb, that.ub] or [that.lb, that.ub] < [lb, ub]
//        return (cg->ltVars(ub, that.lb) || cg->ltVars(that.ub, lb));
//}
//
//// Returns true if this must overlap, with this Above that: that.lb <= lb <= that.ub <= ub
//bool contRangePSet::overlapAbove(const contRangePSet& that) const
//{
//            // that.lb <= lb <= that.ub <= ub
//        return cg->lteVars(that.lb, lb, 1, 1, 0, "    ") &&
//               cg->lteVars(lb, that.ub, 1, 1, 0, "    ") &&
//               cg->lteVars(that.ub, ub, 1, 1, 0, "    ");
//}
//
//// Returns true if this must overlap, with this Below that: lb <= that.lb <= ub <= that.ub
//bool contRangePSet::overlapBelow(const contRangePSet& that) const
//{
//            // lb <= that.lb <= ub <= that.ub
//        return cg->lteVars(lb, that.lb, 1, 1, 0, "    ") &&
//               cg->lteVars(that.lb, ub, 1, 1, 0, "    ") &&
//               cg->lteVars(ub, that.ub, 1, 1, 0, "    ");
//}
//
//// Copies the given variable's constraints from srcCG to cg.
//// !!! May want to label the inserted variables with this pSet object to avoid collisions
//// !!! with other pSet objects.
//void contRangePSet::copyVar(ConstrGraph* srcCG, const varID& var)
//{
//        cg->copyVar(srcCG, var);
//}
//
//// returns a heap-allocated reference to the intersection of this and that
//// if transClosure==true, performs the appropriate (possibly optimized) transitive closure on cg
//pSet& contRangePSet::intersect(const pSet& that_arg) const
//{
//        const contRangePSet& that = dynamic_cast<const contRangePSet&>(that_arg);
//        return intersect(that, false);
//}
//
//pSet& contRangePSet::intersect(const contRangePSet& that, bool transClosure) const
//{
//        //const contRangePSet& that = dynamic_cast<const contRangePSet&>(that_arg);
//        contRangePSet* thisCopy = new contRangePSet(*this);
//        thisCopy->genFreshBounds();
//        //Dbg::dbg << "thisCopy="<<thisCopy<<endl;
//        thisCopy->intersectUpd(that, transClosure);
//        return *thisCopy;
//        /*const contRangePSet& that = (const contRangePSet&)that_arg;
//
//        // If the ranges are equal
//        if(rangeEq(that))
//        {
//                contRangePSet* thisCopy = new contRangePSet(*this);
//                return *thisCopy;
//        }
//        // If that is inside this' range
//        else if(rangeTop(that) || rangeBottom(that))
//        {
//                contRangePSet* thatCopy = new contRangePSet(that);
//                return *thatCopy;
//        }
//        // If this and that have an empty intersection
//        else if(rangeDisjoint(that))
//        {
//                emptyPSet* e = new emptyPSet();
//                return *e;
//        }
//        // If this and that must overlap with this Below that: lb <= that.lb <= ub <= that.ub
//        else if(overlapBelow(that))
//        {
//                contRangePSet* ret = new contRangePSet(that.lb, ub, cg);
//                return *ret;
//        }
//        // If this and that must overlap with this Above that: that.lb <= lb <= that.ub <= ub
//        else if(overlapAbove(that))
//        {
//                contRangePSet* ret = new contRangePSet(lb, that.ub, cg);
//                return *ret;
//        }
//        // Else, this is an un-representable set
//        else
//        {
//                invalidPSet* i = new invalidPSet();
//                return *i;
//        }*/
//}
//
//// updates this with the intersection of this and that
//// returns true if this causes the pSet to change, false otherwise
//// if transClosure==true, performs the appropriate (possibly optimized) transitive closure on cg
//bool contRangePSet::intersectUpd(const pSet& that_arg)
//{
//        const contRangePSet& that = dynamic_cast<const contRangePSet&>(that_arg);
//        return intersectUpd(that, false);
//}
//
//bool contRangePSet::intersectUpd(const contRangePSet& that, bool transClosure)
//{
//        bool modified = false;
//
//        //Dbg::dbg << "contRangePSet::intersectUpd"<<endl;
//
//        // If the ranges are equal
//        if(rangeEq(that))
//        { if(debugLevel>0) Dbg::dbg << "contRangePSet::intersectUpd equal ranges"<<endl; }
//        // If that is inside this' range
//        else if(rangeTop(that) || rangeBottom(that))
//        {
//                if(debugLevel>0) Dbg::dbg << "contRangePSet::intersectUpd this inside that"<<endl;
//                modified = copy(that);
//        }
//        // If this and that have an empty intersection
//        else if(rangeDisjoint(that))
//        {
//                if(debugLevel>0) Dbg::dbg << "contRangePSet::intersectUpd disjoint ranges"<<endl;
//                modified = emptify();
//        }
//        // If this and that must overlap with this Below that: lb <= that.lb <= ub <= that.ub
//        else if(overlapBelow(that))
//        {
//                if(debugLevel>0) Dbg::dbg << "contRangePSet::intersectUpd overlapBelow"<<endl;
//                modified = lb != that.lb;
//                lb = that.lb;
//        }
//        // If this and that must overlap with this Above that: that.lb <= lb <= that.ub <= ub
//        else if(overlapAbove(that))
//        {
//                if(debugLevel>0) Dbg::dbg << "contRangePSet::intersectUpd overlapAbove"<<endl;
//                modified = ub != that.ub;
//                ub = that.ub;
//        }
//        // Else, this is an un-representable set
//        else
//        {
//                if(debugLevel>0) Dbg::dbg << "contRangePSet::intersectUpd invalid intersection"<<endl;
//                if(debugLevel>0) Dbg::dbg << "    this="<<str()<<endl;
//                if(debugLevel>0) Dbg::dbg << "    that="<<that.str()<<endl;
//                modified = invalidate();
//        }
//
//        return modified;
//}
//
//// Returns a heap-allocated reference to this - that
//// if transClosure==true, performs the appropriate (possibly optimized) transitive closure on cg
//pSet& contRangePSet::rem(const pSet& that_arg) const
//{
//        const contRangePSet& that = dynamic_cast<const contRangePSet&>(that_arg);
//        return rem(that, false);
//}
//
//pSet& contRangePSet::rem(const contRangePSet& that, bool transClosure) const
//{
//        contRangePSet* thisCopy = new contRangePSet(*this);
//        thisCopy->genFreshBounds();
//        thisCopy->remUpd(that, transClosure);
//        return *thisCopy;
//}
//
//// Updates this with the result of this - that
//// returns true if this causes the pSet to change, false otherwise
//// if transClosure==true, performs the appropriate (possibly optimized) transitive closure on cg
//bool contRangePSet::remUpd(const pSet& that_arg)
//{
//        const contRangePSet& that = dynamic_cast<const contRangePSet&>(that_arg);
//        return remUpd(that, false);
//}
//
//bool contRangePSet::remUpd(const contRangePSet& that, bool transClosure)
//{
//        bool modified = false;
//
//        if(debugLevel>0) {
//                Dbg::dbg << "remUpd: this="<<str("")<<endl;
//                Dbg::dbg << "        that="<<that.str("")<<endl;
//        }
//        //initialized = dynamic_cast<IfMeetLat*>(that)->initialized;
//        ROSE_ASSERT(cg == that.cg);
//
//        // If the range to be removed is equal to this range
//        if(rangeEq(that))
//        {
//                if(debugLevel>0) Dbg::dbg << "    remUpd: equal"<<endl;
//                modified = emptify() || modified;
//        }
//        // If [lb<=that.lb <= (ub=that.ub)]
//        else if(rangeTop(that))
//        {
//                if(debugLevel>0) Dbg::dbg << "    remUpd: that rangeTop of this"<<endl;
//                // [lb, that.lb) : ub = that.lb-1
//                cg->eraseVarConstr(ub);
//                cg->assertCond(ub, that.lb, 1, 1, -1);
//                cg->assertCond(that.lb, lb, 1, 1, 1);
//                modified = true;
//
//                cg->assertCond(lb, ub, 1, 1, 0);
//                cg->localTransClosure(ub);
//        }
//        // If [that.lb<=lb <= (ub=that.ub)]
//        else if(that.rangeTop(*this))
//        {
//                if(debugLevel>0) Dbg::dbg << "    remUpd: this rangeTop of that"<<endl;
//                modified = emptify() || modified;
//        }
//        // If [(lb=that.lb) <= that.ub<=ub]
//        else if(rangeBottom(that))
//        {
//                if(debugLevel>0) Dbg::dbg << "    remUpd: that rangeBottom of this"<<endl;
//                // (that.ub, ub] : that.ub+1 = lb
//                cg->eraseVarConstr(lb);
//                cg->assertCond(that.ub, lb, 1, 1, -1);
//                cg->assertCond(lb, that.ub, 1, 1, 1);
//                modified = true;
//
//                cg->assertCond(lb, ub, 1, 1, 0);
//                cg->localTransClosure(lb);
//        }
//        // If [lb<=that.lb <= (ub=that.ub)]
//        else if(that.rangeBottom(*this))
//        {
//                if(debugLevel>0) Dbg::dbg << "    remUpd: this rangeBottom of that"<<endl;
//                modified = emptify() || modified;
//        }
//        // If [lb, ub] < [that.lb, that.ub] or [that.lb, that.ub] < [lb, ub]
//        else if(rangeDisjoint(that))
//        {
//                Dbg::dbg << "    remUpd: that rangeDisjoint this"<<endl;
//                // the two sets are non-overlapping, so nothing happens
//                modified = false;
//        }
//        // If this and that must overlap with this Below that: lb <= that.lb <= ub <= that.ub
//        else if(overlapBelow(that))
//        {
//                if(debugLevel>0) Dbg::dbg << "    remUpd: this rangeBelow of that"<<endl;
//                // [lb, that.lb) : ub = that.lb-1
//                cg->eraseVarConstr(ub);
//                cg->assertCond(ub, that.lb, 1, 1, -1);
//                cg->assertCond(that.lb, ub, 1, 1, 1);
//                modified = true;
//
//                cg->assertCond(lb, ub, 1, 1, 0);
//                cg->localTransClosure(ub);
//        }
//        // If this and that must overlap with this Above that: that.lb <= lb <= that.ub <= ub
//        else if(overlapAbove(that))
//        {
//                if(debugLevel>0) Dbg::dbg << "    remUpd: this rangeAbove of that"<<endl;
//                // [that.ub, lb) : that.ub = lb-1
//                cg->eraseVarConstr(lb);
//                cg->assertCond(that.ub, lb, 1, 1, -1);
//                cg->assertCond(lb, that.ub, 1, 1, -1);
//                modified = true;
//
//                cg->assertCond(lb, ub, 1, 1, 0);
//                cg->localTransClosure(lb);
//        }
//        // If the sets may overlap but not in a clean way, the result of the removal
//        // is not representable
//        else
//                modified = invalidate();
//        return modified;
//}
//
//
//// Returns true if this set is empty, false otherwise
//bool contRangePSet::emptySet() const
//{ return emptyRange; }
//
//// Returns true if this is a representable set, false otherwise
//bool contRangePSet::validSet() const
//{ return valid; }
//
//// The size of this process set, either a specific integer or infinity
//int contRangePSet::size() const
//{
///*Dbg::dbg << "contRangePSet::size() : cg->eqVars("<<lb.str()<<", "<<ub.str()<<")="<<cg->eqVars(lb, ub)<<endl;
//Dbg::dbg << "    cg = "<<cg->str()<<endl;*/
//        if(emptySet())
//                return 0;
//        // If the lower bound is equal to the upper bound, the domain has a single element
//        else if(cg->eqVars(lb, ub, "    "))
//                return 1;
//        else
//        {
//                int a, b, c;
//                // If both the lower bound and the upper bound are related to each other via a
//                // lb = ub + c relationship
//                if(cg->isEqVars(lb, ub, a, b, c, "    ") && a==1 && b==1)
//                        return c;
//                // Otherwise, we don't know the exact size of this set and conservatively report infinity
//                else
//                        return infinity;
//        }
//}
//
//// Assigns this to that
//pSet& contRangePSet::operator=(const pSet& that_arg)
//{
//        const contRangePSet& that = dynamic_cast<const contRangePSet&>(that_arg);
//        valid = that.valid;
//        emptyRange = that.emptyRange;
//        cg = that.cg;
//        cg->addVar(lb);
//        cg->addVar(ub);
//        // lb == that.lb
//        cg->assertCond(lb, that.lb, 1, 1, 0);
//        cg->assertCond(that.lb, lb, 1, 1, 0);
//        // ub == that.ub
//        cg->assertCond(ub, that.ub, 1, 1, 0);
//        cg->assertCond(that.ub, ub, 1, 1, 0);
//
//        return *this;
//}
//
///*pSet& contRangePSet::operator=(const pSet& that_arg)
//{
//        const contRangePSet& that = dynamic_cast<const contRangePSet&>(that_arg);
//
//        lb = that.lb;
//        ub = that.ub;
//        cg = that.cg;
//        emptyRange = that.emptyRange;
//
//        return *this;
//}
//bool contRangePSet::operator==(const pSet& that_arg)
//{
//        const contRangePSet& that = dynamic_cast<const contRangePSet&>(that_arg);
//
//        return lb == that.lb &&
//               ub == that.ub &&
//               cg == that.cg &&
//               emptyRange == that.emptyRange;
//}*/
//
//
//// Comparison
//bool contRangePSet::operator==(const pSet& that_arg) const
//{
//        const contRangePSet& that = dynamic_cast<const contRangePSet&>(that_arg);
//        return valid == that.valid &&
//               emptyRange == that.emptyRange &&
//               cg == that.cg &&
//               cg->eqVars(lb, that.lb, "    ") &&
//               cg->eqVars(ub, that.ub, "    ");
//}
//
//bool contRangePSet::operator<(const pSet& that_arg) const
//{
//        const contRangePSet& that = dynamic_cast<const contRangePSet&>(that_arg);
//        if(!valid)
//                return that.valid;
//        else if(!that.valid)
//                return false;
//        else if(emptyRange)
//                return !that.emptyRange;
//        else if(that.emptyRange)
//                return false;
//        // Both sets are valid and not empty
//        else
//                return (lb<that.lb) ||
//                       (lb==that.lb && ub<that.ub);
//}
//
//// Returns a string representation of this set
//string contRangePSet::str(string indent)
//{
//        ostringstream outs;
//        if(emptyRange)
//        { outs << indent << "<contRangePSet: empty>"; }
//        else if(!valid)
//        { outs << indent << "<contRangePSet: invalid>"; }
//        else
//        {
//                outs << indent << "<contRangePSet: [";
//                if(cg)
//                {
//                        outs << lb.str()<<"=";
//                        map<varID, affineInequality> eqLB = cg->getEqVars(lb);
//                        outs << "{" ;
//                        if(eqLB.size()>0)
//                        {
//                                for(map<varID, affineInequality>::iterator it=eqLB.begin(); it!=eqLB.end(); )
//                                {
//                                        if(it->second.getA()!=1)
//                                        {
//                                                if(it->second.getB()!=1)
//                                                {
//                                                        if(it->second.getC()!=0) outs << "("<<it->first.str()<<"*"<<it->second.getB()<<"+"<<it->second.getC()<<")/"<<it->second.getA();
//                                                        else outs << "("<<it->first.str()<<"*"<<it->second.getB()<<")/"<<it->second.getA();
//                                                }
//                                                else
//                                                {
//                                                        if(it->second.getC()!=0) outs << "("<<it->first.str()<<"+"<<it->second.getC()<<")/"<<it->second.getA();
//                                                        else outs << "("<<it->first.str()<<")/"<<it->second.getA();
//                                                }
//                                        }
//                                        else
//                                        {
//                                                if(it->second.getB()!=1)
//                                                {
//                                                        if(it->second.getC()!=0) outs << it->first.str()<<"*"<<it->second.getB()<<"+"<<it->second.getC();
//                                                        else outs << it->first.str()<<"*"<<it->second.getB();
//                                                }
//                                                else
//                                                {
//                                                        if(it->second.getC()!=0) outs << it->first.str()<<"+"<<it->second.getC();
//                                                        else outs << it->first.str();
//                                                }
//                                        }
//                                        it++;
//                                        if(it!=eqLB.end())
//                                                outs << ",";
//                                }
//                        }
//                        else
//                                outs << lb.str();
//
//                        outs << "} - {";
//
//                        map<varID, affineInequality> eqUB = cg->getEqVars(ub);
//                        if(eqUB.size()>0)
//                        {
//                                outs << ub.str()<<"=";
//                                for(map<varID, affineInequality>::iterator it=eqUB.begin(); it!=eqUB.end(); )
//                                {
//                                        if(it->second.getA()!=1)
//                                        {
//                                                if(it->second.getB()!=1)
//                                                {
//                                                        if(it->second.getC()!=0) outs << "("<<it->first.str()<<"*"<<it->second.getB()<<"+"<<it->second.getC()<<")/"<<it->second.getA();
//                                                        else outs << "("<<it->first.str()<<"*"<<it->second.getB()<<")/"<<it->second.getA();
//                                                }
//                                                else
//                                                {
//                                                        if(it->second.getC()!=0) outs << "("<<it->first.str()<<"+"<<it->second.getC()<<")/"<<it->second.getA();
//                                                        else outs << "("<<it->first.str()<<")/"<<it->second.getA();
//                                                }
//                                        }
//                                        else
//                                        {
//                                                if(it->second.getB()!=1)
//                                                {
//                                                        if(it->second.getC()!=0) outs << it->first.str()<<"*"<<it->second.getB()<<"+"<<it->second.getC();
//                                                        else outs << it->first.str()<<"*"<<it->second.getB();
//                                                }
//                                                else
//                                                {
//                                                        if(it->second.getC()!=0) outs << it->first.str()<<"+"<<it->second.getC();
//                                                        else outs << it->first.str();
//                                                }
//                                        }
//                                        it++;
//                                        if(it!=eqUB.end())
//                                                outs << ",";
//                                }
//                        }
//                        else
//                                outs << ub.str();
//                        outs << "}]>";
//                }
//                else
//                {
//                        outs << lb.str() << " - " << ub.str() << "]>";
//                }
//        }
//        return Dbg::escape(outs.str());
//}
//
//string contRangePSet::str(string indent) const
//{
//        ostringstream outs;
//        if(emptyRange)
//        { outs << indent << "<contRangePSet: empty>"; }
//        else if(!valid)
//        { outs << indent << "<contRangePSet: invalid>"; }
//        else
//        {
//                outs << indent << "<contRangePSet: [";
//                if(cg)
//                {
//                        outs << lb.str()<<":";
//                        map<varID, affineInequality> eqLB = cg->getEqVars(lb);
//                        outs << "{" ;
//                        if(eqLB.size()>0)
//                        {
//                                for(map<varID, affineInequality>::iterator it=eqLB.begin(); it!=eqLB.end(); )
//                                {
//                                        if(it->second.getA()!=1)
//                                        {
//                                                if(it->second.getB()!=1)
//                                                {
//                                                        if(it->second.getC()!=0) outs << "("<<it->first.str()<<"*"<<it->second.getB()<<"+"<<it->second.getC()<<")/"<<it->second.getA();
//                                                        else outs << "("<<it->first.str()<<"*"<<it->second.getB()<<")/"<<it->second.getA();
//                                                }
//                                                else
//                                                {
//                                                        if(it->second.getC()!=0) outs << "("<<it->first.str()<<"+"<<it->second.getC()<<")/"<<it->second.getA();
//                                                        else outs << "("<<it->first.str()<<")/"<<it->second.getA();
//                                                }
//                                        }
//                                        else
//                                        {
//                                                if(it->second.getB()!=1)
//                                                {
//                                                        if(it->second.getC()!=0) outs << it->first.str()<<"*"<<it->second.getB()<<"+"<<it->second.getC();
//                                                        else outs << it->first.str()<<"*"<<it->second.getB();
//                                                }
//                                                else
//                                                {
//                                                        if(it->second.getC()!=0) outs << it->first.str()<<"+"<<it->second.getC();
//                                                        else outs << it->first.str();
//                                                }
//                                        }
//                                        it++;
//                                        if(it!=eqLB.end())
//                                                outs << ",";
//                                }
//                        }
//                        else
//                                outs << lb.str();
//
//                        outs << "} - {";
//
//                        map<varID, affineInequality> eqUB = cg->getEqVars(ub);
//                        if(eqUB.size()>0)
//                        {
//                                outs << ub.str()<<":";
//                                for(map<varID, affineInequality>::iterator it=eqUB.begin(); it!=eqUB.end(); )
//                                {
//                                        if(it->second.getA()!=1)
//                                        {
//                                                if(it->second.getB()!=1)
//                                                {
//                                                        if(it->second.getC()!=0) outs << "("<<it->first.str()<<"*"<<it->second.getB()<<"+"<<it->second.getC()<<")/"<<it->second.getA();
//                                                        else outs << "("<<it->first.str()<<"*"<<it->second.getB()<<")/"<<it->second.getA();
//                                                }
//                                                else
//                                                {
//                                                        if(it->second.getC()!=0) outs << "("<<it->first.str()<<"+"<<it->second.getC()<<")/"<<it->second.getA();
//                                                        else outs << "("<<it->first.str()<<")/"<<it->second.getA();
//                                                }
//                                        }
//                                        else
//                                        {
//                                                if(it->second.getB()!=1)
//                                                {
//                                                        if(it->second.getC()!=0) outs << it->first.str()<<"*"<<it->second.getB()<<"+"<<it->second.getC();
//                                                        else outs << it->first.str()<<"*"<<it->second.getB();
//                                                }
//                                                else
//                                                {
//                                                        if(it->second.getC()!=0) outs << it->first.str()<<"+"<<it->second.getC();
//                                                        else outs << it->first.str();
//                                                }
//                                        }
//                                        it++;
//                                        if(it!=eqUB.end())
//                                                outs << ",";
//                                }
//                        }
//                        else
//                                outs << ub.str();
//                        outs << "}]>";
//                }
//                else
//                {
//                        outs << lb.str() << " - " << ub.str() << "]>";
//                }
//        }
//        return Dbg::escape(outs.str());
//}
//
//// Removes the upper and lower bounds of this set from its associated constraint graph
//// or the provided constraint graph (if it is not ==NULL) and sets cg to NULL if tgtCG==NULL.
//// Returns true if this causes the constraint graph to change, false otherwise.
//bool contRangePSet::cgDisconnect()
//{
//        bool modified=false;
//
//        if(emptySet() || !validSet()) return modified;
//
//        if(cg!=NULL)
//        {
//                // Disconnect this process set's bounds from its constraint craph
//                modified = cg->removeVar(getLB()) || modified;
//                modified = cg->removeVar(getUB()) || modified;
//
//                cg = NULL;
//        }
//
//        return modified;
//}
//
//bool contRangePSet::cgDisconnect(ConstrGraph* tgtCG) const
//{
//        bool modified=false;
//
//        if(emptySet() || !validSet()) return modified;
//
//        if(tgtCG!=NULL)
//        {
//                // Disconnect this process set's bounds from its constraint craph
//                modified = tgtCG->removeVar(getLB()) || modified;
//                modified = tgtCG->removeVar(getUB()) || modified;
//        }
//        return modified;
//}
//

