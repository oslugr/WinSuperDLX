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
#ifndef Unit4H
#define Unit4H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
extern "C"{
   #include "tcl.h"
   #include "tclint.h"
   #include "tclutil.h"
#include "superdlx.h"
}
#include "dlx.h"

//#include "tcl.h"
//#include "tclutil.h"
//---------------------------------------------------------------------------
class TDMGen : public TDataModule
{
__published:	// IDE-managed Components
        void __fastcall DMGenCreate(TObject *Sender);
private:	// User declarations
public:		// User declarations
        // Int�rprete TCL
        Tcl_Interp *interp;
        DLX * machPtr;
        int branchPred;  // predicci�n de salto
        int mem_size;  // MEMSIZE est� definida en dlx.h con 66000 bytes
        __fastcall TDMGen(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TDMGen *DMGen;
//---------------------------------------------------------------------------
#endif

