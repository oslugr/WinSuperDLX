#include "superdlx.h"
#include "math.h"
/*
Fichero donde se implementan las funciones de acceso a las estructuras de la predicci�n
de saltos


*/

int indice(BranchInfo* branch,int numCount){
/*funci�n que se le pasan los contadores y devuelve el �ndice del contador que se debe acceder*/
int i,j;
int resul=0;
/*int inicial;
        inicial=(int)(branch->countNumber-1);*/
        for (i=(numCount-1),j=0;i>=0;i--,j++){
             resul+=branch->predictCounter[i]*(pow(2,j));
        }
         return resul;
}

void insertOutcome(BranchInfo * branch,int outcome,int numCount){
//voy a sacar fuera del setPred la inserci�n del �ltimo resultado
//
int i;
//desplazamos a la derecha

        for (i=(numCount-2);i>=1;i--){
                branch->predictCounter[i+1]=branch->predictCounter[i];
        }
        branch->predictCounter[0]=outcome;  //metemos el resultado actual en la primera posici�n
        branch->lastSecuence=indice(branch,numCount);//indice devuelve el contador que debemos tocar para el �ltimo salto*/

}

void setPred(BranchInfo* branch,int* branchSchema,int value,int outcome)
/*
	funci�n que actualizar� la predicci�n de la entrada del btb que se le pasa como par�metro
	se llamar� en el supercalc.c
	se le pasa el vector que guarda el estado de las ejecuciones anteriores de la  instrucc�on
	y el valor de la predicci�n que se debe hacer. outcome es el resultado de la ejecuci�n de la instrucci�n actual de salto
  . En esta situaci�n es en la que se modifica el contador (FIFO)
*/
{

int i;
if (branchSchema[0]==BR_DEFAULT_HISTORY_BITS){//en funci�n del n�mero de bits se hace una cosa u otra :2bit en este caso

	switch (branchSchema[1]) //seg�n el n� de contadores
	{
		case BR_DEFAULT_COUNT: //esquema por defecto del superdlx
				branch->predictState[0]+=value;
				break;
		default:  /*//desplazamos a la derecha
                        for (i=(branch->countNumber-1);i>=1;i--){
                            branch->predictCounter[i+1]=branch->predictCounter[i];
                        }
                        branch->predictCounter[0]=outcome;  //metemos el resultado actual en la primera posici�n
                        branch->lastSecuence=indice(branch);//indice devuelve el contador que debemos tocar para el �ltimo salto*/
                        branch->predictState[branch->lastSecuence]+=value;//actualizamos el contador seg�n el �ltimo resultado
        }


}//else esquema con otro n�mero de contadores

}

void setPredValue(BranchInfo *branch,unsigned int branchSchema[],int value)
/*
Esta funci�n es igual a la anterior pero lo que hace es asignar un valor determinado, no cambiar de estado el aut�mata
se usa cuando bien se inserta una nueva instrucci�n de salto en el btb(en el suprefetch, con offset
- que indica el contador que se va actualizar- a 0), o bien, se queda en el mismo estado por que se ha acertado en la predicci�n, y en este caso
se debe avanzar el offset
*/
{
int i;

if (branchSchema[0]==BR_DEFAULT_HISTORY_BITS){//en funci�n del n�mero de bits se hace una cosa u otra :2bit en este caso

	switch (branchSchema[1]) //seg�n el n� de contadores
	{
		case BR_DEFAULT_COUNT: //esquema por defecto del superdlx: 1 contador, 2 bits de historia
				branch->predictState[0]=value;
				break;
		default: for (i=(branchSchema[1]-1);i>=0;i--){
                            branch->predictCounter[i]=NOT_TAKEN; //ponemos la secuencia como no tomado
                            branch->predictState[i]=value;//asignamos el valor dado a cada contador
                          }

	}


}//else esquema con otro n�mero de BITS

}

/////////////////////////////////////////////////
int getPred(BranchInfo *branch,int* branchSchema)
/*
funci�n que devuelve la predicci�n hecha y que el superfetch mirar� para ver si capta la instrucci�n
desde el pc actual o la que hay en la direcci�n de salto. Lo �nico que debe hacer esta funci�n es mirar
todo el vector predictState y hacer el m�ximo de


*/
{
int prediction;
if (branchSchema[0]==BR_DEFAULT_HISTORY_BITS){//en funci�n del n�mero de bits se hace una cosa u otra :2bit en este caso

	switch (branchSchema[1]) //seg�n el n� de contadores
	{
		case BR_DEFAULT_COUNT: //esquema por defecto del superdlx: 1 contador, 2 bits de historia:
                          //se ignoran el resto de variables a�adidas para el caso gen�rico.
				prediction=branch->predictState[0];
				break;

		default: prediction = branch->predictState[branch->lastSecuence];  //devuelve la predicci�n hecha para la �ltima secuencia
	}

}//else otro n�mero de bits
return prediction;
}



void brInit(BranchInfo * branch,int *branchSchema)
{
/*
inicializa a ciertos valores los campos a�adidos por m� de la estructura BranchInfo
*/
int j;
  	for ( j=0;j<BR_MAX_BITS ;j++ )
	  	{

        branch->predictCounter[j]=NOT_TAKEN;//inicializamos a 0
		  	branch->predictState[j] = TAKEN_;//inicializamos
      }
      branch->lastSecuence=NOT_TAKEN;//inicializamos la �ltima secuencia como todo 0
 //     branch->countNumber=branchSchema[1];//BR_DEFAULT_COUNT; //asignamos al valor estandar



}

