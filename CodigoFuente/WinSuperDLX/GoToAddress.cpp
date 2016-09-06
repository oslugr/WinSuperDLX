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

#include "GoToAddress.h"
#include "Unit4.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormGoToAddress *FormGoToAddress;
//---------------------------------------------------------------------------
__fastcall TFormGoToAddress::TFormGoToAddress(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormGoToAddress::BtnYesClick(TObject *Sender)
{

   AnsiString orden;
   char * p , *cmd;

   if (this->modo == 1) {
     Tcl_GetVar(DMGen->interp, "prompt", 1);
     orden = "go  "  + this->EditAddress->Text ;
     cmd = orden.c_str();
     p = cmd + strlen(cmd);
     (void) Tcl_Eval(DMGen->interp, cmd , 0, &p);
     this->aceptar = 1;
   }
   else if (this->modo == 2) {

     Tcl_GetVar(DMGen->interp, "prompt", 1);
     orden = "step  "  + this->EditAddress->Text ;
     cmd = orden.c_str();
     p = cmd + strlen(cmd);
     (void) Tcl_Eval(DMGen->interp, cmd , 0, &p);
     this->aceptar = 1;
   }



}
//---------------------------------------------------------------------------
void __fastcall TFormGoToAddress::BtnCancelClick(TObject *Sender)
{
   this->aceptar = 0;        
}
//---------------------------------------------------------------------------
void __fastcall TFormGoToAddress::FormShow(TObject *Sender)
{
  this->aceptar = 0;
   if (this->modo == 1) { // go to address
      this->Caption = "Go To Address";
   }
   else {
      this->Caption = "Step To Address";
   }
}
//---------------------------------------------------------------------------

