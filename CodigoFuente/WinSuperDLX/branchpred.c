#include "superdlx.h"
#include "math.h"
/*
Fichero donde se implementan las funciones de acceso a las estructuras de la predicción
de saltos


*/

int indice(BranchInfo* branch,int numCount){
/*función que se le pasan los contadores y devuelve el índice del contador que se debe acceder*/
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
//voy a sacar fuera del setPred la inserción del último resultado
//
int i;
//desplazamos a la derecha

        for (i=(numCount-2);i>=1;i--){
                branch->predictCounter[i+1]=branch->predictCounter[i];
        }
        branch->predictCounter[0]=outcome;  //metemos el resultado actual en la primera posición
        branch->lastSecuence=indice(branch,numCount);//indice devuelve el contador que debemos tocar para el último salto*/

}

void setPred(BranchInfo* branch,int* branchSchema,int value,int outcome)
/*
	función que actualizará la predicción de la entrada del btb que se le pasa como parámetro
	se llamará en el supercalc.c
	se le pasa el vector que guarda el estado de las ejecuciones anteriores de la  instruccíon
	y el valor de la predicción que se debe hacer. outcome es el resultado de la ejecución de la instrucción actual de salto
  . En esta situación es en la que se modifica el contador (FIFO)
*/
{

int i;
if (branchSchema[0]==BR_DEFAULT_HISTORY_BITS){//en función del número de bits se hace una cosa u otra :2bit en este caso

	switch (branchSchema[1]) //según el nº de contadores
	{
		case BR_DEFAULT_COUNT: //esquema por defecto del superdlx
				branch->predictState[0]+=value;
				break;
		default:  /*//desplazamos a la derecha
                        for (i=(branch->countNumber-1);i>=1;i--){
                            branch->predictCounter[i+1]=branch->predictCounter[i];
                        }
                        branch->predictCounter[0]=outcome;  //metemos el resultado actual en la primera posición
                        branch->lastSecuence=indice(branch);//indice devuelve el contador que debemos tocar para el último salto*/
                        branch->predictState[branch->lastSecuence]+=value;//actualizamos el contador según el último resultado
        }


}//else esquema con otro número de contadores

}

void setPredValue(BranchInfo *branch,unsigned int branchSchema[],int value)
/*
Esta función es igual a la anterior pero lo que hace es asignar un valor determinado, no cambiar de estado el autómata
se usa cuando bien se inserta una nueva instrucción de salto en el btb(en el suprefetch, con offset
- que indica el contador que se va actualizar- a 0), o bien, se queda en el mismo estado por que se ha acertado en la predicción, y en este caso
se debe avanzar el offset
*/
{
int i;

if (branchSchema[0]==BR_DEFAULT_HISTORY_BITS){//en función del número de bits se hace una cosa u otra :2bit en este caso

	switch (branchSchema[1]) //según el nº de contadores
	{
		case BR_DEFAULT_COUNT: //esquema por defecto del superdlx: 1 contador, 2 bits de historia
				branch->predictState[0]=value;
				break;
		default: for (i=(branchSchema[1]-1);i>=0;i--){
                            branch->predictCounter[i]=NOT_TAKEN; //ponemos la secuencia como no tomado
                            branch->predictState[i]=value;//asignamos el valor dado a cada contador
                          }

	}


}//else esquema con otro número de BITS

}

/////////////////////////////////////////////////
int getPred(BranchInfo *branch,int* branchSchema)
/*
función que devuelve la predicción hecha y que el superfetch mirará para ver si capta la instrucción
desde el pc actual o la que hay en la dirección de salto. Lo único que debe hacer esta función es mirar
todo el vector predictState y hacer el máximo de


*/
{
int prediction;
if (branchSchema[0]==BR_DEFAULT_HISTORY_BITS){//en función del número de bits se hace una cosa u otra :2bit en este caso

	switch (branchSchema[1]) //según el nº de contadores
	{
		case BR_DEFAULT_COUNT: //esquema por defecto del superdlx: 1 contador, 2 bits de historia:
                          //se ignoran el resto de variables añadidas para el caso genérico.
				prediction=branch->predictState[0];
				break;

		default: prediction = branch->predictState[branch->lastSecuence];  //devuelve la predicción hecha para la última secuencia
	}

}//else otro número de bits
return prediction;
}



void brInit(BranchInfo * branch,int *branchSchema)
{
/*
inicializa a ciertos valores los campos añadidos por mí de la estructura BranchInfo
*/
int j;
  	for ( j=0;j<BR_MAX_BITS ;j++ )
	  	{

        branch->predictCounter[j]=NOT_TAKEN;//inicializamos a 0
		  	branch->predictState[j] = TAKEN_;//inicializamos
      }
      branch->lastSecuence=NOT_TAKEN;//inicializamos la última secuencia como todo 0
 //     branch->countNumber=branchSchema[1];//BR_DEFAULT_COUNT; //asignamos al valor estandar



}

