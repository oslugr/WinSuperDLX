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
//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#include <stdlib.h>
#include <vcl/dstring.h>
#pragma hdrstop

#include "configuracion.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CSPIN"
#pragma resource "*.dfm"

#include "Unit4.h"
#include "Unit1.h"

TFormConfiguracion *FormConfiguracion;


//---------------------------------------------------------------------------
__fastcall TFormConfiguracion::TFormConfiguracion(TComponent* Owner)
        : TForm(Owner)
{

  /* Autor: Mario David Barrag�n Garc�a
     fecha: 13-4-2002

     Constructor Formulario de Configuraci�n
     Inicializa etiquetas a las etiquetas del formulario.

  */

  this->Etiquetas[0] = this->Label1;
  this->Etiquetas[1] = this->Label2;
  this->Etiquetas[2] = this->Label3;
  this->Etiquetas[3] = this->Label4;
  this->Etiquetas[4] = this->Label5;
  this->Etiquetas[5] = this->Label6;
  this->Etiquetas[6] = this->Label7;
  this->Etiquetas[7] = this->Label8;
  this->Etiquetas[8] = this->Label9;
  this->Etiquetas[9] = this->Label10;
  this->Etiquetas[10] = this->Label11;
  this->Etiquetas[11] = this->Label12;
  this->Etiquetas[12] = this->Label13;
  this->Etiquetas[13] = this->Label14;
  this->Etiquetas[14] = this->Label15;
  this->Etiquetas[15] = this->Label16;
  this->Etiquetas[16] = this->Label17;
  this->Etiquetas[17] = this->Label18;
  this->Etiquetas[18] = this->Label19;
  this->Etiquetas[19] = this->Label20;
  this->Etiquetas[20] = this->Label21;
  this->Etiquetas[21] = this->Label22;
  this->Etiquetas[22] = this->Label23;
  this->Etiquetas[23] = this->Label24;
  this->Etiquetas[24] = this->Label25;
  this->Etiquetas[25] = this->Label26;
  this->Etiquetas[26] = this->Label27;
  this->Etiquetas[27] = this->Label28;
  this->Etiquetas[28] = this->Label29;
  this->Etiquetas[29] = this->Label30;
  this->Etiquetas[30] = this->Label31;
  this->Etiquetas[31] = this->Label32;
  this->Etiquetas[32] = this->Label33;
  this->Etiquetas[33] = this->Label34;
  this->Etiquetas[34] = this->Label35;
  this->Etiquetas[35] = this->Label36;
  this->Etiquetas[36] = this->Label37;
  this->Etiquetas[37] = this->Label38;
  this->Etiquetas[38] = this->Label39;
  this->Etiquetas[39] = this->Label40;
  this->Etiquetas[40] = this->Label41;
  this->Etiquetas[41] = this->Label42;
  this->Etiquetas[42] = this->Label43;
  this->Etiquetas[43] = this->Label44;
  this->Etiquetas[44] = this->Label45;
  this->Etiquetas[45] = this->Label46;
  this->Etiquetas[46] = this->Label47;
  this->Etiquetas[47] = this->Label48;
  this->Etiquetas[48] = this->Label49;
  this->Etiquetas[49] = this->Label50;
  this->Etiquetas[50] = this->Label51;
  this->Etiquetas[51] = this->Label52;
  this->Etiquetas[52] = this->Label53;
  this->Etiquetas[53] = this->Label54;
  this->Etiquetas[54] = this->Label55;
  this->Etiquetas[55] = this->Label56;
  this->Etiquetas[56] = this->Label57;


}

//---------------------------------------------------------------------------

void __fastcall TFormConfiguracion::EscribeFichero(AnsiString File) {


  /* Autor: Mario David Barrag�n Garc�a
     fecha: 14-4-2002

     Metodo para escribir el fichero de configuracion de
     la maquina.


  */

  int i;
  int j=0;

  // Se limpia el Memo, borrando las l�neas de texto que tubiera

  this->MemoFichero->Lines->Clear();

  // ------------------*****---------------------------------------------
  // se utiliza un obejeto TMemo para almacenar los datos de
  // configuraci�n.
  for (i=0; i<ETIQUETAS; i++){
    if ((this->Etiquetas[i]->Tag)==2){
      this->MemoFichero->Lines->Add(this->Etiquetas[i]->Caption);
    }
    else
      if ((this->Etiquetas[i]->Tag)==1){
        this->MemoFichero->Lines->Add("    " + this->Etiquetas[i]->Caption);
      }
      if ((this->Etiquetas[i]->Tag)==0){
        this->MemoFichero->Lines->Add("\t" + this->Etiquetas[i]->Caption +"\t\t"+ this->datos[j]);
        j++;
      }
      if ((this->Etiquetas[i]->Tag)==-1){
        this->MemoFichero->Lines->Add(this->Etiquetas[i]->Caption +"\t"+ this->datos[j]);
        j++;
      }
  }

  this->MemoFichero->Lines->SaveToFile(File); //Se escribe la salida en un fichero

}

