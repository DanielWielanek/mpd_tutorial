/*
 * showEffic.C
 *
 *  Created on: 9 cze 2021
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */

#if !defined(__CINT__) && !defined(__CLING__)
#include "TCanvas.h"
#include "TFile.h"
#include "TH2D.h"
#include "TString.h"
#include "TStyle.h"
#include "TSystem.h"


#include <iostream>
using namespace std;
#endif

void showEffic() {
  gStyle->SetPalette(kRainBow);
  TFile* file = new TFile("spectra.root");
  TH2D* mc    = (TH2D*) file->Get("specSim");
  TH2D* rec   = (TH2D*) file->Get("specReco");
  rec->Divide(mc);
  rec->SetMinimum(0);
  rec->SetMaximum(1);
  rec->Draw("colz");
}
