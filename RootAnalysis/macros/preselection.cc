#include <iostream>
#include "preselection.hh"
#include "TString.h"
#include "TH1.h"

using namespace std;


#define PTMIN 5.0
#define PTMAX 9999.0

#define M1PTMIN 4.0
#define M1PTMAX 999.0

#define M2PTMIN 4.0
#define M2PTMAX 999.0

#define FL3DMAX 2.0
#define CHI2DOFMAX 5.0

#define PVIPMAX 0.1
#define PVIPSMAX 4.0

#define PVLIPMAX  1.0
#define PVLIPSMAX 5.0

#define MAXDOCAMAX 0.08
#define CLOSETRKMAX 21

#define FLSXYMIN   4.0

#define FLS3DMIN  4.0
#define FLS3DMAX 200.0

#define DOCATRKMAX 2.5

#define ISOMIN 0.0
#define ALPHAMAX 1.0

#define MASSERRORMAX 0.2

// ----------------------------------------------------------------------
std::string preselection() {
  std::string cut = Form(" (%3.2f<pt)&&(pt<%3.2f) && (%3.2f<m1pt)&&(m1pt<%3.2f) && (%3.2f<m2pt)&&(m2pt<%3.2f)",
			 PTMIN, PTMAX, M1PTMIN, M1PTMAX, M2PTMIN, M2PTMAX);
  cut += Form(" && (flsxy>%3.2f) && (fl3d<%3.2f) && (pvip<%3.2f) && !(TMath::IsNaN(pvips)) && (pvips>0) && (pvips<%3.2f)",
	      FLSXYMIN, FL3DMAX, PVIPMAX, PVIPSMAX);
  cut += Form(" && abs(pvlip) < %3.2f && abs(pvlips) < %3.2f", PVLIPMAX, PVLIPSMAX);
  cut += Form(" && (closetrk<%d) && (fls3d>%3.2f) && (fls3d<%3.2f) && (docatrk<%3.2f) && (maxdoca<%3.2f)",
			  CLOSETRKMAX, FLS3DMIN, FLS3DMAX, DOCATRKMAX, MAXDOCAMAX);
  cut += Form(" && (chi2dof<%3.2f)  && (alpha<%3.2f) && (me<%3.2f)", CHI2DOFMAX, ALPHAMAX, MASSERRORMAX);
  cut += Form(" && (iso>%3.2f) && (m1iso>%3.2f) && (m2iso>%3.2f)", ISOMIN, ISOMIN, ISOMIN);
  cut += Form(" && (m1q*m2q<0)");
  return cut;
}

// ----------------------------------------------------------------------
bool preselection(redTreeData &b) {
  const int verbose(-1);

  //NO?!?!?!?!  if (!b.gmugmid) return false;

  if (b.m1q*b.m2q > 0) return false;

  if (b.pt < PTMIN) return false;
  if (b.pt > PTMAX) return false;
  if (verbose > 9) cout << "passed pT" << endl;

  if (b.m1pt < M1PTMIN) return false;
  if (b.m1pt > M1PTMAX) return false;
  if (b.m2pt < M2PTMIN) return false;
  if (b.m2pt > M2PTMAX) return false;
  if (verbose > 9) cout << "passed muon pT" << endl;

  if (b.flsxy < FLSXYMIN) return false;
  if (b.fl3d > FL3DMAX) return false;
  if (b.pvip > PVIPMAX) return false;
  if (TMath::IsNaN(b.pvips)) return false;
  if (b.pvips < 0) return false;
  if (b.pvips > PVIPSMAX) return false;
  if (verbose > 8) cout << "passed vertexing cuts" << endl;

  if (verbose > 8) cout << "pvlip* = " << b.pvlip << " " << b.pvlips << endl;
  if (TMath::Abs(b.pvlip) > PVLIPMAX) return false;
  if (TMath::Abs(b.pvlips) > PVLIPSMAX) return false;
  if (verbose > 7) cout << "passed pvlip* cuts" << endl;

  if (b.closetrk >= CLOSETRKMAX) return false;
  if (b.fls3d < FLS3DMIN) return false;
  if (b.fls3d > FLS3DMAX) return false;
  if (b.docatrk > DOCATRKMAX) return false;
  if (b.maxdoca > MAXDOCAMAX) return false;
  if (b.me > MASSERRORMAX) return false;
  if (verbose > 6) cout << "passed misc cuts" << endl;

  // NONONO if we want to use the onia for any x-checks!
  //   if (b.m < 4.9) return false;
  //   if (b.m > 5.9) return false;
  //   if (verbose > 4) cout << "passed mass cuts" << endl;
  if (verbose > 5) {
    cout << "chi2dof = " << b.chi2dof
	 << " iso = " << b.iso
	 << " m1iso = " << b.m1iso
	 << " m2iso = " << b.m2iso
	 << " alpha = " << b.alpha
	 << endl;
  }

  // -- physics preselection
  if (b.chi2dof > CHI2DOFMAX) return false;
  if (b.iso < ISOMIN) return false;
  if (b.m1iso < ISOMIN) return false;
  if (b.m2iso < ISOMIN) return false;
  if (b.alpha > ALPHAMAX) return false;
  if (verbose > 0) cout << "passed physics cuts" << endl;

  return true;
}


