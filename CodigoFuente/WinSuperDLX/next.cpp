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
#pragma hdrstop

#include "Historia.h"
#include "next.h"
#include "Unit4.h"
#include "Unit1.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CSPIN"
#pragma resource "*.dfm"
TFormNext *FormNext;
//---------------------------------------------------------------------------
__fastcall TFormNext::TFormNext(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormNext::BtnOkClick(TObject *Sender)
{

AnsiString orden;
char * p , *cmd;
int i;
TRect  rectangulo;
int inicial,ninstrucciones;
bool salir;

   // Se ocultan las ventanas, para no perder tiempo cada vez que se repintan
   FormPrincipal->CTListColaIns->Visible = false;
   FormPrincipal->CTListVentanaInt->Visible = false;
   FormPrincipal->CTListVentanaFloat->Visible = false;
   FormPrincipal->CTListRBInt->Visible = false;
   FormPrincipal->CTListRBFloat->Visible = false;
   FormPrincipal->CTListLoad->Visible = false;
   FormPrincipal->CTListStore->Visible = false;
   // Se ocultan los botones.
   this->BtnOk->Visible = false;
   this->BitBtn2->Visible = false;
   // Se muestra la barra
   this->PBarProgreso->Visible = true;
   this->PBarProgreso->Position = 0;

   if (this->RGOptions->ItemIndex == 0) { // se haran x nuevas instrucciones
     orden = (AnsiString)"next 1"  + "c";
     cmd = orden.c_str();
     inicial = DMGen->machPtr->insCount;
     salir = false;
     ninstrucciones = DMGen->machPtr->insCount - inicial;
     while(!salir) {
        if (DMGen->machPtr->state == END_OF_SIMULATION)
          salir = true;
        else if (ninstrucciones >= this->CSEditPaso->Value)
          salir = true;
        else {
          // Si no se ha llegado al final, se sigue la simulaci�n
          Tcl_GetVar(DMGen->interp, "prompt", 1);
          p = cmd + strlen(cmd);
          (void) Tcl_Eval(DMGen->interp, cmd , 0, &p);
          FormPrincipal->StBar->SimpleText = ((AnsiString)"Clock Cycle : ") + DMGen->machPtr->cycleCount + "   " + ((AnsiString)(DMGen->interp->result));
          FormPrincipal->VisualizarCambios2();
          // Escalera de ciclos
          FormPrincipal->RellenarColaHistoria();
          ninstrucciones = DMGen->machPtr->insCount - inicial;
          this->PBarProgreso->Position = (ninstrucciones*100)/this->CSEditPaso->Value;
        }


    }

   } else if (this->RGOptions->ItemIndex == 1){ // se har�n x nuevos ciclos
       orden = (AnsiString)"next 1" + "c";
       cmd = orden.c_str();

       for(i=0;i< this->CSEditPaso->Value;i++) {
         Tcl_GetVar(DMGen->interp, "prompt", 1);
         p = cmd + strlen(cmd);
         (void) Tcl_Eval(DMGen->interp, cmd , 0, &p);
         FormPrincipal->StBar->SimpleText = ((AnsiString)"Clock Cycle : ") + DMGen->machPtr->cycleCount + "   " + ((AnsiString)(DMGen->interp->result));
         FormPrincipal->VisualizarCambios2();
         // Escalera de ciclos
         FormPrincipal->RellenarColaHistoria();
         this->PBarProgreso->Position = (i*100)/this->CSEditPaso->Value;


       }

   }

     //i = DMGen->machPtr->state;
     FormPrincipal->VisualizarCambios();
     rectangulo = FormColaHistoria->GridHistoria->ClientRect;
     FormColaHistoria->GridHistoria->Canvas->Brush->Color = clWhite;
     FormColaHistoria->GridHistoria->Canvas->FillRect(rectangulo);
     if (FormPrincipal->ClockCycleDiagram1->Checked == true)
         FormColaHistoria->FormPaint(this);
     // Se oculta la barra
     this->PBarProgreso->Visible = false;
     // Se omuestran  los botones.
     this->BtnOk->Visible = true;
     this->BitBtn2->Visible = true;

     FormPrincipal->CTListColaIns->Visible = true;
     FormPrincipal->CTListVentanaInt->Visible = true;
     FormPrincipal->CTListVentanaFloat->Visible = true;
     FormPrincipal->CTListRBInt->Visible = true;
     FormPrincipal->CTListRBFloat->Visible = true;
     FormPrincipal->CTListLoad->Visible = true;
     FormPrincipal->CTListStore->Visible = true;
     FormPrincipal->Statistics2->Enabled = true;
     FormPrincipal->Statistics1->Enabled = true;
     Close();

}
//---------------------------------------------------------------------------
void __fastcall TFormNext::BitBtn2Click(TObject *Sender)
{
   Close();
}
//---------------------------------------------------------------------------

