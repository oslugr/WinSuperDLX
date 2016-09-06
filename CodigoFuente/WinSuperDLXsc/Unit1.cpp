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
//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdlib.h>
#include <string.h>
#pragma hdrstop

#include "Unit1.h"                    // FormPrincipal
#include "Unit2.h"
#include "Unit3.h"
#include "Historia.h"
#include "Configuracion.h"            // configuraciÛn de la m·quina
#include "Unit4.h"                    // MÛdulo de datos con la m·quina y el intÈrprete
#include "Unit5.h"
#include "FloatInstructionWindow.h"   // ventana de instruc. para floats
#include "IntInstructionWindow.h"     // ventana de instruc. para enteros
#include "LoadBuffer.h"               // buffer de loads
#include "StoreBuffer.h"              // buffer de stores
#include "InstructionQueue.h"         // cola de instrucciones
#include "RBInt.h"                    // buffer de reorden para enteros
#include "GoToAddress.h"              // muestra el di·logo de "go to address.."
#include "Asm.h"
#include "varglobal.h"                // variables globales accesibles desde los ficheros C y C++
#include "next.h"                     // muestra el di·logo de next .. instruction o cycle
#include "fichreg.h"                  // fichero de registros
#include "estadisticas.h"             // di·logo de estadÌsticas.
#include "branchform.h"



//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CTListBox"
#pragma resource "*.dfm"
TFormPrincipal *FormPrincipal;

bool fichCargado=false;  //
//---------------------------------------------------------------------------
__fastcall TFormPrincipal::TFormPrincipal(TComponent* Owner)
        : TForm(Owner)
{

}
//---------------------------------------------------------------------------


void __fastcall TFormPrincipal::FormCreate(TObject *Sender)
{

  // ˙nicamente asigna colores a las instrucciones en las diferentes
  // estructuras.
  // TambiÈn se desabilitan algunos botones, ya que al principio
  // no se pueden utilizar
  // Se introducen los colores, 10 en principio

  this->numColores  = 10;
  this->colores[0]  = TColor(11726525);
  this->colores[1]  = TColor(8296191);
  this->colores[2]  = TColor(16768112);
  this->colores[3]  = TColor(10779381);
  this->colores[4]  = TColor(10790133);
  this->colores[5]  = TColor(10603446);
  this->colores[6]  = TColor(9865408);
  this->colores[7]  = TColor(16304008);
  this->colores[8]  = TColor(12434877);
  this->colores[9]  = TColor(11985151);

  Statistics1->Enabled = false;
  Statistics2->Enabled = false;
  Simulation1->Enabled = false;
  BitBtnGo->Enabled = false;
  Button2->Enabled = false;
  BtnNextSingleInstruction->Enabled = false;
//  BtnStepToAddress->Enabled = false;
//aÒadido por antonio
  View1->Enabled=false;
  Configuration1->Enabled=false;
  // Para la historia de ciclos, esto habr· que liberarlo en el formDestroy o en el exit

   this->cola = new TColaHistoria(50);

   // Colores del gr·fico de ciclos

   this->coloresHistoria[0] =  TColor(65535) ;   // color de captaciÛn
   this->coloresHistoria[1] =  TColor(4364016);  // color de decodificaciÛn
   this->coloresHistoria[2] =  TColor(255) ;     // color de ejecuciÛn
   this->coloresHistoria[3] =  TColor(65280) ;   // color de WB
   this->coloresHistoria[4] =  TColor(65280);    // color de WB con espera
   this->coloresHistoria[5] =  TColor(16753152); // color de retirada para enteros
   this->coloresHistoria[6] =  TColor(16750079); // color de retirada para floats

}
//---------------------------------------------------------------------------

void __fastcall TFormPrincipal::Exit1Click(TObject *Sender)
{
// Ejecuta el cÛdigo necesario antes de salir.

AnsiString orden;
char * cmd, * p;
FILE *f;


  if ((f=fopen("conf.tmp","r"))!=NULL){ // Se cierra el fichero de configuraciÛn
    fclose(f);
    remove ("conf.tmp");   // Se elimina el fichero de configuraciÛn.
  }
    // Se manda la orden "quit" al intÈrprete
    Tcl_GetVar(DMGen->interp, "prompt", 1);
    orden = "quit";
    cmd = orden.c_str();
    p = cmd + strlen(cmd);
    (void) Tcl_Eval(DMGen->interp, cmd , 0, &p);
    if (*DMGen->interp->result != 0) {
         ShowMessage((AnsiString)(DMGen->interp->result));
    }

  delete (this->cola);        // se elimina la estructura que contiene la
                              // informaciÛn de las instrucciones
  // Se cierra la aplicaciÛn.
  Application->Terminate(); // Cierra la aplicaciÛn
}
//---------------------------------------------------------------------------

void __fastcall TFormPrincipal::formato(int* pos,int n,AnsiString src)
{
// ***************************************************************************
// Este mÈtodo recibe una cadena como par·metro de entrada y un vector de posiciones (enteros), de
// tamaÒo "n".
// El mÈtodo, pone en this->cadenaDestino la cadena "src" convenientemente formateada, poniendo
// cada elemento "i" de la cadena, en la posiciÛn indicada pos "pos[i]".
// ***************************************************************************

int i,k;            // se utilizan en el bucle for y while respectivamente

AnsiString cabeza;
AnsiString cola;
AnsiString result;  // cadena resultado del proceso.
int pos_blanco;


// result se inicializa como una cadena con 100 espacios
for (i=0;i<5 ;i++ ){   // 20 x 5 = 100
	result=result+"                    ";
}
cola=src;               // cola pasa a ser la cadena de entrada, sin los espacios iniciales
cola = cola.TrimLeft(); // a la izquierda


k=0;
while (k<n){
	pos_blanco=cola.AnsiPos(" "); //caracter del primer blanco
	cabeza = cola.SubString(1,pos_blanco-1);	//cogo el primer valor (desde 1 hasta el blanco - 1)
	result.Insert(cabeza,pos[k]);		//lo meto en el resultado en la posiciÛn q me dice
        if (k != (n-1))
           result.Insert("              ",pos[k+1]-1);
	cola = cola.SubString(pos_blanco,cola.Length()-pos_blanco+1);
	cola = cola.TrimLeft();
	k++;
}
cadenaDestino = result;

}

// --------------------------------------------------------------------------

void __fastcall TFormPrincipal::CTListRBFloatDblClick(TObject *Sender)
{
FormFloatROB->Show();
this->VisualizarCambios();
}
//---------------------------------------------------------------------------

void __fastcall TFormPrincipal::insertaLB(CTListBox * lb, char * cad,int * vector, int numElementos) {
// ***************************************************************************
// Esta funciÛn rellena el listBox que se le pasa "lb"  a partir de la cadena
// "cad". Para ello, separa el contenido de la cadena en los
// distintos registros que se introducir·n en la ventana.

// El formato de la cadena hace que cada registro se separe por dos barras verticales,
// excepto el primero que sÛlo tendr· una barra.
// Par·metros :
//               - Vector -> vector de posiciones para formatear la cadena mediante el mÈtodo "formato"
//               - numElementos-> n˙mero de elementos que tiene cada registro.
// ***************************************************************************

int l ;    // almacena la longitud de la cadena "cad"
int i,j;
char aux[150];
AnsiString  aux2;

l=strlen(cad);       // l pasa a ser la longitud de cad
lb->Items->Clear();  // se lÌmpia el listBox
j=0;                 // j (que recorre aux) pasa a ser cero

for (i=0;i<l;i++ ){             // se recorre la cadena de entrada
	if (cad[i]!='\n'){      // si el elemento no es un "\n"
                if (cad[i] != '|' ) {     // Se copian en aux (excepto si es una barra)
		 aux[j]=cad[i];
		 j++;
                }

	}else{                  // si el elemento es un "\n", se ha completado un registro, por lo que se
             aux[j]='\0';       // formatea
             aux2 = (AnsiString)aux;
             this->formato(vector,numElementos,aux2);
             lb->Items->Add(this->cadenaDestino);  // y se mete en el listBox
             j=0;                   // se inicializa j para comenzar con otra cadena
	}
}

return;

}

// --------------------------------------------------------------------------

