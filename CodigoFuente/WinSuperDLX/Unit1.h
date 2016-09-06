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
#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
#include <ComCtrls.hpp>
#include "CTListBox.h"
#include <ToolWin.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <Buttons.hpp>
#include <jpeg.hpp>
#include <Dialogs.hpp>



#include "ColaHistoria.h"

//---------------------------------------------------------------------------
class TFormPrincipal : public TForm
{
__published:	// IDE-managed Components
        TMainMenu *MainMenu1;
        TMenuItem *File1;
        TMenuItem *Load1;
        TMenuItem *Reset1;
        TMenuItem *N1;
        TMenuItem *Exit1;
        TMenuItem *Simulation1;
        TMenuItem *MenuGoTo;
        TMenuItem *Next1;
        TMenuItem *Statistics1;
        TMenuItem *ROB1;
        TMenuItem *InstructionWindow1;
        TMenuItem *InstructionQueue1;
        TMenuItem *LoadStoreBuffer1;
        TMenuItem *Help1;
        TMenuItem *Configuration1;
        TMenuItem *Machineconfiguration1;
        TMenuItem *N3;
        TMenuItem *BranchPrediction1;
        TMenuItem *Contens1;
        TMenuItem *N4;
        TMenuItem *About1;
        TStatusBar *StBar;
        CTListBox *CTListVentanaInt;
        CTListBox *CTListVentanaFloat;
        CTListBox *CTListColaIns;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TLabel *Label4;
        CTListBox *CTListRBInt;
        CTListBox *CTListRBFloat;
        TLabel *Label5;
        TLabel *Label6;
        TLabel *Label7;
        TLabel *Label8;
        TToolBar *ToolBar1;
        TLabel *Label9;
        CTListBox *CTListLoad;
        CTListBox *CTListStore;
        TLabel *Label10;
        TLabel *Label11;
        TLabel *Label12;
        TLabel *Label13;
        TBitBtn *BitBtnGo;
        TMenuItem *Statistics2;
        TMenuItem *StatisticsView;
        TMenuItem *StatisticsSave;
        TButton *BtnNextSingleInstruction;
        TButton *Button2;
        TOpenDialog *OpenDlgAbrirFichero;
        TMenuItem *View1;
        TMenuItem *Sourcecode1;
        TMenuItem *NextSingleInstruction1;
        TMenuItem *RegisterFile1;
        TMenuItem *Next2;
        TMenuItem *FloatInstructionWindow1;
        TMenuItem *FloatROB1;
        TMenuItem *N5;
        TMenuItem *N6;
        TMenuItem *N7;
        TMenuItem *StoreBuffer1;
        TSaveDialog *SaveDialogSaveStats;
        TMenuItem *ClockCycleDiagram1;
        TMenuItem *JumpPredictionConfiguration1;
        TMenuItem *Go1;
        TLabel *Label14;
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall Exit1Click(TObject *Sender);
        void __fastcall CTListRBFloatDblClick(TObject *Sender);
        void __fastcall About1Click(TObject *Sender);
        void __fastcall Machineconfiguration1Click(TObject *Sender);
        void __fastcall Load1Click(TObject *Sender);
        void __fastcall Sourcecode1Click(TObject *Sender);
        void __fastcall Next1Click(TObject *Sender);
        void __fastcall CTListVentanaFloatDblClick(TObject *Sender);
        void __fastcall CTListVentanaIntDblClick(TObject *Sender);
        void __fastcall CTListRBIntDblClick(TObject *Sender);
        void __fastcall CTListLoadDblClick(TObject *Sender);
        void __fastcall CTListStoreDblClick(TObject *Sender);
        void __fastcall CTListColaInsDblClick(TObject *Sender);
        void __fastcall Reset1Click(TObject *Sender);
        void __fastcall BtnNextSingleInstructionClick(TObject *Sender);
        void __fastcall Next2Click(TObject *Sender);
        void __fastcall RegisterFile1Click(TObject *Sender);
        void __fastcall StatisticsViewClick(TObject *Sender);
        void __fastcall StatisticsSaveClick(TObject *Sender);
        void __fastcall ClockCycleDiagram1Click(TObject *Sender);
        void __fastcall BranchPrediction1Click(TObject *Sender);
        void __fastcall JumpPredictionConfiguration1Click(TObject *Sender);
        void __fastcall MenuGoToClick(TObject *Sender);
        void __fastcall BitBtnGoClick(TObject *Sender);
        void __fastcall Contens1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
        // Atributos

        AnsiString cadenaDestino;  // para el formateo de la salida
        TColor colores[50];        // para los listBox
        TColor coloresHistoria[10]; // para el StringGrid
        int numColores;
        TColaHistoria * cola;


        // m�etodos

        void __fastcall VisualizarCambios(void);
        __fastcall TFormPrincipal(TComponent* Owner);
        void __fastcall insertaLB(CTListBox * lb, char * cadena, int * vector, int numElementos);
        AnsiString __fastcall formatearCadena(char * cadena, int * vector, int numElementos);
        void __fastcall formato(int* pos,int n,AnsiString src);
        AnsiString __fastcall ExtraerElemento(AnsiString src, int i);
        void __fastcall Colorear(int n, CTListBox * lista);
        void __fastcall ColorearVentanaInstrucciones(CTListBox * ventana, CTListBox * rob);

        // A lo mejor no se usan
        void CargarEstadisticas(void);
        void Estadisticas(AnsiString orden);

        //------------------------------------

        void EstadisticasGenerales(void);
        void EstadisticasRenaming(void);
        void EstadisticasInstruction(void);
        void EstadisticasOccupancy(void);
        // -------- PARA LA HISTORIA DE CICLOS -
        void RellenarColaHistoria();
        int BuscarEnROB(int icount, CTListBox * rob);
        int BuscarEnROBNoUnit(int icount, CTListBox * rob);
    void VisualizarCambios2(void);
    void InsertaLB2(CTListBox * lb, char * cadena);
    AnsiString __fastcall ExtraerInstruccion(AnsiString cadena);



};
//---------------------------------------------------------------------------
extern PACKAGE TFormPrincipal *FormPrincipal;
//---------------------------------------------------------------------------
#endif

