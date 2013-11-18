/*
 * RankLattice.h
 *
 *  Created on: Oct 19, 2013
 *      Author: stoero
 */

#ifndef RANKLATTICE_H_
#define RANKLATTICE_H_

#include "rose.h"
#include "latticeFull.h"
#include "rankAnalysis/pSet.h"

extern int rrankAnalysisDebugLevel;

//! The process set container holds all process sets of a node plus additional
//! information about the process set.
//
//!
struct psetStruct
//class PSetCont
{
  //! Process sets vector
  std::vector<PSet> psets_;
  //!
  const SgNode* n_source_;

  //! default + parameterized constructor
  psetStruct(const PSet pset = PSet(), const SgNode* n = NULL)
  {
    this->psets_.empty();
    this->psets_.push_back(pset);
    n_source_ = n;
  }

  //! constructor
  psetStruct(const std::vector<PSet> psets, const SgNode* n = NULL)
  {
    this->psets_.assign(psets.begin(), psets.end());
    n_source_ = n;
  }

  //! constructor
  psetStruct(const psetStruct& pss)
  {
    this->psets_.assign(pss.psets_.begin(), pss.psets_.end());
    this->n_source_ = pss.n_source_;
  }

  //! assignment operator
  psetStruct& operator=(const psetStruct& ps)
  {
    this->n_source_ = ps.n_source_;
    this->psets_.assign(ps.psets_.begin(), ps.psets_.end());
    return *this;
  }

  //! equality comparison. doesn't modify object. therefore constant.
  bool operator==(const psetStruct& ps) const
  {
      return(n_source_ == ps.n_source_ && psets_ == ps.psets_);
  }

  //! equality comparison. doesn't modify object. therefore const.
  bool operator!=(const psetStruct& ps) const
  {
      return !(*this == ps);
  }

  //! copy constructor
  psetStruct& operator()(const psetStruct& ps)
  {
    *this = ps;
    return *this;
  }

  //! copy constructor
  psetStruct& copy(const psetStruct& ps)
  {
    *this = ps;
    return *this;
  }
};


//=======================================================================================
//!
//!
class RankLattice : public FiniteLattice
{
protected:
  //! A sorted vector containing all the p sets of the node.
  //
  //! Sort order is determined by the < operator overloaded at the PSet class.
  //! Multiple elements with the same lower bound possible if decision about upper
  //! bound can't be made for suer. For sorting, the overloaded comparison operator
  //! of the bound structure is sufficient, but for process set merge and split
  //! decision the subtle comparison methods of bound and PSet have to be used.
  std::vector<PSet> pset_cont_;

  //! Used in case of check_pred_ == false as a copy of pset_cont_ in order to
  //! recognize changes in the process sets.
  std::vector<PSet> out_pset_cont_;

  //! Collection of true and false psets_ vectors to be pushed to the next Lattice
  //
  //! The lattice of a conditional node has to provide both process sets, the one for
  //! the if branch and the one for the else branch. If a split like that occurs, it will
  //! be indicated by setting the check_pred flag in order to inform the next node about
  //! the multiple process sets. The direct successors of the conditional node have to
  //! make the decision about what set to choose from the psets_vec_.
  //! Since one node can be successor of multiple conditional nodes, an auto merge for
  //! the in set would not succeed. Therefore, all the true and false sets are stored in
  //! the in Set plus the SgNode* n that is responsible for the true/false set in order
  //! to access this node and find out if the actual node is true or false successor of
  //! node n.
//  std::vector<std::vector<PSet> > psetsNEW_vec_;
//  std::vector< std::pair< std::vector<PSet>, LatticeSourceType > > psetsNEW_vec_;
//  std::vector< std::pair< std::vector<PSet>,
//                          std::pair<SgNode*, LatticeSourceType> > > psetsNEW_;
//  std::vector<psetStruct> in_psets_vec_;
  std::vector<psetStruct> out_psets_vec_;

  //! Set true at direct successor nodes of a conditional Statement.
  //
  //! In case of being a direct successor node of a conditional
  //! statement, the check predecessor statement flag is set to
  //! true. In this case find out if the true or the true or false
  //! pSet has to be merged with the nodes pSet.
  bool check_pred_;

  //! stores information about the node holding the lattice.
  SgNode* node_;

public:
//=======================================================================================
  //! Do we need a default constructor?
  RankLattice();

  //! This constructor builds a constant value lattice.
  RankLattice( int v );

  //! Do we need the copy constructor?
  //
  //! This is the same as the implicit definition, so it might not be required to be
  //! defined explicitly.
  //! I am searching for the minimal example of the use of the data flow classes.
  RankLattice(const RankLattice & X);

//=======================================================================================
  //!
  bool isDepCondScope() const;
  //!
  bool setDepCondScope(bool stmt);
  //!
  SgNode* getNode() const;
  //!
  bool setNode(SgNode* node);

#if 0 // not implemented yet ....
  int getNodeCount() const;                                                              //verify
  bool setNodeCount(int x);                                                              //verify
  bool incrementNodeCount();                                                             //verify
#endif

