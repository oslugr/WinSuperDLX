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

#include "Legend.h"
#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormLegend *FormLegend;
//---------------------------------------------------------------------------
__fastcall TFormLegend::TFormLegend(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TFormLegend::BtnAceptarClick(TObject *Sender)
{
        Close();        
}
//---------------------------------------------------------------------------
void __fastcall TFormLegend::FormPaint(TObject *Sender)
{
   // Se tienen que dibujar los rect�ngulos de los distintos colores.

   long izq;
   long menos, mas;

   izq = LblCaptacion->Left;
   menos = 5;
   mas = 15;

   // Color de captaci�n
   this->Canvas->Brush->Color = FormPrincipal->coloresHistoria[0];
   this->Canvas->Rectangle(izq-30,LblCaptacion->Top-menos,izq-10,LblCaptacion->Top+mas);
   // Color de decodificaci�n
   this->Canvas->Brush->Color = FormPrincipal->coloresHistoria[1];
   this->Canvas->Rectangle(izq-30,LblDecodificacion->Top-menos,izq-10,LblDecodificacion->Top+mas);
   // Color de ejecuci�n
   this->Canvas->Brush->Color = FormPrincipal->coloresHistoria[2];
   this->Canvas->Rectangle(izq-30,LblEjecucion->Top-menos,izq-10,LblEjecucion->Top+mas);
   // Color de WB
   this->Canvas->Brush->Color = FormPrincipal->coloresHistoria[3];
   this->Canvas->Rectangle(izq-30,LblWB->Top-menos,izq-10,LblWB->Top+mas);
   // Color de retirada para enteros
   this->Canvas->Brush->Color = FormPrincipal->coloresHistoria[5];
   this->Canvas->Rectangle(izq-30,LblRetiradaROBI->Top-menos,izq-10,LblRetiradaROBI->Top+mas);
   // Color de retirada para floats
   this->Canvas->Brush->Color = FormPrincipal->coloresHistoria[6];
   this->Canvas->Rectangle(izq-30,LblRetiradaROBF->Top-menos,izq-10,LblRetiradaROBF->Top+mas);
   // "x" para los ciclos de espera
   this->Canvas->Brush->Color = clBlack;
   this->Canvas->MoveTo(izq-30,LblEsperando->Top - menos);
   this->Canvas->LineTo(izq-10,LblEsperando->Top + mas);
   this->Canvas->MoveTo(izq-30,LblEsperando->Top + mas);
   this->Canvas->LineTo(izq-10,LblEsperando->Top - menos);
   // Color de instrucci�n retirada (antes de que termine su ejecuci�n
   this->Canvas->Brush->Color = clRed;
   this->Canvas->Ellipse(izq-30,LblEliminadaCauce->Top-menos,izq-10,LblEliminadaCauce->Top+mas);
}
//---------------------------------------------------------------------------

