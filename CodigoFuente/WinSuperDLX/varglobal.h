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
// Fichero varglobal.h

#ifndef _VAR_GLOBAL_
#define _VAR_GLOBAL_ 1

char  contenidoQueue[10000];               // de dispQueue
char  contenidoWindowInt[10000];       // de dispWindow   (para enteros)
char  contenidoWindowFloat[10000];   // para dispWidow ( para flotas)
char  contenidoRBufferInt[10000];        // para dispRBuffer (para enteros)
char  contenidoRBufferFloat[10000];    // para dispRBuffer (para floats)
char  contenidoDataLoad[5000];                  // dispData    (buffer de loads)
char  contenidoDataStore[5000];         // para dispData   (buffer de stores)
char  contenidoUnitsInt[5000];            // para dispUnits    (unidad de enteros)
char  contenidoUnitsFloat[5000];        // para dispUnits     (unidad de floats)
char *dispData, *dispWindow, *dispRBuffer;


//  Variables usadas en estadisticas
float StatsGeneral[40];
float StatsBranch[6];
float StatsBlkStores[3];

#endif
