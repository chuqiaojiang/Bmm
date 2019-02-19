#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TMath.h"
#include "TROOT.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TText.h"
#include "TH2.h"
#include "TGraph.h"
#include "TPaveStats.h"
#include "TMinuit.h"
#include "TVirtualFitter.h"

#include "tmva1.hh"
#include "common/initFunc.hh"

#include "TMVA/Config.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Factory.h"
#include "TMVA/Reader.h"
#include "TMVA/Tools.h"
#include "TMVA/tmvaglob.h"
#include "TMVA/TMVARegGui.h"

#define MINKS 0.05

ClassImp(tmva1)

using namespace std;

// ----------------------------------------------------------------------
// --
// -- USAGE: a.makeAll(0, 1); > TMVA-0.log
// --
// ----------------------------------------------------------------------
tmva1::tmva1(int year, string vars, string pars) {

  fVariables     = vars;
  fBDTParameters = pars;

  cout << "tmva1 hello: setup"
       << " with variables:  " << vars << endl
       << " with parameters: " << fBDTParameters
       << endl;

  fVariables = vars;
}


// ----------------------------------------------------------------------
tmva1::~tmva1() {
  cout << "tmva1 good bye " << endl;
}


// ----------------------------------------------------------------------
void tmva1::makeAll(int offset, string filename) {
  make(offset, filename);
}

// ----------------------------------------------------------------------
void tmva1::make(int offset, string filename) {

  string oname = Form("TMVA-%d", offset);
  cout << "======================================================================" << endl;
  cout << "==> tmva1(" << oname << ", " << filename << ") " << endl;
  cout << "======================================================================" << endl;
  train(oname, filename);
}