//--------------------------------------------------------------------------

void __fastcall TFormConfiguracion::LeeFichero(AnsiString File) {

  /* Autor: Mario David Barrag�n Garc�a
     fecha: 18-7-2002


  */


  FILE * fichero;
  int i=0;
  char aux[50];
  char *filename;

    filename = File.c_str();
    fichero = fopen(filename,"r");
    // se recorre el fichero hasta el final.
    // solo cogemos los datos y saltamos las etiquetas
    // (se cargaron en el constructor)
    while (!feof(fichero)) {
      fscanf(fichero,"%s ",aux);
      if ( (atoi(aux)!=0) || (aux == "0") ){
        this->datos[i] = atoi(aux);
        i++;
      }
     }
    fclose(fichero);

}

//----------------------------------------------------------------------------

void __fastcall TFormConfiguracion::VisualizaDatos(){

  /* Autor: Mario David Barrag�n Garc�a
     fecha: 18-7-2002


  */

    // una vez cargados los datos se visualizan en el formulario
    this->CSpinEdit1->Value = this->datos[0];
    this->CSpinEdit2->Value = this->datos[1];
    this->CSpinEdit3->Value = this->datos[2];
    this->CSpinEdit4->Value = this->datos[3];
    this->CSpinEdit5->Value = this->datos[4];
    this->CSpinEdit6->Value = this->datos[5];
    this->CSpinEdit7->Value = this->datos[6];
    this->CSpinEdit8->Value = this->datos[7];
    this->CSpinEdit9->Value = this->datos[8];
    this->CSpinEdit10->Value = this->datos[9];
    this->CSpinEdit11->Value = this->datos[10];
    this->CSpinEdit12->Value = this->datos[11];
    this->CSpinEdit13->Value = this->datos[12];
    this->CSpinEdit14->Value = this->datos[13];
    this->CSpinEdit15->Value = this->datos[14];
    this->CSpinEdit16->Value = this->datos[15];
    this->CSpinEdit17->Value = this->datos[16];
    this->CSpinEdit18->Value = this->datos[17];
    this->CSpinEdit19->Value = this->datos[18];
    this->CSpinEdit20->Value = this->datos[19];
    this->CSpinEdit21->Value = this->datos[20];
    this->CSpinEdit22->Value = this->datos[21];
    this->CSpinEdit23->Value = this->datos[22];
    this->CSpinEdit24->Value = this->datos[23];
    this->CSpinEdit25->Value = this->datos[24];
    this->CSpinEdit26->Value = this->datos[25];
    this->CSpinEdit27->Value = this->datos[26];
    this->CSpinEdit28->Value = this->datos[27];
    this->CSpinEdit29->Value = this->datos[28];
    this->CSpinEdit30->Value = this->datos[29];
    this->CSpinEdit31->Value = this->datos[30];
    this->CSpinEdit32->Value = this->datos[31];
    this->CSpinEdit33->Value = this->datos[32];
    this->CSpinEdit34->Value = this->datos[33];
    this->CSpinEdit35->Value = this->datos[34];
    this->CSpinEdit36->Value = this->datos[35];
    this->CSpinEdit37->Value = this->datos[36];
    this->CSpinEdit38->Value = this->datos[37];

}


//---------------------------------------------------------------------------



void __fastcall TFormConfiguracion::SpeedButtonLoadClick(TObject *Sender)
{

  /* Autor: Mario David Barrag�n Garc�a
     fecha: 14-4-2002

     Metodo para cargar los datos de un fichero de configuraci�n
  */


  if(OpenDialogConfiguration->Execute()) {
    LeeFichero(OpenDialogConfiguration->FileName);
    VisualizaDatos();
  }


}

//---------------------------------------------------------------------------

