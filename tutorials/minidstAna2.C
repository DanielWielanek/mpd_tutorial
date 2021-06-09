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

MpdPid* fPID;

void GetSigmas(MpdMiniTrack* track, Double_t& sigPion, Double_t& sigKaon, Double_t& sigProton) {
  TVector3 mom      = track->gMom();
  Double_t p        = mom.Mag();
  Double_t dedx     = track->dEdx() * 15.0;
  Double_t dedx_pi  = fPID->GetDedxPiParam(p);
  Double_t dedx_ka  = fPID->GetDedxKaParam(p);
  Double_t dedx_pr  = fPID->GetDedxPrParam(p);
  Double_t sigma_pi = fPID->GetDedxWidthValue(p, 1) * dedx_pi;
  Double_t sigma_ka = fPID->GetDedxWidthValue(p, 2) * dedx_ka;
  Double_t sigma_pr = fPID->GetDedxWidthValue(p, 3) * dedx_pr;
  sigPion           = (dedx - dedx_pi) / sigma_pi;
  sigKaon           = (dedx - dedx_ka) / sigma_ka;
  sigProton         = (dedx - dedx_pr) / sigma_pr;
}


void minidstAna2() {
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

  fPID = new MpdPid(0, 0, 0, 1, "DEFAULT", "CF", "pikapr");


  TH2D* dedxPion = new TH2D("dedxPion", "dedxPion;p[GeV/];dEdx [AU]", 100, 0, 2, 100, 0, 2000);
  TH2D* tofPion  = new TH2D("tof", "tof;p[GeV/c];m^{2}[GeV^{2}/c^{4}]", 100, 0, 2, 100, -0.1, 1.1);
  TH1D* sig      = new TH1D("sig", "sig", 100, -10, 10);

  for (int i = 0; i < t->GetEntriesFast(); i++) {
    t->GetEntry(i);
    for (int j = 0; j < recoTracks->GetEntriesFast(); j++) {
      MpdMiniTrack* recoTrack = (MpdMiniTrack*) recoTracks->UncheckedAt(j);
      Int_t mcIndex           = recoTrack->id();
      if (mcIndex < 0 || mcIndex > mcTracks->GetEntriesFast()) continue;
      MpdMiniMcTrack* simTrack = (MpdMiniMcTrack*) mcTracks->UncheckedAt(mcIndex);
      Double_t sigPi, sigKa, sigPr;
      GetSigmas(recoTrack, sigPi, sigKa, sigPr);
      TVector3 pReco = recoTrack->gMom();
      if (simTrack->pdgId() == 211) sig->Fill(sigPi);
      if (TMath::Abs(sigPi) < 2.0) {  // is pion?
        dedxPion->Fill(pReco.Mag(), recoTrack->dEdx());

        Int_t tofIndex = recoTrack->bTofPidTraitsIndex();
        if (tofIndex > -1) {
          MpdMiniBTofPidTraits* tof = (MpdMiniBTofPidTraits*) tofData->UncheckedAt(tofIndex);
          Double_t m2               = tof->massSqr();
          tofPion->Fill(pReco.Mag(), m2);
        }
      }
    }
  }
  TCanvas* c = new TCanvas();
  dedxPion->Draw("colz");
  c = new TCanvas();
  tofPion->Draw("colz");
  c = new TCanvas();
  sig->Draw("colz");
}
