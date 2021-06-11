/*
 * minidstAna.C
 *
 *  Created on: 9 cze 2021
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */
#if !defined(__CINT__) && !defined(__CLING__)
#include "TDatabasePDG.h"
#include "TFile.h"
#include "TMath.h"
#include "TROOT.h"
#include "TStopwatch.h"
#include "TString.h"
#include "TSystem.h"
#include "TTree.h"

#include "TClonesArray.h"
#include "TH2D.h"
#include "TpcDetector.h"
#include <iostream>

#include "MpdMiniBTofPidTraits.h"
#include "MpdMiniEvent.h"
#include "MpdMiniMcEvent.h"
#include "MpdMiniMcTrack.h"
#include "MpdMiniTrack.h"


#include "MpdPid.h"
using namespace std;
#endif


void minidstAna() {
  TFile* f                 = new TFile("../mpddst.MiniDst.root");
  TTree* t                 = (TTree*) f->Get("MiniDst");
  TClonesArray* recoTracks = nullptr;
  TClonesArray* recoEvent  = nullptr;
  TClonesArray* mcTracks   = nullptr;
  TClonesArray* mcEvent    = nullptr;
  TClonesArray* tofData    = nullptr;
  t->SetBranchAddress("Event", &recoEvent);
  t->SetBranchAddress("Track", &recoTracks);
  t->SetBranchAddress("BTofPidTraits", &tofData);
  t->SetBranchAddress("McEvent", &mcEvent);
  t->SetBranchAddress("McTrack", &mcTracks);

  TH2D* dedxAll  = new TH2D("dedxAll", "dedxAll;p[GeV/];dEdX [AU]", 100, 0, 2, 100, 0, 2000);
  TH2D* dedxPion = new TH2D("dedxPion", "dedxPion;p[GeV/];dEdX [AU]", 100, 0, 2, 100, 0, 2000);

  for (int i = 0; i < t->GetEntriesFast(); i++) {
    t->GetEntry(i);
    for (int j = 0; j < recoTracks->GetEntriesFast(); j++) {
      MpdMiniTrack* recoTrack = (MpdMiniTrack*) recoTracks->UncheckedAt(j);
      Int_t mcIndex           = recoTrack->mcTrackIndex();
      if (mcIndex < 0 || mcIndex >= mcTracks->GetEntriesFast()) continue;
      MpdMiniMcTrack* simTrack = (MpdMiniMcTrack*) mcTracks->UncheckedAt(mcIndex);
      TVector3 pSim            = simTrack->momentum();
      TVector3 pReco           = recoTrack->gMom();
      TVector3 dP              = pSim - pReco;
      if (simTrack->pdgId() == 211) dedxPion->Fill(pReco.Mag(), recoTrack->dEdx());
      dedxAll->Fill(pReco.Mag(), recoTrack->dEdx());
    }
  }
  TCanvas* c = new TCanvas();
  dedxAll->Draw("colz");
  c = new TCanvas();
  dedxPion->Draw("colz");
}
