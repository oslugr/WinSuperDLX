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
#pragma hdrstop



#include "Historia.h"
#include "ColaHistoria.h"
#include "Unit1.h"
#include "Unit4.h"
#include "Legend.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormColaHistoria *FormColaHistoria;
//---------------------------------------------------------------------------
__fastcall TFormColaHistoria::TFormColaHistoria(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormColaHistoria::BtnAceptarClick(TObject *Sender)
{
   FormPrincipal->ClockCycleDiagram1->Checked = false;
   Close();
   FormLegend->Close();
}
//---------------------------------------------------------------------------

void __fastcall TFormColaHistoria::FormPaint(TObject *Sender)
{
AnsiString cadena;
    int tamano;     // tamaÒo de la cola
    TInfoInstruccion registro; // almacena un registro de la cola
    TRect rectangulo;          // sirve para pintar un rect·ngulo del grid
    int limite;                // lÌmite de estados por los que ha pasado la instrucciÛn.
    int tamanoCiclos;
    TColor color;

    tamano = FormPrincipal->cola->Tamano();
    this->GridHistoria->RowCount = tamano +2;  // Tantas filas como indique el tamaÒo de la cola + 1 (de los ciclos)
    // Visualizar los cambios en el grid

    if (tamano > 0) { // Si hay elementos en la cola, rellinamos
                      // con su informaciÛn la cola
     try {
       // inicializamos algunas variables que nos har·n falta
       registro = FormPrincipal->cola->DevolverRegistro(0);

       // Se linicializa el n˙mero de casillas del grid
       this->GridHistoria->ColCount = DMGen->machPtr->cycleCount - registro.ciclos[0]+2; // 1 m·s para el n˙mero de instrucciÛn
       this->cicloInicial = registro.ciclos[0];   // Primer ciclo que se tendr· en cuenta
       // Se inicializan todas las casillas del Grid
       this->GridHistoria->Repaint();
       // Se insertan los ciclos

     }catch(...) {
       ShowMessage("ERROR : FormPaint");
       return;
     }

    } // if hay elementos
    else
     this->GridHistoria->ColCount = 2;

    return;
}

//---------------------------------------------------------------------------
// **************************************************************************
// --------------------------------------------------------------------------

void __fastcall TFormColaHistoria::GridHistoriaDrawCell(TObject *Sender,
      int ACol, int ARow, TRect &Rect, TGridDrawState State)
{
// Mediante "ARow" podemos saber a que registro nos estamos refiriendo y mediante
// "Acol" y "cicloInicial" podemos saber de que color tenemos que pintar la
// celda seg˙n el estado de la


AnsiString cadena;
int i;
int j;
int k;
int tamano;     // tamaÒo de la cola
TInfoInstruccion registro; // almacena un registro de la cola
TRect rectangulo;          // sirve para pintar un rect·ngulo del grid
int limite;                // lÌmite de estados por los que ha pasado la instrucciÛn.
int tamanoCiclos;
TColor color;
TRect rectanguloAuxiliar;

tamano = FormPrincipal->cola->Tamano();

  //if (this->cicloInicial == -1) return;
  // se calcula el ciclo de la celda

  if (ACol == 0) { // Es una casilla del n˙mero de instrucciÛn
     if ((ARow >= 1) && (ARow < FormPrincipal->cola->Tamano()+1)){
       registro = FormPrincipal->cola->DevolverRegistro(ARow - 1);
       //if (FormPrincipal->cola->Tamano() >= (ARow - 1)){
       // Tiene un n˙mero
       this->GridHistoria->Canvas->Pen->Color = clBlack;
       this->GridHistoria->Canvas->Brush->Color = clGray;
       this->GridHistoria->Canvas->TextRect(Rect,Rect.Left,Rect.Top+1,String(registro.icount));
       rectanguloAuxiliar = Rect;
       rectanguloAuxiliar.left = Rect.left+1;
       rectanguloAuxiliar.top  = Rect.top + 20;
       rectanguloAuxiliar.right = Rect.right;
       rectanguloAuxiliar.bottom = Rect.bottom;

       this->GridHistoria->Canvas->TextRect(rectanguloAuxiliar,rectanguloAuxiliar.Left,rectanguloAuxiliar.Top,AnsiString(registro.instruccion));
       //this->GridHistoria->Canvas->TextRect(Rect,Rect.Left+15,Rect.Top+20,AnsiString(registro.instruccion));
       if (registro.flush == 1) { // Si se ha retirado, se dibuja un cÌrculo, indic·ndolo
          this->GridHistoria->Canvas->Brush->Color = clRed;
          this->GridHistoria->Canvas->Ellipse(Rect.right-30,Rect.top+10,Rect.right-10,Rect.top+30);
       }
     }
     return;
  }

  if (ARow == 0) { // es una casilla de los ciclos
    if ((DMGen->machPtr->cycleCount - this->cicloInicial+1) >= ACol) { // Est· dentro de los lÌmites
      this->GridHistoria->Canvas->Pen->Color = clBlack;
      this->GridHistoria->Canvas->Brush->Color = clSilver;
      this->GridHistoria->Canvas->TextRect(Rect,Rect.Left+1,Rect.Top+1,String(this->cicloInicial + ACol -1));
    }
    return;
  }

  // -----------------------------------------------------
  // Si se sigue por aquÌ, es que es una casilla de colores (o blanca)
  i = ARow -1;
  if (i == 5) {
    registro = FormPrincipal->cola->DevolverRegistro(i);  // Se devuelve el registro
  }
  else
  registro = FormPrincipal->cola->DevolverRegistro(i);  // Se devuelve el registro
  if (i >= tamano) return; // si est· vacÌa, no se hace nada
  //Se recorren el resto de campos;
  //if (registro.estado == 5)
    //    limite = 4;
  //else

  limite = registro.estado;

  for(j=0;j<limite;j++) {      // Se recorren todas las fases de la instrucciÛn

       // tenemos que calcular el tamaÒo en ciclos de cada fase

       if ((registro.icount == 4) && (j==4)) {
              registro.icount = 4;
       }
       tamanoCiclos = registro.ciclos[j+1] - registro.ciclos[j]; // se calcula el tamaÒo en ciclos de la fase
       for(k=0;k< tamanoCiclos; k++) {
          // Se recupera el rect·ngulo y se pinta del color que le toque
          if (( (registro.ciclos[j]- this->cicloInicial+k+1) == ACol) &&
               ((i+1) == ARow)) {
            if ((j == 4) &&((k+1) == tamanoCiclos)){
                if (registro.ROB == 1)
                  this->GridHistoria->Canvas->Brush->Color = FormPrincipal->coloresHistoria[5];
                else
                  this->GridHistoria->Canvas->Brush->Color = FormPrincipal->coloresHistoria[6];
            }
            else
              this->GridHistoria->Canvas->Brush->Color = FormPrincipal->coloresHistoria[j];
            this->GridHistoria->Canvas->FillRect(Rect);
            if ( ((j < 2) &&( k > 0)) || ((j==4)&&((k+1) < tamanoCiclos))) {
                  // se pinta una cruz indicando que est· a la espera
                  // de pasar a la siguiente fase

                  this->GridHistoria->Canvas->MoveTo(Rect.Left+5,Rect.Top+5);
                  this->GridHistoria->Canvas->LineTo(Rect.Right-5,Rect.Bottom-5);
                  this->GridHistoria->Canvas->MoveTo(Rect.Right-5,Rect.Top+5);
                  this->GridHistoria->Canvas->LineTo(Rect.Left+5,Rect.Bottom-5);
                  return;
            }

          }
      } // for interno

 }  // for

  if ((registro.estado == 5)||(registro.flush==1)) {
   tamanoCiclos = 0;
  }
  else
   tamanoCiclos = DMGen->machPtr->cycleCount - registro.ciclos[limite]+1;
 for(k=0;k< tamanoCiclos; k++) {
    if (( (registro.ciclos[j]- this->cicloInicial+k+1) == ACol) &&
       ((i+1) == ARow)) {

     // Se recupera el rect·ngulo y se pinta del color que le toque
     if ((limite >= 6)||(limite <0)) {
        ShowMessage("ERROR : FormPaint -> j est· fuera de rango" + AnsiString(limite));
     }
     switch (j) {
        case 0 :   color =  FormPrincipal->coloresHistoria[0];
                   break;
        case 1 :   color  = FormPrincipal->coloresHistoria[1];
                   break;
        case 2 :   color  = FormPrincipal->coloresHistoria[2];
                   break;
        case 3 :   color  = FormPrincipal->coloresHistoria[3];
                   break;
        case 4 :   color  = FormPrincipal->coloresHistoria[4];
                   if (tamanoCiclos != 1) {
                     color  = FormPrincipal->coloresHistoria[4];
                   }
                   break;
        case 5 :   color = FormPrincipal->coloresHistoria[4];
                   break;
        default :  ShowMessage("Valor de j = " + j);
     }

     this->GridHistoria->Canvas->Brush->Color = color;
     this->GridHistoria->Canvas->FillRect(Rect);
       if (((j < 2) &&( k > 0)) || (j==4)) {                     // se pinta una cruz indicando que est· a la espera
                                                     // de pasar a la siguiente fase
         this->GridHistoria->Canvas->MoveTo(Rect.Left+5,Rect.Top+5);
         this->GridHistoria->Canvas->LineTo(Rect.Right-5,Rect.Bottom-5);
         this->GridHistoria->Canvas->MoveTo(Rect.Right-5,Rect.Top+5);
         this->GridHistoria->Canvas->LineTo(Rect.Left+5,Rect.Bottom-5);
      }
     return;
    } // if;

 }


// -----------------------------------------------------

  return;

}
//---------------------------------------------------------------------------

void __fastcall TFormColaHistoria::FormCreate(TObject *Sender)
{
   this->cicloInicial = 0;
   this->GridHistoria->ColWidths[0] = this->GridHistoria->DefaultColWidth *4;
}
//---------------------------------------------------------------------------


void __fastcall TFormColaHistoria::FormClose(TObject *Sender,
      TCloseAction &Action)
{
          FormPrincipal->ClockCycleDiagram1->Checked = false;
}
//---------------------------------------------------------------------------

void __fastcall TFormColaHistoria::BtnLegendClick(TObject *Sender)
{
   FormLegend->Visible = true;
}
//---------------------------------------------------------------------------


