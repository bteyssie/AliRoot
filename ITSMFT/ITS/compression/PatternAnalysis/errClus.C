/*
This macro creates for each pattern in the database (generated by BuildClTop.C) two histograms, for MChit-COG difference in x and z direction respectively
This histograms are filled with the data from compClusHits.C. With a gaussian fit we get mean value, sigma, chi2, NDF and update the database of patterns
with this information.
*/

#if !defined(__CINT__) || defined(__MAKECINT__)
#include "TObjArray.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TArrayI.h"
#include "TArrayF.h"
#include "TCanvas.h"
#include "TBits.h"
#include "TF1.h"
#include "../ITS/UPGRADE/AliITSUClusterPix.h"
#include "../ITS/UPGRADE/AliITSURecoLayer.h"
#include "../ITS/UPGRADE/AliITSURecoDet.h"
#include "../ITS/UPGRADE/AliITSUHit.h"
#include "../ITS/UPGRADE/AliITSUGeomTGeo.h"
#include "AliITSsegmentation.h"
#include "AliGeomManager.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TPaveStats.h"

#endif

TObjArray *pattDB=0; //it is an array with all the patterns in TBits format (forom the most to the least frequent)
TVectorF* pattFR=0; //frequecy of the pattern in the database
TVectorF* xCentrePix=0; //coordinate of the centre of the pixel containing the COG for the down-left corner in fracion of the pitch
TVectorF* zCentrePix=0;
TVectorF* xCentreShift=0; //Difference between x coordinate fo COG and the centre of the pixel containing the COG, in fraction of the pitch
TVectorF* zCentreShift=0;
TVectorF* NPix=0; //Number of fired pixels
TVectorF* NRow=0; //Number of rows of the pattern
TVectorF* NCol=0; //Number of columns of the pattern

TArrayF DeltaZmean; //mean value of the difference between MChit and COG z coordinate in micron derived from a gaussian fit
TArrayF DeltaZmeanErr;
TArrayF DeltaXmean; //mean value of the difference between MChit and COG x coordinate in micron derived from a gaussian fit
TArrayF DeltaXmeanErr;
TArrayF DeltaZsigma; //sigma of the difference between MChit and COG z coordinate in micron derived from a gaussian fit
TArrayF DeltaZsigmaErr;
TArrayF DeltaXsigma; //sigma of the difference between MChit and COG x coordinate in micron derived from a gaussian fit
TArrayF DeltaXsigmaErr;
TArrayF Chi2x; //Chi2 of the gaussian fit over x
TArrayF Chi2z; //Chi2 of the aussian fut over z
TArrayI NDFx;
TArrayI NDFz;

TObjArray histoArr;

/*Int_t err1counter=0;
Int_t err2counter=0;
Int_t err3counter=0;*/

enum{kXhisto=0,kZhisto=1};

void LoadDB(const char* fname);
void CreateHistos(TObjArray* harr);
TH1* GetHistoX(Int_t pattID, TObjArray* harr);
TH1* GetHistoZ(Int_t pattID, TObjArray* harr);
void DrawReport(TObjArray* harr);
void DrawNP(int pattID, TObjArray* harr);
void UpDateDB(const char* outDBFile);

typedef struct {
  Int_t evID;
  Int_t volID;
  Int_t lrID;
  Int_t clID;
  Int_t nPix;
  Int_t nX;
  Int_t nZ;
  Int_t q;
  Float_t pt;
  Float_t eta;
  Float_t phi;
  Float_t xyz[3];
  Float_t dX;
  Float_t dY;
  Float_t dZ;  
  Bool_t split;  
  Bool_t prim;
  Int_t  pdg;
  Int_t  ntr;
  Float_t alpha; // alpha is the angle in y-radius plane in local frame
  Float_t beta;  // beta is the angle in xz plane, taken from z axis, growing counterclockwise
  Int_t nRowPatt;
  Int_t nColPatt;
  Int_t pattID;
  Float_t freq;
  Float_t xCen;
  Float_t zCen;
  Float_t zShift;
  Float_t xShift;
} clSumm;

static clSumm Summ;

Int_t nPatterns=0;

