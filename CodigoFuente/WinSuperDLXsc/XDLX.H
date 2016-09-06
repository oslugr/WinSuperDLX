/* 
Este fichero es parte de WinSuperDLX, el simulador interactivo de la
    máquina DLX 

    Copyright (C) 2002 Mario David Barragán García (jabade@supercable.com)
                 Antonio Jesús Contreras Sánchez   (anjeconsa@yahoo.es)
                Emilio José Garnica López   (ejgl@hotmail.com)
                Julio Ortega Lopera         (julio@atc.ugr.es)

    Este programa es software libre: usted puede redistribuirlo y/o modificarlo 
    bajo los términos de la Licencia Pública General GNU publicada 
    por la Fundación para el Software Libre, ya sea la versión 3 
    de la Licencia, o (a su elección) cualquier versión posterior.

    Este programa se distribuye con la esperanza de que sea útil, pero 
    SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita 
    MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
    Consulte los detalles de la Licencia Pública General GNU para obtener 
    una información más detallada. 

    Debería haber recibido una copia de la Licencia Pública General GNU 
    junto a este programa. 
    En caso contrario, consulte <http://www.gnu.org/licenses/>.

*/

/*
    Copyright (C) 1993 McGill University.
    Copyright (C) 1993 ACAPS Group.
    Copyright (C) 1993 Cecile Moura

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/


/*************************************************************************
 * Cecile Moura 
 * course  project 308-573A && Masters project
 *
 * File : xdlx.h
 *
 * Main header file for the X-version of superdlx
 * **************************************************************************/

/* include everything needed for X-superDLX, in particular, 
 * the machine structure */

#include "dlx.h"	

#define _BOOL


#define MAX_BUFFER_NB 5
#define HELP_NB 13
#define BUTTON_WIDTH 80 

DLX *init_machPtr;
DLX *machPtr;


int initDefault; 

/* global widgets  */
/*Widget
  topLevel, 
  goShell,
  errorShell,
  errorString,
  helpShell,
  helpMessage,
  memorySize,
  memoryLatency,
  nbFetched,
  nbDecoded,
  nbCommited,
  numIntUnit[MAX_INT_UNITS],
  delayIntUnit[MAX_INT_UNITS],
  numFpUnit[MAX_FP_UNITS],
  delayFpUnit[MAX_FP_UNITS],
  buffer[MAX_BUFFER_NB], 
  loadBufferDisplay,
  storeBufferDisplay,
  iWindowDisplay,
  fpWindowDisplay,
  iReorderDisplay,
  fpReorderDisplay,
  unitsDisplay,
  iQueueDisplay,
  iQueueBox,
  integerBuffer,
  floatBuffer,
  branchBufferSize;
*/
typedef struct{
  Widget helpShell;
  int helpMsgNb;
} helpData;
helpData *help;


	
#define HELPFILES   {"xmain.hlp","xcontrols.hlp", "xmachine.hlp", "go.hlp", "load.hlp", "quit.hlp", "step.hlp", "xiwindow.hlp", "xfpwindow.hlp", "xireorder.hlp", "xfpreorder.hlp", "xloadbuffer.hlp", "xstorebuffer.hlp", "xunit.hlp","xqueue.hlp"}

/* the following are indices to HELPFILES, defined above */

#define HELP_MAIN 0
#define HELP_CONTROLS 1
#define HELP_MACHINE 2
#define HELP_GO 3
#define HELP_LOAD 4
#define HELP_QUIT 5
#define HELP_STEP 6
#define HELP_IWINDOW 7
#define HELP_FPWINDOW 8
#define HELP_IREORDER 9
#define HELP_FPREORDER 10
#define HELP_LOADBUFFER 11
#define HELP_STOREBUFFER 12
#define HELP_UNITS 13
#define HELP_QUEUE 14

#define CONTROL_HEIGHT 30 /* height of control-panel control buttons */ 
#define CONTROL_WIDTH 79 /* width of control-panel control buttons */ 
#define CONTROL_LABEL_WIDTH 400 /*width of control panel label*/
#define INSPECT_HEGHT 30
#define INSPECT_WIDTH 206
#define LABEL_WIDTH  242 /* width of control popup labels */

/* XtTranslations toggleTranslations; */

#define TOGGLE_WIDTH 75 /* width of radio toggle buttons */

/* xmain.c */
void PopupCallback();
void PopdownCallback();
void ExitCallback ();
void initMainCommands(); 
void createExitPopup();
extern void xVersion();

/*xsim.c*/
void StartSimCallback();
void createControlMenu();


/* xconfig.c*/
void MachineInitCallback();
void getIntegerParameters();
void createMachineMenu();
void SimInitDefaultCallBack();
void SimInitCallBack();
void getMachineCommands();
void getMemoryParameter ();
void getInstructionParameters();
void getFpUnitParameters();
void getIntUnitParameters();
void getIntegerParameters();
void getFloatingParameters();
void getBufferSizes();
void getQueueSize();
void getBufferParameters();
void getBranchParameters();
void createMachineMenu();


/*xinspect*/

void IntegerWindowCallback();
void FloatWindowCallback();
void displayWindow();
void InstWindowCallback(); /* to be rem*/
extern void IntegerWindowCallback();
extern void FloatWindowCallback();
extern void createInstWindow();
void displayWindow();
extern void IntegerBufferCallback();
extern void FloatBufferCallback();
extern void createReorderBuffer();
void displayReorderBuffer();
void UpdateDisplayCallback(); /* to be rem*/
extern void LoadBufferCallback();
extern void StoreBufferCallback();
void displayDataBuffer();
extern void createDataBuffer();
void createInspectPopup();
void UnitsCallback();
void QueueCallback();

/* xcontrols.c*/
void LoadCallback();
void createLoadPopup();
void QuitCallback();
void createQuitPopup();
void HelpCallback();
void GoCallback();
void GoPopup();
void createGoPopup();
void FgetCallback();
void createFgetPopup();
void StepCallback();
void createStepPopup();



