#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<iostream>
#include<ctype.h>

#define MAX_LINEAS 50
#define NOM 16
#define EXT 4

using namespace std;

struct texto{
	char linea[MAX_LINEAS];
	texto* siguiente;
};

struct cabezalTxt{
	texto* primero;
	texto* ultimo;
};

struct archivo{
	char nombre[NOM];
	char extension[EXT];
	archivo* siguiente;
	texto* lTexto=NULL; //LISTA DE LINEAS
	cabezalTxt* cabezalTexto = NULL;
	int tamanio;
};
typedef archivo* Archivo;

enum retorno{
	OK, ERROR, NO_IMPLEMENTADA
};
typedef enum retorno TipoRet;

struct nodoDirectorio{
	char nombre[15];
	nodoDirectorio *ph, *sh;
	Archivo lArchivo;
	int tamanio;
};
typedef nodoDirectorio* directorio;

struct nodoRuta{
	char nombreDir[15];
	char nombreArchivo[15];
	char extensionArchivo[4];
	nodoRuta *sig;
	nodoRuta *ant;
};
typedef nodoRuta* listaRuta;

struct sistema{
	directorio raiz;
	directorio actual;
	listaRuta rutaActual;
};
typedef sistema* Sistema;

bool comandoTrabajaConArchivo = false;
bool comandoCreador = false;
bool existeDirectorio = false;
char comando[NOM]="\0";//comando
char archivo1[NOM]="\0";//nombre
char extension1[EXT]="\0";//extension1
char archivo2[NOM]="\0";
char extension2[EXT]="\0";
char textoAux[100]="\0";
int auxSwitch;
int k;
int tamanioTextoAIngresar;
char direc[15]="\0"; //nombre directorio
char rutaAbsRel[100]="\0";
char nombreDirACrear [15];

TipoRet Resultado;
Archivo lArchivo = NULL;
Archivo ultimoBorrado = NULL;
Sistema sis = new sistema;
listaRuta rutaAux;
listaRuta rutaVerificaCamino = NULL;
directorio auxBusca;
char nombreNodoDirectorio[NOM];
bool rutaCorrecta = true;//si se mantiene true, la ruta ingresada es correcta, de lo contrario la ruta ingresada es incorrecta


TipoRet MKDIR();
void inicializarSistema();

Archivo existeArchivo(char,char);
void resetDirec();
void switchComandos();
void evaluaEntrada();
void lecturaEntrada();
void limpiarComandoArchivosExtensiones();
TipoRet CREATE(char,char);
TipoRet DIR();
TipoRet DELETE(char,char);
TipoRet IF(char,char,char);
TipoRet IC(char,char,char);
TipoRet TYPE(Archivo,char,char);
TipoRet BF(char,char,int);
TipoRet BC(char,char,int);
TipoRet CAT(char,char,char,char);

TipoRet PWD();
TipoRet MKDIR();
//TipoRet UNDELETE(Archivo);

Archivo existeArchivo(char nombre[15],char extension[15]){ //RETURN NULL SI NO EXISTE EL ARCHIVO, PUNTERO AL AL ARCHIVO EN CASO DE QUE EXISTA
	bool yaexiste = false;
	archivo* aux = auxBusca->lArchivo;
	while(!yaexiste && aux != NULL){
		if(strcmp(aux->nombre,nombre)==0){
			if(strcmp(aux->extension,extension)==0)
				return aux;
			else
				aux = aux->siguiente;
		}else
			aux = aux->siguiente;
	}
	return NULL;
}
TipoRet CAT(/*Archivo lArchivo,*/char nombre1[NOM],char extension1[EXT],char nombre2[NOM],char extension2[EXT]){
	archivo* puntArchivo1;
	archivo* puntArchivo2;
	puntArchivo1 = existeArchivo(nombre1,extension1); //AUX VUELVE APUNTANDO AL ARCHIVO EN CASO DE QUE ESTE EXISTA O NULL EN CASO CONTRARIO
	if(puntArchivo1 != NULL){
		puntArchivo2 = existeArchivo(nombre2,extension2);
		if(puntArchivo2 != NULL){
			if(puntArchivo2->lTexto!=NULL){
				int tamanioLinea = 0;
				char textoAIngresar[MAX_LINEAS];
				texto* lineaArchivo2 = puntArchivo2->cabezalTexto->primero;
				while(lineaArchivo2 != NULL){
					texto* nuevo = new texto;
					strcpy(nuevo->linea,lineaArchivo2->linea);
					nuevo->siguiente = NULL;
					if(puntArchivo1->cabezalTexto == NULL){
						puntArchivo1->cabezalTexto = new cabezalTxt;
						puntArchivo1->cabezalTexto->primero = nuevo;
						puntArchivo1->cabezalTexto->ultimo = nuevo;
						puntArchivo1->lTexto = nuevo;
					}else{
					puntArchivo1->cabezalTexto->ultimo->siguiente = nuevo;
					puntArchivo1->cabezalTexto->ultimo = nuevo;
					}
					//SUMA AL TAMANIO TOTAL DEL ARCHIVO
					strcpy(textoAIngresar,nuevo->linea);
					tamanioLinea = strlen(textoAIngresar);
					puntArchivo1->tamanio += tamanioLinea;
					auxBusca->tamanio += tamanioLinea;

					lineaArchivo2 = lineaArchivo2->siguiente;
				}
				Resultado = OK;
				return Resultado;
			}
		}else{
			Resultado = ERROR;
			printf("El archivo %s.%s no existe\n",nombre2,extension2);
			return Resultado;
			}
	}else{
		Resultado = ERROR;
		printf("El archivo %s.%s no existe\n",nombre1,extension1);
		return Resultado;
	}
}


TipoRet BC(Archivo &lArchivo, char nombre[NOM], char extension[EXT], int num){
	archivo* aux ;
	aux = existeArchivo(nombre,extension); //AUX VUELVE APUNTANDO AL ARCHIVO EN CASO DE QUE ESTE EXISTA O NULL EN CASO CONTRARIO
	if(aux != NULL){
		int cont=0;
		texto* a_borrar = new texto;
		texto* auxTexto = aux->lTexto;
		while(auxTexto!=NULL){
			cont++;
			auxTexto=auxTexto->siguiente;
		}
		auxTexto=aux->lTexto;
		if(auxTexto!=NULL){
			if(num>=cont){
				auxBusca->tamanio = auxBusca->tamanio - aux->tamanio;
				for(int i=0;i<cont;i++){
					a_borrar = auxTexto;
					auxTexto = auxTexto->siguiente;
					delete a_borrar;
				}
				aux->lTexto=NULL;
				aux->cabezalTexto->primero=aux->lTexto;
				aux->cabezalTexto->ultimo=aux->lTexto;
				aux->tamanio=0;
			}else{
				auxBusca->tamanio = auxBusca->tamanio - aux->tamanio;
				for (int i=0;i<num;i++){
					a_borrar=auxTexto;
					aux->lTexto=aux->lTexto->siguiente;
					auxTexto=aux->lTexto;
					aux->cabezalTexto->primero=aux->lTexto;
					delete a_borrar;
				}
				auxTexto=aux->lTexto;
				aux->tamanio=0;
				while(auxTexto!=NULL){
					aux->tamanio += strlen(auxTexto->linea);
					auxTexto=auxTexto->siguiente;
				}
				auxBusca->tamanio += aux->tamanio;
				a_borrar=NULL;
				Resultado = OK;
			}
		}else{
			Resultado = ERROR;
			printf("El archivo no tiene lineas\n");
		}
	}else{
			printf("No existe el archivo indicado\n");
			Resultado = ERROR;
	}
	return Resultado;
}

