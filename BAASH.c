//     -baash-

//--------------------------------      
//- TP LABORATORIO n°2 - 
//Alumnos:
//			-Morales Esteban
//			-Zapata Illak
//--------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>



#define BRIGHT 0
#define GREEN 32
#define BG_BLACK 0

#define STDOUT_FID 1



typedef struct{
	char** comando_padre;
	char** comando_hijo;
}par_ejecucion;
char* obtener_ruta();
char** obtener_arreglo_de_rutas();
int tamanio(char**);
char* concatenar(char*,char*);
char** parseo_comandos(char*);
char* obtener_ruta_comando(char**, char**);
void ejecutar(char*,char**);
char* leer_linea_terminal();
void imprimir_arreglo_de_cadenas(char**);
char** obtener_arreglo_de_argumentos(const char*);
void rutina_ejecucion();
void cambiar_directorio(char**);
void ejecutar_detras(char**);
void ejecutar_estandar(char**);
char* cadena_final(char**);
char caracter_final(char*);
char** modificar_argumentosxejecucion_detras(char**);
char** tipo_redireccion(char**);
char** modificar_argumentosxredireccion(char**, char**,char**);
char* detectar_redireccion(char**);
int contar_caracter_enArreglo_deCadenas(char, char**);
int tamanio_enCaracteres(char**);
char** copia_arreglo_deCadenas(char**);
int error_en_redireccion(char** argumentos);
par_ejecucion obtener_comandos_ejecucionxtuberias(char*);
void ejecutar_tuberias(par_ejecucion);
void imprimir_spot();
void imprimir_ayuda();

int main(int argc, char **argv)
{
	struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    while(w.ws_col<96.0){
		printf("%c[%d;%dm\nEl ancho de la ventana de la terminal (%dcol) donde se esta ejecutando BAASH no es el adecuado (96col mínimo) para una ejecucion óptima ajustelo y presione <ENTER>.\n %c[%dm", 0x1B,1,31,w.ws_col,0x1B,0);
		leer_linea_terminal();
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	}

	imprimir_spot();
	rutina_ejecucion();
	system("clear");
	 return 0;
}

