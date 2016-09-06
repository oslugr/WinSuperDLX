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
#ifndef estadisticasH
#define estadisticasH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TfrmEstadisticas : public TForm
{
__published:	// IDE-managed Components
        TPageControl *PageControl;
        TTabSheet *TabSheetGeneral;
        TTabSheet *TabSheetRenaming;
        TBevel *Bevel1;
        TBitBtn *BitBtnClose;
        TTabSheet *TabSheetInstruction;
        TTabSheet *TabSheetOccupancy;
        TPanel *Panel1;
        TBitBtn *BitBtn1;
        TLabel *Label1;
        TEdit *EditCycles;
        TLabel *Label2;
        TLabel *Label3;
        TLabel *Label4;
        TLabel *Label5;
        TLabel *Label6;
        TLabel *Label7;
        TLabel *Label8;
        TLabel *Label9;
        TLabel *Label10;
        TEdit *EditFetched;
        TEdit *EditDecoded;
        TEdit *EditIssued;
        TEdit *EditIssuedInt;
        TEdit *EditIssuedFloat;
        TEdit *EditCommited;
        TEdit *EditCommitedInt;
        TEdit *EditCommitedFloat;
        TEdit *EditCommitedWrite;
        TLabel *Label11;
        TLabel *Label12;
        TLabel *Label13;
        TLabel *Label14;
        TLabel *Label15;
        TEdit *EditRatesIssue;
        TEdit *EditRatesDecode;
        TEdit *EditRatesFetch;
        TEdit *EditRatesCommit;
        TEdit *EditFetchedPer;
        TLabel *Label16;
        TBevel *Bevel2;
        TEdit *EditIssuedPer;
        TLabel *Label17;
        TLabel *Label18;
        TEdit *EditIssuedPerInt;
        TLabel *Label19;
        TEdit *EditIssuedPerFloat;
        TLabel *Label20;
        TEdit *EditCommitedPer;
        TLabel *Label21;
        TEdit *EditCommitedPerInt;
        TLabel *Label22;
        TEdit *EditCommitedPerFloat;
        TLabel *Label23;
        TEdit *EditCommitedPerWrites;
        TLabel *Label24;
        TBevel *Bevel3;
        TLabel *Label25;
        TEdit *EditCommitedUseless;
        TEdit *EditCommitedPerUseless;
        TLabel *Label26;
        TLabel *Label27;
        TLabel *Label28;
        TLabel *Label29;
        TLabel *Label30;
        TLabel *Label31;
        TEdit *EditBlkStore;
        TEdit *EditBlkPer;
        TLabel *Label32;
        TLabel *Label33;
        TEdit *EditBranches;
        TEdit *EditBranchTaken;
        TEdit *EditBranchUntaken;
        TLabel *Label34;
        TLabel *Label35;
        TEdit *EditBranchTakenPer;
        TEdit *EditBranchUntakenPer;
        TLabel *Label36;
        TLabel *Label37;
        TLabel *Label38;
        TEdit *EditFetchStalls;
        TEdit *EditFetchStallsPer;
        TLabel *Label39;
        TLabel *Label40;
        TEdit *EditFetchStallsDue;
        TEdit *EditFetchStallsDuePer;
        TLabel *Label41;
        TLabel *Label42;
        TEdit *EditDecodeStalls;
        TEdit *EditDecodeStallsPer;
        TLabel *Label43;
        TBevel *Bevel4;
        TBevel *Bevel5;
        TBevel *Bevel6;
        TBevel *Bevel7;
        TLabel *Label44;
        TLabel *Label45;
        TLabel *Label46;
        TLabel *Label47;
        TBevel *Bevel9;
        TBevel *Bevel10;
        TBevel *Bevel11;
        TLabel *Label48;
        TLabel *Label49;
        TLabel *Label50;
        TLabel *Label51;
        TLabel *Label52;
        TLabel *Label53;
        TLabel *Label54;
        TLabel *Label55;
        TLabel *Label56;
        TLabel *Label57;
        TListBox *ListBoxOpRenaming;
        TBevel *Bevel8;
        TBevel *Bevel12;
        TBevel *Bevel13;
        TBevel *Bevel14;
        TBevel *Bevel15;
        TBevel *Bevel16;
        TBevel *Bevel17;
        TEdit *EditRenamed;
        TLabel *Label58;
        TLabel *Label59;
        TEdit *EditRenamedInt;
        TLabel *Label60;
        TEdit *EditRenamedFloat;
        TLabel *Label61;
        TLabel *Label62;
        TListBox *ListBoxSearching;
        TLabel *Label63;
        TLabel *Label64;
        TLabel *Label65;
        TLabel *Label66;
        TLabel *Label67;
        TLabel *Label68;
        TLabel *Label69;
        TLabel *Label70;
        TLabel *Label71;
        TLabel *Label72;
        TBevel *Bevel18;
        TBevel *Bevel19;
        TBevel *Bevel20;
        TBevel *Bevel21;
        TLabel *Label73;
        TLabel *Label74;
        TLabel *Label75;
        TLabel *Label76;
        TLabel *Label77;
        TLabel *Label78;
        TLabel *Label79;
        TLabel *Label80;
        TLabel *Label81;
        TLabel *Label82;
        TBevel *Bevel22;
        TBevel *Bevel23;
        TBevel *Bevel24;
        TLabel *Label83;
        TListBox *ListBoxIssue;
        TLabel *Label84;
        TBevel *Bevel25;
        TBevel *Bevel26;
        TBevel *Bevel27;
        TBevel *Bevel28;
        TBevel *Bevel29;
        TBevel *Bevel30;
        TBevel *Bevel31;
        TLabel *Label85;
        TLabel *Label86;
        TLabel *Label87;
        TLabel *Label88;
        TLabel *Label89;
        TLabel *Label90;
        TLabel *Label91;
        TLabel *Label92;
        TLabel *Label93;
        TLabel *Label94;
        TListBox *ListBoxDelay;
        TLabel *Label95;
        TBevel *Bevel32;
        TBevel *Bevel33;
        TBevel *Bevel34;
        TBevel *Bevel35;
        TBevel *Bevel36;
        TBevel *Bevel37;
        TBevel *Bevel38;
        TLabel *Label96;
        TLabel *Label97;
        TLabel *Label98;
        TLabel *Label99;
        TLabel *Label100;
        TLabel *Label101;
        TLabel *Label102;
        TLabel *Label103;
        TLabel *Label104;
        TLabel *Label105;
        TListBox *ListBoxCommit;
        TStatusBar *StatusBar1;
        TLabel *Label106;
        TBevel *Bevel39;
        TBevel *Bevel40;
        TBevel *Bevel41;
        TBevel *Bevel42;
        TBevel *Bevel43;
        TBevel *Bevel44;
        TLabel *Label107;
        TLabel *Label108;
        TLabel *Label109;
        TLabel *Label110;
        TLabel *Label111;
        TLabel *Label112;
        TLabel *Label113;
        TListBox *ListBoxIntOccupancy;
        TLabel *Label114;
        TBevel *Bevel45;
        TBevel *Bevel46;
        TBevel *Bevel47;
        TBevel *Bevel48;
        TBevel *Bevel49;
        TBevel *Bevel50;
        TLabel *Label115;
        TLabel *Label116;
        TLabel *Label117;
        TLabel *Label118;
        TLabel *Label119;
        TLabel *Label120;
        TLabel *Label121;
        TListBox *ListBoxFloatOccupancy;
        void __fastcall BitBtn1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TfrmEstadisticas(TComponent* Owner);
        void LimpiaListBox();
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmEstadisticas *frmEstadisticas;
//---------------------------------------------------------------------------
#endif