void __fastcall TFormPrincipal::About1Click(TObject *Sender)
{
// Visualiza el di·logo de "Acerca de ..."

    FormAbout->ShowModal();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void __fastcall TFormPrincipal::Machineconfiguration1Click(TObject *Sender)
{

  FILE * f;

  if ((f=fopen("conf.tmp","r"))!= NULL){
    FormConfiguracion->LeeFichero("conf.tmp");
    FormConfiguracion->VisualizaDatos();
    fclose(f);
  }
  else {
    FormConfiguracion->CapturaDefecto();
    FormConfiguracion->VisualizaDatos();
  }

    FormConfiguracion->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TFormPrincipal::Load1Click(TObject *Sender)
{
AnsiString orden;
//char  p[100];
//char  cmd[1000];
char * cmd, * p;

if (this->OpenDlgAbrirFichero->Execute()) {
    // Se pasa la orden al intÈrprete
    FormColaHistoria->Close();
    //aÒadido por antonio lo del reset
    if (!this->OpenDlgAbrirFichero->FileName.IsEmpty()){
        this->Reset1Click(this);
        if (this->BranchPrediction1->Checked==true){
                //ShowMessage((int)FormPred->CSEditCounter->Value);
                DMGen->machPtr->branchSchema[1]=(int)FormPred->CSEditCounter->Value;
        }
    }
    if (this->ClockCycleDiagram1->Enabled==false)
        this->ClockCycleDiagram1->Enabled=true;

    //ShowMessage("Se ha elegido un fichero");
    Tcl_GetVar(DMGen->interp, "prompt", 1);
    orden = "load " + this->OpenDlgAbrirFichero->FileName.LowerCase(); // nombre del fichero en min˙sculas
    orden = orden.LowerCase();
    cmd = orden.c_str();
    p = cmd + strlen(cmd);
    (void) Tcl_Eval(DMGen->interp, cmd , 0, &p);
    if (*DMGen->interp->result != 0) {
         ShowMessage((AnsiString)(DMGen->interp->result));
    }

//    ShowMessage("File loaded");
//aÒadido por antonio
    this->Caption="";
    this->Caption="WinSuperDLX - "+ this->OpenDlgAbrirFichero->FileName.LowerCase();
    FormCodigo->MemoCodigo->Lines->Clear();
    FormCodigo->MemoCodigo->Lines->LoadFromFile(this->OpenDlgAbrirFichero->FileName);


  Simulation1->Enabled = true;
  BitBtnGo->Enabled = true;
  Button2->Enabled = true;
  BtnNextSingleInstruction->Enabled = true;
//  BtnStepToAddress->Enabled = true;
//aÒadido por antonio
  View1->Enabled=true;
  Configuration1->Enabled=true;
  if (DMGen->machPtr->state == END_OF_SIMULATION) {
    DMGen->machPtr->state = END_OF_SIMULATION;
  }


}

}
//---------------------------------------------------------------------------
AnsiString __fastcall TFormPrincipal::formatearCadena(char * cadena, int * vector, int numElementos) {

// ESTE MÀTODO SE PUEDE ELIMINAR, HACE LO MISMO QUE "FORMATO" Y NO SE LLAMA NUNCA

int i,j,palabra, contador;
AnsiString linea;
char cadena_aux[100];
int numEspacios;


linea = "";
contador = 0;
for(i=0;i < numElementos; i++) {
    palabra = 0;
    while(!palabra) {
      if (cadena[contador] == ' ')
         contador++;
      else
         palabra = 1;
    }
    j=0;
    while((cadena[contador] != ' ') && (cadena[contador] != '\0')){
        cadena_aux[j] = cadena[contador];
        contador++;
        j++;
    }
    // Se insertan los espacios necesarios
    if (i != 6) {
      numEspacios = vector[i+1] - vector[i];
    } else numEspacios = 0;
    //numEspacios = numEspacios - j;
    for(;j<numEspacios;j++) {
       cadena_aux[j]= ' ';
    }
    cadena_aux[j] = '\0';
    // Se copia la cadena a la cadena real
    linea = linea + (AnsiString)cadena_aux;

}

 return linea;

}

// ---------------------------------------------------------------------------

void __fastcall TFormPrincipal::Sourcecode1Click(TObject *Sender)
{

   FormCodigo->Visible = true;

}
//---------------------------------------------------------------------------
void __fastcall TFormPrincipal::VisualizarCambios(void) {

// ***************************************************************************
// Este mÈtodo actualiza todas las estructuras que podemos ver en la ventana principal del
// simulador y asigna los colores a las distintas instrucciones de las unidades.
// ***************************************************************************

 AnsiString orden;   // para hacer el "print" en el intÈrprete.
 char * p , *cmd;

 int i;        // variables para el fichero de registros
 float aux;

 // variables para formatear el contenido de las distintas unidades
 int  vector_cola_instrucciones[]    = {0, 4,  9, 21, 27, 30, 33, 36};
 int  vector_ventana_instrucciones[] = {0, 3,  9, 20, 23, 34, 36, 40, 50, 52,56};
 int  vector_buffer_reorden[]        = {0, 4, 10, 15, 18, 28, 36, 39, 44};
 int  vector_buffer_load[]           = {0, 4, 10, 16, 23, 27};
 int  vector_buffer_store[]          = {0, 3, 8, 14, 20, 29, 34, 37, 40};

    // DespuÈs de darle a Next, hay que hacer un print

    Tcl_GetVar(DMGen->interp, "prompt", 1);
    orden = "print" ;
    cmd = orden.c_str();
    p = cmd + strlen(cmd);
    (void) Tcl_Eval(DMGen->interp, cmd , 0, &p);
    if (*DMGen->interp->result != 0) {
         ShowMessage((AnsiString)(DMGen->interp->result));
    }

    // Se copia la cola de instrucciones a la lista de instrucciones
    this->insertaLB(this->CTListColaIns,contenidoQueue,vector_cola_instrucciones,8);


    // Se copian las ventanas de instrucciones a las correspondientes
    // listas de instrucciones
    this->insertaLB(this->CTListVentanaInt,contenidoWindowInt,vector_ventana_instrucciones,11);
    this->insertaLB(this->CTListVentanaFloat,contenidoWindowFloat,vector_ventana_instrucciones,11);
    // Se copian los buffers de ROB
    this->insertaLB(this->CTListRBInt,contenidoRBufferInt,vector_buffer_reorden,9);
    this->insertaLB(this->CTListRBFloat,contenidoRBufferFloat,vector_buffer_reorden,9);


    // Se copian el buffer de load / store

    this->insertaLB(this->CTListLoad,contenidoDataLoad,vector_buffer_load,6);
    this->insertaLB(this->CTListStore,contenidoDataStore,vector_buffer_store,9);


    // Se le aÒaden los colores

       this->Colorear(1,this->CTListColaIns); // se le pasa el Ìndice con el n˙mero de instrucciÛn en el listbox
       this->Colorear(2,this->CTListRBInt);
       this->Colorear(2,this->CTListRBFloat);
       this->ColorearVentanaInstrucciones(this->CTListVentanaInt,this->CTListRBInt);
       this->ColorearVentanaInstrucciones(this->CTListVentanaFloat,this->CTListRBFloat);
       this->Colorear(2,this->CTListLoad);
       this->Colorear(2,this->CTListStore);



    // Se actualizan todas las ventanas

    /* ---------------------------------  VENTANAS -------------------------- */

    // Buffer de reorden de flotantes
    if (FormFloatROB->Visible == true) {
     FormFloatROB->CTListRBFloat->Items->Clear();
     FormFloatROB->CTListRBFloat->Items->AddStrings(this->CTListRBFloat->Items);
     this->Colorear(2,FormFloatROB->CTListRBFloat);
    }
    // Ventana de instrucciones de Floats
    if (FormFloatInstructionWindow->Visible == true) {
     FormFloatInstructionWindow->CTListIWFloat->Items->Clear();
     FormFloatInstructionWindow->CTListIWFloat->Items->AddStrings(this->CTListVentanaFloat->Items);
     this->ColorearVentanaInstrucciones(FormFloatInstructionWindow->CTListIWFloat,this->CTListRBFloat);
    }
    // Buffer de reorden de Enteros
    if (FormRBInt->Visible == true) {
     FormRBInt->CTListRBInt->Items->Clear();
     FormRBInt->CTListRBInt->Items->AddStrings(this->CTListRBInt->Items);
     this->Colorear(2,FormRBInt->CTListRBInt);
    }
    // Ventana de instrucciones de Ints
    if (FormIntInstructionWindow->Visible == true) {
     FormIntInstructionWindow->CTListIWInt->Items->Clear();
     FormIntInstructionWindow->CTListIWInt->Items->AddStrings(this->CTListVentanaInt->Items);
     this->ColorearVentanaInstrucciones(FormIntInstructionWindow->CTListIWInt,this->CTListRBInt);
    }
    // Buffer de loads y Buffer de Stores

    if (FormLoadBuffer->Visible == true) {
       FormLoadBuffer->CTListLoad->Items->Clear();
       FormLoadBuffer->CTListLoad->Items->AddStrings(this->CTListLoad->Items);
       this->Colorear(2,FormLoadBuffer->CTListLoad);
    }


    if (FormStoreBuffer->Visible == true) {
      FormStoreBuffer->CTListStore->Items->Clear();
      FormStoreBuffer->CTListStore->Items->AddStrings(this->CTListStore->Items);
      this->Colorear(2,FormStoreBuffer->CTListStore);
    }

    if (FormInstructionQueue->Visible == true) {
      // Cola de Instrucciones
      FormInstructionQueue->CTListColaIns->Items->Clear();
      FormInstructionQueue->CTListColaIns->Items->AddStrings(this->CTListColaIns->Items);
      this->Colorear(1,FormInstructionQueue->CTListColaIns);
    }
    // Ventana de registros

    if (FormFichReg->Visible == true) {         // Si est· visible
        FormFichReg->CTListBoxInteger->Clear();    // Se borra por completo
        FormFichReg->CTListBoxFloat->Clear();

        for (i=0;i<NUM_INT_REGS;i++){           // Se carga con los valores de todos los registros
           FormFichReg->CTListBoxInteger->Items->Add("I"+(AnsiString)i+": "+DMGen->machPtr->regs[i]);
        }
        for (i=NUM_INT_REGS;i<NUM_INT_REGS+NUM_FP_REGS;i++){
            memcpy(&aux,&(DMGen->machPtr->regs[i]),4);
            FormFichReg->CTListBoxFloat->Items->Add("F"+(AnsiString)(i-NUM_INT_REGS)+": "+ aux);
        }
    }
}


// --------------------------------------------------------------------------
void __fastcall TFormPrincipal::Next1Click(TObject *Sender)
{
// ***************************************************************************
// Este mÈtodo realiza todo lo necesario para hacer un "next" en el simulador
// pas·ndole la orden al intÈrprete y visulizando los cambios m·s tarde. TambiÈn
// se activan las estadÌsticas.
// ***************************************************************************

 AnsiString orden;
 char * p , *cmd;
 TRect  rectangulo;
  // Si se ha llegado al final de la simulaciÛn, se termina
    if (DMGen->machPtr->state == END_OF_SIMULATION)
       return;

    Tcl_GetVar(DMGen->interp, "prompt", 1);
    orden = "next" ;
    cmd = orden.c_str();
    p = cmd + strlen(cmd);
    (void) Tcl_Eval(DMGen->interp, cmd , 0, &p);
    if (*DMGen->interp->result != 0) {
       StBar->SimpleText = ((AnsiString)"Clock Cycle : ") + DMGen->machPtr->cycleCount + "   " + ((AnsiString)(DMGen->interp->result));
    }
    this->VisualizarCambios();

    // Escalera de ciclos
    this->RellenarColaHistoria();
    rectangulo = FormColaHistoria->GridHistoria->ClientRect;
    FormColaHistoria->GridHistoria->Canvas->Brush->Color = clWhite;
    FormColaHistoria->GridHistoria->Canvas->FillRect(rectangulo);
      if (this->ClockCycleDiagram1->Checked == true)
    FormColaHistoria->FormPaint(this);
    //FormColaHistoria->Repaint();
    // EstadÌsticas
    Statistics2->Enabled = true;
    Statistics1->Enabled = true;

   if (frmEstadisticas->Visible){
     frmEstadisticas->LimpiaListBox();
     this->EstadisticasGenerales();
     this->EstadisticasRenaming();
     this->EstadisticasInstruction();
     this->EstadisticasOccupancy();
   }

}
//---------------------------------------------------------------------------
void __fastcall TFormPrincipal::CTListVentanaFloatDblClick(TObject *Sender)
{
// ****************************************************************************
// Se pone el "VisualizarCambios" ya que si una ventana no est· visible, la informaciÛn que
// contiene no est· actualizada, y es necesario llamar a este mÈtodo para que se actualice.
// ****************************************************************************
   FormFloatInstructionWindow->Visible = true;
   this->VisualizarCambios();
}
//---------------------------------------------------------------------------

void __fastcall TFormPrincipal::CTListVentanaIntDblClick(TObject *Sender)
{
   FormIntInstructionWindow->Visible = true;
   this->VisualizarCambios();
}
//---------------------------------------------------------------------------

void __fastcall TFormPrincipal::CTListRBIntDblClick(TObject *Sender)
{
      FormRBInt->Visible = true;
      this->VisualizarCambios();
}
//---------------------------------------------------------------------------

void __fastcall TFormPrincipal::CTListLoadDblClick(TObject *Sender)
{
    FormLoadBuffer->Visible = true;
    this->VisualizarCambios();
}
//---------------------------------------------------------------------------

void __fastcall TFormPrincipal::CTListStoreDblClick(TObject *Sender)
{
    FormStoreBuffer->Visible = true;
    this->VisualizarCambios();
}
//---------------------------------------------------------------------------

void __fastcall TFormPrincipal::CTListColaInsDblClick(TObject *Sender)
{
     FormInstructionQueue->Visible = true;
     this->VisualizarCambios();
}
//---------------------------------------------------------------------------

AnsiString __fastcall TFormPrincipal::ExtraerElemento(AnsiString src,int i)

// **************************************************************************
// Este mÈtodo sirve para extraer el i-Èsimo elemento de la cadena "src" que
// representa un registro.
// **************************************************************************

{

int k;

AnsiString cabeza;
AnsiString cola;
int pos_blanco;

k=0;
cola=src;
cola = cola.TrimLeft();
// Va eliminando elementos de "cola" hasta llegar al i-Èsimo elemento, que copia en la cadena
// "cabeza"

while (k<=i){
	pos_blanco=cola.AnsiPos(" "); //caracter del primer blanco

	if(k==i) {
            cabeza = cola.SubString(1,pos_blanco-1);
        }
	cola = cola.SubString(pos_blanco,cola.Length()-pos_blanco+1);
	cola = cola.TrimLeft();
	k++;
        if (cola.Length() == 0)   // Introducido por si la cadena se queda vacÌa, por alg˙n error de
           return cabeza;         // programaciÛn ( i sea mayor que el n˙mero de elementos de la cadena)
}

return cabeza;
}

// ---------------------------------------------------------------------------

void __fastcall TFormPrincipal::Colorear(int n, CTListBox * lista) {

// **************************************************************************
// MÈtodo que colorea cada lÌnea de un listBox, atendiendo al n˙mero de instrucciÛn de
// cada lÌnea. Ese n˙mero de instrucciÛn, se encuentra en la posiciÛn indicada por "n",
// por lo que ˙nicamente hay que recuperar el n-Èsimo elemento de cada item del listBox
// y asignarle el color indicado de la lista de colores.
// **************************************************************************

    int i;                   // para recorrer todos los items del listBox
    AnsiString cadena;       // almacena un item del listBox
    AnsiString stringValor;  // n-esimo elemento
    int intValor;            // valor entero del n-Èsimo elemento

    for(i=0;i < lista->Items->Count; i++) {
       try{
        cadena = lista->Items->Strings[i];
        stringValor = this->ExtraerElemento(cadena,n);
        intValor = stringValor.ToInt();
        intValor = intValor % this->numColores; // (estar· entre 0 y numColores - 1)
        lista->anadir_color(i,this->colores[intValor]);
       }catch(...) {
          ShowMessage("ERROR: intValor no es un entero v·lido (mÈtodo Colorear)");
          return;
       }
   }
   lista->Repaint(); // Se fuerza a que se repinte la lista.
}

// ----------------------------------------------------------------------------

void __fastcall TFormPrincipal::ColorearVentanaInstrucciones(CTListBox * ventana, CTListBox * rob){

// ****************************************************************************
// Colorear una ventana de instrucciones. Como la ventana no tiene el n˙mero de instrucciÛn, es
// necesario utilizar tambiÈn, su buffer de reorden asociado.
//
// para colorear la ventana hay que :

      // para cada lÌnea del rob
      //     buscar en la ventana una lÌnea con la misma entrada del rob
      //     colorear la lÌnea con el n˙mero de instrucciÛn correspondiente en el rob
// ****************************************************************************

      int i,j;
      AnsiString icount;               // n˙mero de instrucciÛn (en AnsiString)
      int intIcount;                   // n˙mero de instrucciÛn (en entero)
      AnsiString robEntry,robEntry2;   // entradas en el ROB (en AnsiString)
      int intRobEntry;                 // entradas en el ROB (en entero)
      int intRobEntry2;
      bool encontrado;                 // indica si la instrucciÛn que est· en el ROB se ha encontrado
                                       // en su ventana de instrucciones asociada
      bool salir;


      for(i=0;i< rob->Items->Count; i++) {   // Se recorre todo el ROB
        try {
          icount = this->ExtraerElemento(rob->Items->Strings[i],2); // se calcula el icount de la instrucciÛn
          intIcount = icount.ToInt();
          robEntry = this->ExtraerElemento(rob->Items->Strings[i],0); // y su entrada en el ROB
          intRobEntry = robEntry.ToInt();

          // Ahora Buscaremos esta instrucciÛn en la ventana de instrucciones

          encontrado = false;
          salir = false;
          j=0;
          while((!encontrado)&&(!salir)) {

              if (j== ventana->Items->Count ) {  // Si se ha llegado al final de la ventana, se sale del bucle
                 salir = true;
              }
              else {                         // en otro caso, se comprueba la entrada RobEntry de la instruccciÛn
                 robEntry2 = this->ExtraerElemento(ventana->Items->Strings[j],3);
                 intRobEntry2 = robEntry2.ToInt();
                 if (intRobEntry2 == intRobEntry) { // si concuerda con la del ROB, lo hemos encontrado
                   encontrado = true;
                 }
                 else j++;             // si no concuerda, pasamos al siguiente elemento de la ventana
              }

          }

          if (encontrado) {
             intIcount = intIcount % this->numColores ;
             ventana->anadir_color(j,this->colores[intIcount]);
          }
        }catch(...) {
           ShowMessage("ERROR: conversiÛn de string a  entero no v·lida");
           return;
        }
      } // for que recorre el ROB
      return;
}

// ----------------------------------------------------------

void __fastcall TFormPrincipal::Reset1Click(TObject *Sender)
{
 AnsiString orden;
 char * p , *cmd;



    Tcl_GetVar(DMGen->interp, "prompt", 1);
    orden = "reset" ;
    cmd = orden.c_str();
    p = cmd + strlen(cmd);
    (void) Tcl_Eval(DMGen->interp, cmd , 0, &p);
    if (*DMGen->interp->result != 0) {
       StBar->SimpleText = ((AnsiString)"Clock Cycle : ") + DMGen->machPtr->cycleCount + "   " + ((AnsiString)(DMGen->interp->result));
    }
    this->VisualizarCambios();

  Statistics1->Enabled = false;
  Statistics2->Enabled = false;
  Simulation1->Enabled = false;
  BitBtnGo->Enabled = false;
  Button2->Enabled = false;
  BtnNextSingleInstruction->Enabled = false;
//  BtnStepToAddress->Enabled = false;

  // Se iniciliza la cola de historia

  this->cola->Inicializar();
  FormColaHistoria->cicloInicial = 0;

  this->Caption="WinSuperDLX";

}
//---------------------------------------------------------------------------
void __fastcall TFormPrincipal::BtnNextSingleInstructionClick(TObject *Sender)
{

// AnsiString orden;
// char * p , *cmd;
// bool salir;
// int ninstrucciones;
// int inicial;
  /*

    Tcl_GetVar(DMGen->interp, "prompt", 1);
    orden = "next 1i" ;
    cmd = orden.c_str();
    p = cmd + strlen(cmd);
    (void) Tcl_Eval(DMGen->interp, cmd , 0, &p);
    if (*DMGen->interp->result != 0) {
       StBar->SimpleText = ((AnsiString)"Clock Cycle : ") + DMGen->machPtr->cycleCount + "   " + ((AnsiString)(DMGen->interp->result));
    }
    this->VisualizarCambios();

  Statistics2->Enabled = true;
  Statistics1->Enabled = true;

   if (frmEstadisticas->Visible){
     frmEstadisticas->LimpiaListBox();
     this->EstadisticasGenerales();
     this->EstadisticasRenaming();
     this->EstadisticasInstruction();
     this->EstadisticasOccupancy();
   }

    */
    //aÒadido:

    // Si  se ha llegado al final de la simulaciÛn, se termina
    if (DMGen->machPtr->state == END_OF_SIMULATION)
        return;
    FormNext->RGOptions->ItemIndex=0; //opciÛn correcta
    FormNext->CSEditPaso->Value=1;
    FormNext->BtnOkClick(this);
/*
   // orden = "next 1i" ;
   // cmd = orden.c_str();

    inicial = DMGen->machPtr->insCount;
    salir = false; ninstrucciones = 0;
    while(!salir) {
        if (DMGen->machPtr->state == END_OF_SIMULATION)
          salir = true;
        else if (ninstrucciones >= 1)
          salir = true;
        else {
           this->Next1Click(this);
           ninstrucciones = DMGen->machPtr->insCount - inicial;
          // Si no se ha llegado al final, se sigue la simulaciÛn
          //Tcl_GetVar(DMGen->interp, "prompt", 1);
          //p = cmd + strlen(cmd);
          //(void) Tcl_Eval(DMGen->interp, cmd , 0, &p);
          //FormPrincipal->StBar->SimpleText = ((AnsiString)"Clock Cycle : ") + DMGen->machPtr->cycleCount + "   " + ((AnsiString)(DMGen->interp->result));
          //FormPrincipal->VisualizarCambios();
          //// Escalera de ciclos
          //FormPrincipal->RellenarColaHistoria();
          //ninstrucciones = DMGen->machPtr->insCount - inicial;
          //this->PBarProgreso->Position = (ninstrucciones*100)/this->CSEditPaso->Value;

        }


    }*/


}
//---------------------------------------------------------------------------




void __fastcall TFormPrincipal::Next2Click(TObject *Sender)
{
  FormNext->Visible = true;
}
//---------------------------------------------------------------------------

void __fastcall TFormPrincipal::RegisterFile1Click(TObject *Sender)
{
   FormFichReg->Visible = true;
}
//---------------------------------------------------------------------------

void TFormPrincipal::EstadisticasGenerales(void){

  float WritesAll;

//  Instrucciones
  frmEstadisticas->EditCycles->Text = DMGen->machPtr->cycleCount;
  frmEstadisticas->EditFetched->Text = DMGen->machPtr->insCount;
  frmEstadisticas->EditDecoded->Text = DMGen->machPtr->counts->decoded;
  if (DMGen->machPtr->insCount!=0)
    frmEstadisticas->EditFetchedPer->Text = (float)(100*DMGen->machPtr->counts->decoded)/DMGen->machPtr->insCount;

// Instruction Issued
  frmEstadisticas->EditIssued->Text = DMGen->machPtr->counts->issued[ALL];
  if (DMGen->machPtr->insCount!=0)
    frmEstadisticas->EditIssuedPer->Text = (float)(100*DMGen->machPtr->counts->issued[ALL])/DMGen->machPtr->insCount;
  frmEstadisticas->EditIssuedInt->Text = DMGen->machPtr->counts->issued[INTEGER];
  if (DMGen->machPtr->insCount!=0)
    frmEstadisticas->EditIssuedPerInt->Text = (float)(100*DMGen->machPtr->counts->issued[INTEGER])/DMGen->machPtr->insCount;
  frmEstadisticas->EditIssuedFloat->Text = DMGen->machPtr->counts->issued[FLOAT];
  if (DMGen->machPtr->insCount!=0)
    frmEstadisticas->EditIssuedPerFloat->Text = (float)(100*DMGen->machPtr->counts->issued[FLOAT])/DMGen->machPtr->insCount;


//  Instruction Commited
  frmEstadisticas->EditCommited->Text = DMGen->machPtr->counts->commited[ALL];
  if (DMGen->machPtr->insCount!=0)
    frmEstadisticas->EditCommitedPer->Text = (float)(100*DMGen->machPtr->counts->commited[ALL])/DMGen->machPtr->insCount;
  frmEstadisticas->EditCommitedInt->Text = DMGen->machPtr->counts->commited[INTEGER];
  if (DMGen->machPtr->insCount!=0)
    frmEstadisticas->EditCommitedPerInt->Text = (float)(100*DMGen->machPtr->counts->commited[INTEGER])/DMGen->machPtr->insCount;
  frmEstadisticas->EditCommitedFloat->Text = DMGen->machPtr->counts->commited[FLOAT];
  if (DMGen->machPtr->insCount!=0)
    frmEstadisticas->EditCommitedPerFloat->Text = (float)(100*DMGen->machPtr->counts->commited[FLOAT])/DMGen->machPtr->insCount;
  WritesAll = DMGen->machPtr->counts->writes[INTEGER]+DMGen->machPtr->counts->writes[FLOAT];
  frmEstadisticas->EditCommitedWrite->Text = WritesAll;
  if (DMGen->machPtr->counts->commited[ALL]!=0)
    frmEstadisticas->EditCommitedPerWrites->Text = (float)(100*WritesAll)/DMGen->machPtr->counts->commited[ALL];
  frmEstadisticas->EditCommitedUseless->Text = (float)DMGen->machPtr->counts->uselessWrites[ALL];
  if (WritesAll!=0)
    frmEstadisticas->EditCommitedPerUseless->Text = (float)(100*DMGen->machPtr->counts->uselessWrites[ALL])/WritesAll;


// Per Cycle Rates

  frmEstadisticas->EditRatesFetch->Text = (float)DMGen->machPtr->insCount / DMGen->machPtr->cycleCount;
  frmEstadisticas->EditRatesDecode->Text = (float)DMGen->machPtr->counts->decoded / DMGen->machPtr->cycleCount;
  frmEstadisticas->EditRatesIssue->Text = (float)DMGen->machPtr->counts->issued[ALL] / DMGen->machPtr->cycleCount;
  frmEstadisticas->EditRatesCommit->Text = (float)DMGen->machPtr->counts->commited[ALL] / DMGen->machPtr->cycleCount;


// Loads Blockeds by Stores

   frmEstadisticas->EditBlkStore->Text = DMGen->machPtr->counts->loadBlck;
   if (DMGen->machPtr->counts->numLoads!=0)
     frmEstadisticas->EditBlkPer->Text = (float)(100*DMGen->machPtr->counts->loadBlck) / DMGen->machPtr->counts->numLoads;


// Saltos

   frmEstadisticas->EditBranches->Text = DMGen->machPtr->counts->branchYes + DMGen->machPtr->counts->branchNo;
   frmEstadisticas->EditBranchTaken->Text = DMGen->machPtr->counts->branchYes;
   if ((DMGen->machPtr->counts->branchYes + DMGen->machPtr->counts->branchNo)!=0)
     frmEstadisticas->EditBranchTakenPer->Text = (float)(100*DMGen->machPtr->counts->branchYes)/(DMGen->machPtr->counts->branchYes + DMGen->machPtr->counts->branchNo);
   frmEstadisticas->EditBranchUntaken->Text = DMGen->machPtr->counts->branchNo;
   if ((DMGen->machPtr->counts->branchYes + DMGen->machPtr->counts->branchNo)!=0)
     frmEstadisticas->EditBranchUntakenPer->Text = (float)(100*DMGen->machPtr->counts->branchNo)/(DMGen->machPtr->counts->branchYes + DMGen->machPtr->counts->branchNo);

   /*  FALTA ALGO DE LOS SALTOS MIRAR LUEGO */


// FETCH

  frmEstadisticas->EditFetchStalls->Text = DMGen->machPtr->counts->fetchStalls;
  if (DMGen->machPtr->cycleCount!=0)
    frmEstadisticas->EditFetchStallsPer->Text = (float)(100*DMGen->machPtr->counts->fetchStalls)/DMGen->machPtr->cycleCount;
  frmEstadisticas->EditFetchStallsDue->Text = DMGen->machPtr->counts->fullBufferStalls;
  if (DMGen->machPtr->counts->fetchStalls!=0)
    frmEstadisticas->EditFetchStallsDuePer->Text = (float)(100*DMGen->machPtr->counts->fullBufferStalls)/DMGen->machPtr->counts->fetchStalls;
  frmEstadisticas->EditDecodeStalls->Text = DMGen->machPtr->counts->decodeStalls;
  if (DMGen->machPtr->cycleCount!=0)
    frmEstadisticas->EditDecodeStallsPer->Text = (float)(100*DMGen->machPtr->counts->decodeStalls)/DMGen->machPtr->cycleCount;
}


//------------------------------------------------------------------
void InsertaSubcad (char *cad, char *subcad, int pos){

  // Inserta subcad en cad a partir de pos

  int i, j;
  j=0; i=pos;

  while (subcad[j]!='\0'){
    cad[i]=subcad[j];
    i++;
    j++;
  }

}

//-------------------------------------------------------------------

void ConstruyeFila(char *cad, AnsiString *datos, int numDatos){

  // Construye la fila que se inserta en cada listbox de estadisticas

  int i; //,j;
  char *aux;

  for (i=0;i<200;i++){
    cad[i]=' ';
  }

  for (i=0;i<numDatos;i++){
    aux=datos[i].c_str();
    //j=0;
    switch (i){
      case 0:
        InsertaSubcad(cad,aux,2);
        break;
      case 1:
        InsertaSubcad(cad,aux,13);
        break;
      case 2:
        InsertaSubcad(cad,aux,26);
        break;
      case 3:
        InsertaSubcad(cad,aux,40);
        break;
      case 4:
        InsertaSubcad(cad,aux,52);
        break;
      case 5:
        InsertaSubcad(cad,aux,66);
        break;
      case 6:
        InsertaSubcad(cad,aux,78);
        break;
    }

  }

}


//---------------------------------------------------------------------
void OperandsRenaming(void) {

  int i, numAll=0, numInt=0, numFp=0;
  int base;
  float bound;
  float precision=0;
  char aux[200];
  AnsiString datos[7];

// Operands Renaming
  if (DMGen->machPtr->counts->searchedOp[ALL]!=0)
    frmEstadisticas->EditRenamed->Text = (float)(100 * DMGen->machPtr->counts->renames[ALL])/DMGen->machPtr->counts->searchedOp[ALL];
  if (DMGen->machPtr->counts->searchedOp[INTEGER]!=0)
    frmEstadisticas->EditRenamedInt->Text = (float)(100 * DMGen->machPtr->counts->renames[INTEGER])/DMGen->machPtr->counts->searchedOp[INTEGER];
  if (DMGen->machPtr->counts->searchedOp[FLOAT]!=0)
    frmEstadisticas->EditRenamedFloat->Text = (float)(100 * DMGen->machPtr->counts->renames[FLOAT])/DMGen->machPtr->counts->searchedOp[FLOAT];

// Tener en cuenta precision que se pasa por linea comandos
  bound = precision * DMGen->machPtr->cycleCount/100.0;
  base = DMGen->machPtr->cycleCount;

  for (i=0; i<NUM_RENAME; i++){
    numAll = DMGen->machPtr->counts->numRename[ALL][i];
    numInt = DMGen->machPtr->counts->numRename[INTEGER][i];
    if (DMGen->machPtr->counts->issued[FLOAT])
      numFp = DMGen->machPtr->counts->numRename[FLOAT][i];

    if (numAll > bound || numInt > bound || numFp > bound){
        datos[0]=i;datos[1]=numAll;datos[2]=(100*numAll)/base;
        datos[3]=numInt;datos[4]=(100*numInt)/base;
        datos[5]=numFp;datos[6]=(100*numFp)/base;

        ConstruyeFila(aux,datos,7);

        frmEstadisticas->ListBoxOpRenaming->Items->Add(aux);
    }
  }

}

//----------------------------------------------------------------

void OperandsSearching(void){

  int i, numAll=0, numInt=0, numFp=0;
  int base;
  float bound;
  float precision=0;
  char aux[200];
  AnsiString datos[7];

  base = DMGen->machPtr->cycleCount;
  bound = precision * base/100.0;

// Operands Searching
  for (i=0; i<NUM_RENAME; i++){
    numAll = DMGen->machPtr->counts->numSearched[ALL][i];
    numInt = DMGen->machPtr->counts->numSearched[INTEGER][i];
    if (DMGen->machPtr->counts->searchedOp[FLOAT])
      numFp = DMGen->machPtr->counts->numSearched[FLOAT][i];

    if (numAll > bound || numInt > bound || numFp > bound){
        datos[0]=i;datos[1]=numAll;datos[2]=(100*numAll)/base;
        datos[3]=numInt;datos[4]=(100*numInt)/base;
        datos[5]=numFp;datos[6]=(100*numFp)/base;

        ConstruyeFila(aux,datos,7);

        frmEstadisticas->ListBoxSearching->Items->Add(aux);
    }

  }

}

//----------------------------------------------------------------------
void TFormPrincipal::EstadisticasRenaming(void){
  OperandsRenaming();
  OperandsSearching();
}

//---------------------------------------------------------------------
void InstructionIssue(void){

  int i, numAll=0, numInt=0, numFp=0;
  int base;
  float bound;
  float precision=0;
  char aux[200];
  AnsiString datos[7];


// Tener en cuenta precision que se pasa por linea comandos
  bound = precision * DMGen->machPtr->cycleCount/100.0;
  base = DMGen->machPtr->cycleCount;

// Instruction Issue Distribution
  for (i=0; i<NUM_OPS; i++){
    numAll = DMGen->machPtr->counts->numIssued[ALL][i];
    numInt = DMGen->machPtr->counts->numIssued[INTEGER][i];
    if (DMGen->machPtr->counts->issued[FLOAT])
      numFp = DMGen->machPtr->counts->numIssued[FLOAT][i];

    if (numAll > bound || numInt > bound || numFp > bound){
        datos[0]=i;datos[1]=numAll;datos[2]=(100*numAll)/base;
        datos[3]=numInt;datos[4]=(100*numInt)/base;
        datos[5]=numFp;datos[6]=(100*numFp)/base;

        ConstruyeFila(aux,datos,7);

        frmEstadisticas->ListBoxIssue->Items->Add(aux);
    }

  }


}

//------------------------------------------------------------------

void IssueDelay(void){

  int i, numAll=0, numInt=0, numFp=0;
  int base;
  float bound;
  float precision=0;
  char aux[200];
  AnsiString datos[7];

// Issue Delay Distribution
  base = DMGen->machPtr->counts->issued[ALL];
  bound = precision * DMGen->machPtr->insCount/100.0;

  for (i=0; i<NUM_OPS; i++){
    numAll = DMGen->machPtr->counts->numWait[ALL][i];
    numInt = DMGen->machPtr->counts->numWait[INTEGER][i];
    if (DMGen->machPtr->counts->issued[FLOAT])
      numFp = DMGen->machPtr->counts->numWait[FLOAT][i];

    if (numAll > bound || numInt > bound || numFp > bound){
        datos[0]=i;datos[1]=numAll;datos[2]=(100*numAll)/base;
        datos[3]=numInt;datos[4]=(100*numInt)/base;
        datos[5]=numFp;datos[6]=(100*numFp)/base;

        ConstruyeFila(aux,datos,7);

        frmEstadisticas->ListBoxDelay->Items->Add(aux);
    }

  }

}

//--------------------------------------------------------------------

void InstructionCommit(void) {

  int i, numAll=0, numInt=0, numFp=0;
  int base;
  float bound;
  float precision=0;
  char aux[200];
  AnsiString datos[7];

// Instruction Commit Distribution
  for (i=0; i<NUM_OPS; i++){
    numAll = DMGen->machPtr->counts->numCommit[ALL][i];
    numInt = DMGen->machPtr->counts->numCommit[INTEGER][i];
    if (DMGen->machPtr->counts->issued[FLOAT])
      numFp = DMGen->machPtr->counts->numCommit[FLOAT][i];

    if (numAll > bound || numInt > bound || numFp > bound){
        datos[0]=i;datos[1]=numAll;datos[2]=(100*numAll)/base;
        datos[3]=numInt;datos[4]=(100*numInt)/base;
        datos[5]=numFp;datos[6]=(100*numFp)/base;

        ConstruyeFila(aux,datos,7);

        frmEstadisticas->ListBoxCommit->Items->Add(aux);
    }

  }

}

//----------------------------------------------------------------------
void TFormPrincipal::EstadisticasInstruction(void){


  if (DMGen->machPtr->counts->issued[ALL]){
    InstructionIssue();
    IssueDelay();
  }
  if (DMGen->machPtr->counts->commited[ALL])
    InstructionCommit();
}


//-------------------------------------------------------------------

void IntegerBuffer(void){

  int i, numReorder=0, numWindow=0;
  int base;
  float bound;
  float precision=0;
  char aux[200];
  AnsiString datos[5];

// Tener en cuenta precision que se pasa por linea comandos
  bound = precision * DMGen->machPtr->cycleCount/100.0;
  base = DMGen->machPtr->cycleCount;
// Integer Buffer
  for (i=0; i<11; i++){
    numReorder = DMGen->machPtr->counts->intBufferOcc[REORDER_BUF][i];
    numWindow = DMGen->machPtr->counts->intBufferOcc[INST_WINDOW][i];

    if (numReorder > bound || numWindow > bound){
        datos[0]=i*10;datos[1]=numWindow;datos[2]=(100*numWindow)/base;
        datos[3]=numReorder;datos[4]=(100*numReorder)/base;

        ConstruyeFila(aux,datos,5);

        frmEstadisticas->ListBoxIntOccupancy->Items->Add(aux);
    }

  }


}

//-----------------------------------------------------------------

void FloatBuffer(void){

  int i, numReorder=0, numWindow=0;
  int base;
  float bound;
  float precision=0;
  char aux[200];
  AnsiString datos[5];


// Tener en cuenta precision que se pasa por linea comandos
  bound = precision * DMGen->machPtr->cycleCount/100.0;
  base = DMGen->machPtr->cycleCount;
  // Float Buffer
  for (i=0; i<11; i++){
    numReorder = DMGen->machPtr->counts->fpBufferOcc[REORDER_BUF][i];
    numWindow = DMGen->machPtr->counts->fpBufferOcc[INST_WINDOW][i];

    if (numReorder > bound || numWindow > bound){
        datos[0]=i*10;datos[1]=numWindow;datos[2]=(100*numWindow)/base;
        datos[3]=numReorder;datos[4]=(100*numReorder)/base;

        ConstruyeFila(aux,datos,5);

        frmEstadisticas->ListBoxFloatOccupancy->Items->Add(aux);
    }

  }

}

//----------------------------------------------------------------------
void TFormPrincipal::EstadisticasOccupancy(void){

  if (DMGen->machPtr->counts->commited[INTEGER])
    IntegerBuffer();
  if (DMGen->machPtr->counts->commited[FLOAT])
    FloatBuffer();
}


//-----------------------------------------------------------------
void __fastcall TFormPrincipal::StatisticsViewClick(TObject *Sender)
{

  frmEstadisticas->LimpiaListBox();

  EstadisticasGenerales();
  EstadisticasRenaming();
  EstadisticasInstruction();
  EstadisticasOccupancy();
  frmEstadisticas->Show();
}
//---------------------------------------------------------------------------
void TFormPrincipal::CargarEstadisticas(void) {

}

// -----------------------------------------------------

void TFormPrincipal::Estadisticas(AnsiString orden) {

 char * p , *cmd;


    Tcl_GetVar(DMGen->interp, "prompt", 1);
    cmd = orden.c_str();
    p = cmd + strlen(cmd);
    (void) Tcl_Eval(DMGen->interp, cmd , 0, &p);
    if (*DMGen->interp->result != 0) {
       StBar->SimpleText = ((AnsiString)"Clock Cycle : ") + DMGen->machPtr->cycleCount + "   " + ((AnsiString)(DMGen->interp->result));
    }


}
// -----------------------------------------------------

void __fastcall TFormPrincipal::StatisticsSaveClick(TObject *Sender)
{
  AnsiString orden;

  if (FormPrincipal->SaveDialogSaveStats->Execute()){
    orden = "stats -f " + FormPrincipal->SaveDialogSaveStats->FileName;
    this->Estadisticas(orden);
  }
}

//---------------------------------------------------------------------------
void TFormPrincipal::RellenarColaHistoria()
{

    //TODO: Add your source code here

// ***************************************************************************
// A la hora de instroducir elementos en la cola, se har· lo siguiente:


// * Se introducen las nuevas instrucciones que se hallan captado y se insertan al final de la cola.
//   Esto puede provocar que algunas instrucciones m·s viejas se salgan de la cola y se pierda su
//   informaciÛn.

// * Se recorre toda la cola, excepto las instrucciones que se han captado nuevas

//    *Para cada registro
//       * ver en la situaciÛn que estaba y ver a la situaciÛn que ha pasado
//       * modificar convenientemente el registro.



// 1∫. Para ver las instrucciones que se van a introducir nuevas en la cola hay que hacer lo siguiente:

//      - Se supone que la ˙ltima instrucciÛn introducida en la cola estar· en la ˙ltima posiciÛn, por lo
//        que se recupera y se ve su n˙mero de instrucciÛn. Con ese n˙mero de instrucciÛn, buscanos en la
//        cola de instrucciones de la m·quina y metemos en nuestra estructura, aquellas intrucciones que
//        tengan un "icount" mayor que el de referencia.


// 2∫. Ver en la situaciÛn que estaban las intrucciones y ver en la situaciÛn en la que pasan a
//     estar en el ciclo actual.

//     - Se recorren todos los registros antiguos (todos menos los introducidos en el paso anterior)
//         - Ver en el estado que estaban  (registro[i].estado != 0)
//         - Ver en el estado que est·n en el ciclo actual (puede ser que sigan en el mismo estado o
//           estÈn en el estado siguiente).

//         - Actualizar el registro convenientemente con el nuevo estado (si ha cambiado) o no hacer nada
//           si se sique en el mismo estado.

// Con lo anterior ya podemos actualizar convenientemente la estructura que nos mantiene la informaciÛn
// del estado de las distintas instrucciones.
// ****************************************************************************

// A continuaciÛn se realiza la implementaciÛn de estos algoritmos.

    bool salir;
    int ultimoIcount; // ˙ltimo icount introducido en la cola de historias
    int icount;
    int j;      // recorre la cola de instrucciones de la m·quina
    TInfoInstruccion registro; // almacena un registro de historia
    AnsiString cadena;
    int estado;
    int numRegistros;          // n˙mero de registros nuevos introducidos en la cola (instrucciones captadas
    AnsiString instruccion;

    // ----------------------------------- ***
    // Primero se calcula, la ˙ltima instrucciÛn introducida.
    if (this->cola->Tamano() == 0) {
        ultimoIcount = 0;
    }
    else {
       registro = this->cola->DevolverRegistro(cola->Tamano()-1);
       ultimoIcount = registro.icount;
    }
    // ----------------------------------- ***
    // buscanos en la cola de instrucciones de la m·quina y metemos en nuestra estructura, aquellas intrucciones que
    // tengan un "icount" mayor que el de referencia.
    if (this->CTListColaIns->Items->Count > 0) {
      cadena = this->CTListColaIns->Items->Strings[0]; // debe tener la ˙ltima instrucciÛn introducida
      cadena = this->ExtraerElemento(cadena,1);
      icount = cadena.ToInt();
    }
    else icount = ultimoIcount; // para que no meta ninguno
    // ----------------------------------- ***

    // habr· que introducir desde icount - ultimoIcount
    // ----------------------------------- ***
    numRegistros = 0;
    salir = false;
    j = this->CTListColaIns->Items->Count-1;
    while (!salir) {
      if (j < 0) salir = true;
      else {
         cadena = this->ExtraerElemento(this->CTListColaIns->Items->Strings[j],1);
         registro.icount = cadena.ToInt();
         if (registro.icount <= ultimoIcount) {
                j--;
         }
         else {
            registro.ciclos[1] = 0;     // Se iniciliza el registro que se va a introducir
            registro.ciclos[2] = 0;
            registro.ciclos[3] = 0;
            registro.ciclos[0] = DMGen->machPtr->cycleCount;
            registro.estado = 0;
            registro.flush  = 0;
            instruccion = this->ExtraerInstruccion(this->CTListColaIns->Items->Strings[j]);
            strcpy(registro.instruccion,instruccion.c_str());
            this->cola->InsetarRegistro(registro);
            numRegistros++;
            j--;
         }
      }
    } // While (!salir);


    // ----------------------------------- ***

    // si no hay instrucciones, se termina
    if (this->cola->Tamano() == 0) return;

    // Ya se han introducido las nuevas instrucciones.
    // Se recorren todos los registros antiguos
    numRegistros = this->cola->Tamano() - numRegistros;
    for(j=0; j < numRegistros; j++) {

        // Se recupera el registro

        registro = this->cola->DevolverRegistro(j);
        if ((registro.flush == 0)&&(registro.estado != 5)) {
          switch (registro.estado) { // Seg˙n el estado del registro, se podr· pasar a uno u otro estado, las
                                   // posibilidades son las siguientes:
           // de 0 -- 1  De la captaciÛn sÛlo se puede ir  a la decodificaciÛn o FLUSH
           // de 1 -- 2  de la decodificaciÛn se puede pasar a la ejecuciÛn    o FLUSH
           //    1 -- 3  de la decodificaciÛn se puede pasar a la ejecuciÛn y luego a WB (en el mismo ciclo) o FLUSH
           // de 2 -- 3  de ejecuciÛn sÛlo puede pasar a WB  o FLUSH
           // de 3 -- 4  de WB sÛlo se puede pasar a fin de la instrucciÛn (estado 4) o a FLUSH
           // de 4 -- 5  de fin de instrucciÛn se pasa a "retirada del ROB"
           case 0: // Estaba en estado de "captado", por lo que comprobamos
                  // si sigue en ese estado o ya se ha decodificado.
                   if (this->CTListColaIns->Items->Count >0) {
                    cadena = this->CTListColaIns->Items->Strings[this->CTListColaIns->Items->Count - 1];
                    cadena = this->ExtraerElemento(cadena,1);
                    icount = cadena.ToInt();
                   }
                   else
                        icount = registro.icount +1;
                   if (icount >  registro.icount ) { // si la primera instrucciÛn en la cola de instrucciones
                                                     // tiene un icount mayor que la de la instrucciÛn, es que
                                                     // ya se ha decodificado, ya que se decodifican en orden
                      // ya se ha decodificado O SE HA ELIMINADO DEL CAUCE
                      // Comprobamos si se ha eliminado del cauce, para ello, buscaremos la instrucciÛn
                      // en alguno de los dos ROB

                      // buscamos en el ROB de enteros
                      estado  = this->BuscarEnROB(registro.icount,this->CTListRBInt);
                      if ((estado == -1)||(this->BuscarEnROBNoUnit(registro.icount,this->CTListRBInt))) {
                         // si no est·, buscamos en el de floats
                         estado  = this->BuscarEnROB(registro.icount,this->CTListRBFloat);
                         if (estado == -1)
                           // no est· en ninguno, por lo que SE HA ELIMINADO DEL CAUCE
                             registro.flush = 1;
                         else
                             registro.ROB = 2;
                      }
                      else
                         registro.ROB = 1;
                      // Como se ha encontrado en  un buffer, es que se ha decodificado
                      registro.ciclos[1] = DMGen->machPtr->cycleCount;
                      registro.estado = 1;   // pasa a decodificacion
                   }
                   // si no entra en el if, sigue como estaba, sin decodificarse
                   break;

           case 1 : // Estaba en la ventana de instrucciones, tenemos que
                   // comprobar si ha comenzado su ejecuciÛn, para ello
                   // hay que buscar la instrucciÛn en los ROB y ver si
                   // su campo "ready" est· a un valor distinto de 0.
                   // TambiÈn tenemos que VER SI SE HA RETIRADO DEL CAUCE
                   // buscamos en el ROB de enteros
                   if (registro.ROB == 1) {
                      estado  = this->BuscarEnROB(registro.icount,this->CTListRBInt);
                   }
                   else
                      estado  = this->BuscarEnROB(registro.icount,this->CTListRBFloat);
                   if (estado == -1) {
                       // si no est·, buscamos en el de floats
                       registro.flush = 1;
                       registro.ciclos[2] = DMGen->machPtr->cycleCount;
                       registro.estado = 2;
                   }
                   // si est· en alguno de los dos ROB y "ready" es distinto de 0, es que
                   if (estado > 0) {                          // se ha pasado a ejecutar
                      registro.ciclos[2] =  DMGen->machPtr->cycleCount;
                      registro.estado = 2;
                   }
                   else
                     break;
                   // hay que ver si se ha pasado a wb (instrucciones que sÛlo
                   // consuman un ciclo de reloj

                   // no poner break, para que compruebe el siguiente caso

           case 2 : // Si se estaba ejecutando, hay que ver  si estamos
                   // en el ciclo marcado por "ready" menos 1, ya que en
                   // ese caso, pasa al estado WB
                   if (registro.ROB == 1)
                      estado  = this->BuscarEnROB(registro.icount,this->CTListRBInt);
                   else
                      estado  = this->BuscarEnROB(registro.icount,this->CTListRBFloat);
                   if (estado == -1) {
                      registro.flush = 1;
                      registro.ciclos[3] = DMGen->machPtr->cycleCount;
                      registro.estado = 3;
                   }
                   if ((estado) == (int)DMGen->machPtr->cycleCount ) { // se ha pasado a WB
                      registro.ciclos[3] =  estado;
                      registro.estado = 3;
                   }
                   break;

           case 3 : if (registro.ciclos[3] <= (int)DMGen->machPtr->cycleCount) {
                           registro.estado = 4;
                           registro.ciclos[4] = DMGen->machPtr->cycleCount;
                   }
                   else {
                       if (registro.ROB == 1)
                          estado  = this->BuscarEnROB(registro.icount,this->CTListRBInt);
                       else
                          estado  = this->BuscarEnROB(registro.icount,this->CTListRBFloat);
                       if (estado == -1) {
                            registro.flush = 1;
                            registro.ciclos[3] = DMGen->machPtr->cycleCount;
                            registro.estado = 3;
                       }
                       if ((estado) == (int)DMGen->machPtr->cycleCount ) { // se ha pasado a WB
                         registro.ciclos[3] =  estado;
                         registro.estado = 3;
                       }
                       else if ((estado) > (int)DMGen->machPtr->cycleCount) {
                         registro.estado = 3;
                       }
                   }
                   //break;

           case 4 : // Vemos si se ha pasado al estado 5 que es de eliminaciÛn del ROB

                   if (registro.ROB == 1)
                      estado =this->BuscarEnROB(registro.icount,this->CTListRBInt);
                   else
                      estado = this->BuscarEnROB(registro.icount,this->CTListRBFloat);
                   if (estado == -1) {
                         registro.estado = 5;
                         registro.ciclos[5] =  DMGen->machPtr->cycleCount+1;
                   }

          } // switch
          this->cola->ModificarRegistro(j,registro);
        } // if
      // Se vuelve a introducir el registro en la cola

    } // for
    // -----------------------------------  ***
} // fin del mÈtodo

// --------------------------------------------------------------------------


int TFormPrincipal::BuscarEnROB(int icount, CTListBox * rob)
{
    //TODO: Add your source code here

// Este mÈtodo sirve para buscar en "rob" una instrucciÛn con el icount especificado.
// si la encuentra, devuelve la entrada  "ready" del registro con ese icount, si no
// lo encuentra, devuelve -1

int i;
AnsiString cadena,cadena2;
int aux;
bool encontrado;

 encontrado = false;
 i = 0;
 while((!encontrado) && (i < rob->Items->Count)) {

     cadena = rob->Items->Strings[i];
     cadena2 = this->ExtraerElemento(cadena,2);
     aux = cadena2.ToInt();
     if ( aux == icount) {
          encontrado = true;
          cadena2 = this->ExtraerElemento(cadena,7);
     }
     else i++;
 }

 if (encontrado) return (cadena2.ToInt());
 else return -1;

}
// ---------------------------------------------------------------------------

 // --------------------------------------------------------------------------


int TFormPrincipal::BuscarEnROBNoUnit(int icount, CTListBox * rob)
{
    //TODO: Add your source code here

// Este mÈtodo sirve para buscar en "rob" una instrucciÛn con el icount especificado.
// e indica si utiliza para su ejecuciÛn un NoUnit, si lo encuentra devuelve 1, y si no
// lo encuentra, devuelve 0
// Si la instrucciÛn no est· en el ROB, devuelve -1

int i;
AnsiString cadena,cadena2;
int aux;
bool encontrado;

 if (icount == 7) {
    encontrado = false;
 }
 encontrado = false;
 i = 0;
 while((!encontrado) && (i < rob->Items->Count)) {

     cadena = rob->Items->Strings[i];
     cadena2 = this->ExtraerElemento(cadena,2);
     aux = cadena2.ToInt();
     if ( aux == icount) {
          encontrado = true;
          cadena2 = this->ExtraerElemento(cadena,4);
          if (cadena2 == AnsiString("NO_UNIT")) {
             return 1;
          }
          else return 0;
     }
     else i++;
 }
 return -1;

}
// ---------------------------------------------------------------------------

void __fastcall TFormPrincipal::ClockCycleDiagram1Click(TObject *Sender)
{
// Visualiza u oculta el diagrama de historia de ciclos
  if (this->ClockCycleDiagram1->Checked == false) {
      FormColaHistoria->Show();
      this->ClockCycleDiagram1->Checked = true;
  }
  else {
      FormColaHistoria->Hide();
      this->ClockCycleDiagram1->Checked = false;
  }

}
//---------------------------------------------------------------------------


void TFormPrincipal::VisualizarCambios2(void)
{
// ***************************************************************************
    //TODO: Add your source code here
    //
// Este mÈtodo actualiza todas las estructuras que podemos ver en la ventana principal del
// simulador y asigna los colores a las distintas instrucciones de las unidades.
// ***************************************************************************

 AnsiString orden;   // para hacer el "print" en el intÈrprete.
 char * p , *cmd;

    Tcl_GetVar(DMGen->interp, "prompt", 1);
    orden = "print" ;
    cmd = orden.c_str();
    p = cmd + strlen(cmd);
    (void) Tcl_Eval(DMGen->interp, cmd , 0, &p);
    if (*DMGen->interp->result != 0) {
         ShowMessage((AnsiString)(DMGen->interp->result));
    }
    // Se copia la cola de instrucciones a la lista de instrucciones
    this->InsertaLB2(this->CTListColaIns,contenidoQueue);
    // Se copian las ventanas de instrucciones a las correspondientes
    // listas de instrucciones
    //this->InsertaLB2(this->CTListVentanaInt,contenidoWindowInt,vector_ventana_instrucciones,11);
    //this->InsertaLB2(this->CTListVentanaFloat,contenidoWindowFloat,vector_ventana_instrucciones,11);
    // Se copian los buffers de ROB
    this->InsertaLB2(this->CTListRBInt,contenidoRBufferInt);
    this->InsertaLB2(this->CTListRBFloat,contenidoRBufferFloat);
    // Se copian el buffer de load / store
    //this->insertaLB(this->CTListLoad,contenidoDataLoad,vector_buffer_load,6);
    //this->insertaLB(this->CTListStore,contenidoDataStore,vector_buffer_store,9);

}

// ----------------------------------------------------------------------------

void TFormPrincipal::InsertaLB2(CTListBox * lb, char * cad)
{
// ***************************************************************************
//TODO: Add your source code here
// Esta funciÛn rellena el listBox que se le pasa "lb"  a partir de la cadena
// "cad". Para ello, separa el contenido de la cadena en los
// distintos registros que se introducir·n en la ventana.

// El formato de la cadena hace que cada registro se separe por dos barras verticales,
// excepto el primero que sÛlo tendr· una barra.
// Par·metros :
//               - Vector -> vector de posiciones para formatear la cadena mediante el mÈtodo "formato"
//               - numElementos-> n˙mero de elementos que tiene cada registro.
// ***************************************************************************

int l ;    // almacena la longitud de la cadena "cad"
int i,j;
char aux[150];
AnsiString  aux2;

l=strlen(cad);       // l pasa a ser la longitud de cad
lb->Items->Clear();  // se lÌmpia el listBox
j=0;                 // j (que recorre aux) pasa a ser cero

for (i=0;i<l;i++ ){             // se recorre la cadena de entrada
	if (cad[i]!='\n'){      // si el elemento no es un "\n"
           if (cad[i] != '|' ) {     // Se copian en aux (excepto si es una barra)
               aux[j]=cad[i];
	       j++;
           }

	}else{                  // si el elemento es un "\n", se ha completado un registro, por lo que se
             aux[j]='\0';       // formatea
             aux2 = (AnsiString)aux;
             lb->Items->Add(aux2);  // y se mete en el listBox
             j=0;                   // se inicializa j para comenzar con otra cadena
	}
}

return;

}

//---------------------------------------------------------------------------
void __fastcall TFormPrincipal::BranchPrediction1Click(TObject *Sender)
{
        this->Reset1Click(this); //hacemos un reset
        if(this->BranchPrediction1->Checked==false){
                //habilitar la opciÛn de mostrar el formulario de configuraciÛn
                this->JumpPredictionConfiguration1->Enabled=true;
//                FormPred->Show();
                //acceder a las variables de la m·quina necesarias para habilitar la predicciÛn de saltos
                DMGen->machPtr->branchPred=1;
                this->StBar->SimpleText="Branch Prediction Enabled with "+(AnsiString)DMGen->machPtr->branchSchema[1]+" counters";
                this->BranchPrediction1->Checked=true;
        }else{
                DMGen->machPtr->branchPred=0;
                this->StBar->SimpleText="Branch Prediction Disabled";
                this->BranchPrediction1->Checked=false;
                this->JumpPredictionConfiguration1->Enabled=false;
        }

}
//---------------------------------------------------------------------------

void __fastcall TFormPrincipal::JumpPredictionConfiguration1Click(
      TObject *Sender)
{
//mostramos el formulario para cambiar la configuraciÛn
          FormPred->ShowModal();
//          this->StBar->SimpleText="Branch Prediction Enabled with "+(AnsiString)DMGen->machPtr->branchSchema[1]+" counters";
}
//---------------------------------------------------------------------------


AnsiString __fastcall TFormPrincipal::ExtraerInstruccion(AnsiString cadena)
{
    //TODO: Add your source code here
// **************************************************************************
// Este mÈtodo sirve para extraer una instrucciÛn de la cadena que se le pasa
// como par·metro
// **************************************************************************

    AnsiString resultado;
    int k;
    AnsiString cola;
    int pos_blanco;
    AnsiString temporal[5];

k=0;
cola=cadena;
cola = cola.TrimRight(); // Se eliminan espacios a la derecha
// Va eliminando elementos de "cola" hasta llegar al i-Èsimo elemento, que copia en la cadena
// "cabeza"

while (k<=4){
	pos_blanco=cola.LastDelimiter(" "); //caracter del ˙ltimo  blanco

        temporal[4-k] = cola.SubString(pos_blanco + 1, cola.Length() - pos_blanco);
       	cola = cola.SubString(1,pos_blanco-1);
	cola = cola.TrimRight();
	k++;
}

resultado = temporal[0] + " " + temporal[1]+"," + temporal[2] + "," + temporal[3];

return resultado;


}

//------------------------------------------------------------------

void __fastcall TFormPrincipal::MenuGoToClick(TObject *Sender)
{
   FormGoToAddress->modo = 1;
   FormGoToAddress->ShowModal();
   if (FormGoToAddress->aceptar == 1) {
     StBar->SimpleText = ((AnsiString)"Clock Cycle : ") + DMGen->machPtr->cycleCount + "   " + ((AnsiString)(DMGen->interp->result));
     this->VisualizarCambios();
   }
  Statistics2->Enabled = true;
  Statistics1->Enabled = true;

}
//---------------------------------------------------------------------------

void __fastcall TFormPrincipal::BitBtnGoClick(TObject *Sender)
{
AnsiString orden;
char * cmd, * p;
    //aÒadido para cerrar la ventana de los ciclos de reloj
    if (this->ClockCycleDiagram1->Checked==true){
      FormColaHistoria->Close();
      this->ClockCycleDiagram1->Checked=false;
    }
    this->ClockCycleDiagram1->Enabled=false;


    Tcl_GetVar(DMGen->interp, "prompt", 1);
    orden = "go";
    cmd = orden.c_str();
    p = cmd + strlen(cmd);
    (void) Tcl_Eval(DMGen->interp, cmd , 0, &p);
    if (*DMGen->interp->result == 0) {
         ShowMessage((AnsiString)(DMGen->interp->result));
    }
    StBar->SimpleText = ((AnsiString)"Clock Cycle : ") + DMGen->machPtr->cycleCount + "   " + ((AnsiString)(DMGen->interp->result));
    this->VisualizarCambios();

    Statistics2->Enabled = true;
    Statistics1->Enabled = true;

}
//---------------------------------------------------------------------------










void __fastcall TFormPrincipal::Contens1Click(TObject *Sender)
{
    Application->HelpCommand(HELP_CONTENTS, 0);        
}
//---------------------------------------------------------------------------