char* obtener_ruta(){
	
	char cont[500];	
	char* string;
	string=getenv("PATH");
	strcpy(cont,string);
	return strdup(cont);
}
char** obtener_arreglo_de_rutas(){
	char** rutas;
	char* rutas_sistema ;
	rutas_sistema = obtener_ruta();
	char* una_ruta;
	una_ruta=malloc(sizeof(char)*strlen(rutas_sistema)); 
	rutas=malloc(sizeof(char)*strlen(rutas_sistema));
	
	int i = 0;
	int tamanio = 0;
	int c=0;
	
	for(i=0; i<strlen(rutas_sistema);i++){
		una_ruta[c] = rutas_sistema[i];//copia cada caracter de las rutas grabadas en rutas_sistema.
		c++;//me permite agregar el '/0' al final de cada ruta.
		if((rutas_sistema[i+1] == ':')||(i==strlen(rutas_sistema)-1))
		{
			i++;
			una_ruta[c]='\0';
	
			char* copia=malloc(sizeof(char)*strlen(una_ruta));//se define un puntero a caracter copia con una asignacion de memoria suficiente.
			strcpy(copia,una_ruta);//se copia la ruta grabada en una_ruta.
			*rutas=copia;//se establece copia como valor para el puntero a caracter *rutas
			
			rutas++;//se avanza sobre el arreglo de rutas.
			c=0;//necesario para iniciar la grabacion de una nueva ruta en una_ruta desde la posicion 0.
			tamanio++;//cuenta la cantidad de rutas (elementos en el arreglo).
		}
	}
	rutas=rutas-tamanio;
	//free(una_ruta);
	return rutas;
}
int tamanio(char* vector[]){
	int k=0;
	char** aux=vector;
	while(*aux!=NULL){
		 k++;
		 aux++;
		 }
		return k;
}
char** obtener_arreglo_de_argumentos(const char* linea){
	char** argumentos;
	char* linea_terminal=malloc(sizeof(char)*1024);
	strcpy(linea_terminal, linea);
	char* un_argumento;
	un_argumento=malloc(sizeof(char)*strlen(linea_terminal)); 
	argumentos=(char**)malloc(sizeof(char)*strlen(linea_terminal));
	
	int c=0, tamanio = 0;
	int bandera = 0;
	while(*linea_terminal!='\0'){
				
		
		while((*linea_terminal!=' ')&&(*linea_terminal!='\t')&&(*linea_terminal!='\n')&&(*linea_terminal!='\0')){
			un_argumento[c]=*linea_terminal;
			c++;
			linea_terminal++;
			bandera = 1;
		}
		if (bandera==1){
		un_argumento[c]='\0';	
		char* copia=malloc(sizeof(char)*strlen(un_argumento));//se define un puntero a caracter copia con una asignacion de memoria suficiente.
		strcpy(copia,un_argumento);//se copia la ruta grabada en una_ruta.
		*argumentos=copia;//se establece copia como valor para el puntero a caracter *rutas.
		argumentos++;//se avanza sobre el arreglo de rutas.
		c=0;//necesario para iniciar la grabacion de una nueva ruta en una_ruta desde la posicion 0.
		tamanio++;//cuenta la cantidad de rutas (elementos en el arreglo).
		
		}
		
		while((*linea_terminal==' ')||(*linea_terminal=='\t')||(*linea_terminal=='\n')){
			linea_terminal++;
			bandera = 0;
		}
		
		
		
	}
	
	*argumentos=NULL;
	
	argumentos=argumentos - tamanio;
	return argumentos;
}
char* obtener_ruta_comando(char** rutas, char** argumentos){
	char** rut = rutas;
	char** args = argumentos;
	char* semiruta=concatenar("/",*args);
	char* copia=malloc(sizeof(char)*1024);
	copia="NADA";
	copia=concatenar(getcwd(NULL,0),semiruta);
	FILE* archivo=fopen(copia,"r");
	if(archivo){
		return copia;
	}
	while(*rut!=NULL){
		copia=concatenar(*rut,semiruta);
		rut++;
		archivo=fopen(copia,"r");
		if(archivo)
		return copia;
		else copia="NADA";
		}
	return copia;
	
}
char* concatenar(char* str1,char* str2){
	char copia[strlen(str1)];
	int j=0;
	for(j=0;j<strlen(str1)+1;j++)
	copia[j]=str1[j];
	strcat(copia,str2);
	char* rdo=malloc(sizeof(char)*strlen(copia));
	rdo=copia;
	return strdup(rdo);
}
void ejecutar_estandar(char** argumentos){
	int pid, status;
	char** rutas;
	rutas=obtener_arreglo_de_rutas();
	char* ruta_ejecucion=malloc(sizeof(char)*50);
	if(contar_caracter_enArreglo_deCadenas('/',argumentos)>0){
		FILE* archivo=fopen(*argumentos,"r");
		if(archivo){
		while((**argumentos)!='\0'){
		(*argumentos)++;
		}
		while((**argumentos)!='/')
		(*argumentos)--;
		(*argumentos)++;
		}
		
	}
	if(contar_caracter_enArreglo_deCadenas('&',argumentos)>0){
						ejecutar_detras(argumentos);
	}
	else{
		
		 /*
		  * Creo un hijo.
          */
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
	/*  NOTA: perror() genera un mensaje de error breve en la 
		* salida de errores describiendo el ultimo error encontrado
		* durante una llamada al sistema o funcion de la biblioteca.
        */
    }
     /*
     *  El proceso hijo ejecuta el codigo dentro del if.
     */
    if (pid == 0) { 
			//~ imprimir_arreglo_de_cadenas(argumentos);
			char* a=detectar_redireccion(argumentos);
			//~ imprimir_arreglo_de_cadenas(argumentos);
			if((strcmp(a,">")==0)||(strcmp(a,"<")==0)){
					
					char *nombre_archivo;
					char** nvos_args=malloc(sizeof(char)*tamanio(argumentos));
					nvos_args=modificar_argumentosxredireccion(tipo_redireccion(argumentos),argumentos,&nombre_archivo);
					if(nvos_args==NULL)
					exit(1);
					ruta_ejecucion=obtener_ruta_comando(rutas,nvos_args);
					if(strcmp(ruta_ejecucion,"NADA")==0){
							printf("Baash: %s: No se encontro la orden.\n",*nvos_args);
							exit(1);
					}
					else{
						if((strcmp(a,">")==0)){
							int salida;

							salida = open(nombre_archivo, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
							
							dup2(salida, 1);
							
							close(salida);

							execv(ruta_ejecucion,nvos_args);
							perror(ruta_ejecucion);
						}
						else{
							int entrada;
							entrada = open(nombre_archivo, O_RDONLY);
							dup2(entrada, 0);
							close(entrada);
/*
							int i=0;
							while(*nvos_args!=NULL){							
								nvos_args++;
								i++;
							}
							*nvos_args=nombre_archivo;
							nvos_args++;
							*nvos_args=NULL;
							nvos_args=nvos_args-i-1;
*/
							execv(ruta_ejecucion,nvos_args);
							fclose(stdout);
							perror(ruta_ejecucion);
							exit(1);
						}
					}
			}
			else{
				
				ruta_ejecucion=obtener_ruta_comando(rutas,argumentos);
				if(strcmp(ruta_ejecucion,"NADA")==0){
						printf("Baash: %s: No se encontro la orden.\n",*argumentos);
						exit(1);
				}
				else{
				execv(ruta_ejecucion,argumentos);
				perror(ruta_ejecucion);
				exit(1);
				}
			}
		}
    
    /*
     * El padre ejecuta el wait.
     */
    while (wait(&status) != pid)
        /* Vacio */ ;
		
	}
}
void ejecutar_detras(char** argumentos){
	int pid;
	char* ruta_ejecucion=NULL;
	char** args_sin_ampersand=NULL;
	char** rutas;
	rutas=obtener_arreglo_de_rutas();
	
	 /*
     * Creo un hijo.
     */
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
	/*  NOTA: perror() genera un mensaje de error breve en la 
		* salida de errores describiendo el ultimo error encontrado
		* durante una llamada al sistema o funcion de la biblioteca.
        */
    }
     /*
     *  El proceso hijo ejecuta el codigo dentro del if.
     */
    if (pid == 0) {
				char* a=detectar_redireccion(argumentos);
				if((strcmp(a,">")==0)||(strcmp(a,"<")==0)){
					char *nombre_archivo;
					char** nvos_args=malloc(sizeof(char)*tamanio(argumentos));
					args_sin_ampersand=modificar_argumentosxejecucion_detras(nvos_args);					
					nvos_args=modificar_argumentosxredireccion(tipo_redireccion(argumentos),args_sin_ampersand,&nombre_archivo);
					
					ruta_ejecucion=obtener_ruta_comando(rutas,args_sin_ampersand);
					if(strcmp(ruta_ejecucion,"NADA")==0){
							printf("Baash: %s: No se encontro la orden.\n",*args_sin_ampersand);
							exit(1);
					}
					else{
					if((strcmp(a,">")==0)){
							int salida;

							salida = open(nombre_archivo, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
							
							dup2(salida, 1);
							
							close(salida);

							execv(ruta_ejecucion,nvos_args);
							perror(ruta_ejecucion);

							/*int fid;
							int flags,perm;
							flags = O_WRONLY|O_CREAT|O_TRUNC;
							perm = S_IWUSR|S_IRUSR;
							fid = open(nombre_archivo, flags, perm);
							if (fid<0) { perror("open");exit(1); }
							close(STDOUT_FID); // stdout def. en stdio.h es un FILE* 
							if (dup(fid)<0) { perror("dup"); exit(1); }
							close(fid);
							execv(ruta_ejecucion,args_sin_ampersand);
							perror(ruta_ejecucion);*/
						}
						else{
							int entrada;
							entrada = open(nombre_archivo, O_RDONLY);
							dup2(entrada, 0);
							close(entrada);
							/*
							int i=0;
							while(args_sin_ampersand!=NULL){							
								args_sin_ampersand++;
								i++;
							}
							*args_sin_ampersand=nombre_archivo;
							args_sin_ampersand++;
							*args_sin_ampersand=NULL;
							args_sin_ampersand=args_sin_ampersand-i-1;
							*/

							execv(ruta_ejecucion,nvos_args);
							fclose(stdout);
							perror(ruta_ejecucion);
							exit(1);
						}
					}
			}
			else{
				args_sin_ampersand=modificar_argumentosxejecucion_detras(argumentos);
				ruta_ejecucion=obtener_ruta_comando(rutas,args_sin_ampersand);
				if(strcmp(ruta_ejecucion,"NADA")==0){
						printf("Baash: %s: No se encontro la orden.\n",*args_sin_ampersand);
						exit(1);
				}
				else{
				execv(ruta_ejecucion,args_sin_ampersand);
				perror(ruta_ejecucion);
				exit(1);
				}
			}
	}
}
char* leer_linea_terminal(){
	char* comando=malloc(sizeof(char)*1024);
	fgets(comando,1024,stdin);
	if(feof(stdin)){
	exit(0);
	}
	return comando;
}
void imprimir_arreglo_de_cadenas(char** arreglo){
	char** aux=arreglo;
	int i=0;
	while(*aux!=NULL){
	printf("\nCadena %i:***%s***\n",i,*aux);
	i++;
	aux++;
	
	}
	
}
void cambiar_directorio(char** argumentos){
	argumentos++;
	chdir(*argumentos);
}
void rutina_ejecucion(){
	char* linea=malloc(sizeof(char)*1024);
	
	char* nom_usuario;
	char* nom_host=malloc(sizeof(char)*257);
	nom_usuario = getenv("USER");
	FILE* archivo=fopen("/proc/sys/kernel/hostname","r");
	nom_host = fgets(nom_host,257,archivo);
	char* aux;
	aux=nom_host;
	while(*aux!='\n')
	aux++;
	*aux='\0';
	while(1) 
	{
        /*
         * Pide y lee un comando.
         */
        
		printf("%c[%d;%dm%s@%s:%s > %c[%dm", 0x1B, BRIGHT,GREEN,nom_usuario,nom_host,getcwd(NULL,0),0x1B,0);
		printf("%c[%dm", 0x1B,0);
		linea=leer_linea_terminal();
		
        char** args=obtener_arreglo_de_argumentos(linea);
        if (*linea == '\n');
		else{
			if(strcmp("cd",*args)==0)
			cambiar_directorio(args);
			else{
				char** copia_args=args;
				if((strcmp("exit",*copia_args++)==0)&&(*copia_args==NULL))
				exit(0);
				else{
					copia_args--;
					if((strcmp("-h",*copia_args)==0)||(strcmp("--help",*copia_args)==0)||(strcmp("help",*copia_args)==0)||(strcmp("-a",*copia_args)==0)||(strcmp("--ayuda",*copia_args)==0)||(strcmp("ayuda",*copia_args)==0))
						imprimir_ayuda();
					else{
						if(contar_caracter_enArreglo_deCadenas('|',copia_args)==1)
							ejecutar_tuberias(obtener_comandos_ejecucionxtuberias(linea));
						else
							ejecutar_estandar(args);
					}
				}
			}
		}
	}
}
char* cadena_final(char** arreglo){
	char** copia=malloc(sizeof(char)*tamanio(arreglo));
	copia=arreglo;
	while(*copia!=NULL)
	copia++;
	copia--;
	return *copia;
}
char caracter_final(char* cadena){
	int fin=strlen(cadena);
	return cadena[fin-1];
}
char** modificar_argumentosxejecucion_detras(char** argumentos){

	char** aux=argumentos;
	if(strcmp("&",cadena_final(argumentos))==0){//En el caso de que el '&' esté como una cadena aparte en el arreglo de comandos.
		int i=0;
		while(*aux!=NULL){
			i++;
			aux++;
		}
		aux--;
		i--;
		*aux=NULL;//La última cadena del arreglo de comandos es eliminada.
		aux=aux-i;
	}
	else{
		char caracter=caracter_final(cadena_final(argumentos));
		if(caracter=='&'){//En el caso de que el '&' esté al final de la última cadena del arreglo de comandos.
			int i=0;
			while(*aux!=NULL){//Se recorre el argumento de comandos hasta llegar al final y se mantiene el registro de la posicion en 'i'.
				aux++;
				i++;
			}
			i--;
			aux--;
			int j=0;
			while(**aux!='&'){//Se recorre la última cadena del arreglo y se reemplaza el '&' por '\0'. 
				(*aux)++;
				j++;
			}
			**aux='\0';
			*aux=*aux-j;
			aux=aux-i;
		}
	}
	char** retorno=copia_arreglo_deCadenas(aux);
	return retorno;
}
char** tipo_redireccion(char** argumentos){
	char** par_retorno=(char**)malloc(sizeof(char)*3);
	if(error_en_redireccion(argumentos)==0){
		*par_retorno="5";
		par_retorno++;
		*par_retorno=" ";
		par_retorno++;
		*par_retorno=NULL;
		par_retorno=par_retorno-2;
		return par_retorno;
	}
	int largo=tamanio(argumentos);
	char** copia_args=argumentos;
	int i=0;
	char* redireccion=malloc(sizeof(char));
	redireccion=detectar_redireccion(copia_args);
	if((strcmp(redireccion,">")==0)||(strcmp(redireccion,"<")==0)){//SE PROCEDE SÓLO SI SE DETECTA ALGÚN SIGNO '>'.
	if(largo>1){
		while(i!=(largo-2)){
			copia_args++;
			i++;
		}
	}
	if(strchr(*copia_args,*redireccion)!=NULL){
		if(strcmp(*copia_args,redireccion)==0)
		*par_retorno="1";
		else{
			char* aux=malloc(sizeof(char)*strlen(*copia_args));
			strcpy(aux,*copia_args);
			char* mayor_final=strrchr(aux,*redireccion);
			char* copia_mf=mayor_final;
			copia_mf++;
				if((mayor_final!=NULL)&&(*copia_mf=='\0'))
				*par_retorno="2";
				else{
					if((mayor_final!=NULL)&&(*copia_mf!='\0'))
					*par_retorno="4";
				}
				
					
			}
	}
	else{
		copia_args++;
		if(strchr(*copia_args,*redireccion)!=NULL){
			char* aux=malloc(sizeof(char)*strlen(*copia_args));
			strcpy(aux,*copia_args);
			char* mayor_inicial=strrchr(aux,*redireccion);
			char* copia_mi=mayor_inicial;
			copia_mi++;
			if((*mayor_inicial==*redireccion)&&(*mayor_inicial==**copia_args))
				*par_retorno="3";
			else{
				if((mayor_inicial!=NULL)&&(*copia_mi!='\0'))//
					*par_retorno="4";
			}
			}
		}
	}
	par_retorno++;
	*par_retorno=redireccion;
	par_retorno++;
	*par_retorno=NULL;
	par_retorno=par_retorno-2;
	return par_retorno;
}
char** modificar_argumentosxredireccion(char** par_tipo, char** argumentos, char** nombre_archivo){
	int largo=tamanio(argumentos);
	char** nvos_args=(char**) malloc(sizeof(char)*500);
	char** copia_args=argumentos;
	*nombre_archivo=(char*)malloc(sizeof(char)*tamanio_enCaracteres(argumentos));
	*nombre_archivo="NINGUNO";
	int i=0;
	char nro=**par_tipo;
	int tipo=atoi(&nro);
	char* redireccion=malloc(sizeof(char));
	par_tipo++;
	strcpy(redireccion,*par_tipo);
	if(tipo==5){
		printf("\nError: Hay demasiados archivos para redireccionar. Intente nuevamente con un solo archivo\n");
		return NULL;
	}
	else{
		if(largo>2){
			while(i!=(largo-2)){
				char aux[50];
				strcpy(aux,*copia_args);
				*nvos_args=strdup(aux);
				nvos_args++;
				copia_args++;
				i++;
			}
			switch(tipo){
			case (1):
				{
					*nvos_args=NULL;
					nvos_args=nvos_args-i;
					copia_args++;
					*nombre_archivo=strdup(*copia_args);					
					break;
				}
			case (2):
				{
					nvos_args--;
					char* mem=malloc(sizeof(char)*strlen(*nvos_args));
					strcpy(mem,*nvos_args);
					nvos_args++;	
					char rdo[30];
					int j;
					char* aux=malloc(sizeof(char)*50);
					strcpy(aux,*copia_args);
					int tam=strlen(aux);
					for(j=0;j<tam;j++){
						rdo[j]=*aux;
						aux++;
					}
					strtok(rdo,redireccion);
					nvos_args--;
					*nvos_args=mem;
					nvos_args++;
					*nvos_args=rdo;
					copia_args++;
					*nombre_archivo=*copia_args;
					nvos_args++;
					*nvos_args=NULL;
					nvos_args=nvos_args-(i+1);
					break;
				}
			case (3):
				{
					char* aux=malloc(sizeof(char)*strlen(*copia_args));
					strcpy(aux,*copia_args);
					*nvos_args=aux;
					nvos_args++;
					*nvos_args=NULL;
					nvos_args=nvos_args-i-1;
					copia_args++;
					char* otro=malloc(sizeof(char)*strlen(*copia_args));
					strcpy(otro,*copia_args);
					char* mayor_inicial=strchr(otro,*redireccion);
					mayor_inicial++;
					*nombre_archivo=mayor_inicial;
					break;
				}
			case (4):
				{
					char* aux=malloc(sizeof(char)*strlen(*copia_args));
					strcpy(aux,*copia_args);
					*nvos_args=aux;
					nvos_args++;
					//copie el anteúltimo argumento.
					copia_args++;
					char* otro=malloc(sizeof(char)*strlen(*copia_args));
					strcpy(otro,*copia_args);
					//copie en 'otro' la cadena final de la forma |____|>|____|.
					
					char arg[strlen(*copia_args)];
					int j=0;
					while(*otro!='\0'){
						arg[j]=*otro;
						otro++;
						j++;
					}
					char* aux2=malloc(sizeof(char)*strlen(otro));
					aux2=strchr(arg,*redireccion);
					aux2++;
					*nombre_archivo=strdup(aux2);
					char* arg_final=malloc(sizeof(char)*100);
					arg_final=strtok(arg,redireccion);
					*nvos_args=strdup(arg_final);
					nvos_args++;
					*nvos_args=NULL;
					nvos_args=nvos_args-i-2;
					imprimir_arreglo_de_cadenas(nvos_args);
					imprimir_arreglo_de_cadenas(nvos_args);
					break;
				}
		}
	}
		else{
			switch(tipo){
			case (2):
				{	
					char rdo[30];
					int j;
					char* aux=malloc(sizeof(char)*50);
					strcpy(aux,*copia_args);
					int tam=strlen(aux);
					for(j=0;j<tam;j++){
						rdo[j]=*aux;
						aux++;
					}
					strtok(rdo,redireccion);
					*nvos_args=rdo;
					copia_args++;
					*nombre_archivo=*copia_args;
					nvos_args++;
					*nvos_args=NULL;
					nvos_args=nvos_args-(i+1);
					break;
				}
			case (3):
				{
					char* aux=malloc(sizeof(char)*strlen(*copia_args));
					strcpy(aux,*copia_args);
					*nvos_args=aux;
					nvos_args++;
					*nvos_args=NULL;
					nvos_args=nvos_args-i-1;
					copia_args++;
					char* otro=malloc(sizeof(char)*strlen(*copia_args));
					strcpy(otro,*copia_args);
					char* mayor_inicial=strchr(otro,*redireccion);
					mayor_inicial++;
					*nombre_archivo=mayor_inicial;
					break;
				}
			case (4):
				{
					if(largo==2){
						char* copia_c=malloc(sizeof(char)*strlen(*copia_args));
						strcpy(copia_c,*copia_args);
						*nvos_args=strdup(copia_c);
						copia_args++;
						nvos_args++;
					}
					char* otro=malloc(sizeof(char)*strlen(*copia_args));
					strcpy(otro,*copia_args);
					//copie en 'otro' la cadena final de la forma "|____|>|____|".
					
					char arg[strlen(*copia_args)];
					int j=0;
					//copio todos los valores de 'otro' en el arreglo 'arg'.
					while(*otro!='\0'){
						arg[j]=*otro;
						otro++;
						j++;
					}
					arg[j]='\0';
					char* aux2=malloc(sizeof(char)*strlen(arg));
					aux2=strchr(arg,*redireccion);
					aux2++;
					*nombre_archivo=strdup(aux2);
					char* arg_final=malloc(sizeof(char)*100);
					arg_final=strtok(arg,redireccion);
					*nvos_args=strdup(arg_final);
					nvos_args++;
					*nvos_args=NULL;
					if(largo==2)
					nvos_args=nvos_args-i-2;
					else
					nvos_args=nvos_args-i-1;
					break;
				}
		}
	}
}
		char** copia_final=copia_arreglo_deCadenas(nvos_args);
		return copia_final;
	
}
char* detectar_redireccion(char** argumentos){
	char** copia_args=malloc(sizeof(char)*tamanio(argumentos));
	char* caracter_ret=malloc(sizeof(char));
	copia_args=argumentos;
	while(*copia_args!=NULL){
		char* aux=*copia_args;
		while(*aux!='\0'){
			if(*aux=='>')
				caracter_ret = ">";
			else{
				 if(*aux=='<')
					caracter_ret= "<";
					}
			(aux)++;
			}
		copia_args++;
		}
	return strdup(caracter_ret);
}
int contar_caracter_enArreglo_deCadenas(char caracter, char** arreglo){
	char** copia_arreglo=arreglo;
	int contador=0;
	while(*copia_arreglo!=NULL){
		char* aux=*copia_arreglo;
		while(*aux!='\0'){
		if(*aux==caracter)
		contador++;
		(aux)++;
		}
		copia_arreglo++;
	}
	return contador;
}
int tamanio_enCaracteres(char** arreglo){
	char** copia_arreglo=arreglo;
	int contador=0;
	int i=0;
	while(*copia_arreglo!=NULL){
		char* aux=*copia_arreglo;
		while(*aux!='\0'){
		contador++;
		(aux)++;
		}
		copia_arreglo++;
		i++;
	}
	return contador;
}
char** copia_arreglo_deCadenas(char** arreglo){
	char** aux=arreglo;
	char** copia=malloc(sizeof(char)*tamanio(arreglo));
	int i=0;
	while(*aux!=NULL){
		char* temp=malloc(sizeof(char)*100);		
		strcpy(temp,*aux);
		(*copia)=temp;
		aux++;
		copia++;
		i++;
	}
	*copia=NULL;
	copia=copia-i;
	return copia;
}
int error_en_redireccion(char** argumentos){
	char** aux=argumentos;
	char* redireccion=malloc(sizeof(char));
	int bandera=0;
	redireccion=detectar_redireccion(argumentos);
	if((strcmp(redireccion,">")==0)||(strcmp(redireccion,"<")==0)){
		while(*aux!=NULL)
			aux++;
			if(tamanio(argumentos)>2){
			aux=aux-2;}
			else{
			aux=aux-1;}
		while(*aux!=NULL){
		if((strchr(*aux,'>')!=NULL)||(strchr(*aux,'<')!=NULL)||(strchr(*aux,'|')!=NULL))
		bandera=1;
		aux++;	
		}
	}
		return bandera;
}
par_ejecucion obtener_comandos_ejecucionxtuberias(char*linea_terminal){
	par_ejecucion retorno={NULL,NULL};
	char linea[strlen(linea_terminal)];
	int i=0,contador=0;
	char* copia_linea;
	copia_linea=linea_terminal;
	while(*copia_linea!='\n'){
		if(*copia_linea=='|')
		contador++;
		copia_linea++;
	}
	if(contador>1)
	return retorno;
	while(*linea_terminal!='\n'){
		linea[i]=*linea_terminal;
		linea_terminal++;
		i++;
	}
	linea_terminal=linea_terminal-i;
	char* comandos1=malloc(sizeof(char)*strlen(linea));
	char* comandos2=malloc(sizeof(char)*strlen(linea));
	
	comandos1=strtok(linea,"|");
	char* aux=strchr(linea_terminal,'|');
	aux++;
	comandos2=strdup(aux);
	retorno.comando_hijo=obtener_arreglo_de_argumentos(comandos1);
	retorno.comando_padre=obtener_arreglo_de_argumentos(comandos2);
	return retorno;
}
void ejecutar_tuberias(par_ejecucion comandos){
	if((comandos.comando_hijo==NULL)&&(comandos.comando_padre==NULL)){
	printf("Demasiados argumentos para operar con tuberías.");
	}
	else{
		pid_t pid1, pid2;
		int status1, status2;
		int p[2];
		
		if ( (pid1=fork()) == 0 )
		{ /* hijo (1a generacion) = padre */
			pipe(p);/* Declara que la tubería será entre éste proceso 
						y su hijo únicamente de manera tal que el abuelo 
						 no podrá ver estas comunicaciones*/
			if ( (pid2=fork()) == 0 )
			{ /* hijo (2a generacion) = nieto */
				//~ printf("Soy el nieto (%d, hijo de %d)\n",getpid(), getppid());
				close(p[0]); /* cerramos el lado de lectura del pipe */
				dup2(p[1], 1); /* STDOUT = extremo de salida del pipe */
				close(p[1]); /* cerramos el descriptor de fichero que sobra
									 tras el dup2 */
				
				ejecutar_estandar(comandos.comando_hijo);
				_exit(1); /* salir sin flush */
				//~ ejecutar_detras(comandos.comando_padre);
				
			}
			else
			{ /* padre (2a generacion) = padre */
				wait(&status2);
				//~ printf("Soy el padre (%d, hijo de %d)\n",getpid(), getppid());
				close(p[1]); /* cerramos el lado de escritura del pipe */
				dup2(p[0], 0); /* STDIN = extremo de entrada del pipe */
				close(p[0]); /* cerramos el descriptor de fichero que sobra
									 tras el dup2 */
				ejecutar_estandar(comandos.comando_padre);
				_exit(1); /* salir sin flush */
				//~ ejecutar_estandar(comandos.comando_padre);
			}
		}
		else
		{ /* padre (1a generacion) = abuelo */
			wait(&status1);

		//~ printf("Soy el abuelo (%d, hijo de %d)\n", getpid(),getppid());
		}
	}
}
void imprimir_spot(){
char* help= "Bienvenido a Baash. Para obtener ayuda ingrese -a , --ayuda ó ayuda.";
	printf("\n");
	printf("__  ___        _____       _____   ______  __        	_______________________________________\n");
	printf("\\ \\/   \\ \\   / /   | |  / /   | | / /   \\| \\ \\       	Baash 2013: Implementación en C del\n");
	printf("| |     | | | |    | | | |    | | \\_\\___   | |_____  	..........<<Bourn-Shell>> de LINUX.\n");
	printf("| |____/_/  | |    | | | |    | |      \\ \\ | |   \\ \\ 	Autor: Morales Esteban	\n");
	printf("| |    \\ \\   \\_\\_ /\\_\\  \\_\\_ /\\_\\ |\\___/_/ | |   | | 	Consultas a: andresteve07@gmail.com   \n");
	printf("| |     | |   _______________________      | |   | | 	Errores a: andresteve07@gmail.com  \n");
	printf("/_/\\___/_/    |Versión: Octubre 2011|      /_/   /_/   ________________________________________\n");
printf("%c[%d;%dm%s%c[%dm", 0x1B, 1,33,help,0x1B,0);
		printf("%c[%dm", 0x1B,0);	
printf("\n\n");
}
void imprimir_ayuda(){
	imprimir_spot();
	printf("\nTodo lo que escriba en la consola intentará ser procesado como una secuencia de comandos separados por espacios en blanco y/o tabulaciones.\n");
}
