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

/*
    Copyright (C) 1993 McGill University.
    Copyright (C) 1993 ACAPS Group.
    Copyright (C) 1993 Cecile Moura

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef _TRAPS_
#define _TRAPS_ 1

#define TRAP_OPEN		1
#define TRAP_CLOSE		2
#define TRAP_READ		3
#define TRAP_WRITE		4
#define TRAP_PRINTF		5
#define TRAP_SPRINTF		6
#define TRAP_FPRINTF		7
#define TRAP_SQRT		8
#define TRAP_EXP		9
#define TRAP_FOPEN		10
#define TRAP_FCLOSE		11
#define TRAP_GETS		12
#define TRAP_FGETS		13
#define TRAP_MALLOC		14
#define TRAP_CALLOC		15
#define TRAP_FREE		16

#define TRAP_STRCAT		17
#define TRAP_STRNCAT		18
#define TRAP_STRDUP		19
#define TRAP_STRCMP		20
#define TRAP_STRNCMP		21
#define TRAP_STRCASECMP		22
#define TRAP_STRICMP		22
#define TRAP_STRCMPI		22
#define TRAP_STRNCASECMP	23
#define TRAP_STRNICMP		23
#define TRAP_STRCPY		24
#define TRAP_STRNCPY		25
#define TRAP_STRLEN		26
#define TRAP_STRCHR		27
#define TRAP_INDEX		27
#define TRAP_STRRCHR		28
#define TRAP_RINDEX		28
#define TRAP_STRPBRK		29
#define TRAP_STRSPN		30
#define TRAP_STRCSPN		31
#define TRAP_STRSTR		32
#define TRAP_STRTOK		33
#define TRAP_STRTOD		34
#define TRAP_STRTOL		35
#define TRAP_STPCPY		36
/* traps that have been added for superDLX: attention, they are not generated in
 dlxcc. So they have to be artificially added in the .s file*/
#define TRAP_COS		37
#define TRAP_SIN		38
#define TRAP_TAN		39
#define TRAP_ATAN		40
#define TRAP_LOG		41

#define RETURN_VALUE  (machPtr->regs[1])
#define RETURN_DVALUE_L  (machPtr->regs[1])
#define RETURN_DVALUE_H  (machPtr->regs[2])
#define ARG(n)  (machPtr->memPtr[ADDR_TO_INDEX(machPtr->regs[14])+(n)].value)
#define SCRATCH_ADDR(n)  ((char *)machPtr->memScratch + (n))

int Handle_Open(DLX *machPtr);
int Handle_Fopen(DLX *machPtr);
int Handle_Close(DLX *machPtr);
int Handle_Fclose(DLX *machPtr);
int Handle_Read(DLX * machPtr);
int Handle_Write(DLX * machPtr);
int Handle_Gets(DLX * machPtr);
int Handle_Fgets(DLX *machPtr);
int Handle_Printf(DLX * machPtr);
int Handle_Sprintf(DLX *machPtr);
int Handle_Fprintf(DLX *machPtr);
int Handle_Xprintf(DLX *machPtr, char *dest, int argNumber);
Handle_Malloc (DLX *machPtr);
Handle_Calloc (DLX *machPtr);
Handle_Free (DLX *machPtr);
check_malloc_init (DLX* machPtr);
do_malloc_init (DLX *machPtr, unsigned addr);
int Handle_Sqrt(DLX *machPtr);
int Handle_Exp(DLX *machPtr);
int Handle_Cos(DLX *machPtr);
int Handle_Sin(DLX * machPtr);
int Handle_Tan(DLX *machPtr);
int Handle_Atan(DLX *machPtr);
int Handle_Log(DLX *machPtr);
Set_Errno(DLX *machPtr, int errorNumber);
String_From_Scratch (DLX *machPtr, unsigned firstAddr);
int String_To_Scratch(DLX *machPtr, register int firstAddr);
int Block_From_Scratch(DLX *machPtr, register unsigned firstAddr,register unsigned length);
Handle_Stpcpy (DLX *machPtr);
Handle_Strcat (DLX *machPtr);
Handle_Strchr (DLX *machPtr);
Handle_Strcmp (DLX *machPtr);
Handle_Strcpy (DLX *machPtr);
Handle_Strcspn (DLX *machPtr);
Handle_Strdup (DLX *machPtr);
Handle_Stricmp (DLX *machPtr);
Handle_Strlen (DLX *machPtr);
Handle_Strncat (DLX *machPtr);
Handle_Strncmp (DLX *machPtr);
Handle_Strncpy (DLX *machPtr);
Handle_Strnicmp (DLX *machPtr);
Handle_Strpbrk (DLX *machPtr);
Handle_Strrchr (DLX *machPtr);
Handle_Strspn (DLX * machPtr);
Handle_Strstr (DLX *machPtr);
Handle_Strtod (DLX *machPtr);
Handle_Strtok (DLX *machPtr);
Handle_Strtol (DLX *machPtr);

#endif

