 /**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/* AliAnaysisTaskMyTask
 *
 * empty task which can serve as a starting point for building an analysis
 * as an example, one histogram is filled
 */

#include "TChain.h"
#include "TH1F.h"
#include "TList.h"
#include "TFile.h"
#include "TMath.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TClonesArray.h"
#include "TRandom3.h"
#include "TComplex.h"

#include "AliAnalysisTask.h"
#include "AliAnalysisManager.h"
#include "AliAnalysisFilter.h"
#include "AliAnalysisUtils.h"
#include "AliESDEvent.h"
#include "AliESDtrack.h"
#include "AliESDVertex.h"
#include "AliESDtrackCuts.h"
#include "AliAODITSsaTrackCuts.h"
#include "AliVEvent.h"
#include "AliVTrack.h"
#include "AliCentrality.h"
#include "AliAODVertex.h"
#include "AliAODTrack.h"
#include "AliVVertex.h"
#include "AliAODMCParticle.h"
#include "AliAODEvent.h"
#include "AliAODInputHandler.h"
#include "AliAnalysisTaskMyTask.h"
#include "AliExternalTrackParam.h"
#include "AliMultSelection.h"
#include "AliInputEventHandler.h"

class AliAnalysisTaskMyTask;    // your analysis class
#include <iostream>
using namespace std;            // std namespace: so you can do things like 'cout'


ClassImp(AliAnalysisTaskMyTask) // classimp: necessary for root

