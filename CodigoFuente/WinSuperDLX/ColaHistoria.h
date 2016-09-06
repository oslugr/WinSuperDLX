/* 
Este fichero es parte de WinSuperDLX, el simulador interactivo de la
    m√°quina DLX 

    Copyright (C) 2002 Mario David Barrag√°n Garc√≠a (jabade@supercable.com)
                 Antonio Jes√∫s Contreras S√°nchez   (anjeconsa@yahoo.es)
                Emilio Jos√© Garnica L√≥pez   (ejgl@hotmail.com)
                Julio Ortega Lopera         (julio@atc.ugr.es)

    Este programa es software libre: usted puede redistribuirlo y/o modificarlo 
    bajo los t√©rminos de la Licencia P√∫blica General GNU publicada 
    por la Fundaci√≥n para el Software Libre, ya sea la versi√≥n 3 
    de la Licencia, o (a su elecci√≥n) cualquier versi√≥n posterior.

    Este programa se distribuye con la esperanza de que sea √∫til, pero 
    SIN GARANT√çA ALGUNA; ni siquiera la garant√≠a impl√≠cita 
    MERCANTIL o de APTITUD PARA UN PROP√ìSITO DETERMINADO. 
    Consulte los detalles de la Licencia P√∫blica General GNU para obtener 
    una informaci√≥n m√°s detallada. 

    Deber√≠a haber recibido una copia de la Licencia P√∫blica General GNU 
    junto a este programa. 
    En caso contrario, consulte <http://www.gnu.org/licenses/>.

*/

// ColaHistoria.h

#ifndef _TCOLA_HISTORIA_

#define _TCOLA_HISTORIA_ 0
#define LONG_INST  50
#include <string.h>

typedef  struct  _TInfoInstruccion {

          int icount;              // icount de la instrucciÛn (asignada por el simulador)
          int ciclos[6];           // ciclos de captaciÛn, decodificaciÛn, ejecuciÛn y WB
          int estado;              // indicar· uno de los 4 estados.
          char instruccion[LONG_INST];
          int ROB;                 // indica el ROB utilizado por la instrucciÛn. Ser· 1 si es entero
                                   //                                                  2 si es float
                                   //                                                  0 si no est· inicializado a uno v·lido.
                                   // Es necesario para saber de que color pintar la ˙ltima etapa.
          int flush;               // indica si una instrucciÛn se ha retirado del cauce.
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
         // MÈtodos de la clase
         // --------------------------------
         // ----- constructor --------------------------------------
         TColaHistoria(int tamano_maximo);
         // -------- destructor ------------------------------------
         ~TColaHistoria(void);
         // ------ InserciÛn de un nuevo registro -------------------
         void InsetarRegistro(TInfoInstruccion registro);
         // ------ Devolver un Registro dado de una posiciÛn
         TInfoInstruccion DevolverRegistro(int posicion);
         // ----Modificar un registro de la cola -----------------
         void  ModificarRegistro(int posicion, TInfoInstruccion registro);
         // --- Tamano --------------------------------------------
         void Inicializar(void);
         int Tamano(void);

};

#endif
