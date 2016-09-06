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

// ColaHistoria.h

#ifndef _TCOLA_HISTORIA_

#define _TCOLA_HISTORIA_ 0
#define LONG_INST  50
#include <string.h>

typedef  struct  _TInfoInstruccion {

          int icount;              // icount de la instrucci�n (asignada por el simulador)
          int ciclos[6];           // ciclos de captaci�n, decodificaci�n, ejecuci�n y WB
          int estado;              // indicar� uno de los 4 estados.
          char instruccion[LONG_INST];
          int ROB;                 // indica el ROB utilizado por la instrucci�n. Ser� 1 si es entero
                                   //                                                  2 si es float
                                   //                                                  0 si no est� inicializado a uno v�lido.
                                   // Es necesario para saber de que color pintar la �ltima etapa.
          int flush;               // indica si una instrucci�n se ha retirado del cauce.
}TInfoInstruccion;


class  TColaHistoria {

         protected:

            int tamano;
            int tamanoMaximo;
            int punteroCola;
            int punteroCabeza;
            TInfoInstruccion * ColaInstrucciones;

         public:
         // --------------------------------
         // M�todos de la clase
         // --------------------------------
         // ----- constructor --------------------------------------
         TColaHistoria(int tamano_maximo);
         // -------- destructor ------------------------------------
         ~TColaHistoria(void);
         // ------ Inserci�n de un nuevo registro -------------------
         void InsetarRegistro(TInfoInstruccion registro);
         // ------ Devolver un Registro dado de una posici�n
         TInfoInstruccion DevolverRegistro(int posicion);
         // ----Modificar un registro de la cola -----------------
         void  ModificarRegistro(int posicion, TInfoInstruccion registro);
         // --- Tamano --------------------------------------------
         void Inicializar(void);
         int Tamano(void);

};

#endif
