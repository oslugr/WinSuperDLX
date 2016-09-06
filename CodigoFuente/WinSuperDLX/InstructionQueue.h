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
#ifndef InstructionQueueH
#define InstructionQueueH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "CTListBox.h"
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TFormInstructionQueue : public TForm
{
__published:	// IDE-managed Components
        TLabel *Label9;
        CTListBox *CTListColaIns;
        TBevel *Bevel1;
        TBitBtn *BitBtn1;
        TLabel *Label14;
        void __fastcall BitBtn1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TFormInstructionQueue(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormInstructionQueue *FormInstructionQueue;
//---------------------------------------------------------------------------
#endif

