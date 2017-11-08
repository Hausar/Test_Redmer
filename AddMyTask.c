///////////////////////////////////////////////////////////////////
//                                                               //            
// AddMyTask                                                     //
// Author: Redmer A. Bertens, Utrecht University, 2012           //
//                                                               //
///////////////////////////////////////////////////////////////////



#if !defined (__CINT__) || defined (__CLING__)
#include "AliAnalysisManager.h"
#include "AliAnalysisTaskMyTask.h"
#include <TString.h>
#include <TList.h>
#endif


AliAnalysisTaskMyTask* AddMyTask(TString name = "MyTaskResults")
{
    // get the manager via the static access member. since it's static, you don't need
    // an instance of the class to call the function
    AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
    if (!mgr) {
        return 0x0;
    }
    // get the input event handler, again via a static method. 
    // this handler is part of the managing system and feeds events
    // to your task
    if (!mgr->GetInputEventHandler()) {
        return 0x0;
    }
    // by default, a file is open for writing. here, we get the filename
    TString fileName = AliAnalysisManager::GetCommonFileName();
    //fileName += ":MyTaskResults";      // create a subfolder in the file
    fileName += Form(":%s",name.Data());      // create a subfolder in the file
    
    //TString fileName = "FlowOutput.root : MyTask";
    // now we create an instance of your task
    AliAnalysisTaskMyTask* task = new AliAnalysisTaskMyTask(name.Data());   
    if(!task) {
        return 0x0;
    }
    
    // add your task to the manager
    mgr->AddTask(task);

    // Creating containers
    AliAnalysisDataContainer* cInput0  = mgr->GetCommonInputContainer();
    AliAnalysisDataContainer* cOutput1 = mgr->CreateContainer(Form("Flow_Refs_%s",name.Data()), TList::Class(), AliAnalysisManager::kOutputContainer, Form("%s",fileName.Data()));
    AliAnalysisDataContainer* cOutput2 = mgr->CreateContainer(Form("Flow_Weight_%s",name.Data()), TList::Class(), AliAnalysisManager::kOutputContainer, Form("%s",fileName.Data()));
    
    // your task needs input: here we connect the manager to your task
    //mgr->ConnectInput(task,0,mgr->GetCommonInputContainer());
    // same for the output
    //mgr->ConnectOutput(task,1,mgr->CreateContainer("MyOutputContainer", TList::Class(), AliAnalysisManager::kOutputContainer, fileName.Data()));

    
    // Connecting containers to task
    mgr->ConnectInput(task,0,cInput0); // your task needs input: here we connect the manager to your task
    mgr->ConnectOutput(task,1,cOutput1);
    mgr->ConnectOutput(task,2,cOutput2);
    
    // in the end, this macro returns a pointer to your task. this will be convenient later on
    // when you will run your analysis in an analysis train on grid
    return task;
}