  //!
  std::vector<PSet> getPSets() const;
  //!
  std::vector<PSet> getPSets(const SgNode* n) const;
  //!
  std::vector<PSet> getOutPSets() const;
  //!
  std::vector<PSet> getOutPSets(const SgNode* n) const;
  //!
  void setPset(const PSet& ps);
  //!
  void setPsets(const std::vector<PSet>& ps);
  //!
  void setOutPset(const PSet& ps);
  //!
  void setOutPsets(const std::vector<PSet>& ps);

#if 0
  //!
  std::vector<PSet> getPsets(RankLattice::LatticeSourceType source_type, SgNode* n);
#endif

//=======================================================================================
  //! Adds one process set range to psets_
  //
  //! Finds the lower bound of the p_set and adds it at the appropriate psets_cont_
  //! position. Merge one process set to the others.
  //! returns true if merge caused changes at the lattice.
  //! returns false if added PSet was already covered by the process sets in psets_cont_
  bool addPSet(PSet pset);
#if 0
  //! Set the pset on the according position in the process set container which is
  //! inside the process set structure.
  void addHardPSet(PSet pset);
  //! Finds the position of pset in the PSet vector and inserts pset there.
  void addHardPSet(std::vector<PSet>& psets, PSet pset);
#endif
#if 0
  //! Takes argument and overrides psets_cont_
  void assignPSets(const std::vector<PSet>& psets);
#endif
  //! Pushes the psetStruct to the end of pset_vec_
  void attachPSets(const psetStruct& sets);

  //! Takes the process sets of lattice l and merges all sets from pset_cont_ with the
  //! process set from this.pset_cont_.
  //
  //! Adds all process set ranges from lattice to this->psets_
  //! returns true if changes happened
  //! returns false if added PSets were already in the psets_ (no changes)
  bool mergePSets(const RankLattice& l);
  //! Takes the process sets of lattice l and merges all sets from pset_cont_ with the
  //! process set from this.pset_cont_.
  bool mergePSets(const std::vector<PSet>& psets);
  //! Adds one process set range to the process set container
  //! returns true if changes happened
  //! returns false if added PSet was already in scope of the process sets (no changes).
  bool mergePSets(const PSet& p_set);

#if 0
  //!
  //!
  std::vector<PSet> combinePSets(const std::vector<PSet>& sets1,                        //verify
                                 const std::vector<PSet>& sets2);

  PSet combinePSets(const PSet p_set1, const PSet p_set2);                              //verify
#endif

  //! Takes the pset_cont_ and copies it onto the out_pset_cont_.
  //
  //! return false, if out_pset_cont_ was already equal to pset_cont_.
  //! return true, if changes occurred at the out_pset_cont_.
  bool pushPSetToOutSet();
  //! Takes the PSet vector and the SgNode* to create a psetStruct which is going
  //! to be pushed to the out_pset_vec.
  //
  //! return false if out_pset_vec already contained psv.
  //! return true if changes occurred
  bool pushPSetToOutVec(const std::vector<PSet>& psv, const SgNode* n);

#if 0
  //!
  //!
  std::vector<PSet> splitPSetsAtBound(bool lowbound, _Bound_ b);                          //verify
  //!
  //!
  std::vector<PSet> splitPSetsAtValue(bool val_set, _Bound_ b);                           //verify
  //!
  //!
  std::vector<PSet> computeTrueSet(const DataflowNode& n);                              //verify
  //!
  //!
  std::vector<PSet> computeFalseSet(const DataflowNode& n);                             //verify
  //!
  bool existsTrueSet(const SgNode* n) const;                                            //verify
  //!
  bool existsFalseSet(const SgNode* n) const;                                           //verify
#endif

#if 0
  //! Empties the whole psets_ map
  void clearPsets();                                                                    //verify
  //! Empties the whole psets_ map
  bool clearPsetsMap();                                                                 //verify
#endif

public:
  //! Returns the intersection of pset_cont_ with PSet ps.
  std::vector<PSet> intersect(const PSet& ps) const;
  //! Returns pset_cont_ without PSet ps.
  std::vector<PSet> remove(const PSet& ps) const;

//=======================================================================================
public:
  // **********************************************
  // Required definition of pure virtual functions.
  // **********************************************
  //! computes the meet of this and that and saves the result in this
  //! returns true if this causes this to change and false otherwise
  bool meetUpdate(Lattice* that);

  //!
  void initialize();

  //! returns a hard copy of this lattice
  Lattice* copy() const;

  //! overwrites the state of "this" Lattice with "that" Lattice
  void copy(Lattice* that);

  //! compares values from "this" Lattice to "that" Lattice
  bool operator==(Lattice* that) /*const*/;
  //!
  std::string str(std::string indent="");
  //!
  std::string toString() const;
  //!
  string psetsToString() const;
  //!
  string toStringForDebugging() const;
  //!
  string procSetsStr(const std::vector<PSet>& ps) const;
  //!
  std::string procSetsVecStr() const;
};

#endif /* RANKLATTICE_H_ */
