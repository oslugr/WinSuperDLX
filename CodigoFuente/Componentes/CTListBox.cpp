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

#include "CTListBox.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(CTListBox *)
{
        new CTListBox(NULL);
}
//---------------------------------------------------------------------------
__fastcall CTListBox::CTListBox(TComponent* Owner)
        : TListBox(Owner)
{
}
// --------------------------------------------------------------------------
void __fastcall CTListBox::DrawItem(int Index, const Windows::TRect &Rect, TOwnerDrawState State){

      TCanvas *pCanvas = this->Canvas;
      pCanvas->Brush->Color = this->colores[Index];
      pCanvas->FillRect(Rect);
      pCanvas->TextOut(Rect.Left,Rect.Top, this->Items->Strings[Index]);

}
// --------------------------------------------------------------------------
void __fastcall CTListBox::anadir_color(int indice, TColor color){
   TRect  rect;
   TOwnerDrawState estado;

   estado << odFocused	;

   if ((indice >= 0)&&(indice < EMNumColores)){
      rect = this->ItemRect(indice);
      colores[indice] = color;
      DrawItem(indice, rect,estado);
      //Invalidate();
  }
}

//---------------------------------------------------------------------------
namespace Ctlistbox
{
        void __fastcall PACKAGE Register()
        {
                 TComponentClass classes[1] = {__classid(CTListBox)};
                 RegisterComponents("Samples", classes, 0);
        }
}
//---------------------------------------------------------------------------