void __fastcall TFormConfiguracion::CapturaDatos(){

  /* Autor: Mario David Barrag�n Garc�a
     fecha: 18-7-2002


  */

  this->datos[0] = this->CSpinEdit1->Value;
  this->datos[1] = this->CSpinEdit2->Value;
  this->datos[2] = this->CSpinEdit3->Value;
  this->datos[3] = this->CSpinEdit4->Value;
  this->datos[4] = this->CSpinEdit5->Value;
  this->datos[5] = this->CSpinEdit6->Value;
  this->datos[6] = this->CSpinEdit7->Value;
  this->datos[7] = this->CSpinEdit8->Value;
  this->datos[8] = this->CSpinEdit9->Value;
  this->datos[9] = this->CSpinEdit10->Value;
  this->datos[10] = this->CSpinEdit11->Value;
  this->datos[11] = this->CSpinEdit12->Value;
  this->datos[12] = this->CSpinEdit13->Value;
  this->datos[13] = this->CSpinEdit14->Value;
  this->datos[14] = this->CSpinEdit15->Value;
  this->datos[15] = this->CSpinEdit16->Value;
  this->datos[16] = this->CSpinEdit17->Value;
  this->datos[17] = this->CSpinEdit18->Value;
  this->datos[18] = this->CSpinEdit19->Value;
  this->datos[19] = this->CSpinEdit20->Value;
  this->datos[20] = this->CSpinEdit21->Value;
  this->datos[21] = this->CSpinEdit22->Value;
  this->datos[22] = this->CSpinEdit23->Value;
  this->datos[23] = this->CSpinEdit24->Value;
  this->datos[24] = this->CSpinEdit25->Value;
  this->datos[25] = this->CSpinEdit26->Value;
  this->datos[26] = this->CSpinEdit27->Value;
  this->datos[27] = this->CSpinEdit28->Value;
  this->datos[28] = this->CSpinEdit29->Value;
  this->datos[29] = this->CSpinEdit30->Value;
  this->datos[30] = this->CSpinEdit31->Value;
  this->datos[31] = this->CSpinEdit32->Value;
  this->datos[32] = this->CSpinEdit33->Value;
  this->datos[33] = this->CSpinEdit34->Value;
  this->datos[34] = this->CSpinEdit35->Value;
  this->datos[35] = this->CSpinEdit36->Value;
  this->datos[36] = this->CSpinEdit37->Value;
  this->datos[37] = this->CSpinEdit38->Value;

}


//---------------------------------------------------------------------------

void __fastcall TFormConfiguracion::SpeedButtonSaveClick(TObject *Sender)
{
  /* Autor: Mario David Barrag�n Garc�a
     fecha: 12-4-2002

     Boton para guardar los datos introducidos por el
     usuario.

  */


  if (SaveDialogConfiguration->Execute()){

    CapturaDatos();
    this->EscribeFichero(SaveDialogConfiguration->FileName);
    this->Close();

  }

}

//---------------------------------------------------------------------------
void __fastcall TFormConfiguracion::BitBtnAplicarClick(TObject *Sender)
{
  /* Autor: Mario David Barrag�n Garc�a
     fecha: 18-7-2002

     Boton para confirmar los datos introducidos por el
     usuario.

  */

  FILE * fichero;
  int result;

    CapturaDatos();
    EscribeFichero ("conf.tmp");


    if ((fichero=fopen("conf.tmp","r"))== NULL) {
      ShowMessage ("Cannot open file");
      this->Close();
    }
    else {
//      liberaStruct(DMGen->machPtr);
      FormPrincipal->Reset1Click(this);
      result=readConfigFile(fichero,DMGen->interp,DMGen->machPtr);
      fclose(fichero);
      Sim_Create(DMGen->machPtr,1);
    }

    FormPrincipal->VisualizarCambios();
    this->Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormConfiguracion::CapturaDefecto(){


  this->datos[0] = NUM_FETCH;
  this->datos[1] = NUM_DECODE;
  this->datos[2] = NUM_COMMIT;

   /* sizes of queues, window, buffers .. have default parameters */
  this->datos[3] = MEMSIZE;     //
  this->datos[4] = MEM_LATENCY;
  this->datos[5] = NUM_MEM_ACCESS;
  this->datos[6] = NUM_I_REORDER;
  this->datos[7] = NUM_FP_REORDER;
  this->datos[8] = NUM_I_WINDOW;
  this->datos[9] = NUM_FP_WINDOW;
  this->datos[10] = NUM_LOAD_BUFF;
  this->datos[11] = NUM_STORE_BUFF;
  this->datos[12] = NUM_INS_QUEUE;
  this->datos[13] = NUM_BB_ENTRIES;

  /* functional units  latencies and numbers*/
  this->datos[14] = NUM_ALU_UNITS;
  this->datos[15] = INT_ALU_LATENCY;
  this->datos[16] = NUM_SHIFT_UNITS;
  this->datos[17] = INT_SHIFT_LATENCY;
  this->datos[18] = NUM_COMP_UNITS;
  this->datos[19] = INT_COMP_LATENCY;
  this->datos[20] = NUM_DATA_UNITS; //
  this->datos[21] = INT_ALU_LATENCY;
  this->datos[22] = NUM_BRANCH_UNITS;
  this->datos[23] = INT_ALU_LATENCY;

  this->datos[24] = NUM_FADD_UNITS;
  this->datos[25] = FP_ADD_LATENCY;
  this->datos[26] = NUM_FMUL_UNITS;
  this->datos[27] = FP_MUL_LATENCY;
  this->datos[28] = NUM_FDIV_UNITS;
  this->datos[29] = FP_DIV_LATENCY;
  this->datos[30] = NUM_FCONV_UNITS;
  this->datos[31] = FP_CONV_LATENCY;
  this->datos[32] = NUM_FCOMP_UNITS;
  this->datos[33] = FP_COMP_LATENCY;
  this->datos[34] = NUM_FDATA_UNITS; //
  this->datos[35] = INT_ALU_LATENCY;
  this->datos[36] = NUM_FBRANCH_UNITS;
  this->datos[37] = INT_ALU_LATENCY;

}