// ----------------------------------------------------------------------
void tmva1::train(string oname, string filename, string cut) {
  // This loads the library
  TMVA::Tools::Instance();

  (TMVA::gConfig().GetVariablePlotting()).fNbins1D = 40;
  (TMVA::gConfig().GetVariablePlotting()).fNbinsMVAoutput = 40;

  // -- Create a ROOT output file where TMVA will store ntuples, histograms, etc.
  TString outfileName(Form("%s.root", oname.c_str()));
  TFile* outputFile = TFile::Open(outfileName, "RECREATE" );

  TH1D *hSetup = new TH1D("hSetup", "hSetup", 100, 0., 100.);
  int i(0);

  cout << "----------------------------------------------------------------------" << endl;
  cout << "==> oname: " << oname <<  endl;
  string optstring = "V:!Silent:!Color:!DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Regression ";
  optstring        = "V:!Silent:!Color:!DrawProgressBar:AnalysisType=Regression";
  cout << "==> Factory: " << optstring << endl;
  TMVA::Factory *factory = new TMVA::Factory(Form("%s", oname.c_str()), outputFile,  optstring.c_str());
  TMVA::DataLoader *dataloader = new TMVA::DataLoader("dataset");

  // -- parse string with all variables into a vector
  vector<string> vVar;
  string allvar = fVariables;
  string::size_type m0(allvar.size());
  cout << "--> allvar = " << allvar << endl;
  string var;
  while (string::npos != m0) {
    m0 = allvar.find(":");
    var = allvar.substr(0, m0);
    vVar.push_back(var);
    allvar = allvar.substr(m0+1);
    m0 = allvar.find(":");
  }
  var = allvar;
  vVar.push_back(var);

  for (unsigned int i = 0; i < vVar.size(); ++i) {
    cout << " addVariable:  " << vVar[i] << endl;
    //      if (string::npos != vVar[i].find("closetrk")) {
    //        factory->AddVariable(vVar[i].c_str(), 'I');
    //      } else {
    dataloader->AddVariable(vVar[i].c_str(), 'F');
    //      }
  }

  dataloader->AddTarget("gparanu");

  TFile* inFile;

  inFile = TFile::Open(filename.c_str());
  cout << "opening " << filename << " inFile = " << inFile << endl;

  TTree *regTree = (TTree*)inFile->Get("candAnaRecoil/events");
  double regWeight(1.0);

  dataloader->AddRegressionTree(regTree, regWeight);
  TCut mycut = "flstau>0."; // for example: TCut mycut = "abs(var1)<0.5 && abs(var2-0.5)<1";
  // tell the DataLoader to use all remaining events in the trees after training for testing: 
  dataloader->PrepareTrainingAndTestTree(mycut,
                                         "nTrain_Regression=20000:nTest_Regression=0:SplitMode=Random:NormMode=NumEvents:!V" );

  // factory->BookMethod(dataloader, TMVA::Types::kBDT, "BDTG",
  // 		      "!H:!V:NTrees=2000::BoostType=Grad:Shrinkage=0.1:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=20:MaxDepth=3:MaxDepth=4" );

  // factory->BookMethod(dataloader, TMVA::Types::kBDT, "BDTG",
  // 		      TString("!H:!V:NTrees=200:BoostType=Grad:Shrinkage=0.3:nCuts=20:MaxDepth=4:")
  // 		      + TString("RegressionLossFunctionBDTG=AbsoluteDeviation"));


  factory->BookMethod( dataloader,  TMVA::Types::kBDT, "BDTG",
		       "!H:!V:NTrees=2000::BoostType=Grad:Shrinkage=0.1:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=20:MaxDepth=3:MaxDepth=4" );



  if (0) {
    // -- so far I have not found good parameters to achieve better performance than for BDTG!
    TString layoutString ("Layout=TANH|100,LINEAR");
    TString training0 ("LearningRate=1e-5,Momentum=0.5,Repetitions=1,ConvergenceSteps=500,BatchSize=50,TestRepetitions=7,WeightDecay=0.01,Regularization=NONE,DropConfig=0.5+0.5+0.5+0.5,DropRepetitions=2,Multithreading=True");
    TString training1 ("LearningRate=1e-5,Momentum=0.9,Repetitions=1,ConvergenceSteps=170,BatchSize=30,TestRepetitions=7,WeightDecay=0.01,Regularization=L2,DropConfig=0.1+0.1+0.1,DropRepetitions=1,Multithreading=True");
    TString training2 ("LearningRate=1e-5,Momentum=0.3,Repetitions=1,ConvergenceSteps=150,BatchSize=40,TestRepetitions=7,WeightDecay=0.01,Regularization=NONE,Multithreading=True");
    TString training3 ("LearningRate=1e-6,Momentum=0.1,Repetitions=1,ConvergenceSteps=500,BatchSize=100,TestRepetitions=7,WeightDecay=0.0001,Regularization=NONE,Multithreading=True");

    TString trainingStrategyString ("TrainingStrategy=");
    trainingStrategyString += training0 + "|" + training1 + "|" + training2 + "|" + training3;
    TString nnOptions ("!H:V:ErrorStrategy=SUMOFSQUARES:VarTransform=G:WeightInitialization=XAVIERUNIFORM");
    //       TString nnOptions ("!H:V:VarTransform=Normalize:ErrorStrategy=CHECKGRADIENTS");
    nnOptions.Append (":"); nnOptions.Append (layoutString);
    nnOptions.Append (":"); nnOptions.Append (trainingStrategyString);
    factory->BookMethod(dataloader, TMVA::Types::kDNN, "DNN", nnOptions );
  }

  cout << "==> TrainAllMethods " << endl;
  factory->TrainAllMethods();

  // ---- Evaluate all MVAs using the set of test events
  cout << "==> TestAllMethods " << endl;
  factory->TestAllMethods();

  // ----- Evaluate and compare performance of all configured MVAs
  cout << "==> EvaluateAllMethods " << endl;
  factory->EvaluateAllMethods();

  // Save the output
  outputFile->Close();
  delete factory;
  delete dataloader;
  if (!gROOT->IsBatch()) TMVA::TMVARegGui( outfileName );

  if (0) {
    gROOT->Clear();
    gROOT->DeleteAll();
    cout << "==> TMVAClassification is done!" << endl;
  }

}