TipoRet BF(Archivo &lArchivo, char nombre[NOM], char extension[EXT], int num){
	archivo* aux;
	aux = existeArchivo(nombre,extension); //AUX VUELVE APUNTANDO AL ARCHIVO EN CASO DE QUE ESTE EXISTA O NULL EN CASO CONTRARIO
	if(aux != NULL){

		int cont=0;
		texto* auxTexto = aux->lTexto;
		texto* a_borrar = new texto;
		while(auxTexto!=NULL){
			cont++;
			auxTexto=auxTexto->siguiente;
		}
		auxTexto=aux->lTexto;
		if(auxTexto!=NULL){
			if(num>=cont){
				auxBusca->tamanio = auxBusca->tamanio - aux->tamanio;
				for(int i=0;i<cont;i++){
					a_borrar = auxTexto;
					auxTexto = auxTexto->siguiente;
					delete a_borrar;
				}
				aux->lTexto=NULL;
				aux->cabezalTexto->primero=aux->lTexto;
				aux->cabezalTexto->ultimo=aux->lTexto;
				aux->tamanio=0;
			}else{
				auxBusca->tamanio = auxBusca->tamanio - aux->tamanio;
				for(int i=1;i<cont-num;i++){
					auxTexto=auxTexto->siguiente;
				}
				aux->cabezalTexto->ultimo=auxTexto;
				for(int i=0;i<num;i++){
					a_borrar = auxTexto->siguiente;
					auxTexto->siguiente = a_borrar->siguiente;
					delete a_borrar;
				}
				auxTexto=aux->lTexto;
				aux->tamanio=0;
				while(auxTexto!=NULL){
					aux->tamanio += strlen(auxTexto->linea);
					auxTexto=auxTexto->siguiente;
				}
				auxBusca->tamanio += aux->tamanio;
			}
		a_borrar = NULL;
		Resultado = OK;
		return Resultado;
		}else{
			Resultado = ERROR;
			printf("El archivo no tiene lineas\n");
			return Resultado;
		}
	}else{
		printf("No existe el archivo indicado\n");
		Resultado = ERROR;
		return Resultado;
	}
}

TipoRet TYPE(char nombre[NOM], char extension[EXT]){
	archivo* aux ;
	aux = existeArchivo(nombre,extension);
	if(aux != NULL){
		texto* auxTexto = aux->lTexto;
		if(auxTexto==NULL){
			printf("El archivo no contiene lineas\n");
		}else{
			while(auxTexto!=NULL){
				printf("%s\n", auxTexto->linea);
				auxTexto=auxTexto->siguiente;
			}
			Resultado = OK;
			return Resultado;
		}
	}else{
		Resultado = ERROR;
		printf("No existe un archivo con ese nombre en este directorio\n");
		return Resultado;
	}
}

TipoRet IC(char nombre[NOM], char extension[EXT], char textoIns[MAX_LINEAS]){
	archivo* aux ;
	aux = existeArchivo(nombre,extension);
	if(aux != NULL){
		tamanioTextoAIngresar = strlen(textoIns);
		texto* nuevo = new texto;
		strcpy(nuevo->linea,textoIns);
		if(aux->lTexto==NULL){
			aux->tamanio += tamanioTextoAIngresar;
			auxBusca->tamanio += aux->tamanio;
			aux->lTexto = nuevo;
			aux->cabezalTexto = new cabezalTxt;
			aux->cabezalTexto->primero=aux->lTexto;
			aux->cabezalTexto->ultimo=aux->lTexto;
			aux->lTexto->siguiente = NULL;
			Resultado = OK;
			return Resultado;
		}else{
			aux->tamanio += tamanioTextoAIngresar;
			auxBusca->tamanio += tamanioTextoAIngresar;
			nuevo->siguiente = aux->lTexto;
			aux->lTexto = nuevo;
			aux->cabezalTexto->primero = aux->lTexto;
			Resultado = OK;
			return Resultado;
		}
	}else{
		Resultado = ERROR;
		return Resultado;
	}
}

TipoRet IF(Archivo &lArchivo, char nombre[NOM], char extension[EXT], char textoAIngresar[MAX_LINEAS]){
	archivo* aux ;
	aux = existeArchivo(nombre,extension); //AUX VUELVE APUNTANDO AL ARCHIVO EN CASO DE QUE ESTE EXISTA O NULL EN CASO CONTRARIO
	if(aux != NULL){
		tamanioTextoAIngresar = strlen(textoAIngresar);
		if(aux->lTexto==NULL){
			aux->tamanio += tamanioTextoAIngresar;
			auxBusca->tamanio += aux->tamanio;
			texto* nuevo = new texto;
			aux->lTexto = nuevo;
			aux->cabezalTexto = new cabezalTxt;
			aux->cabezalTexto->primero=aux->lTexto;
			aux->cabezalTexto->ultimo=aux->lTexto;
			aux->lTexto->siguiente = NULL;
			strcpy(aux->lTexto->linea,textoAIngresar);
			Resultado = OK;
			return Resultado;
		}else{
			aux->tamanio += tamanioTextoAIngresar;
			auxBusca->tamanio += tamanioTextoAIngresar;
			texto* nuevo = new texto;
			aux->cabezalTexto->ultimo->siguiente = nuevo;
			aux->cabezalTexto->ultimo = nuevo;
			nuevo->siguiente = NULL;
			strcpy(nuevo->linea,textoAIngresar);
			Resultado = OK;
			return Resultado;
		}
	}else{
		Resultado = ERROR;
		printf("No existe el archivo indicado\n");
		return Resultado;
	}
}