AliAnalysisTaskMyTask::AliAnalysisTaskMyTask() : AliAnalysisTaskSE(), 
    fAOD(0), fOutputList(0), fHistPt(0), fHistPhi(0), fHistEta(0), fHistEtaPhi2D(0),fHistWeightPhi(0), fHistWeightEta(0), fHistWeightEtaPhi2D(0), fHistPhiWithWeights(0),fHistEtaWithWeights(0), fHistEtaPhi2DWithWeight(0), fHistPtAfter(0), fHistPhiAfter(0), fHistEtaAfter(0), fHistEventCount(0), fHistCentrality(0), fHistMult(0), fVtxCut(0), fSample(0), fNUE(0), fNUA(0), fAnalysisType(0), numberEvents(0), fFilterbit(96), fEtaCut(0.8), fMinPt(0.2), fMaxPt(3.0), fTPCclusters(70), fPileupCut(true), fPeriod(""), fEventCuts(0)
{   
    // default constructor, don't allocate memory here!
    // this is used by root for IO purposes, it needs to remain empty
}
//_____________________________________________________________________________
AliAnalysisTaskMyTask::AliAnalysisTaskMyTask(const char* name) : AliAnalysisTaskSE(name),
    fAOD(0), fOutputList(0), fHistPt(0), fHistPhi(0), fHistEta(0), fHistEtaPhi2D(0),fHistWeightPhi(0), fHistWeightEta(0), fHistWeightEtaPhi2D(0), fHistPhiWithWeights(0),fHistEtaWithWeights(0), fHistEtaPhi2DWithWeight(0), fHistPtAfter(0), fHistPhiAfter(0), fHistEtaAfter(0), fHistEventCount(0), fHistCentrality(0), fHistMult(0), fVtxCut(0), fSample(0), fNUE(0), fNUA(0), fAnalysisType(0), numberEvents(0), fFilterbit(96), fEtaCut(0.8), fMinPt(0.2), fMaxPt(3.0), fTPCclusters(70), fPeriod(""), fEventCuts(0)
{
    // constructor
    DefineInput(0, TChain::Class());    // define the input of the analysis: in this case we take a 'chain' of events
                                        // this chain is created by the analysis manager, so no need to worry about it, 
                                        // it does its work automatically
    DefineOutput(1, TList::Class());    // define the ouptut of the analysis: in this case it's a list of histograms 
                                        // you can add more output objects by calling DefineOutput(2, classname::Class())
                                        // if you add more output objects, make sure to call PostData for all of them, and to
                                        // make changes to your AddTask macro!
}
//_____________________________________________________________________________
AliAnalysisTaskMyTask::~AliAnalysisTaskMyTask()
{
    // destructor
    if(fOutputList) {
        delete fOutputList;     // at the end of your task, it is deleted from memory by calling this function
    }
}
//_____________________________________________________________________________
void AliAnalysisTaskMyTask::UserCreateOutputObjects()
{
    // create output objects
    //
    // this function is called ONCE at the start of your analysis (RUNTIME)
    // here you ceate the histograms that you want to use 
    //
    // the histograms are in this case added to a tlist, this list is in the end saved
    // to an output file
    //
    fOutputList = new TList();          // this is a list which will contain all of your histograms
                                        // at the end of the analysis, the contents of this list are written
                                        // to the output file
    fOutputList->SetOwner(kTRUE);       // memory stuff: the list is owner of all objects it contains and will delete them
                                        // if requested (dont worry about this now)

    // Create the histograms:
    fHistPt = new TH1F("fHistPt", "fHistPt", 100, 0, 10);       // create your histogram
    fHistPt->Sumw2();

    fHistPhi = new TH1F("fHistPhi", "fHistPhi", 360, 0, TMath::TwoPi());   // create your histogram for phi
    fHistPhi->Sumw2();

    fHistEta = new TH1F("fHistEta", "fHistEta", 100, -2, 2);   // create histogram for eta 
    fHistEta->Sumw2();

    fHistEtaPhi2D = new TH2F("fHistEtaPhi2D", "test of 2D histogram; phi; eta", 360, 0, TMath::TwoPi(), 100, -2, 2);
    fHistEtaPhi2D->Sumw2();

    fHistWeightPhi = new TH1F("fHistWeightPhi","fHistWeightPhi", 360, 0, TMath::TwoPi()); //
    fHistWeightPhi->Sumw2();

    fHistWeightEta = new TH1F("fHistWeightEta", "fHistWeightEta", 100, -2, 2);  //create histogram for weightEta
    fHistWeightEta->Sumw2();

    fHistWeightEtaPhi2D = new TH2F("fHistWeightEtaPhi2D", "fHistWeightEtaPhi2D", 360, 0, TMath::TwoPi(), 100, -2, 2);
    fHistWeightEtaPhi2D->Sumw2();

    //opening weight file
    // TFile* fileWeights = TFile::Open("Weights_2D.root","READ");
    // if(fileWeights)
    // {
    //     fHistWeightEtaPhi2D = (TH2F*) fileWeights->Get("fHistWeightEtaPhi2D");
    // }
    // else cout << "weights not found" << endl;


    fHistPhiWithWeights = new TH1F("fHistPhiWithWeights","fHistPhiWithWeights", 360, 0, TMath::TwoPi()); //
    fHistPhiWithWeights->Sumw2();

    fHistEtaWithWeights = new TH1F("fHistEtaWithWeights","fHistEtaWithWeights", 100, -2, 2); //
    fHistEtaWithWeights->Sumw2();

    fHistEtaPhi2DWithWeight = new TH2F("fHistEtaPhi2DWithWeight", "2D Eta vs. Phi With weight; phi; eta", 360, 0, TMath::TwoPi(), 100, -2, 2);
    fHistEtaPhi2DWithWeight->Sumw2();

    fHistPtAfter = new TH1F("fHistPtAfter", "fHistPtAfter", 100, 0, 10);   // Pt histogram after cut
    fHistPtAfter->Sumw2();
                    
    fHistPhiAfter = new TH1F("fHistPhiAfter", "fHistPhiAfter", 360, 0, TMath::TwoPi());   // Phi_histogram aftercut
    fHistPhiAfter->Sumw2();

    fHistEtaAfter = new TH1F("fHistEtaAfter", "fHistEtaAfter", 100, -2, 2);             // Eta_histogram aftercut 
    fHistEtaAfter->Sumw2();

    fHistEventCount = new TH1F("fHistEventCount", "; centrality;;", 1, 0, 1);      //histogram of the number of event
    fHistEventCount->Sumw2();

    fHistCentrality = new TH1F("fHistCentralityDis", "centrality distribution; centrality; Counts", 100, 0, 100); //histogram for centrality
    fHistCentrality->Sumw2();

    fHistMult = new TH1F("fHistMult", ";number of tracks; entries", 200, 0, 200);     //histogram for Multiplicity
    fHistMult->Sumw2();
    


    fOutputList->Add(fHistPt);          // don't forget to add it to the list! the list will be written to file, so if you want
    fOutputList->Add(fHistPhi); 
    fOutputList->Add(fHistEta);                                    // your histogram in the output file, add it to the list!
    fOutputList->Add(fHistEtaPhi2D);
    fOutputList->Add(fHistWeightPhi);
    fOutputList->Add(fHistWeightEta);
    fOutputList->Add(fHistWeightEtaPhi2D);
    fOutputList->Add(fHistPhiWithWeights);
    fOutputList->Add(fHistEtaWithWeights);
    fOutputList->Add(fHistEtaPhi2DWithWeight);    
    fOutputList->Add(fHistPtAfter);
    fOutputList->Add(fHistPhiAfter);
    fOutputList->Add(fHistEtaAfter);
    fOutputList->Add(fHistEventCount);
    fOutputList->Add(fHistCentrality);
    fOutputList->Add(fHistMult);
    
    PostData(1, fOutputList);           // postdata will notify the analysis manager of changes / updates to the 
                                        // fOutputList object. the manager will in the end take care of writing your output to file
                                        // so it needs to know what's in the output
}
//_____________________________________________________________________________
void AliAnalysisTaskMyTask::UserExec(Option_t *)
{
    //if(numberEvents > 50) return;
    // return acts in a same way as "break" but for function use "return" and for "if" and "while" loop --> "break".
    // user exec
    // this function is called once for each event
    // the manager will take care of reading the events from file, and with the static function InputEvent() you 
    // have access to the current event. 
    // once you return from the UserExec function, the manager will retrieve the next event from the chain
    
    //..apply Physics Selection (PS)
	UInt_t fSelectMask = ((AliInputEventHandler*)(AliAnalysisManager::GetAnalysisManager()->GetInputEventHandler()))->IsEventSelected();
	Bool_t isTrigselected = false;
	isTrigselected = fSelectMask&AliVEvent::kINT7;   //Trigger will be selected after passing some criteria for each event,"kINT7" is V0AND minimum bias trigger.  
	if(isTrigselected == false) return;

    fHistEventCount->Fill("after PS", 1.);
    
    fAOD = dynamic_cast<AliAODEvent*>(InputEvent());    // get an event (called fAOD) from the input file
                                                        // there's another event format (ESD) which works in a similar way
                                                        // but is more cpu/memory unfriendly. for now, we'll stick with aod's
    if(!fAOD) return;                                   // if the pointer to the event is empty (getting it failed) skip this event
    //if(IsGoodEvent(fAOD) == false) return;
    // The new method to do EventSelection: fEventCuts is a variable in AliEventCuts and then this variable has acess to all function in this class. so fEventCuts.AcceptEvent will do the cuts for us, we don't use the manual way to do EventCuts anymore. 
    
    if(!fEventCuts.AcceptEvent(fAOD)){
        PostData(1,fOutputList);
        return;
    }

    //..AliMultSelection
	float fMultV0Meq = 0;
	float fMultMeanV0M = 0;
	float centrV0 = 0;
    
    //DO The AliMultSelection: 
    AliMultSelection *MultSelection = (AliMultSelection*)fInputEvent->FindListObject("MultSelection");
	fMultV0Meq = MultSelection->GetEstimator("V0M")->GetValue();
	fMultMeanV0M = MultSelection->GetEstimator("V0M")->GetMean();
    centrV0 = MultSelection->GetMultiplicityPercentile("V0M");
    fHistCentrality->Fill(centrV0);
    
    /////////////////////////////////////////////////////////////////////////////
    // example part: i'll show how to loop over the tracks in an event //////////
    // and extract some information from them which we'll store in a histogram///
    /////////////////////////////////////////////////////////////////////////////
    Int_t NumberOfTracks = 0;

    Int_t iTracks(fAOD->GetNumberOfTracks());           // see how many tracks there are in the event
    for(Int_t i(0); i < iTracks; i++) {                 // loop over all these tracks
        AliAODTrack* track = static_cast<AliAODTrack*>(fAOD->GetTrack(i));         // get a track (type AliAODTrack) from the event
        if(!track) continue;                            // if we failed, skip this track
        
        //if(!track->TestFilterBit(128)) continue;       //this will gives us only the tracks from Only TPC.   
        //if(!track->TestFilterBit(32)) continue;         //this is the 
        if(!track->TestFilterBit(fFilterbit)) continue;  //this is the Global track=96
        
        //Filling the histograms:
        fHistPt->Fill(track->Pt());
        fHistPhi->Fill(track->Phi());                     // plot the pt value of the track in a histogram
        fHistEta->Fill(track->Eta());

        //...Do the Cut:
    	if((track->Pt() < 0.2) || (track->Pt() > 3.0)) {
			continue;
		}
		if((track->Eta() < -0.8) ||  (track->Eta() > 0.8)) {
			continue;
		}
		if(track->GetTPCNcls() < fTPCclusters) {
			continue;
        }

        //... Fill the histograms after cut:        
        fHistPtAfter->Fill(track->Pt());
        fHistPhiAfter->Fill(track->Phi());
        fHistEtaAfter->Fill(track->Eta());
        //fHistMult->Fill(naftercut);
        fHistEtaPhi2D->TH2::Fill(track->Phi(), track->Eta());
        // continue until all the tracks are processed
        NumberOfTracks += 1;
    }
    fHistMult->Fill(NumberOfTracks);
    
    // filling the weight-Histogram:
    for(int j = 1; j < fHistPhi->GetNbinsX()+1; j++)
    {
        // fHistWeightPhi->SetBinContent(j, median / bins[j-1]);
        if(fHistPhi->GetBinContent(j) > 0.)
            fHistWeightPhi->SetBinContent(j, fHistPhi->GetMaximum() / fHistPhi->GetBinContent(j) );

        else continue;
    }
    
    for(int j = 1; j < fHistEta->GetNbinsX()+1; j++)
    {
        if(fHistEta->GetBinContent(j) > 0.)
            fHistWeightEta->SetBinContent(j, fHistEta->GetMaximum() / fHistEta->GetBinContent(j) );
        
        else continue;
    }
    // end of filling weights in 1D Weight-Histogram
    // Start to get weight 2D:

    double weight;
    double weightEta; 
    double weightPhi;
    double weightEtaPhi;
    //printf("Max %f",fHistEtaPhi2D->GetMaximum());
    
    for(int i = 1; i < fHistEtaPhi2D->GetNbinsX()+1; i++)
    {
        for(int j = 1; j < fHistEtaPhi2D->GetNbinsY()+1; j++)
        {
            if(fHistEtaPhi2D->GetBinContent(i, j) > 0.)
            {
                fHistEtaPhi2D->GetYaxis()->SetRange(j,j);  
                weight = fHistEtaPhi2D->GetMaximum() / fHistEtaPhi2D->GetBinContent(i,j);
                //cout << "weight: " << weight << endl;
            }
            else
            {
                weight = 1.;        
            }
            fHistWeightEtaPhi2D->SetBinContent(i, j, weight);
        }
    }

    //*************************************************************************
    // new method:

    // double weight;
    // for(int i = 1; i < fHistEtaPhi2D->GetNbinsX()+1; i++)
    // {
    //     for(int j = 1; j < fHistEtaPhi2D->GetNbinsY()+1; j++)
    //     {
    //         if(fHistEtaPhi2D->GetBinContent(i,j) > 0.)
    //             weight = fHistEtaPhi2D->GetMaximum() / fHistEtaPhi2D->GetBinContent(i,j);
    //         else
    //             weight = 1.;

    //         fHistWeightEtaPhi2D->SetBinContent(i, j, weight);
    //     }
    // }
    //*************************************************************************

    AliAODTrack* track2 = 0x0;   // it is a ssme as initial with 0, but looks nice and it means is for pointer as well. 
    float phi = 0;
    float eta = 0;

    // looping over tracks:
    for(Int_t h(0); h < iTracks; h++) {           
        track2 = static_cast<AliAODTrack*>(fAOD->GetTrack(h));
       
        if(!track2) continue;    // if we failed, skip this track
          
        if(!track2->TestFilterBit(fFilterbit)) continue;       //if(!track2->TestFilterBit(128)) continue;      

        if((track2->Pt() < 0.2) || (track2->Pt() > 3.0)) {
			continue;
		}
		if((track2->Eta() < -0.8) ||  (track2->Eta() > 0.8)) {
			continue;
		}
		if(track2->GetTPCNcls() < fTPCclusters) {
			continue;
        }

        phi = track2->Phi();
        eta = track2->Eta();

        weightPhi = fHistWeightPhi->GetBinContent( fHistWeightPhi->FindBin(phi));
        fHistPhiWithWeights->TH1::Fill(phi,weightPhi);
       
        weightEta = fHistWeightEta->GetBinContent( fHistWeightEta->FindBin(eta));
        fHistEtaWithWeights->TH1::Fill(eta, weightEta);

        weightEtaPhi = fHistWeightEtaPhi2D->GetBinContent( fHistWeightEtaPhi2D->FindBin(track2->Phi(),track2->Eta()) );
        fHistEtaPhi2DWithWeight->Fill(track2->Phi(),track2->Eta(),weightEtaPhi);
        //cout << "weight2D" << weightEtaPhi << endl;            
    }   
    // stream the results the analysis of this event to
    // the output manager which will take care of writing it to a file
   
    PostData(1, fOutputList); 
    //numberEvents++;
}
//_____________________________________________________________________________
void AliAnalysisTaskMyTask::Terminate(Option_t *)
{
    // terminate
    // called at the END of the analysis (when all events are processed)
}
//_____________________________________________________________________________