void errClus(){

	//importing data

  LoadDB("clusterTopology.root");

  nPatterns = (pattDB->GetEntriesFast());

  printf("\n\n nPatterns %d\n\n",nPatterns);

  DeltaZmean.Set(nPatterns+100);
  DeltaZmeanErr.Set(nPatterns+100);
  DeltaXmean.Set(nPatterns+100);
  DeltaXmeanErr.Set(nPatterns+100);
  DeltaZsigma.Set(nPatterns+100);
  DeltaZsigmaErr.Set(nPatterns+100);
  DeltaXsigma.Set(nPatterns+100);
  DeltaXsigmaErr.Set(nPatterns+100);
  Chi2x.Set(nPatterns+100);
  Chi2z.Set(nPatterns+100);
  NDFx.Set(nPatterns+100);
  NDFz.Set(nPatterns+100);

  TFile *input = new TFile("clInfo.root","read");
	TTree *clitsu = (TTree*) input->Get("clitsu");

	Int_t ntotCl = (Int_t) clitsu->GetEntries();
	TBranch *brdZ = clitsu->GetBranch("dZ");
 	brdZ->SetAddress(&Summ.dZ);
  TBranch *brdX = clitsu->GetBranch("dX");
  brdX->SetAddress(&Summ.dX);
  TBranch *brpattID = clitsu->GetBranch("pattID");
  brpattID->SetAddress(&Summ.pattID);

  CreateHistos(&histoArr);

  for(Int_t iCl=0; iCl<ntotCl; iCl++){
    clitsu->GetEntry(iCl);
    GetHistoX(Summ.pattID, &histoArr)->Fill(Summ.dX);
    GetHistoZ(Summ.pattID, &histoArr)->Fill(Summ.dZ);  
  }
  
  DrawReport(&histoArr);

  //printf("\n\nNDF is 0 : %d times\n\nNDF and fitstati not corresponding: %d times\n\nNDF not corresponding : %d times\n\n", err3counter,err1counter, err2counter);

  UpDateDB("clusterTopology2.root");

}

void LoadDB(const char* fname){ //load the data base built by BuildClTopDB.C

  // load database
  TFile* fl = TFile::Open(fname);
  if(!fl){printf("Could not find %s",fname); exit(1);}
  pattDB = (TObjArray*)fl->Get("TopDB");
  pattFR = (TVectorF*)fl->Get("TopFreq");
  xCentrePix =(TVectorF*)fl->Get("xCOG");
  zCentrePix =(TVectorF*)fl->Get("zCOG");
  xCentreShift =(TVectorF*)fl->Get("xShift");
  zCentreShift =(TVectorF*)fl->Get("zShift");
  NPix =(TVectorF*)fl->Get("NPix");
  NCol =(TVectorF*)fl->Get("NCol");
  NRow =(TVectorF*)fl->Get("NRow");
}

void CreateHistos(TObjArray* harr){ // creates two histograms for each pattern in the DB

  printf("\n\n Creating histograms...   ");

  Int_t histonumber=0;

  if(!harr) harr = &histoArr;

  for(Int_t i=0; i<nPatterns; i++){

    TH1* h = new TH1F(Form("hX%d", i),Form("#DeltaX for pattID %d", i),100,-30,30);
    h->SetDirectory(0);
    harr->AddAtAndExpand(h, i);
    histonumber++;
  }

  for(Int_t i=0; i<nPatterns; i++){

    TH1* h = new TH1F(Form("hZ%d", i),Form("#DeltaZ for pattID %d", i),100,-30,30);
    h->SetDirectory(0);
    harr->AddAtAndExpand(h, i+nPatterns);
    histonumber++;
  }

  printf(" %d histograms created, corresponding to %d patterns\n\n", histonumber, histonumber/2);
}

TH1* GetHistoX(Int_t pattID, TObjArray* harr){

  TH1* h=0;
  if(!harr) harr = &histoArr;

  h=(TH1*)harr->At(pattID);
  if (!h) {printf("Unknown histo id=%d\n",pattID); exit(1);}
  return h;
}

TH1* GetHistoZ(Int_t pattID, TObjArray* harr){

  Int_t zID=nPatterns+pattID;

  TH1* h=0;
  if(!harr) harr = &histoArr;
  
  h=(TH1*)harr->At(zID);
  if (!h) {printf("Unknown histo id=%d\n",zID); exit(1);}
  return h;
}

