/*
 * MyTaskCompiled.h
 *
 *  Created on: 10 cze 2021
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */
#ifndef MPDROOT_MYTASK_MYTASKCOMPILED_H_
#define MPDROOT_MYTASK_MYTASKCOMPILED_H_

#include "MpdEvent.h"
#include "MpdMCTrack.h"
#include "MpdTrack.h"

#include <FairRootManager.h>
#include <FairTask.h>
#include <RtypesCore.h>
#include <TClonesArray.h>
#include <TH2.h>
#include <TLorentzVector.h>
#include <TObjArray.h>


class MyTaskCompiled : public FairTask {
  TClonesArray* fRecoTracks;
  TClonesArray* fRecoEvent;
  TClonesArray* fMcTracks;
  TClonesArray* fMcEvent;
  TClonesArray* fTofData;
  TH2D* fYPtSim;
  TH2D* fYPtReco;

protected:
  virtual InitStatus Init();
  virtual void Finish() {
    fYPtSim->Write();
    fYPtReco->Write();
  };

public:
  MyTaskCompiled() :
    fRecoTracks(nullptr),
    fRecoEvent(nullptr),
    fMcTracks(nullptr),
    fMcEvent(nullptr),
    fTofData(nullptr),
    fYPtSim(nullptr),
    fYPtReco(nullptr) {};
  void Exec(Option_t* opt = "");
  virtual ~MyTaskCompiled() {};
  ClassDef(MyTaskCompiled, 1)
};


#endif /* MPDROOT_MYTASK_MYTASKCOMPILED_H_ */
