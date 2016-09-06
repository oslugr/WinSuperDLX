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
#ifndef configuracionH
#define configuracionH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "CSPIN.h"
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ToolWin.hpp>


#define ETIQUETAS 57
#define DATOS 38

//---------------------------------------------------------------------------
class TFormConfiguracion : public TForm
{
__published:	// IDE-managed Components
        TLabel *Label1;
        TLabel *Label20;
        TLabel *Label2;
        TLabel *Label3;
        TLabel *Label4;
        TLabel *Label5;
        TLabel *Label6;
        TLabel *Label7;
        TLabel *Label8;
        TLabel *Label9;
        TLabel *Label10;
        TLabel *Label11;
        TLabel *Label13;
        TLabel *Label14;
        TLabel *Label15;
        TLabel *Label16;
        TLabel *Label17;
        TLabel *Label18;
        TLabel *Label19;
        TLabel *Label21;
        TLabel *Label22;
        TLabel *Label23;
        TLabel *Label24;
        TLabel *Label25;
        TLabel *Label26;
        TLabel *Label27;
        TLabel *Label28;
        TLabel *Label29;
        TLabel *Label30;
        TLabel *Label31;
        TLabel *Label32;
        TLabel *Label33;
        TLabel *Label34;
        TLabel *Label35;
        TLabel *Label36;
        TLabel *Label37;
        TLabel *Label38;
        TLabel *Label39;
        TLabel *Label40;
        TLabel *Label41;
        TLabel *Label42;
        TLabel *Label49;
        TLabel *Label50;
        TLabel *Label51;
        TLabel *Label52;
        TLabel *Label53;
        TLabel *Label54;
        TLabel *Label55;
        TLabel *Label56;
        TLabel *Label57;
        TLabel *Label43;
        TLabel *Label44;
        TLabel *Label45;
        TLabel *Label46;
        TLabel *Label47;
        TLabel *Label48;
        TCSpinEdit *CSpinEdit1;
        TCSpinEdit *CSpinEdit2;
        TCSpinEdit *CSpinEdit3;
        TCSpinEdit *CSpinEdit4;
        TCSpinEdit *CSpinEdit5;
        TCSpinEdit *CSpinEdit6;
        TCSpinEdit *CSpinEdit7;
        TCSpinEdit *CSpinEdit8;
        TCSpinEdit *CSpinEdit9;
        TCSpinEdit *CSpinEdit10;
        TCSpinEdit *CSpinEdit11;
        TCSpinEdit *CSpinEdit12;
        TCSpinEdit *CSpinEdit13;
        TCSpinEdit *CSpinEdit14;
        TCSpinEdit *CSpinEdit15;
        TCSpinEdit *CSpinEdit16;
        TCSpinEdit *CSpinEdit17;
        TCSpinEdit *CSpinEdit18;
        TCSpinEdit *CSpinEdit19;
        TCSpinEdit *CSpinEdit20;
        TCSpinEdit *CSpinEdit21;
        TCSpinEdit *CSpinEdit22;
        TCSpinEdit *CSpinEdit23;
        TCSpinEdit *CSpinEdit24;
        TCSpinEdit *CSpinEdit25;
        TCSpinEdit *CSpinEdit26;
        TCSpinEdit *CSpinEdit27;
        TCSpinEdit *CSpinEdit28;
        TCSpinEdit *CSpinEdit29;
        TCSpinEdit *CSpinEdit30;
        TCSpinEdit *CSpinEdit31;
        TCSpinEdit *CSpinEdit32;
        TCSpinEdit *CSpinEdit33;
        TCSpinEdit *CSpinEdit34;
        TCSpinEdit *CSpinEdit35;
        TCSpinEdit *CSpinEdit36;
        TCSpinEdit *CSpinEdit37;
        TCSpinEdit *CSpinEdit38;
        TLabel *Label12;
        TBevel *Bevel1;
        TBevel *Bevel2;
        TMemo *MemoFichero;
        TOpenDialog *OpenDialogConfiguration;
        TSaveDialog *SaveDialogConfiguration;
        TToolBar *ToolBar1;
        TSpeedButton *SpeedButtonLoad;
        TSpeedButton *SpeedButtonSave;
        TBitBtn *BitBtnAplicar;
        TBitBtn *BitBtnCancel;
        void __fastcall SpeedButtonLoadClick(TObject *Sender);
        void __fastcall SpeedButtonSaveClick(TObject *Sender);
        void __fastcall BitBtnAplicarClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        TLabel * Etiquetas[ETIQUETAS];
        long datos[DATOS];
        void __fastcall EscribeFichero(AnsiString File);
        void __fastcall LeeFichero(AnsiString File);
        void __fastcall VisualizaDatos();
        void __fastcall CapturaDatos();
        void __fastcall CapturaDefecto();
        __fastcall TFormConfiguracion(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormConfiguracion *FormConfiguracion;
//---------------------------------------------------------------------------
#endif