void DrawReport(TObjArray* harr) 
{
  // plot all the nPatterns cluster
  for (int i=0;i<nPatterns;i++) {
    DrawNP(i,harr);
  }
}

void DrawNP(int pattID, TObjArray* harr)
{
  static TF1* gs = new TF1("gs","gaus",-500,500);
  if (!harr) harr = &histoArr;

  printf("\n\nProcessing #DeltaX of pattern %d...",pattID);

  Int_t fitStatusX=0;

  TH1* hdx = (TH1*)harr->At(pattID);
  hdx->GetXaxis()->SetTitle("#DeltaX, #mum");
  hdx->SetTitle(Form("#DeltaX for pattID %d",pattID));
  gs->SetParameters(hdx->GetMaximum(),hdx->GetMean(),hdx->GetRMS());
  if((hdx->GetEntries())<100) fitStatusX = hdx->Fit("gs","Ql0");
  else fitStatusX = hdx->Fit("gs","Q0");

  if(fitStatusX==0){
    Double_t px1 = gs->GetParameter(1);
    DeltaXmean[pattID]=px1;
    Double_t ex1 = gs->GetParError(1);
    DeltaXmeanErr[pattID]=ex1;
    Double_t px2 = gs->GetParameter(2);
    DeltaXsigma[pattID]=px2;
    Double_t ex2 = gs->GetParError(2);
    DeltaXsigmaErr[pattID]=ex2;
    Double_t ChiSqx = gs->GetChisquare();
    Chi2x[pattID] = ChiSqx;
    Int_t varNDFx = gs->GetNDF();
    if(varNDFx>=0)
    NDFx[pattID] = varNDFx;
    else{
      /*if(varNDFx==0){
        printf("\n\nNDF = 0, Chi2 = %e for pattern %d\n\n",ChiSqx,pattID);
        err3counter++;
      }*/
    NDFx[pattID]=-1;
    }
  }
  else{
    DeltaXmean[pattID]=0;
    DeltaXmeanErr[pattID]=0;
    DeltaXsigma[pattID]=0;
    DeltaXsigmaErr[pattID]=0;
    Chi2x[pattID] = -1;   
  }

  printf("done!!\n\n");

  printf("Processing #DeltaZ of pattern %d... ",pattID);

  Int_t fitStatusZ=0;

  TH1* hdz = (TH1*)harr->At(pattID+nPatterns);
  hdz->SetTitle(Form("#DeltaZ for pattID %d",pattID));
  hdz->GetXaxis()->SetTitle("#DeltaZ, #mum");
  gs->SetParameters(hdz->GetMaximum(),hdz->GetMean(),hdz->GetRMS());
  if((hdz->GetEntries())<100) fitStatusZ = hdz->Fit("gs","Ql0");
  else fitStatusZ = hdz->Fit("gs","Q0");

  if(fitStatusZ==0){
    Double_t pz1=gs->GetParameter(1);
    DeltaZmean[pattID]=pz1;
    Double_t ez1 = gs->GetParError(1);
    DeltaZmeanErr[pattID]=ez1;
    Double_t pz2 = gs->GetParameter(2);
    DeltaZsigma[pattID]=pz2;
    Double_t ez2 = gs->GetParError(2);
    DeltaZsigmaErr[pattID]=ez2;
    Double_t ChiSqz = gs->GetChisquare();
    Chi2z[pattID]=ChiSqz;
    Int_t varNDFz = gs->GetNDF();
    if(varNDFz>=0)
    NDFz[pattID] = varNDFz;
    else
    NDFz[pattID] = -1;
  }
  else{
    DeltaZmean[pattID]=0;
    DeltaZmeanErr[pattID]=0;
    DeltaZsigma[pattID]=0;
    DeltaZsigmaErr[pattID]=0;
    Chi2z[pattID] = -1;
  }

  /*if((NDFz[pattID]!=NDFx[pattID])&&fitStatusZ!=fitStatusX){
    printf("\n\nERR: neither NDF nor FitStatus correspond!\n\n");
    err1counter++;
  }
  else if(NDFz[pattID]!=NDFx[pattID]){
    printf("\n\nERR: NDFx and NDFz donnot correspond!\n\n");
    err2counter++;
  }*/

  printf("done!!\n\n");

}

