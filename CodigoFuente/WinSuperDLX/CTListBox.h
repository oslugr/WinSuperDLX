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
// Emilio Jos� Garnica L�pez
// Componente EMTListBox  versi�n  1.0
// Tiene un comportamiento similar a TListBox pero puede
// almacenar un color por cada elemento que tenga el listbox
// Para que funcione hay que poner la propiedad Style a lbOwnerDrawFixed o lbOwnerDrawVariable
//---------------------------------------------------------------------------
#ifndef CTListBoxH
#define CTListBoxH
#define EMNumColores 50
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>
//---------------------------------------------------------------------------
class PACKAGE CTListBox : public TListBox
{
private:
     TColor colores[50]; // la estructura no maneja m�s de 50 elementos
     virtual void __fastcall DrawItem(int Index, const Windows::TRect &Rect,TOwnerDrawState State);
protected:
public:
        __fastcall CTListBox(TComponent* Owner);
 void   __fastcall anadir_color(int indice,TColor color);




};
//---------------------------------------------------------------------------
#endif