TipoRet DIR(){
	//ARCHIVOS
	bool tieneArchivos = true;
	if(auxBusca->lArchivo == NULL){
		tieneArchivos = false;
	}else{
		Archivo lArchivoDir = auxBusca->lArchivo;
		char tipoArchivo[8] = {'A','r','c','h','i','v','o','\0'};
		while(lArchivoDir != NULL){
				printf("%s.%s     %s     %d\n",lArchivoDir->nombre,lArchivoDir->extension,tipoArchivo,lArchivoDir->tamanio);
			lArchivoDir = lArchivoDir->siguiente;
		}
	}
	//ARCHIVOS

	//DIRECTORIOS
	bool tieneDirectoriosHijos = true;
	char tipoDirectorio[11] = {'D','i','r','e','c','t','o','r','i','o','\0'};
  	if(auxBusca->ph !=NULL){
	    nodoDirectorio* aux=auxBusca->ph;
	    while (aux->sh!=NULL){
	      printf("%s     %s     %d\n", aux->nombre,tipoDirectorio, aux->tamanio);
	      aux=aux->sh;
	    }
	    printf("%s     %s     %d\n", aux->nombre,tipoDirectorio, aux->tamanio); //ultima carpeta
	}else
		tieneDirectoriosHijos = false;
	//DIRECTORIOS

	if(!tieneDirectoriosHijos && !tieneArchivos){
		printf("No hay archivos ni directorios en este directorio\n");
		PWD();
	}
	Resultado = OK;
}

void DIR_S(directorio &auxBusca){
	if (auxBusca!=NULL){
	bool tieneContenido=false;//si el directorio tiene contendio, imprimo las llaves
	if (auxBusca->lArchivo!=NULL || auxBusca->ph!=NULL)
        tieneContenido=true;
		if (auxBusca!=sis->actual)
		 printf("Directorio %s\n", auxBusca->nombre);
			if (auxBusca!=sis->actual && tieneContenido)
				printf("<contenido: %s>\n", auxBusca->nombre);
			DIR_S(auxBusca->ph);
			Archivo aux=auxBusca->lArchivo;

			while (aux!=NULL){//imprimo los archivos de cada subdirectorio
				printf("Archivo %s.%s\n", aux->nombre, aux->extension);
				aux=aux->siguiente;
			}//end while

			if (auxBusca!=sis->actual && tieneContenido)
				printf("<contenido: %s/>\n", auxBusca->nombre);
		if (auxBusca!=sis->raiz)
			DIR_S(auxBusca->sh);
	}//end if
}

TipoRet CREATE(char nombre[NOM],char extension[EXT]){
	archivo* aux ;
	aux = existeArchivo(nombre,extension);
	if(aux != NULL){ //YA EXISTE EN ESTE DIRECTORIO UN ARCHIVO CON ESTE NOMBRE
		printf("El nombre del archivo ya existe en este directorio\n");
		Resultado = ERROR;
		return Resultado;
	}else{
		archivo* nuevo = new archivo;
		nuevo->tamanio=0;
		strcpy(nuevo->nombre,nombre);
		strcpy(nuevo->extension,extension);

		if(auxBusca->lArchivo == NULL){
			auxBusca->lArchivo = nuevo;
			nuevo->siguiente = NULL;
		}else{
			bool encontreLugar = false;
			bool antesDelPrimero = false;
			archivo* aux2 = auxBusca->lArchivo;;
			aux = auxBusca->lArchivo;	//insercion ordenada
			//COMPARACION CON EL PRIMERO
			if(strcmp(aux->nombre,nuevo->nombre)>0){ //nuevo->nombre va antes del nombre del primer elemento de la lista ??
				encontreLugar = true;
				antesDelPrimero = true;
			}else
			if(strcmp(aux->nombre,nuevo->nombre)==0)
				if(strcmp(aux->extension,nuevo->extension)>0){ // nuevo->extension va antes de la extension del primero ??
					encontreLugar = true;
					antesDelPrimero = true;
				}
			//COMPARACION CON EL RESTO
			while(!encontreLugar && aux->siguiente!=NULL){
				if(strcmp(aux->siguiente->nombre,nuevo->nombre)>0){ //nuevo->nombre va antes que aux->siguiente->nombre ?? (preciso estar parado uno antes del lugar encontrado)
					encontreLugar = true;
					aux2 = aux;
				}else
				if(strcmp(aux->siguiente->nombre,nuevo->nombre)==0)
					if(strcmp(aux->siguiente->extension,nuevo->extension)>0){ //nuevo->extension va antes que aux->siguiente->extension ??
					encontreLugar = true;
					aux2 = aux;
				}

				if(aux->siguiente->siguiente == NULL && !encontreLugar)
					aux2 = aux->siguiente;

				if(!encontreLugar)
				aux = aux->siguiente;
			}
			if(!encontreLugar){
				nuevo->siguiente = NULL;
				aux2->siguiente = nuevo;
			}else{
				if(!antesDelPrimero){
					nuevo->siguiente = aux2->siguiente;
					aux2->siguiente = nuevo;
				}else{
					nuevo->siguiente = aux2;
					auxBusca->lArchivo = nuevo;
				}
			}

		}

	}
	return Resultado = OK;
}

