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


/* default resources for xdlx */

String fallback_resources[] = 
{
/* default font for all menus */
	"*font:	-adobe-courier-bold-r-*-*-12-*-*-*-*-*-*-*",

/* fonts for the System Configuration window */
	"*startCommand*font:	-adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*exitCommand*font:	-adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*helpCommand*font:	-adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*", 
	"*machineCommand*font:	-adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*mainLabel*font:	-adobe-courier-bold-r-*-*-18-*-*-*-*-*-*-*",
	"*headingLabel*font:	-adobe-courier-bold-r-*-*-18-*-*-*-*-*-*-*",

	
/* machine configuration commands*/
	"*confirmCommand*font:	-adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*cancelCommand*font:	-adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
       

/* position of the curser within the System Configuration dialog boxes */
	
	"*nbFetched*Text*insertPosition:1",
        "*nbDecoded*Text*insertPosition:1",
	"*nbCommited*Text*insertPosition:1",
	"*memorySize*font:	-adobe-courier-bold-r-*-*-12-*-*-*-*-*-*-*",
	"*memorySize*Text*insertPosition: 5",
	"*memoryLatency*Text*insertPosition:1",
	"*numIntUnit[0]*Text*insertPosition:1",
	"*numIntUnit[1]*Text*insertPosition:1",
	"*numIntUnit[2]*Text*insertPosition:1",
	"*numIntUnit[3]*Text*insertPosition:1",
	"*numIntUnit[4]*Text*insertPosition:1",

      	"*delayIntUnit[0]*Text*insertPosition:1",
      	"*delayIntUnit[1]*Text*insertPosition:1",
      	"*delayIntUnit[2]*Text*insertPosition:1",
      	"*delayIntUnit[3]*Text*insertPosition:1",
      	"*delayIntUnit[4]*Text*insertPosition:1",

	"*numFpUnit[0]*Text*insertPosition:1",
	"*numFpUnit[1]*Text*insertPosition:1",
	"*numFpUnit[2]*Text*insertPosition:1",
	"*numFpUnit[3]*Text*insertPosition:1",
	"*numFpUnit[4]*Text*insertPosition:1",
	"*numFpUnit[5]*Text*insertPosition:1",	
	"*numFpUnit[6]*Text*insertPosition:1",	

       	"*delayFpUnit[0]*Text*insertPosition:2",
       	"*delayFpUnit[1]*Text*insertPosition:2",
       	"*delayFpUnit[2]*Text*insertPosition:2",
       	"*delayFpUnit[3]*Text*insertPosition:2",
       	"*delayFpUnit[4]*Text*insertPosition:2",
       	"*delayFpUnit[5]*Text*insertPosition:2",
       	"*delayFpUnit[6]*Text*insertPosition:2",

	"*buffer[0]*Text*insertPosition:2",
	"*buffer[1]*Text*insertPosition:2",
	"*buffer[2]*Text*insertPosition:2",
	"*buffer[3]*Text*insertPosition:2",
	"*buffer[4]*Text*insertPosition:2",
	"*buffer[5]*Text*insertPosition:2",	

	"*iQueueBox*Text*insertPosition:2",
	

/* Size of System Configuration dialog text data entry boxes */

	"*nbFetched*Text*width:70",
	"*nbDecoded*Text*width:70",
	"*nbCommited*Text*width:70",
	"*memorySize*Text*width: 70",
	"*memoryLatency*Text*width:70",


       	"*numIntUnit[0]*Text*width:50",
	"*numIntUnit[1]*Text*width:50",
       	"*numIntUnit[2]*Text*width:50",
       	"*numIntUnit[3]*Text*width:50",
	"*numIntUnit[4]*Text*width:50",
	"*numIntUnit[5]*Text*width:50",

       	"*numFpUnit[0]*Text*width:50",
	"*numFpUnit[1]*Text*width:50",
       	"*numFpUnit[2]*Text*width:50",
       	"*numFpUnit[3]*Text*width:50",
	"*numFpUnit[4]*Text*width:50",
	"*numFpUnit[5]*Text*width:50",
	"*numFpUnit[6]*Text*width:50",

       	"*delayIntUnit[0]*Text*width:50",
	"*delayIntUnit[1]*Text*width:50",
       	"*delayIntUnit[2]*Text*width:50",
       	"*delayIntUnit[3]*Text*width:50",
	"*delayIntUnit[4]*Text*width:50",
	"*delayIntUnit[5]*Text*width:50",

       	"*delayFpUnit[0]*Text*width:50",
	"*delayFpUnit[1]*Text*width:50",
       	"*delayFpUnit[2]*Text*width:50",
       	"*delayFpUnit[3]*Text*width:50",
	"*delayFpUnit[4]*Text*width:50",
	"*delayFpUnit[5]*Text*width:50",
	"*delayFpUnit[6]*Text*width:50",
       
/* sizes of the buffer entry Boxes */

	"*buffer[0]*Text*width:70",
	"*buffer[1]*Text*width:70",
	"*buffer[2]*Text*width:70",
	"*buffer[3]*Text*width:70",
	"*buffer[4]*Text*width:70",
	"*buffer[5]*Text*width:70",



	"*iQueueBox*Text*width:90",
/* font for the System Configuration exit popup menu */
	"*exitBox*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*exitShell*x:	500",
	"*exitShell*y:	350",

/* Commands menu fonts */
	"*quitCommand*font: -adobe-courier-bold-r-*-*-17-*-*-*-*-*-*-*",
	"*helpCommand*font: -adobe-courier-bold-r-*-*-17-*-*-*-*-*-*-*",

/* Help Continue font */
	"*quitHelp*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",

/* error popup from the System Configuration menu */
	"*errorCommand*font:	-adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*errorString*font:	-adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*errorShell*x:	500",
	"*errorShell*y:	350",

/* quit popup fonts for Control menu */
	"*quitBox*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*quitShell*x:	500",
	"*quitShell*y:	350",

/* load popup from the Control menu */
	"*loadLabel*font: -adobe-courier-bold-r-*-*-18-*-*-*-*-*-*-*",
	"*loadExecCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*loadContCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*loadHelpCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*loadDialog*Text*width: 234",

/* asm popup from the Control menu */
	"*asmLabel*font: -adobe-courier-bold-r-*-*-18-*-*-*-*-*-*-*",
	"*asmExecCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*asmContCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*asmHelpCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*asmDialog*Text*width: 234",

/* plot popup from the Control menu */
	"*plotLabel*font: -adobe-courier-bold-r-*-*-18-*-*-*-*-*-*-*",
	"*plotExecCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*plotContCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*plotHelpCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*plotDialog*Text*width: 234",

/* stop popup from the Control menu */
	"*stopLabel*font: -adobe-courier-bold-r-*-*-18-*-*-*-*-*-*-*",
	"*stopExecCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*stopContCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*stopHelpCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*stopDialog*Text*width: 234",

/* fput popup from the Control menu */
	"*fputLabel*font: -adobe-courier-bold-r-*-*-18-*-*-*-*-*-*-*",
	"*fputExecCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*fputContCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*fputHelpCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*fputDialog*Text*width: 234",

/* put popup from the Control menu */
	"*putLabel*font: -adobe-courier-bold-r-*-*-18-*-*-*-*-*-*-*",
	"*putExecCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*putContCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*putHelpCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*putDialog*Text*width: 234",

/* trace popup from the Control menu */
	"*traceLabel*font: -adobe-courier-bold-r-*-*-18-*-*-*-*-*-*-*",
	"*traceExecCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*traceContCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*traceHelpCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",

/* dump popup from the Control menu */
	"*dumpLabel*font: -adobe-courier-bold-r-*-*-18-*-*-*-*-*-*-*",
	"*dumpExecCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*dumpContCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*dumpHelpCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",

/* go popup from the Control menu */
	"*goLabel*font: -adobe-courier-bold-r-*-*-18-*-*-*-*-*-*-*",
	"*goExecCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*goContCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*goHelpCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*goDialog*Text*width: 234",
	"*goDialog*Text*insertPosition: 5",

/* collect form from the go popup */
	"*collectLabel*font: -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",

/* fget popup from the Control menu */
	"*fgetLabel*font: -adobe-courier-bold-r-*-*-18-*-*-*-*-*-*-*",
	"*fgetExecCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*fgetContCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*fgetHelpCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*fgetDialog*Text*width: 234",
	"*fgetDialog*Text*insertPosition: 1",

/* get popup from the Control menu */
	"*getLabel*font: -adobe-courier-bold-r-*-*-18-*-*-*-*-*-*-*",
	"*getExecCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*getContCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*getHelpCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*getDialog*Text*width: 234",
	"*getDialog*Text*insertPosition: 5",

/* step popup from the Control menu */
	"*stepLabel*font: -adobe-courier-bold-r-*-*-18-*-*-*-*-*-*-*",
	"*stepExecCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*stepContCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*stepHelpCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*stepDialog*Text*width: 234",
	"*stepDialog*Text*insertPosition: 5",

/* stats popup from the Control menu */
	"*statsLabel*font: -adobe-courier-bold-r-*-*-18-*-*-*-*-*-*-*",
	"*statsResetCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*statsContCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*statsHelpCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",

/* integer operations popup */
	"*intCountContCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*intCountLabel*font: -adobe-courier-bold-r-*-*-18-*-*-*-*-*-*-*",
	"*intCountShell*x:	500",
	"*intCountShell*y:	350",

/* floating-point operations popup */
	"*fpCountContCommand*font:   -adobe-courier-bold-r-*-*-14-*-*-*-*-*-*-*",
	"*fpCountLabel*font: -adobe-courier-bold-r-*-*-18-*-*-*-*-*-*-*",
	"*fpCountShell*x:	500",
	"*fpCountShell*y:	350",
	NULL,
};


