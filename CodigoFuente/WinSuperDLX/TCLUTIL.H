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
#ifndef _TCLUTIL_
#define _TCLUTIL_ 1
#include "tcl.h"
void Tcl_Return(Tcl_Interp *interp,char *string,int status);
char Tcl_Backslash(char *src, int *readPtr);
//int TclFindElement(Tcl_Interp * interp,register char * list,char **  elementPtr,char ** nextPtr,int * sizePtr
//  ***********             , int * bracePtr);
// *************** void TclCopyAndCollapse(int count, register char * src, register char * dst);
// *****************char * TclWordEnd(char * start,int  nested);
int Tcl_StringMatch(register char *string,register char * pattern);
char * Tcl_Merge(int argc,char ** argv);
char * Tcl_Concat(int argc, char **argv);
int Tcl_SplitList(Tcl_Interp * interp,char * list,int * argcPtr,char *** argvPtr);



#endif