TipoRet UNDELETE(Archivo &lArchivo){
	if(ultimoBorrado==NULL){
		printf("No hay archivos borrados\n");
		Resultado = ERROR;
		return Resultado;
	}else{
		bool yaexiste = false;
		archivo* aux = lArchivo;
		while(!yaexiste && aux != NULL){
			if(strcmp(aux->nombre,ultimoBorrado->nombre)==0){
				if(strcmp(aux->extension,ultimoBorrado->extension)==0)
					yaexiste = true;
				else
					aux = aux->siguiente;
			}else
				aux = aux->siguiente;
		}

		if(yaexiste){
			printf("Ya existe un archivo con el mismo nombre del archivo a restaurar\n");
			archivo* auxUltimoBorrado = ultimoBorrado; //PARA BORRAR DESP
			ultimoBorrado = ultimoBorrado->siguiente;
			Resultado = ERROR;
			delete auxUltimoBorrado;
			return Resultado;
		}else{
			if(lArchivo == NULL){
				archivo* a_restaurar = ultimoBorrado;
				ultimoBorrado = a_restaurar->siguiente;
				a_restaurar->siguiente = lArchivo;
				lArchivo = a_restaurar;
			}else{
				bool encontreLugar = false;
				bool antesDelPrimero = false;
				archivo* aux2 = lArchivo;
				aux = lArchivo;	//insercion ordenada
				//COMPARACION CON EL PRIMERO
				if(strcmp(aux->nombre,ultimoBorrado->nombre)>0){ //nuevo->nombre va antes del nombre del primer elemento de la lista
					encontreLugar = true;
					antesDelPrimero = true;
				}else
				if(strcmp(aux->nombre,ultimoBorrado->nombre)==0)
					if(strcmp(aux->extension,ultimoBorrado->extension)>0){ // nuevo->extension va antes de la extension del primero
						encontreLugar = true;
						antesDelPrimero = true;
					}
				//COMPARACION CON EL RESTO
				while(!encontreLugar && aux->siguiente!=NULL){
					if(strcmp(aux->siguiente->nombre,ultimoBorrado->nombre)>0){ //nuevo->nombre va antes que aux->siguiente->nombre (preciso estar parado uno antes del lugar encontrado)
						encontreLugar = true;
						aux2 = aux;
					}else
					if(strcmp(aux->siguiente->nombre,ultimoBorrado->nombre)==0)
						if(strcmp(aux->siguiente->extension,ultimoBorrado->extension)>0){ //nuevo->extension va antes que aux->siguiente->extension
						encontreLugar = true;
						aux2 = aux;
					}

					if(aux->siguiente->siguiente == NULL && !encontreLugar)
						aux2 = aux->siguiente;

					if(!encontreLugar)
					aux = aux->siguiente;
				}
				if(!encontreLugar){ //VA A LO ULTIMO DE LA LISTA
					archivo* a_restaurar = ultimoBorrado;
					ultimoBorrado = a_restaurar->siguiente;

					aux2->siguiente = a_restaurar;
					aux2->siguiente->siguiente = NULL;
				}else{
					if(!antesDelPrimero){
						archivo* a_restaurar = ultimoBorrado;
						ultimoBorrado = a_restaurar->siguiente;

						a_restaurar->siguiente = aux2->siguiente;
						aux2->siguiente = a_restaurar;
					}else{
						archivo* a_restaurar = ultimoBorrado;
						ultimoBorrado = a_restaurar->siguiente;
						a_restaurar->siguiente = lArchivo;
						lArchivo = a_restaurar;
					}
				}

			}

		}

	}
}

TipoRet DELETE(char nombre[NOM], char extension[EXT]){
	if(auxBusca->lArchivo == NULL){
		printf("Error, no existen archivos en este directorio\n");
		Resultado = ERROR;
		return Resultado;
	}
	bool esPrimero = false;
	//COMPARACION CON EL PRIMERO
	if(strcmp(auxBusca->lArchivo->nombre, nombre)==0){
		if(strcmp(auxBusca->lArchivo->extension, extension)==0){
				esPrimero = true;
				archivo* nuevoBorrado = auxBusca->lArchivo;
				auxBusca->lArchivo=auxBusca->lArchivo->siguiente;
				if(ultimoBorrado==NULL){
					nuevoBorrado->siguiente = ultimoBorrado;
					ultimoBorrado = nuevoBorrado;
					Resultado = OK;
					return Resultado;
				}else{
					nuevoBorrado->siguiente = ultimoBorrado;
					ultimoBorrado = nuevoBorrado;;
					Resultado = OK;
					return Resultado;
				}
		}
	}

	if(!esPrimero){
		//COMPARACION CON EL RESTO
		bool yaBorre=false;
		archivo* aux=auxBusca->lArchivo;
		while(!yaBorre && aux->siguiente!=NULL){
			if(strcmp(aux->siguiente->nombre, nombre)==0){
				if(strcmp(aux->siguiente->extension, extension)==0){
					archivo* nuevoBorrado = aux->siguiente;
					aux->siguiente=nuevoBorrado->siguiente;
					if(ultimoBorrado==NULL){
						nuevoBorrado->siguiente = ultimoBorrado;
						ultimoBorrado = nuevoBorrado;
						Resultado = OK;
						return Resultado;
					}else{
						nuevoBorrado->siguiente = ultimoBorrado;
						ultimoBorrado = nuevoBorrado;
						Resultado = OK;
						return Resultado;
					}
					yaBorre = true;
				}
				else
					aux=aux->siguiente;
			}
			else
				aux=aux->siguiente;

		}
		if(aux->siguiente==NULL){
			printf("No existe el archivo con ese nombre en el directorio actual\n");
			Resultado = ERROR;
			return Resultado;
		}
	}
}
void limpiarComandoArchivosExtensiones(){
	for(int i=0;i<15;i++){
		//INICIALIZAR AUX
		comando[i] = '\0';
		nombreNodoDirectorio[i] = '\0';
		archivo1[i] = '\0';
		archivo2[i] = '\0';
		nombreDirACrear[i] = '\0';
	}

	for(int i=0;i<3;i++){
		extension1[i] = '\0';
		extension2[i] = '\0';
	}

	for(int i=0;i<100;i++){
		textoAux[i] = '\0';
	}

	for(int i=0;i<100;i++){
		direc[i] = '\0';
	}

	existeDirectorio = false;
}

TipoRet MKDIR(){
		directorio nuevo = new nodoDirectorio;
		strcpy(nuevo->nombre,nombreDirACrear);
		nuevo->ph = NULL;
		nuevo->sh = NULL;
		nuevo->tamanio=0;

		if(auxBusca->ph == NULL){
			auxBusca->ph = nuevo;
		}else{
			directorio aux = auxBusca->ph;
			bool existe=false;
			while(aux->sh!=NULL && existe==false){//hasta llegar al NULL
				if (strcmp(aux->nombre, nombreDirACrear)==0)
					existe=true;
				else aux=aux->sh;
			}//endwhile

			if(strcmp(aux->nombre, nombreDirACrear)==0)
				existe=true;//si a la primera es igual, no lo inserto

				if(existe==true){
					Resultado = ERROR;
					printf("Este directorio ya existe\n");
					return Resultado;
					delete nuevo;//como no lo creo, borro el nuevo que iba a insertar
				}else{
					bool encontreLugar = false;
					bool antesDelPrimero = false;
					directorio aux2 = auxBusca->ph;
					aux = auxBusca->ph;	//insercion ordenada
					//COMPARACION CON EL PRIMERO
					if(strcmp(aux->nombre,nuevo->nombre)>0){ //nuevo->nombre va antes del nombre del primer elemento de la lista
						encontreLugar = true;
						antesDelPrimero = true;
					}
					//COMPARACION CON EL RESTO
					while(!encontreLugar && aux->sh!=NULL){
						if(strcmp(aux->sh->nombre,nuevo->nombre)>0){ //nuevo->nombre va antes que aux->siguiente->nombre  (preciso estar parado uno antes del lugar encontrado)
							encontreLugar = true;
							aux2 = aux;
						}
						if(aux->sh->sh == NULL && !encontreLugar)
							aux2 = aux->sh;

						if(!encontreLugar)
						aux = aux->sh;
					}
					if(!encontreLugar){
						nuevo->sh = NULL;
						aux2->sh = nuevo;
					}else{
						if(!antesDelPrimero){
							nuevo->sh = aux2->sh;
							aux2->sh = nuevo;
						}else{
							nuevo->sh = aux2;
							auxBusca->ph = nuevo;
						}
					}
					Resultado = OK;
					return Resultado;
				}
			}
}

