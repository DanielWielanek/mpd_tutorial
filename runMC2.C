/*
 * runMC2.C
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

#include "MpdConstField.h"
#include "MpdEmc.h"
#include "MpdFfd.h"
#include "MpdFieldMap.h"
#include "MpdGetNumEvents.h"
#include "MpdMultiField.h"
#include "MpdMultiFieldPar.h"
#include "MpdTof.h"
#include "MpdZdc.h"
#include "TpcDetector.h"

#include <iostream>
using namespace std;
#endif

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/mpd/geometry_stage1.C"
#include "macro/mpd/mpdloadlibs.C"
//#include "macro/mpd/geometry_v2.C"

#define BOX     // Choose generator: URQMD VHLLE FLUID PART ION BOX HSD LAQGSM  SMASH
#define GEANT3  // Choose: GEANT3 GEANT4

// inFile - input file with generator data, default: auau.09gev.mbias.98k.ftn14
// nStartEvent - for compatibility, any number
// nEvents - number of events to transport, default: 1
// outFile - output file with MC data, default: evetest.root
// flag_store_FairRadLenPoint
// FieldSwitcher: 0 - corresponds to the ConstantField (0, 0, 5) kG (It is used by default); 1 - corresponds to the FieldMap
// ($VMCWORKDIR/input/B-field_v2.dat)
void runMC2(TString inFile                    = "auau.04gev.0_3fm.10k.f14.gz",
            TString outFile                   = "evetest.root",
            Int_t nStartEvent                 = 0,
            Int_t nEvents                     = 10,
            Bool_t flag_store_FairRadLenPoint = kFALSE,
            Int_t FieldSwitcher               = 0) {
  TStopwatch timer;
  timer.Start();
  gDebug = 0;

  FairRunSim* fRun = new FairRunSim();
  // Choose the Geant Navigation System
#ifdef GEANT3
  fRun->SetName("TGeant3");
#else
  fRun->SetName("TGeant4");
#endif

  geometry_stage1(fRun);  // load mpd geometry
  // geometry_v2(fRun); // load mpd geometry

  // Use extended MC Event header instead of the default one.
  // MpdMCEventHeader* mcHeader = new MpdMCEventHeader();
  // fRun->SetMCEventHeader(mcHeader);

  // Create and Set Event Generator
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  // smearing of beam interaction point
  primGen->SetBeam(0.0, 0.0, 0.1, 0.1);
  primGen->SetTarget(0.0, 24.0);
  primGen->SmearGausVertexZ(kTRUE);
  primGen->SmearVertexXY(kTRUE);

  // Use user defined decays https://fairroot.gsi.de/?q=node/57
  fRun->SetUserDecay(kTRUE);
  // Use external decayer
  // fRun->SetPythiaDecayer(TString("$VMCWORKDIR/gconfig/LambdaDecayConfig.C"));

#ifdef ION  // <---- Ion Generator
  FairIonGenerator* fIongen = new FairIonGenerator(79, 197, 79, 1, 0., 0., 25, 0., 0., -1.);
  primGen->AddGenerator(fIongen);

#else
#ifdef BOX  // <---- Box Generator
  gRandom->SetSeed(0);

  FairBoxGenerator* boxProton = new FairBoxGenerator(2212, 100);  // 2212 = proton; 1 = multipl.
  boxProton->SetPRange(0.25, 2.5);                                // GeV/c, setPRange vs setPtRange
  boxProton->SetPhiRange(0, 360);                                 // Azimuth angle range [degree]
  boxProton->SetThetaRange(0, 180);                               // Polar angle in lab system range [degree]
  boxProton->SetXYZ(0., 0., 0.);                                  // mm o cm ??
  primGen->AddGenerator(boxProton);

  FairBoxGenerator* boxPion = new FairBoxGenerator(211, 300);
  boxPion->SetPRange(0.25, 2.5);
  boxPion->SetPhiRange(0, 360);
  boxPion->SetThetaRange(0, 180);
  boxPion->SetXYZ(0., 0., 0.);
  primGen->AddGenerator(boxPion);

  FairBoxGenerator* boxKaon = new FairBoxGenerator(321, 20);  // 13 = muon; 1 = multipl.
  boxKaon->SetPRange(0.25, 2.5);
  boxKaon->SetPhiRange(0, 360);
  boxKaon->SetThetaRange(0, 180);
  boxKaon->SetXYZ(0., 0., 0.);
  primGen->AddGenerator(boxKaon);


#endif
#endif

  fRun->SetOutputFile(outFile.Data());

  // Magnetic Field Map - for proper use in the analysis MultiField is necessary here
  MpdMultiField* fField = new MpdMultiField();

  if (FieldSwitcher == 0) {
    MpdConstField* fMagField = new MpdConstField();
    fMagField->SetField(0., 0., 5.);  // values are in kG:  1T = 10kG
    fMagField->SetFieldRegion(-230, 230, -230, 230, -375, 375);
    fField->AddField(fMagField);
    fRun->SetField(fField);
    cout << "FIELD at (0., 0., 0.) = (" << fMagField->GetBx(0., 0., 0.) << "; " << fMagField->GetBy(0., 0., 0.) << "; "
         << fMagField->GetBz(0., 0., 0.) << ")" << endl;
  } else if (FieldSwitcher == 1) {
    MpdFieldMap* fMagField = new MpdFieldMap("B-field_v2", "A");
    fMagField->Init();
    fField->AddField(fMagField);
    fRun->SetField(fField);
    cout << "FIELD at (0., 0., 0.) = (" << fMagField->GetBx(0., 0., 0.) << "; " << fMagField->GetBy(0., 0., 0.) << "; "
         << fMagField->GetBz(0., 0., 0.) << ")" << endl;
  }

  fRun->SetStoreTraj(kTRUE);
  fRun->SetRadLenRegister(flag_store_FairRadLenPoint);  // radiation length manager

  fRun->Init();

  // -Trajectories Visualization (TGeoManager Only)
  // Set cuts for storing the trajectories
  FairTrajFilter* trajFilter = FairTrajFilter::Instance();
  trajFilter->SetStepSizeCut(0.01);  // 1 cm
  //  trajFilter->SetVertexCut(-2000., -2000., 4., 2000., 2000., 100.);
  trajFilter->SetMomentumCutP(.50);  // p_lab > 500 MeV
  //  trajFilter->SetEnergyCut(.2, 3.02); // 0 < Etot < 1.04 GeV

  trajFilter->SetStorePrimaries(kTRUE);
  trajFilter->SetStoreSecondaries(kFALSE);

  // Fill the Parameter containers for this run
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();

  Bool_t kParameterMerged   = kTRUE;
  FairParRootFileIo* output = new FairParRootFileIo(kParameterMerged);
  // AZ output->open(parFile.Data());
  output->open(gFile);
  rtdb->setOutput(output);

  MpdMultiFieldPar* Par = (MpdMultiFieldPar*) rtdb->getContainer("MpdMultiFieldPar");
  if (fField) Par->SetParameters(fField);
  Par->setInputVersion(fRun->GetRunId(), 1);
  Par->setChanged();
  // Par->printParams();

  rtdb->saveOutput();
  rtdb->print();

  // Transport nEvents
  fRun->Run(nEvents);

  timer.Stop();
  Double_t rtime = timer.RealTime(), ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);
  cout << "Macro finished successfully." << endl;  // marker of successful execution for CDASH
}