void UpDateDB(const char* outDBFile){

  printf("\n\n\nStoring data in the DataBase\n\n\n");

  TString outDBFileName = outDBFile;
  if (outDBFileName.IsNull()) outDBFileName = "clusterTopology2.root";

  TVectorF arrDeltaZmean(nPatterns);
  TVectorF arrDeltaZmeanErr(nPatterns);
  TVectorF arrDeltaXmean(nPatterns);
  TVectorF arrDeltaXmeanErr(nPatterns);
  TVectorF arrDeltaZsigma(nPatterns);
  TVectorF arrDeltaZsigmaErr(nPatterns);
  TVectorF arrDeltaXsigma(nPatterns);
  TVectorF arrDeltaXsigmaErr(nPatterns);
  TVectorF arrChi2x(nPatterns);
  TVectorF arrNDFx(nPatterns);
  TVectorF arrChi2z(nPatterns);
  TVectorF arrNDFz(nPatterns);

  for(Int_t ID=0; ID<nPatterns; ID++){

    printf("Processing pattern %d... ", ID);

    arrDeltaZmean[ID]=DeltaZmean[ID];
    arrDeltaZmeanErr[ID]=DeltaZmeanErr[ID];
    arrDeltaXmean[ID]=DeltaXmean[ID];
    arrDeltaXmeanErr[ID]=DeltaXmeanErr[ID];
    arrDeltaZsigma[ID]=DeltaZsigma[ID];
    arrDeltaZsigmaErr[ID]=DeltaZsigmaErr[ID];
    arrDeltaXsigma[ID]=DeltaXsigma[ID];
    arrDeltaXsigmaErr[ID]=DeltaXsigmaErr[ID];
    arrChi2x[ID]=Chi2x[ID];
    arrNDFx[ID]=NDFx[ID];
    arrChi2z[ID]=Chi2z[ID];
    arrNDFz[ID]=NDFz[ID];

    printf("done!!\n\n");
  }

  TFile* flDB = TFile::Open(outDBFileName.Data(), "recreate");
  flDB->WriteObject(pattDB,"TopDB","kSingleKey");
  flDB->WriteObject(NPix,"NPix","kSingleKey");
  flDB->WriteObject(NRow,"NRow","kSingleKey");
  flDB->WriteObject(NCol,"NCol","kSingleKey");
  flDB->WriteObject(pattFR, "TopFreq","kSingleKey");
  flDB->WriteObject(xCentrePix,"xCOG","kSingleKey");
  flDB->WriteObject(xCentreShift,"xShift","kSingleKey");
  flDB->WriteObject(zCentrePix,"zCOG","kSingleKey");
  flDB->WriteObject(zCentreShift,"zShift","kSingleKey");
  flDB->WriteObject(&arrDeltaZmean,"DeltaZmean","kSingleKey");
  flDB->WriteObject(&arrDeltaZmeanErr,"DeltaZmeanErr","kSingleKey");
  flDB->WriteObject(&arrDeltaXmean,"DeltaXmean","kSingleKey");
  flDB->WriteObject(&arrDeltaXmeanErr,"DeltaXmeanErr","kSingleKey");
  flDB->WriteObject(&arrDeltaZsigma,"DeltaZsigma","kSingleKey");
  flDB->WriteObject(&arrDeltaZsigmaErr,"DeltaZsigmaErr","kSingleKey");
  flDB->WriteObject(&arrDeltaXsigma,"DeltaXsigma","kSingleKey");
  flDB->WriteObject(&arrDeltaXsigmaErr,"DeltaXsigmaErr","kSingleKey");
  flDB->WriteObject(&arrChi2x,"Chi2x","kSingleKey");
  flDB->WriteObject(&arrChi2z,"Chi2z","kSingleKey");
  flDB->WriteObject(&arrNDFx,"NDFx","kSingleKey");
  flDB->WriteObject(&arrNDFz,"NDFz","kSingleKey");
  //
  flDB->Close();
  delete flDB;

  printf("\n\nDB Complete!!\n\n");
}