void insertarFinal(listaRuta &rutaAux, char nombre[NOM]){
	nodoRuta* nuevo=new nodoRuta;
	strcpy(nuevo->nombreDir, nombre);

	if (rutaAux == NULL){
		rutaAux = nuevo;
		nuevo->sig = NULL;
		nuevo->ant = NULL;
	} else {
			nodoRuta* aux = rutaAux;
			while (aux->sig!=NULL)
				aux = aux->sig;
			aux->sig = nuevo;
			nuevo->ant = aux;
			nuevo->sig = NULL;
			//rutaAux = nuevo;
	}//endelse
}

void CDRutaRelativa(char nombreMeMuevo[15]){//estando en un directorio, me muevo a un subdirectorio
	directorio aux=sis->actual->ph;
	if (aux==NULL){
		printf("La carpeta %s no contiene directorio\n",sis->actual->nombre);
	}else{
		bool existe = false;
		while(aux->sh!=NULL && !existe){
			if (strcmp(aux->nombre, nombreMeMuevo)==0){
				sis->actual=aux;
				existe = true;
			} else aux=aux->sh;
		}//endwhile
		if (strcmp(aux->nombre, nombreMeMuevo)==0){
			sis->actual=aux;//EVALUA AL ULTIMO HERMANO
			existe = true;
		}//end if
		if (existe == true){
			insertarFinal(sis->rutaActual, nombreMeMuevo);
			auxBusca=sis->actual;
		}
		//else printf("ERROR El directorio destino no existe\n");
	}//end if aux==NULL
}

bool obtenerCamino(directorio arbolDir, char nombre[NOM], listaRuta &rutaAux){//delNodo corresponde al nodo del cual quiero obterner el camino
    if (arbolDir==NULL)
      return false;
    if (strcmp(arbolDir->nombre, nombre)==0){
      insertarFinal(rutaAux, arbolDir->nombre);
      return true;
    }
    if (obtenerCamino(arbolDir->sh, nombre, rutaAux))
      return true;
    if (obtenerCamino(arbolDir->ph, nombre, rutaAux)){
      insertarFinal(rutaAux, arbolDir->nombre);
      return true;
    }
    return false;
}//esta me sirve para obtener el camino de cualquier carpeta, teniendo en cuenta su id (todavia no implementado)

TipoRet PWD(){
	listaRuta aux = sis->rutaActual;
	while(aux->sig!=NULL){
		if(strcmp(aux->nombreDir,"/")==0)
			printf("%s", aux->nombreDir);
		else
			printf("%s/",aux->nombreDir);
		aux=aux->sig;
	}
	printf("%s\n", aux->nombreDir);
}

void RMDIR(directorio &auxBusca){//destruyo todo lo que hay adentro
	if (auxBusca!=NULL){
		RMDIR(auxBusca->ph);
		RMDIR(auxBusca->sh);
		Archivo a_borrar;//recorro la lista de archivos para irlos borrarndo
		texto *a_borrarlTexto;
		while (auxBusca->lArchivo!=NULL){
			a_borrar = auxBusca->lArchivo;
			auxBusca->lArchivo = auxBusca->lArchivo->siguiente;
			while(a_borrar->lTexto != NULL){
				a_borrarlTexto = a_borrar->lTexto;
				a_borrar->lTexto = a_borrar->lTexto->siguiente;
			}
			delete a_borrar;
		}
		delete auxBusca;
		auxBusca=NULL;
	}
}

directorio buscoSoloEnHijos(directorio padre,char nombreHijo[15]){
	if(padre == NULL){
		return NULL;
	}
	if(strcmp(nombreHijo,"/")==0){
		return padre;
	}
	if (padre->ph!=NULL){
		if(strcmp(padre->ph->nombre,nombreHijo)==0){
			return padre->ph;
		}else{
			directorio auxBro = padre->ph->sh;
			while(auxBro != NULL){
				if(strcmp(auxBro->nombre,nombreHijo)==0){
					return auxBro;
				}else{
					auxBro = auxBro->sh;
				}
			}
		return auxBro; //NULL
		}
	}else{
		return NULL;
	}
}

