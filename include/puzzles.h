#ifndef SAMPLE_UI_H
#define SAMPLE_UI_H

#include "gba/types.h"
#include "main.h"

//Comment out to disable intro cutscene.
#define ENABLE_INTRO
//#define ACTUAL_INTRO

// Launch the basic version of the UI
void Task_OpenSampleUi_StartHere(u8 taskId);

// Launch the sliding panel version of the UI
void Task_OpenSampleUi_SlidingPanel(u8 taskId);

// Launch the blank template version of the UI
void Task_OpenSampleUi_BlankTemplate(u8 taskId);

//For the Nessie Painting
void Task_OpenNessiePainting(u8 taskId);

//For the Greehaseet
void Task_OpenGreehaseetPuzzle(u8 taskId);

//For prologue
void Task_OpenPrologueScreen(u8 taskId);
#endif
