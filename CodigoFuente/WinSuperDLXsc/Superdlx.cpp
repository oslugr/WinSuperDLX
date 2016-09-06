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
USERES("superdlx.res");
USEFORM("Unit1.cpp", FormPrincipal);
USEFORM("Unit2.cpp", FormFloatROB);
USEFORM("Unit3.cpp", FormAbout);
USEUNIT("asm.c");
USEUNIT("Getput.c");
USEUNIT("hash.c");
USEUNIT("inspect.c");
USEUNIT("Malltrap.c");
USEUNIT("Read.c");
USEUNIT("sim.c");
USEUNIT("strtrap.c");
USEUNIT("supercalc.c");
USEUNIT("superdecode.c");
USEUNIT("superfetch.c");
USEUNIT("Supersim.c");
USEUNIT("supertra.c");
USEUNIT("sym.c");
USEUNIT("trap.c");
USEFORM("Unit4.cpp", DMGen); /* TDataModule: DesignClass */
USEFORM("configuracion.cpp", FormConfiguracion);
USELIB("tcl.lib");
USEFORM("Unit5.cpp", FormCodigo);
USEFORM("FloatInstructionWindow.cpp", FormFloatInstructionWindow);
USEFORM("IntInstructionWindow.cpp", FormIntInstructionWindow);
USEFORM("RBInt.cpp", FormRBInt);
USEFORM("LoadBuffer.cpp", FormLoadBuffer);
USEFORM("StoreBuffer.cpp", FormStoreBuffer);
USEFORM("InstructionQueue.cpp", FormInstructionQueue);
USEFORM("GoToAddress.cpp", FormGoToAddress);
USEFORM("next.cpp", FormNext);
USEFORM("fichreg.cpp", FormFichReg);
USEFORM("estadisticas.cpp", frmEstadisticas);
USEUNIT("ColaHistoria.cpp");
USEFORM("Historia.cpp", FormColaHistoria);
USEFORM("branchform.cpp", FormPred);
USEUNIT("branchpred.c");
USEFORM("Legend.cpp", FormLegend);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
        try
        {
                 Application->Initialize();
                 Application->Title = "WinSuperDLX";
                 Application->HelpFile = "D:\\FACULTAD\\proyecto\\v2\\WinSuperDLX.HLP";
                 Application->CreateForm(__classid(TFormPrincipal), &FormPrincipal);
                 Application->CreateForm(__classid(TFormFloatROB), &FormFloatROB);
                 Application->CreateForm(__classid(TFormAbout), &FormAbout);
                 Application->CreateForm(__classid(TDMGen), &DMGen);
                 Application->CreateForm(__classid(TFormConfiguracion), &FormConfiguracion);
                 Application->CreateForm(__classid(TFormCodigo), &FormCodigo);
                 Application->CreateForm(__classid(TFormFloatInstructionWindow), &FormFloatInstructionWindow);
                 Application->CreateForm(__classid(TFormIntInstructionWindow), &FormIntInstructionWindow);
                 Application->CreateForm(__classid(TFormRBInt), &FormRBInt);
                 Application->CreateForm(__classid(TFormLoadBuffer), &FormLoadBuffer);
                 Application->CreateForm(__classid(TFormStoreBuffer), &FormStoreBuffer);
                 Application->CreateForm(__classid(TFormInstructionQueue), &FormInstructionQueue);
                 Application->CreateForm(__classid(TFormGoToAddress), &FormGoToAddress);
                 Application->CreateForm(__classid(TFormNext), &FormNext);
                 Application->CreateForm(__classid(TFormFichReg), &FormFichReg);
                 Application->CreateForm(__classid(TfrmEstadisticas), &frmEstadisticas);
                 Application->CreateForm(__classid(TFormColaHistoria), &FormColaHistoria);
                 Application->CreateForm(__classid(TFormPred), &FormPred);
                 Application->CreateForm(__classid(TFormLegend), &FormLegend);
                 Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }
        return 0;
}
//---------------------------------------------------------------------------