void switchComandos(){
	switch(auxSwitch){
		case 1:
			//Comando DIR
			Resultado = DIR();
			lecturaEntrada();
			break;
		case 2:
			//Comando CREATE
				if(archivo1[0] == '\0'){
					printf("No se ingreso archivo a crear\n");
					lecturaEntrada();
				}else{
					Resultado = CREATE(archivo1,extension1);
					lecturaEntrada();
				}
			break;

		case 3:
			//Comando DELETE
			if(archivo1[0] == '\0'){
				printf("No se ingreso archivo1\n");
				lecturaEntrada();
			}else{
				Resultado = DELETE(archivo1, extension1);
				lecturaEntrada();
			}
			break;

		case 4:
			//Comando UNDELETE
			Resultado = UNDELETE(lArchivo);
			lecturaEntrada();
			break;

		case 5:
			//Comando IC
				if(archivo1[0]== '\0' || extension1[0]=='\0'){
				printf("No existe un archivo con el nombre indicado en este directorio\n");
				Resultado = ERROR;
				lecturaEntrada();
			}else{
				if(archivo2[0]!='\0'){
				Resultado = ERROR;
				printf("El texto a ingresar debe estar entre comillas\n");
				lecturaEntrada();
				}else{
					if(strlen(textoAux)>MAX_LINEAS){
						Resultado = ERROR;
						printf("No se pudo insertar el texto, el mismo supera el largo permitido\n");
						lecturaEntrada();
					}else{
						Resultado = IC(archivo1, extension1, textoAux);
						lecturaEntrada();
					}
				}
			}
			break;

		case 6:
			//Comando IF
			if(archivo1[0]== '\0' || extension1[0]=='\0'){
				printf("No existe un archivo con el nombre indicado en este directorio\n");
				Resultado = ERROR;
				lecturaEntrada();
			}else{
				if(archivo2[0]!='\0'){
				Resultado = ERROR;
				printf("El texto a ingresar debe estar entre comillas\n");
				lecturaEntrada();
				}else{
					if(strlen(textoAux)>MAX_LINEAS){
						Resultado = ERROR;
						printf("No se pudo insertar el texto, el mismo supera el largo permitido\n");
						lecturaEntrada();
					}else{
						Resultado = IF(lArchivo, archivo1, extension1, textoAux);
						lecturaEntrada();
						}
				}
			}
			break;

		case 7:
			//Comando BC
			if(archivo1[0]== '\0'){
				printf("No se ingreso archivo\n");
				lecturaEntrada();
			}
			else{
				Resultado = BC(lArchivo, archivo1, extension1, k);
				lecturaEntrada();
			}
			break;

		case 8:
			//Comando BF
			if(archivo1[0]== '\0'){
				printf("No se ingreso archivo\n");
				lecturaEntrada();
			}
			else{
				Resultado = BF(lArchivo, archivo1, extension1, k);
				lecturaEntrada();
			}
			break;

		case 9:
			//Comando CAT
			if(archivo1[0] == '\0' || archivo2[0] == '\0' ){
					printf("No se ingresaron archivos\n");
					lecturaEntrada();
				}else{
					Resultado = CAT(archivo1,extension1,archivo2,extension2);
					lecturaEntrada();
				}
			break;

		case 10:
			//Comando TYPE
			if(archivo1[0]== '\0'){
				printf("No se ingreso archivo1\n");
				lecturaEntrada();
			}
			else{
				Resultado = TYPE(archivo1, extension1);
				lecturaEntrada();
			}
			break;

		case 11:{
				//Comando CD
				listaRuta rutaIngresada;

				if (archivo1[0]!='/'){//Ruta relativa\n
					if (sis->actual->ph!=NULL)
						CDRutaRelativa(archivo1);
					else
						printf("ERROR No existe el subdirectorio destino\n");
				} else {
					rutaIngresada=rutaVerificaCamino->sig;//el segundo nodo
					if (strcmp(archivo1, "/")==0 && rutaVerificaCamino->sig==NULL){
						listaRuta aux=sis->rutaActual->sig, aux2;
						if (sis->actual!=sis->raiz){
							sis->actual=sis->raiz;
							while (aux->sig!=NULL){
								aux2=aux;
								aux=aux->sig;
								delete aux2;
							}
							delete aux;
							sis->rutaActual->sig=NULL;
						}
					} else {
						if (rutaCorrecta){
							listaRuta nuevaRuta=NULL, recorroRuta=rutaVerificaCamino->sig;
							nodoRuta *inserto=new nodoRuta, *anterior;//corresponde al ultimo valor ingresado, sirve para engancharlo con insertar
							strcpy(inserto->nombreDir, recorroRuta->nombreDir);
							inserto->sig=NULL;
							nuevaRuta=inserto;
							if (sis->actual!=sis->raiz){
								listaRuta aux=sis->rutaActual->sig, aux2;//con esto voy recorriendo la rutaActual para vaciarla
							 //si el segundo de la lista ==NULL, al copiar elementos, no hago el while
								if (aux!=NULL){//en este caso tengo que ir vaciando la rutaActual
									while (aux->sig!=NULL){
										aux2=aux;
										aux=aux->sig;
										delete aux2;
									}
									delete aux;
									sis->rutaActual->sig=NULL;
									recorroRuta=recorroRuta->sig;//avanzo al tercer elemento
								}//en caso contrario, no es necesario borrar
								anterior=inserto;
								if (recorroRuta!=NULL){
									while(recorroRuta->sig!=NULL){
										inserto=new nodoRuta;
										strcpy(inserto->nombreDir, recorroRuta->nombreDir);
										inserto->ant=anterior;
										anterior->sig=inserto;
										recorroRuta=recorroRuta->sig;
										anterior=inserto;
									}
									//creo e inserto el ultimo nodo
									inserto=new nodoRuta;
									strcpy(inserto->nombreDir, recorroRuta->nombreDir);
									inserto->sig=NULL;
									inserto->ant=anterior;
									anterior->sig=inserto;
								}
							} else {
								recorroRuta=recorroRuta->sig;
								anterior=inserto;
								if (recorroRuta!=NULL){//si no me muevo a una sola carpeta
									while(recorroRuta->sig!=NULL){
										inserto=new nodoRuta;
										strcpy(inserto->nombreDir, recorroRuta->nombreDir);
										inserto->ant=anterior;
										anterior->sig=inserto;
										recorroRuta=recorroRuta->sig;
										anterior=inserto;
									}
									//ultimo nodo que inserto
									inserto=new nodoRuta;
									strcpy(inserto->nombreDir, recorroRuta->nombreDir);
									inserto->sig=NULL;
									inserto->ant=anterior;
									anterior->sig=inserto;
									//ultimo nodo que inserto
								}//end if
							}//end else
							sis->rutaActual->sig=nuevaRuta;//engancho la rutaNueva a rutaActual
							sis->actual=auxBusca;//actualizo la posicion en el sistema
						}
					}
				}
				lecturaEntrada();
		}
		break;

		case 12: // MKDIR
			if(nombreDirACrear[0] == '/'){
				Resultado = ERROR;
				printf("ERROR No se puede crear el directorio con nombre '/'\n");
			}else{
				if(nombreDirACrear[0] == '\0' || nombreDirACrear[0] == '\n'){
					printf("Debe ingresar el nombre del directorio a crear\n");
					lecturaEntrada();
				}else
				MKDIR();
			}
			lecturaEntrada();
		break;

		case 13:{
			//Comando RMDIR
			if (strcmp(archivo1, "/")!=0){//CASO DE RUTA RELATIVA
				nodoDirectorio *aux=sis->actual->ph, *auxAnt;
				if (aux!=NULL){
					if (strcmp(aux->nombre, archivo1)==0){//lo hace bien
						sis->actual->ph=aux->sh;
						aux->sh=NULL;
						RMDIR(aux);
					} else {//si hay mas de un hijo
						auxAnt=aux;
						aux=aux->sh;//me muevo al segundo hijo
						bool existe=false;

						if (aux!=NULL){
							while (aux->sh!=NULL && !existe){
								if (strcmp(aux->nombre, archivo1)==0)
									existe=true;

								else {
									auxAnt=aux;
									aux=aux->sh;
								}
							}//end while
							if (strcmp(aux->nombre, archivo1)==0)//pregunto en caso de que sea el ultimo
								existe=true;
						}
						if (existe){
							auxAnt->sh=aux->sh;
                            aux->sh=NULL;
							RMDIR(aux);
						}else printf("ERROR No existe el subdirectorio a eliminar\n");
					}//end if
				}else
                    printf("ERROR No existe el subdirectorio a eliminar\n");
			} else {
					if (sis->actual!=auxBusca){
						if (sis->raiz->ph==NULL)
							printf("ERROR No existe el subdirectorio a eliminar\n");
						else {
							if (rutaCorrecta){
								nodoDirectorio* anteriorAlBorrar=sis->raiz;//si su hijo o siguiente hermano es auxBusca
								listaRuta anteriorEnLista=rutaVerificaCamino;
								listaRuta auxRutaActual=sis->rutaActual;
								while(anteriorEnLista->sig->sig!= NULL && anteriorAlBorrar != NULL){ //avanzo hasta el penultimo
									anteriorEnLista = anteriorEnLista->sig;
									anteriorAlBorrar = buscoSoloEnHijos(anteriorAlBorrar,anteriorEnLista->nombreDir);
								}
								while (auxRutaActual->sig!=NULL)//avanzo hasta el ultimo, es el nodo que corresponde al actual
									auxRutaActual=auxRutaActual->sig;
								bool borro=true;

								if (sis->actual!=sis->raiz && strcmp(auxBusca->nombre, auxRutaActual->ant->nombreDir)==0)//si no estoy en la raiz, me fijo si es ancestral a la actual
									borro=false;//es ancestral, no puedo
								if (borro==true){//si la que quiero borrar no es ancestral
									//si el nombre del directorio anterior no es igual al del directorio actual
									if (anteriorAlBorrar->ph==auxBusca)
										anteriorAlBorrar->ph=auxBusca->sh;
									else {
										anteriorAlBorrar=anteriorAlBorrar->ph;
										while (anteriorAlBorrar->sh!=auxBusca)
											anteriorAlBorrar=anteriorAlBorrar->sh;
										anteriorAlBorrar->sh=auxBusca->sh;
										auxBusca->sh=NULL;
									}
									RMDIR(auxBusca);
								}
							}else printf("ERROR sdsd No existe el subdirectorio a eliminar\n");
						}//endelse (caso de ruta absoluta)
					}
			}//end if
			lecturaEntrada();
		}//end case
		break;

		case 14:
			//"Comando PWD
			PWD();
			lecturaEntrada();
		break;

		//OPCIONALES
		case 15:
			printf("Comando COPY\n");
			Resultado = NO_IMPLEMENTADA;
			printf("NO_IMPLEMENTADA\n");
			lecturaEntrada();
		break;

		case 16:
			//Comando DIR S
			printf("Comando DIR S\n");
			DIR_S(sis->actual);
			lecturaEntrada();
		break;
		//OPCIONALES

		default:
			printf("Comando invalido, ingrese nuevamente\n");
			lecturaEntrada();
			break;
	}

}

