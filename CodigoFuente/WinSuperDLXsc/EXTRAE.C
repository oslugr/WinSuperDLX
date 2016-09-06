AnsiString funcion3(AnsiString src, int i)
//devuelve el elemento i-esimo de la cadena cad
{

int i,j,l,k;

AnsiString cabeza;
AnsiString cola;
AnsiString result;
int pos_blanco;

k=0;
cola=src;
cola = cola.TrimLeft();

j=0;
while (k<=i){
	pos_blanco=cola.AnsiPos(" "); //caracter del primer blanco
	cabeza=cola;
	if(k==i)	cabeza = cabeza.SubString(j,pos_blanco-(j+1));	
	cola = cola.SubString(pos_blanco,cola.Length()-pos_blanco+1);
	cola = cola.TrimLeft();
	k++;
}

return cabeza;
}
