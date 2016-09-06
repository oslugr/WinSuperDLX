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
#include <stdlib.h>
#include "ColaHistoria.h"


TColaHistoria::TColaHistoria(int tamano_maximo) {
    int i;

        this->tamanoMaximo = tamano_maximo;  // se inicializa al tamaÒo m·ximo
        this->tamano = 0;                    // el tamaÒo en la creaciÛn es de 0 elementos
        // Se reserva memoria para la cola
        this->ColaInstrucciones = (TInfoInstruccion*) malloc (sizeof(TInfoInstruccion) * (this->tamanoMaximo));
        // se inicializa cada uno de los registros de la cola
        for(i=0; i < this->tamanoMaximo; i++) {
           this->ColaInstrucciones[i].ciclos[0] =  0;
           this->ColaInstrucciones[i].ciclos[1] =  0;
           this->ColaInstrucciones[i].ciclos[2] =  0;
           this->ColaInstrucciones[i].ciclos[3] =  0;
           this->ColaInstrucciones[i].ciclos[4] =  0;
           this->ColaInstrucciones[i].ciclos[5] =  0;
           this->ColaInstrucciones[i].estado    = -1;
           this->ColaInstrucciones[i].icount    = -1;  // no tiene ninguna asociadaa
           this->ColaInstrucciones[i].ROB       =  0;  // no tiene ninguno asociado
           this->ColaInstrucciones[i].flush     =  0;
        }
        this->punteroCola = 0;    // se inicializan los punteros a la cabeza y a la cola
        this->punteroCabeza = 0;

}


// -------- destructor ------------------------------------

TColaHistoria::~TColaHistoria(void) {

        // se libera la memoria reservada y se inicializan los dem·s campos
        // (esto ˙ltimo no serÌa necesario)

        free(this->ColaInstrucciones);
        this->tamano        = 0;
        this->tamanoMaximo  = 0;
        this->punteroCola   = 0;
        this->punteroCabeza = 0;


}

// ------ InserciÛn de un nuevo registro -------------------


 void TColaHistoria::InsetarRegistro(TInfoInstruccion registro) {

       // Se inserta en el registro indicado por "punteroCola".

       int i;

       this->ColaInstrucciones[this->punteroCola].icount = registro.icount;
       this->ColaInstrucciones[this->punteroCola].estado = registro.estado;
       this->ColaInstrucciones[this->punteroCola].ROB    = registro.ROB;
       this->ColaInstrucciones[this->punteroCola].flush  = registro.flush;
       for(i=0;i< 6; i++) {
          this->ColaInstrucciones[this->punteroCola].ciclos[i]  = registro.ciclos[i];
       }
       memcpy(this->ColaInstrucciones[this->punteroCola].instruccion,registro.instruccion,LONG_INST);
      
       // Si no se ha alcanzado el tamaÒo m·ximo, se aumenta el tamaÒo
       if (this->tamano < this->tamanoMaximo) {
           this->tamano ++;
            // Se incrementa el puntero de cola
            this->punteroCola = (this->punteroCola + 1) % this->tamanoMaximo;
       }  // si se ha alcanzado el tamaÒo m·ximo, no se aumenta el tamaÒo, sino que se
          // sobreescriben registros antiguos
       else {
           // Se incrementa el puntero de cola
           this->punteroCola = (this->punteroCola + 1) % this->tamanoMaximo;
           this->punteroCabeza = this->punteroCola;
       }

}

// ------ Devolver un Registro dado de una posiciÛn


 TInfoInstruccion TColaHistoria::DevolverRegistro(int posicion) {

   // Las posiciones se consideran desde "punteroCabeza" hasta "punteroCola"
   // p.e. la posiciÛn 1 serÌa en realidad la ("punteroCabeza" + 1) mod TamanoMaximo.

   TInfoInstruccion registro;
   int i;
   int posicionAuxiliar;

   posicionAuxiliar = (this->punteroCabeza + posicion)%this->tamanoMaximo;

   for(i=0;i<6;i++) {
      registro.ciclos[i] = this->ColaInstrucciones[posicionAuxiliar].ciclos[i];
   }
   registro.icount    = this->ColaInstrucciones[posicionAuxiliar].icount;
   registro.estado    = this->ColaInstrucciones[posicionAuxiliar].estado;
   registro.ROB       = this->ColaInstrucciones[posicionAuxiliar].ROB;
   registro.flush     = this->ColaInstrucciones[posicionAuxiliar].flush;
   memcpy(registro.instruccion,this->ColaInstrucciones[posicionAuxiliar].instruccion,LONG_INST);

   // se devuelve el registro
   return registro;
}


// ----Modificar un registro de la cola -----------------


 void  TColaHistoria::ModificarRegistro(int posicion, TInfoInstruccion registro) {

   // Las posiciones se consideran desde "punteroCabeza" hasta "punteroCola"
   // p.e. la posiciÛn 1 serÌa en realidad la ("punteroCabeza" + 1) mod TamanoMaximo.

    // Se modifica la posiciÛn con todos los valores que tiene "registro".
    int i;
    int posicionAuxiliar;

    posicionAuxiliar =(this->punteroCabeza + posicion)%this->tamanoMaximo;
    for(i=0;i<6;i++) {
      this->ColaInstrucciones[posicionAuxiliar].ciclos[i] = registro.ciclos[i];
    }
    this->ColaInstrucciones[posicionAuxiliar].icount = registro.icount;
    this->ColaInstrucciones[posicionAuxiliar].estado = registro.estado;
    this->ColaInstrucciones[posicionAuxiliar].ROB    = registro.ROB;
    this->ColaInstrucciones[posicionAuxiliar].flush  = registro.flush;
    memcpy(this->ColaInstrucciones[posicionAuxiliar].instruccion,registro.instruccion,LONG_INST);
}

// ---------------------------------------------------------

 TColaHistoria::Tamano(void) {

    return this->tamano;
}

// ----------------------------------------------------------
void TColaHistoria::Inicializar(void){
int i;

  for (i=0; i < this->tamanoMaximo ; i++) {
     this->ColaInstrucciones[i].ciclos[0] =  0;
     this->ColaInstrucciones[i].ciclos[1] =  0;
     this->ColaInstrucciones[i].ciclos[2] =  0;
     this->ColaInstrucciones[i].ciclos[3] =  0;
     this->ColaInstrucciones[i].ciclos[4] =  0;
     this->ColaInstrucciones[i].ciclos[5] =  0;
     this->ColaInstrucciones[i].icount    =  0;
     this->ColaInstrucciones[i].estado    = -1;
     this->ColaInstrucciones[i].ROB       =  0;
     this->ColaInstrucciones[i].flush     =  0;
  }
  this->tamano = 0;
  this->punteroCola = 0;    // se inicializan los punteros a la cabeza y a la cola
  this->punteroCabeza = 0;
  return;
}