void evaluaEntrada(){
		auxSwitch = 0;
		if(strcmp(comando,"DIR")==0)
			auxSwitch = 1;
		if(strcmp(comando,"CREATE")==0)
			auxSwitch = 2;
		if(strcmp(comando,"DELETE")==0)
			auxSwitch = 3;
		if(strcmp(comando,"UNDELETE")==0)
			auxSwitch = 4;
		if(strcmp(comando,"IC")==0)
			auxSwitch = 5;
		if(strcmp(comando,"IF")==0)
			auxSwitch = 6;
		if(strcmp(comando,"BC")==0)
			auxSwitch = 7;
		if(strcmp(comando,"BF")==0)
			auxSwitch = 8;
		if(strcmp(comando,"CAT")==0)
			auxSwitch = 9;
		if(strcmp(comando,"TYPE")==0)
			auxSwitch = 10;
		if(strcmp(comando,"CD")==0)
			auxSwitch = 11;
		if(strcmp(comando,"MKDIR")==0)
			auxSwitch = 12;
		if(strcmp(comando,"RMDIR")==0)
			auxSwitch = 13;
		if(strcmp(comando,"PWD")==0)
			auxSwitch = 14;
		if(strcmp(comando,"COPY")==0)
			auxSwitch = 15;
		if(strcmp(comando,"DIR S")==0)
			auxSwitch = 16;

		switchComandos();
}

void resetDirec(){
	for(int s=0 ; s<15 ; s++){
		direc[s] = '\0';
	}
}

void borroLista(listaRuta &rutaVerificaCamino){
	if(rutaVerificaCamino == NULL)
		return;
	else{
		nodoRuta* borro=rutaVerificaCamino;
		rutaVerificaCamino=rutaVerificaCamino->sig;
		delete borro;
		borroLista(rutaVerificaCamino);
	}
}

void evaluaComando(){
		if(strcmp(comando,"CREATE")==0)
			comandoTrabajaConArchivo = true;
		if(strcmp(comando,"DELETE")==0)
			comandoTrabajaConArchivo = true;
		if(strcmp(comando,"IC")==0)
			comandoTrabajaConArchivo = true;
		if(strcmp(comando,"IF")==0)
			comandoTrabajaConArchivo = true;
		if(strcmp(comando,"BC")==0)
			comandoTrabajaConArchivo = true;
		if(strcmp(comando,"BF")==0)
			comandoTrabajaConArchivo = true;
		if(strcmp(comando,"CAT")==0)
			comandoTrabajaConArchivo = true;
		if(strcmp(comando,"TYPE")==0)
			comandoTrabajaConArchivo = true;
		if(strcmp(comando,"MKDIR")==0)
			comandoCreador = true;
}

//------------LECTURA ENTRADA-----------------/