// ----------------------------------------------------------------------
TH1D* getPreselectionNumbers() {
  TH1D *h  = new TH1D("hpresel", "", 100, 0., 100.);
  int ibin(1);
  ibin = 1; h->SetBinContent(ibin, PTMIN); h->GetXaxis()->SetBinLabel(ibin, "ptmin");
  ibin = 2; h->SetBinContent(ibin, PTMAX); h->GetXaxis()->SetBinLabel(ibin, "ptmax");
  ibin = 3; h->SetBinContent(ibin, M1PTMIN); h->GetXaxis()->SetBinLabel(ibin, "m1ptmin");
  ibin = 4; h->SetBinContent(ibin, M1PTMAX); h->GetXaxis()->SetBinLabel(ibin, "m1ptmax");
  ibin = 5; h->SetBinContent(ibin, M2PTMIN); h->GetXaxis()->SetBinLabel(ibin, "m2ptmin");
  ibin = 6; h->SetBinContent(ibin, M2PTMAX); h->GetXaxis()->SetBinLabel(ibin, "m2ptmax");

  ibin = 10; h->SetBinContent(ibin, FL3DMAX); h->GetXaxis()->SetBinLabel(ibin, "fl3dmax");
  ibin = 11; h->SetBinContent(ibin, FLS3DMIN); h->GetXaxis()->SetBinLabel(ibin, "fls3dmin");
  ibin = 12; h->SetBinContent(ibin, FLS3DMAX); h->GetXaxis()->SetBinLabel(ibin, "fls3dmax");
  ibin = 13; h->SetBinContent(ibin, FLSXYMIN); h->GetXaxis()->SetBinLabel(ibin, "flsxymin");
  ibin = 14; h->SetBinContent(ibin, CHI2DOFMAX); h->GetXaxis()->SetBinLabel(ibin, "chi2dofmax");

  ibin = 20; h->SetBinContent(ibin, PVIPMAX); h->GetXaxis()->SetBinLabel(ibin, "pvipmax");
  ibin = 21; h->SetBinContent(ibin, PVIPSMAX); h->GetXaxis()->SetBinLabel(ibin, "pvipsmax");
  ibin = 22; h->SetBinContent(ibin, PVLIPMAX); h->GetXaxis()->SetBinLabel(ibin, "pvlipmax");
  ibin = 23; h->SetBinContent(ibin, PVLIPSMAX); h->GetXaxis()->SetBinLabel(ibin, "pvlipsmax");

  ibin = 30; h->SetBinContent(ibin, MAXDOCAMAX); h->GetXaxis()->SetBinLabel(ibin, "maxdocamax");
  ibin = 31; h->SetBinContent(ibin, CLOSETRKMAX); h->GetXaxis()->SetBinLabel(ibin, "closetrkmax");
  ibin = 32; h->SetBinContent(ibin, DOCATRKMAX); h->GetXaxis()->SetBinLabel(ibin, "docatrkmax");

  ibin = 40; h->SetBinContent(ibin, ISOMIN); h->GetXaxis()->SetBinLabel(ibin, "isomin");
  ibin = 41; h->SetBinContent(ibin, ALPHAMAX); h->GetXaxis()->SetBinLabel(ibin, "alphamax");

  return h;
}

// ----------------------------------------------------------------------
void printRedTreeEvt(redTreeData &b) {
  std::cout << b.run << "/" << b.evt << " mu&hlt: " << b.gmuid << "/" << b.hlt1
	    << std::endl << "  "
	    << " B: " << " " << b.pt << "/" << b.eta << " "
	    << " mu: " << " " << b.m1q << "/" << b.m2q << " "
	    << b.m1pt << "/" << b.m2pt << ", "  << b.m1eta << "/" << b.m2eta
	    << " ->c" << ((TMath::Abs(b.m1eta) < 1.4 && TMath::Abs(b.m2eta) < 1.4)?0 :1)
	    << " fl: " << b.flsxy << " " << b.fls3d << " (" << b.fl3d
	    << ") ip: " << b.pvip << " " << b.pvips << " " << b.pvlip << " " << b.pvlips << " "
	    << std::endl << "  "
	    << " iso: " << b.iso << " " << b.closetrk  << " " << b.docatrk << " "
	    << " miso: " << b.m1iso << " " << b.m2iso
	    << " vtx: dca " << b.maxdoca << " chi " << b.chi2dof << " a " << b.alpha << " me " << b.me
	    << std::endl << "  "
	    << " pvw8: " << b.pvw8 << " gt: " << b.gtqual
	    << std::endl << "   "
	    << preselection()
	    << std::endl;
}
