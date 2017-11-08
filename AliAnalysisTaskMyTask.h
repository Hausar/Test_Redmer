/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. */
/* See cxx source for full Copyright notice */
/* $Id$ */

#ifndef AliAnalysisTaskMyTask_H
#define AliAnalysisTaskMyTask_H

#include "AliAnalysisTaskSE.h"
#include "AliEventCuts.h"

class TList;
class TH1F;
class TH1;
class TH2;
class TH3F;
class TH1F;
class TH2F;
class TH3D;
class TProfile;
class TProfile2D;
class TComplex;
class AliESDEvent;
class AliAODEvent;
class AliVEvent;
class AliVTrack;
class AliVVertex;
class AliESDtrackCuts;
class AliAODITSsaTrackCuts;
class AliInputEventHandler;


class AliAnalysisTaskMyTask : public AliAnalysisTaskSE  
{
    public:
                                AliAnalysisTaskMyTask();
                                AliAnalysisTaskMyTask(const char *name);
        virtual                 ~AliAnalysisTaskMyTask();

        virtual void            UserCreateOutputObjects();
        virtual void            UserExec(Option_t* option);
        virtual void            Terminate(Option_t* option);

        virtual void            SetAnalysisType(const char* analysisType) {fAnalysisType = analysisType;}
        virtual void	        SetFilterbit(Int_t bit){fFilterbit = bit;}
        virtual void            SetEtaCut(Double_t etaCut){fEtaCut = etaCut;}
        virtual void            SetVtxCut(Double_t vtxCut){fVtxCut = vtxCut;}
        virtual void            SetMinPt(Double_t minPt){fMinPt = minPt;}
        virtual void            SetMaxPt(Double_t maxPt){fMaxPt = maxPt;}
        virtual void	        SetTPCclusters(Int_t tpcClus){fTPCclusters = tpcClus;}
        virtual void            SetIsSample(Int_t IsSample){fSample = IsSample;}
        virtual void	        SetNUEFlag(Bool_t NUE){fNUE = NUE;}
        virtual void	        SetNUA(Bool_t NUA){fNUA = NUA;}
        virtual void	        SetPileupCut(Bool_t pileup){fPileupCut = pileup;}
        virtual void	        SetPeriod(TString period){fPeriod = period;}
                                AliEventCuts fEventCuts;  /// Event Cuts
                                
    private:
        AliAODEvent*            fAOD;           //! input event
        TList*                  fOutputList;    //! output list  
        TH1F*                   fHistPt;        //! dummy histogram
        TH1F*                   fHistPhi;       //! Phi histogram
        TH1F*                   fHistEta;      //! Eta histogram
        TH1F*                   fHistWeightPhi;    //! weigts
        TH1F*                   fHistWeightEta;  //! EtaWeights
        TH1F*                   fHistPhiWithWeights; //! histogram with weights
        TH1F*                   fHistEtaWithWeights;    //! histogram of Eta with weights
        TH1F*                   fHistPtAfter;     //! histogram for Pt aftercut
        TH1F*                   fHistPhiAfter;    //! histogram for Phi aftercut
        TH1F*                   fHistEtaAfter;   //! histogram for Eta aftercut 
        TH1F*                   fHistEventCount;  //! histogram for EventCount
        TH1F*                   fHistCentrality;    //! histogram for Centrality
        TH1F*                   fHistMult;       //! histogram for Multiplicity
        TH2F*                   fHistEtaPhi2D;   //! 2D histogram for Weight
        TH2F*                   fHistWeightEtaPhi2D; //! container for loading weights for givenrun
        TH2F*                   fHistEtaPhi2DWithWeight;  //! 2Dhist (Phi,Eta) With Weight 
        Int_t                   numberEvents;
        Int_t				    fFilterbit;								// flag for filter bit        
        Double_t                fEtaCut;             			// Eta cut used to select particles
        Double_t                fVtxCut; 
        Double_t                fMinPt;              			// Min pt - for histogram limits
        Double_t                fMaxPt;                         // Max pt - for histogram limits
        Int_t				    fTPCclusters;						// min. TPC clusters
        Int_t      	            fSample;             			            // number of sample
        Bool_t			        fNUE;											// flag for NUE correction
        Bool_t			        fNUA;											// 0: no NUA correction, 1: NUA correction
        Bool_t			        fPileupCut;								// flag for pileup cuts
        TString			        fPeriod;									// period
        TString                 fAnalysisType;           //  "ESD" or "AOD"
        
        AliAnalysisTaskMyTask(const AliAnalysisTaskMyTask&); // not implemented
        AliAnalysisTaskMyTask& operator=(const AliAnalysisTaskMyTask&); // not implemented

        ClassDef(AliAnalysisTaskMyTask, 1);
};

#endif
