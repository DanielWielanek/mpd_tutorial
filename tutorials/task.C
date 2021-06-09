/*
 * task.C
 *
 *  Created on: 9 cze 2021
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */
#if !defined(__CINT__) && !defined(__CLING__)
#include "TDatabasePDG.h"
#include "TMath.h"
#include "TROOT.h"
#include "TStopwatch.h"
#include "TString.h"
#include "TSystem.h"

#include "FairCave.h"
#include "FairMagnet.h"
#include "FairParRootFileIo.h"
#include "FairPipe.h"
#include "FairPrimaryGenerator.h"
#include "FairRunSim.h"
#include "FairRuntimeDb.h"
#include "FairTrajFilter.h"
#include "FairUrqmdGenerator.h"
#include "MpdLAQGSMGenerator.h"

#include "FairFileSource.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairTask.h"
#include "MpdConstField.h"
#include "MpdEmc.h"
#include "MpdEvent.h"
#include "MpdFfd.h"
#include "MpdFieldMap.h"
#include "MpdGetNumEvents.h"
#include "MpdMCTrack.h"
#include "MpdMultiField.h"
#include "MpdMultiFieldPar.h"
#include "MpdTof.h"
#include "MpdTrack.h"
#include "MpdZdc.h"
#include "TClonesArray.h"
#include "TH2D.h"
#include "TpcDetector.h"
#include <iostream>
using namespace std;
#endif


class MyAna : public FairTask {
  MpdEvent* fEvent;
  TClonesArray* fMC;
  TH2D* fYPtSim;
  TH2D* fYPtReco;

protected:
  virtual InitStatus Init() {
    FairRootManager* mng = FairRootManager::Instance();
    fEvent               = (MpdEvent*) mng->GetObject("MPDEvent.");
    fMC                  = (TClonesArray*) mng->GetObject("MCTrack");
    fYPtSim              = new TH2D("specSim", "spec;#eta;p_{T} [GeV/c]", 20, -2, 2, 20, 0, 2);
    fYPtReco             = new TH2D("specReco", "spec;#eta;p_{T} [GeV/c]", 20, -2, 2, 20, 0, 2);
    return kSUCCESS;
  };
  virtual void Finish() {
    fYPtSim->Write();
    fYPtReco->Write();
  };

public:
  MyAna() : fEvent(nullptr), fMC(nullptr), fYPtSim(nullptr), fYPtReco(nullptr) {};
  void Exec(Option_t* opt = "") {
    for (int i = 0; i < fMC->GetEntriesFast(); i++) {
      MpdMCTrack* mc = (MpdMCTrack*) fMC->UncheckedAt(i);
      TLorentzVector p;
      mc->Get4Momentum(p);
      if (mc->GetPdgCode() == 211) fYPtSim->Fill(p.Eta(), p.Pt());
    }
    TClonesArray* reco = fEvent->GetGlobalTracks();
    for (int i = 0; i < reco->GetEntriesFast(); i++) {
      MpdTrack* track = (MpdTrack*) reco->UncheckedAt(i);
      Int_t match     = track->GetID();
      if (match >= 0 && match < fMC->GetEntriesFast()) {
        MpdMCTrack* mc = (MpdMCTrack*) fMC->UncheckedAt(match);
        TLorentzVector p;
        mc->Get4Momentum(p);
        if (mc->GetPdgCode() == 211) fYPtReco->Fill(p.Eta(), p.Pt());
      }
    }
  }
  virtual ~MyAna() {};
  ClassDef(MyAna, 1)
};

void task() {
  FairRunAna* ana        = new FairRunAna();
  FairFileSource* source = new FairFileSource("../mpddst.root");
  ana->SetSource(source);
  ana->SetOutputFile("spectra.root");
  ana->AddTask(new MyAna());
  ana->Init();
  ana->Run();
}
