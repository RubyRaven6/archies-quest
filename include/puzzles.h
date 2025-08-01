#ifndef SAMPLE_UI_H
#define SAMPLE_UI_H

#include "gba/types.h"
#include "main.h"

//Comment out to disable intro cutscene.
//#define ENABLE_INTRO
//#define ACTUAL_INTRO

// Launch the basic version of the UI
void Task_OpenSampleUi_StartHere(u8 taskId);

// Launch the sliding panel version of the UI
void Task_OpenSampleUi_SlidingPanel(u8 taskId);

// Launch the blank template version of the UI
void Task_OpenSampleUi_BlankTemplate(u8 taskId);

//For the new cutscene
//TBD

//For the Nessie Painting version
void Task_OpenNessiePainting(u8 taskId);

#endif