void lecturaEntrada(){
	auxBusca = sis->actual;
	rutaCorrecta = true;//sirve para ver si la ruta ingresada, existe. si encuentro un nodo que no exista, da false
	comandoTrabajaConArchivo = false;
 	comandoCreador = false;
	bool rutaAbs = false;
	bool rutaRel = false;
	bool rutaDirAct = false;
	bool esArchivo = false;
	bool esDir = false;
	borroLista(rutaVerificaCamino);
	limpiarComandoArchivosExtensiones();
	bool esDirectorio = false;
	char aux = '\0';
	//COMANDO
	int i=-1;
	while(aux!=' '&& aux!='\n'){
		i++;
		aux = getchar();
		if(aux!=' '&& aux!='\n')
			comando[i]=aux;
	}
	comando[i] = '\0';
	//COMANDO
	bool esDIR_S=false; //si es true, llamo a DIR_S
	//ARCHIVO 1
	if(aux==' '){
		aux=getchar();
		i=0;
		if(aux == '/')
			rutaAbs = true;
		else
			if (aux == 'S' && strcmp(comando,"DIR")==0){
				esDIR_S=true;
				strcpy(comando,"DIR S");//actualizo el comando
			}

		archivo1[i] = aux;

		if(!rutaAbs){ //LEI ALGO DIFERENTE A UNA /
			if(aux!='.' && aux!='\n' && aux!='/')
				archivo1[i]=aux;
			while(aux!='.' && aux!='\n' && aux!='/'){
				i++;
				aux = getchar();
				if(aux!='.' && aux!='\n' && aux!='/')
					archivo1[i]=aux;
				if(aux == '.' || aux == '\n')
					archivo1[i] ='\0';
			}
			//EVALUO PORQUE SALI DEL WHILE
			if(aux == '.'){ //ESTOY LEYENDO UN ARCHIVO Y ME FALTA LEER LA EXTENSION, TRABAJO SOBRE EL DIRECTORIO ACTUAL
				rutaDirAct = true;
				esArchivo = true;
			}
			if(aux == '\n'){ // LEI UN DIRECTORIO Y TRABAJO SOBRE EL DIR ACTUAL
				rutaDirAct = true;
				esDir = true;
			}
			if(aux == '/'){ // LEI UN DIRECTORIO Y VOY A SEGUIR LEYENDO OTRO DIR O ARCHIVO PARA FORMAR UNA RUTA RELATIVA
				rutaRel = true;
			}
		}
	}
	//ARCHIVO 1 o DIR S
	if (esDIR_S==false){
		if(esArchivo && rutaDirAct){
			//EXTENSION 1
			i=-1;
			while(aux != '\n' && aux != ' '){
				i++;
				aux = getchar();
				if(aux != '\n' && aux != ' ')
					extension1[i]=aux;
				else
					extension1[i] = '\0';
			}
			//EXTENSION 1

			//ARCHIVO 2 O TEXTO O ENTERO K
			if(aux==' '){
				aux = getchar();
				if(aux == '"'){ // TEXTO
					aux = getchar(); //ASUMO QUE NO SE INGRESA UNA COMILLA ENSEGUIDA DE LA OTRA ("")
					i=0;
					textoAux[i] = aux;
					while(aux!='"'){
						i++;
						aux = getchar();
						if(aux!='"')
						textoAux[i] = aux;
						else
						textoAux[i] = '\0';
					}
					getchar();
				}else{ // ARCHIVO2 O ENTERO K
					if(isdigit(aux)==1){  //Si lo q ingresa es un numero, entra al if
						char num[10];
						i=0;
						while(aux!='\n'){
							num[i]=aux;
							i++;
							aux = getchar();
						}
						k=atoi(num); //Convierte los numeros dentro del arreglo de caracteres num a enteros
					}else{ //ARCHIVO 2
						i=0;
						archivo2[i] = aux; //para que entre al while
						while(aux!='.' && aux!='\n' ){
							i++;
							aux = getchar();
							if(aux!='.' && aux!='\n')
							archivo2[i]=aux;
						}
						archivo2[i] = '\0';

						// ARCHIVO 2

						//EXTENSION 2
						i=-1;
						while(aux != '\n'){
							i++;
							aux = getchar();
							if(aux != '\n')
							extension2[i]=aux;
						}
						extension2[i] = '\0';
						//EXTENSION 2
					}//end else ARCHIVO2
				}//end else ARCHIVO2 o ENTEROK
			}//end if aux==' '
		}

		if(esDir && rutaDirAct)
			strcpy(nombreDirACrear,archivo1);

		if(rutaRel || rutaAbs){
			strcpy(direc,archivo1);
			insertarFinal(rutaVerificaCamino,direc);
			resetDirec();

			i=0;
			aux=getchar();
			if(aux != '\n' && aux !='/')
				direc[i]=aux;
			while(aux!='\n'){
				aux = getchar();
				if(aux != '\n' && aux !='/'){
					i++;
					direc[i]=aux;
				}
				if(aux == '\n'|| aux == '/'){
					i++;
					direc[i] = '\0';
					insertarFinal(rutaVerificaCamino,direc);
					resetDirec();
					i=-1;
				}
		 	}

			if(rutaAbs) // EMPIEZO A RECORRER EL ARBOL DESDE LA RAIZ
				auxBusca = sis->raiz;
			else // RECORRO EL ARBOL DESDE SISTEMA->ACTUAL
				auxBusca = sis->actual;

			evaluaComando();
			if(comandoTrabajaConArchivo || comandoCreador){ // FUNCIONA PARA CREATE-TYPE-DELETE // MKDIR
				listaRuta auxVerificoExistencia = rutaVerificaCamino;
				while(auxVerificoExistencia->sig != NULL && auxBusca != NULL){ // VERIFICO HASTA EL PENULTIMO DIRECTORIO (EL ULTIMO SE PRETENDE CREAR ARCHIVO/DIR)
					auxBusca = buscoSoloEnHijos(auxBusca,auxVerificoExistencia->nombreDir);
					auxVerificoExistencia = auxVerificoExistencia->sig;
				}
				//EN CASO DE RUTA CORRECTA, auxBusca (GLOBAL) SALE APUNTANDO AL DIRECTORIO DONDE SE PRETENDE TRABAJAR
				if(auxBusca!= NULL){
					// VERIFICO SI LO ULTIMO ES UN DIR O ARCHIVO
					esArchivo = false;
					int u = 0;
					int p = 0;
					bool encontrePunto = false;
					char aux = auxVerificoExistencia->nombreDir[p];
					while(aux != '\0'){
						if(aux != '.' && !encontrePunto){
							nombreDirACrear[p] = aux;
							p++;
						}
						if(encontrePunto){
							extension1[u] = aux;
							u++;
							p++;
						}
						if(aux == '.'){
							encontrePunto = true;
							strcpy(archivo1,nombreDirACrear);
							p++;
						}
					aux = auxVerificoExistencia->nombreDir[p];
					}
				}else{
					//printf("ERROR El directorio destino no existe\n");
					lecturaEntrada();
				}
			}else{ // CD - RMDIR
				listaRuta auxVerificoExistencia = rutaVerificaCamino;
				while(auxVerificoExistencia != NULL && auxBusca != NULL){ // VERIFICO LA EXISTENCIA DE TODA LA RUTA
					auxBusca = buscoSoloEnHijos(auxBusca,auxVerificoExistencia->nombreDir);
					auxVerificoExistencia = auxVerificoExistencia->sig;
				}

				if(auxBusca == NULL){
                    if(strcmp(comando,"CD")==0){
                        rutaCorrecta=false;
                        printf("ERROR El directorio destino no existe\n");
                        lecturaEntrada();
                    }else{
                        rutaCorrecta=false;
                        printf("ERROR no existe el subdirectorio a eliminar\n");
                        lecturaEntrada();
                    }

				}
			}
		}
	}//end if es DIR_S==false
	evaluaEntrada();
}

//--------------------------------LECTURA ENTRADA-------------------------

void inicializarSistema(){
	directorio nuevo = new nodoDirectorio;
	strcpy(nuevo->nombre,"/");
	nuevo->ph = NULL;
	nuevo->sh = NULL;
	sis->raiz = nuevo;
	sis->actual=sis->raiz;

	nodoRuta* root = new nodoRuta;
	strcpy(root->nombreDir, "/");
	root->sig=root->ant=NULL;
	sis->rutaActual = root;

	//cuando inicializo el sistema, el actual va a ser la raiz
}

int main() {
	inicializarSistema();
	lecturaEntrada();
}