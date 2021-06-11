/*
 * MyTaskCompiled.cxx
 *
 *  Created on: 10 cze 2021
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */
#include "MyTaskCompiled.h"

#include "MpdMiniMcTrack.h"
#include "MpdMiniTrack.h"
#include <TVector3.h>

void MyTaskCompiled::Exec(Option_t* opt) {
  for (int i = 0; i < fMcTracks->GetEntriesFast(); i++) {
    MpdMiniMcTrack* mc = (MpdMiniMcTrack*) fMcTracks->UncheckedAt(i);
    TVector3 p         = mc->momentum();
    if (mc->pdgId() == 2212) fYPtSim->Fill(p.Eta(), p.Pt());
  }
  for (int i = 0; i < fRecoTracks->GetEntriesFast(); i++) {
    MpdMiniTrack* track = (MpdMiniTrack*) fRecoTracks->UncheckedAt(i);
    Int_t match         = track->mcTrackIndex();
    if (match >= 0 && match < fMcTracks->GetEntriesFast()) {
      MpdMiniMcTrack* mc = (MpdMiniMcTrack*) fMcTracks->UncheckedAt(match);
      TVector3 p         = mc->momentum();
      if (mc->pdgId() == 2212) fYPtReco->Fill(p.Eta(), p.Pt());
    }
  }
}

InitStatus MyTaskCompiled::Init() {
  FairRootManager* mng = FairRootManager::Instance();
  fRecoEvent           = (TClonesArray*) mng->GetObject("Event");
  fRecoTracks          = (TClonesArray*) mng->GetObject("Track");
  fMcEvent             = (TClonesArray*) mng->GetObject("McEvent");
  fMcTracks            = (TClonesArray*) mng->GetObject("McTrack");
  fTofData             = (TClonesArray*) mng->GetObject("BTofPidTraits");
  fYPtSim              = new TH2D("specSimCompiled", "spec;#eta;p_{T} [GeV/c]", 20, -2, 2, 20, 0, 2);
  fYPtReco             = new TH2D("specRecoCompiled", "spec;#eta;p_{T} [GeV/c]", 20, -2, 2, 20, 0, 2);
  return kSUCCESS;
}
