#ifdef __CLING__
#include "AliAnalysisAlien.h"
#include "AliAnalysisManager.h"
#include "AliAODInputHandler.h"
#include "AliAnalysisTaskMyTask.h"
#endif

void runAnalysis()
{   
    // we can make two extra Bool_t grid = KTRUE or Bool_t grid = KFALSE 
    // set if you want to run the analysis locally (kTRUE), or on grid (kFALSE)
    Bool_t local = kFALSE;
    // if you run on grid, specify test mode (kTRUE) or full grid model (kFALSE)
    Bool_t gridTest = kFALSE;


    // Use AliRoot includes to compile our task
    // since we will compile a class, tell root where to look for headers  
#if !defined (__CINT__) || defined (__CLING__)
    gInterpreter->ProcessLine(".include $ROOTSYS/include");
    gInterpreter->ProcessLine(".include $ALICE_ROOT/include");
    gInterpreter->ProcessLine(".include $ALICE_PHYSICS/include");
#else
    gROOT->ProcessLine(".include $ROOTSYS/include");
    gROOT->ProcessLine(".include $ALICE_ROOT/include");
    gROOT->ProcessLine(".include $ALICE_PHYSICS/include");
#endif
    Int_t runNumber[] = {
        //..LHC16q, 32 runs but I changed it to 28 runs by remove " 265309, 265344, 265378, 265422 runs."
        265525, 265521, 265501, 265500, 265499, 265435, 265427, 265426, 265425, 265424, 265421, 265420, 265419, 265388, 265387, 265385, 265384, 265383, 265381, 265377, 265343, 265342, 265339, 265338, 265336, 265335, 265334, 265332
    };	
    const int nn = 28;

    // create the analysis manager
    AliAnalysisManager *mgr = new AliAnalysisManager("AnalysisTaskExample");
    AliAODInputHandler *aodH = new AliAODInputHandler();
    mgr->SetInputEventHandler(aodH);

    // compile the class (locally)
    //    gROOT->LoadMacro("AliAnalysisTaskMyTask.cxx++g");
    // load the addtask macro
    //    gROOT->LoadMacro("AddMyTask.C");

    //..Physics Selection
    //	gROOT->ProcessLine(".L $ALICE_PHYSICS/OADB/macros/AddTaskPhysicsSelection.C");
    //   AliPhysicsSelectionTask *physSelTask = AddTaskPhysicsSelection(kFALSE, kTRUE); //..MC, Pileup

    AliPhysicsSelectionTask *pPhysSelTask = reinterpret_cast<AliPhysicsSelectionTask*>(gInterpreter->ExecuteMacro("$ALICE_PHYSICS/OADB/macros/AddTaskPhysicsSelection.C(kFALSE, kTRUE)"));


    //Add this here: run before your task, but after definition of manager and input handler
    AliMultSelectionTask * MStask = reinterpret_cast<AliMultSelectionTask*>(gInterpreter->ExecuteMacro("$ALICE_PHYSICS/OADB/COMMON/MULTIPLICITY/macros/AddTaskMultSelection.C(kFALSE)")); // user mode:


    // create an instance of your analysis task
    gInterpreter->LoadMacro("AliAnalysisTaskMyTask.cxx++g");
    AliAnalysisTaskMyTask *taskFlow = reinterpret_cast<AliAnalysisTaskMyTask*>(gInterpreter->ExecuteMacro("AddMyTask.C"));
    // AliAnalysisTaskMyTask* taskFlow reinterpret_cast<AliAnalysisTaskMyTask*>(gInterpreter->ExecuteMacro("AddMyTask(96, 0.8, 10, 0.2, 5.0, 70, 10, false, 1, \"LHC16q\", \"test\")));
    taskFlow->SetDebugLevel(0);

    if(!mgr->InitAnalysis()) return;
    mgr->SetDebugLevel(2);
    mgr->PrintStatus();
    mgr->SetUseProgressBar(1, 25);

    if(local) {
        // if you want to run locally, we need to define some input
        TChain* chain = new TChain("aodTree");
        // add a few files to the chain (change this so that your local files are added)
        chain->Add("/Users/Helena/alice/Data/AliAOD_2.root");
        // start the analysis locally, reading the events from the tchain
        mgr->StartAnalysis("local", chain);
    } else {
        // if we want to run on grid, we create and configure the plugin
        AliAnalysisAlien *alienHandler = new AliAnalysisAlien();
        // also specify the include (header) paths on grid
        alienHandler->AddIncludePath("-I. -I$ROOTSYS/include -I$ALICE_ROOT -I$ALICE_ROOT/include -I$ALICE_PHYSICS/include");
        // make sure your source files get copied to grid
        alienHandler->SetAdditionalLibs("AliAnalysisTaskMyTask.cxx AliAnalysisTaskMyTask.h");
        alienHandler->SetAnalysisSource("AliAnalysisTaskMyTask.cxx");
        // select the aliphysics version. all other packages
        // are LOADED AUTOMATICALLY!
        alienHandler->SetAliPhysicsVersion("vAN-20171108-1"); 
        // set the Alien API version
        alienHandler->SetAPIVersion("V1.1x");
        // select the input data
        alienHandler->SetGridDataDir("/alice/data/2016/LHC16q/");  
        alienHandler->SetDataPattern("/pass1_CENT_wSDD/AOD/*/AliAOD.root"); 
        // MC has no prefix, data has prefix 000
        alienHandler->SetRunPrefix("000");
        // runnumber
        for (Int_t i = 0; i < nn; i++)
            alienHandler->AddRunNumber(runNumber[i]);
        //alienHandler->AddRunNumber(167813);
        // number of files per subjob
        alienHandler->SetSplitMaxInputFileNumber(40);
        alienHandler->SetExecutable("myTask.sh");
        // specify how many seconds your job may take
        alienHandler->SetTTL(30000);
        alienHandler->SetJDLName("myTask.jdl");

        alienHandler->SetOutputToRunNo(kTRUE);
        alienHandler->SetKeepLogs(kTRUE);
        // merging: run with kTRUE to merge on grid
        // after re-running the jobs in SetRunMode("terminate") 
        // (see below) mode, set SetMergeViaJDL(kFALSE) 
        // to collect final results
        alienHandler->SetMaxMergeStages(1);
        alienHandler->SetMergeViaJDL(kTRUE);

        // define the output folders
        alienHandler->SetGridWorkingDir("Effieciency_Weights_pPb");
        alienHandler->SetGridOutputDir("output");

        // connect the alien plugin to the manager
        mgr->SetGridHandler(alienHandler);
        if(gridTest) {
            // speficy on how many files you want to run
            alienHandler->SetNtestFiles(1);
            // and launch the analysis
            alienHandler->SetRunMode("test");
            mgr->StartAnalysis("grid");
        } else {
            // else launch the full grid analysis
            alienHandler->SetRunMode("full");
            mgr->StartAnalysis("grid");
        }
    }
}


