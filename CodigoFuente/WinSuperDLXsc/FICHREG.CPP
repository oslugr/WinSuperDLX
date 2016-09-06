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

#include "fichreg.h"
#include "Unit4.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CTListBox"
#pragma resource "*.dfm"
TFormFichReg *FormFichReg;
//---------------------------------------------------------------------------
__fastcall TFormFichReg::TFormFichReg(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormFichReg::FormShow(TObject *Sender)
{
int i;
float aux;
    CTListBoxInteger->Clear();
    CTListBoxFloat->Clear();

for (i=0;i<NUM_INT_REGS;i++){
        CTListBoxInteger->Items->Add("I"+(AnsiString)i+": "+DMGen->machPtr->regs[i]);
}
for (i=NUM_INT_REGS;i<NUM_INT_REGS+NUM_FP_REGS;i++){
        memcpy(&aux,&(DMGen->machPtr->regs[i]),4);
        //aux = (float)DMGen->machPtr->regs[i];
/*        if (aux != 0) {
          ShowMessage(aux);
        }*/

        CTListBoxFloat->Items->Add("F"+(AnsiString)(i-NUM_INT_REGS)+": "+ aux);
}

}
//---------------------------------------------------------------------------
void __fastcall TFormFichReg::BitBtnFichRegClick(TObject *Sender)
{
Close();
}
//---------------------------------------------------------------------------

