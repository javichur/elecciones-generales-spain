/*Reto Elecciones Generales (rev.3eb) 24-5-04*/
/*Fco. Javier Campos*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
//#include "memdinamic.h" 

#define N_INPUT 6 //nº datos iniciales de entrada

#define POBLACION_DERECHO_PAIS datos_ini[0]
#define TOTAL_DIPUTADOS datos_ini[1]
#define MODO_PAIS datos_ini[2]
#define NUM_CIRCUNSCRIPCIONES datos_ini[3]
#define TOTAL_PARTIDOS datos_ini[4]
//nº max de partidos presentados en una misma circunscripción
#define MAX_PARTIDOS 30
#define NOMBRE_MAS_LARGO 25 //Santa Cruz de Tenerife

typedef struct {
  char nombre_partido[20];
  int votos_partido;
  float porcentaje;
  int escanyos;
}partido;

typedef struct {
  char nombre[NOMBRE_MAS_LARGO];
  int poblacion_derecho;
  int num_votos;
  int votos_nulos;
  int escanyos;
  int resto;
  int num_partidos_zona;
  partido *partidos;
}circunscripcion;

void mostrar_cabecera_cir(circunscripcion *c)
{
  printf("\n\n\nCIRCUNSCRIPCION: %s\n",c->nombre);
  printf("POBLACION DE DERECHO: %d\n",c->poblacion_derecho);
  printf("NUMERO DE VOTOS ESCRUTADOS: %d\n",c->num_votos);
  printf("VOTOS NULOS: %d\n",c->votos_nulos);
  printf("NUMERO DE PARTIDOS PRESENTADOS: %d\n",c->num_partidos_zona);
  printf("ESCAÑOS:%d\n\n",c->escanyos);
}

void obtener_esc(circunscripcion *c,int j,float cuota_de_reparto,int *rest)
{
  //obtiene el nº de escaños para cada circunscripción, según Ley D'Hont:

  //inicialmente, 2 diputados por circunscripcion, excepto 1 a Ceuta y Melilla.
  //Para los diputados restantes (248 en el caso de España), primero se obtiene
  //una CUOTA DE REPARTO, resultante de dividir el total de la población de
  //derecho española entre el nº de diputados restantes (248 en Esp).
  //Se adjudica a cada circunscripcion, tantos diputados como el resultado (en
  //ENTEROS), de dividir la poblacion de derecho en esa circunscripcion entre
  //la CUOTA DE REPARTO.Los diputados que se queden sin repartir, serán para
  //las provincias cuyo cociente de la división anterior sea mayor.

  //diputados 'extra':parte entera de la división (TRUNCAMIENTO)
  int extra=(int)(c->poblacion_derecho/cuota_de_reparto);

  if(j) //si es ceuta o melilla, de primeras, 1 diputados
    c->escanyos=1;
  else
    c->escanyos=2;

  c->escanyos+=extra;
  c->resto=((c->poblacion_derecho)%(int)cuota_de_reparto);
  (*rest)-=extra;
}

int lee_circunscripcion(FILE *f, circunscripcion *destino) 
{
  //Almacena las cabeceras de las circunscripciones, leyendolas
  //del archivo de texto y guardándolas en el vector "cir"

  int j,c;
  fscanf(f,"%s",destino->nombre);
  j=0;
  while ((c=getc(f))!=EOF) {
    switch (c) {
    case ':':
      if(j==0){ //leo poblacion derecho
	fscanf(f,"%d",&destino->poblacion_derecho);
	j++;
      }
      else if(j==1){ //nº votos
	fscanf(f,"%d",&destino->num_votos);
	j++;
      }
      else if(j==2){ //votos nulos
	fscanf(f,"%d",&destino->votos_nulos);
	j++;
      }
      else if(j==3){ //nº de partidos presentados
	fscanf(f,"%d",&destino->num_partidos_zona);
	return 0;
      }
      break;
    }
  }
  return 0;
}


int leer_datos(char fnombre[100],circunscripcion **cirs,int datos_ini[N_INPUT],int *dip_rest)
{
  int i,j,k;
  FILE *fd;
  char car=' ',cadena_aux[20];
  partido *p;
  float cuota_de_reparto;
  circunscripcion *cir;
  
  fd=fopen(fnombre,"r");
  if(fd==NULL){
    printf("Error al abrir la base de datos\n");
    return (-1);
  }
  
  i=0;
  while(i<N_INPUT){ //cargo los datos iniciales en 'datos_ini'
    car=getc(fd);
    if(car=='#'){	 
      fscanf(fd,"%d",&datos_ini[i]);
      i++;
    }
  }
  
  //creo vector a circunscripciones
  cir=(circunscripcion *)malloc((NUM_CIRCUNSCRIPCIONES+MODO_PAIS)*sizeof(circunscripcion));
  if(cir==NULL){
    printf("ERROR #1:Memoria insuficiente. Prueba a reiniciar XD\n");
    return -1;
  }
  
  //diputados restantes después de asignar 2 por circunscripcion 
  //(ceuta y melilla, sólo 1)
  *dip_rest=TOTAL_DIPUTADOS-NUM_CIRCUNSCRIPCIONES*2;
  
  //calculo la CUOTA DE REPARTO
  cuota_de_reparto=(((float)(POBLACION_DERECHO_PAIS))/(*dip_rest));
  
  printf("Cuota de reparto2:%.3f\n",cuota_de_reparto);
  printf("Diputados Restantes:%d\n",*dip_rest);
  printf("Poblacion Derecho pais:%d\n",POBLACION_DERECHO_PAIS);

  i=0;
  j=0;
  
  //recorro todas las circunscripciones, en el archivo de texto
  while(i<NUM_CIRCUNSCRIPCIONES+MODO_PAIS)
    {
      car=getc(fd);
      switch(car)
	{
	case '#': //leo información circunscripcion (cabecera)
	  lee_circunscripcion(fd,cir+i);
	  break;
	case '=':
	  fscanf(fd,"%s",cadena_aux); //'ignoro' la línea de "====="
	  
	  //reservo espacio para los partidos de esta circunscripcion
	  (cir+i)->partidos=(partido *)malloc(((cir+i)->num_partidos_zona)*sizeof(partido)); 
	  if((cir+i)->partidos==NULL){
	    printf("ERROR #2:Memoria insuficiente!! Prueba a reiniciar XD\n");
	    return -1;
	  }
	  
	  if(datos_ini[2]) //si es españa (vamos a encontrar 'ceuta' y 'melilla')
	    { //si encuentro ceuta o melilla...parte con 1 escaño
	      if(!strcmp("CEUTA",(cir+i)->nombre)||!strcmp("MELILLA",(cir+i)->nombre))
		obtener_esc((cir+i),1,cuota_de_reparto,dip_rest); 
	      else //si no es...parte con 2 escaños
		obtener_esc((cir+i),0,cuota_de_reparto,dip_rest);	    
	    }	    
	  else //si no es España, todas las circunscripciones parten con 2 escaños
	    obtener_esc((cir+i),0,cuota_de_reparto,dip_rest);
	  
	  if(dip_rest<0) //si no hay suficientes diputados para repartir...Error!!
	    {
	      printf("Error. Faltan escaños.\n");
	      printf("Revisa datos entrada (poblacion derecho,nº diputados,etc)\n");
	      return -1;
	    }
	  
	  //mientras queden partidos en esa circunscripcion...
	  for(k=0;k<(cir+i)->num_partidos_zona;k++) {
	    p=&((cir+i)->partidos[k]);
	      
	    //almaceno nº votos y nombre del partido (ojo, en partido "p" (aux))
	    fscanf(fd,"%s %d",(p->nombre_partido),&(p->votos_partido)); 
	      
	    //saco % de votos a ese partido (ojo:los votos nulos no se cuentan)
	    p->porcentaje=((float)(p->votos_partido)*100)/(((cir+i)->num_votos)-(cir+i)->votos_nulos);
	      
	    //si tiene % menor al 3%,no obtiene representacion.
	    if(p->porcentaje<3)
	      p->escanyos=-1;
	    else //si %>3, aplicaré ley D'Hont
	      p->escanyos=0;
	    }
	  i++;
	  break;  	  
	}
    }
  fclose(fd);
  *cirs=cir;
  return 0;
}

int reparto_dip_restantes(int *d_restantes, int num_cir,int m_pais,circunscripcion *circ)
{
  //Reparte diputados restantes tras aplicar Ley D'Hont (función "obtener_esc")
  //a todas las circunscripciones. Los diputados restantes serán repartidos
  //entre las provincias cuyo cociente de la división: 
  //          "poblacion derecho"/"cuota de reparto" 
  //sea mayor. (sólo 1 diputado más por circunscripción).

  int i,resto_mayor=0,j=0;

  if(*d_restantes<num_cir+m_pais)
    while(*d_restantes>0){
      for(i=0;i<num_cir;i++)
	{	      
	  if(((circ+i)->resto)>resto_mayor)
	    {
	      resto_mayor=(circ+i)->resto;
	      j=i;      
	    }
	}

      ((circ+j)->resto)=0;
      resto_mayor=0;
      ((circ+j)->escanyos)++;
      (*d_restantes)--;
    }
   
  else //si los diputados restantes son más q las circunscripciones...Error!!
    return -1;
  return 0;    
}

int reparto_entre_partidos(circunscripcion *c,int **matriz_ley_dhont)
{
  //Reparte el total de escaños de una circunscripcion entre los partidos
  
  int j,k,z,max,max_j,max_k,filas;
  for(j=0;j<c->num_partidos_zona && j<c->escanyos;j++) //recorro todos los partidos
    {
      //si un partido no obtuvo representacion...
      if(c->partidos[j].escanyos==-1)
	break; //...salgo del bucle; sólo me interesan partidos con % votos >3%

      //reservo espacio para hacer la tabla de Ley D'Hont
      //tantas filas como partidos con % votos >3
      //La primera fila tiene dimension igual al nº de escaños en juego
      //Resto de filas tienen la longitud de su fila anterior -1
      matriz_ley_dhont[j]=(int *)malloc(((c->escanyos)-j)*sizeof(int));
      if(matriz_ley_dhont[j]==NULL){
	printf("ERROR #3:Memoria insuficiente. Prueba a reiniciar XD\n");
	return -1;
      }
    
      //A cada partido con % >3% le corresponde una fila.
      //1ªcolumna: votos obtenidos. 2ª: votos/2. 3ª votos/3...etc
      //Así tantas columnas como escaños en juego.
      for(k=0;k<((c->escanyos)-j);k++)//relleno la matriz
	matriz_ley_dhont[j][k]=(c->partidos[j].votos_partido)/(k+1);
    } 

  filas=j; //nº de partidos con % >3%
  
  for(z=0;z<c->escanyos;z++) //busca los 'n' mayores valores de la matriz,
    {                        //siendo 'n' el nº de escaños a repartir
      max=0;	
      for(j=0;j<(c->num_partidos_zona);j++)
	{	  
	  if(c->partidos[j].escanyos==-1)
	    break;
	  for(k=0;k<(c->escanyos)-j;k++)
	    if(matriz_ley_dhont[j][k]>max)
	      {
		max=matriz_ley_dhont[j][k];
		max_j=j;
		max_k=k;
	      }
	}
      matriz_ley_dhont[max_j][max_k]=0;
      //doy los escaños a los partidos de las filas donde están los 'n'
      //mayores nºs encontrados 
      c->partidos[max_j].escanyos++;                                               	  
    }   
  return filas;
}

void mostrar_datos_partido(partido *p)
{
  //imprime datos partido

  //partidos sin representacion tienen '0' escaños
  if(p->escanyos==-1)
    p->escanyos=0;
  
  printf("%s\t\t\t",p->nombre_partido);
  printf("%d\t",p->votos_partido);
  printf("%.2f %%\t",p->porcentaje);
  printf("%d diputados\n",p->escanyos);
}

void almaceno_partido(partido *gen,partido *p)
{
  //Almaceno todos los datos de los partidos a nivel nacional

  strcpy(gen->nombre_partido,p->nombre_partido); //guardo nombre
  gen->votos_partido=p->votos_partido; //guardo votos
  gen->escanyos=p->escanyos; //guardo escanyos
}

int busco_partido(int p_reg,partido gen[],partido p)
{     
  int k;
  for(k=0;k<p_reg;k++)
    {
      //si datos q llegan son de partido ya fichado, sumo datos a los q ya tengo
      if(!strcmp((gen[k].nombre_partido),(p.nombre_partido))) 
	{     
	  return k; //partido encontrado entre los ya fichados 
	  break;      
	}
    }
  return -1;
}

void liberar_filas(int **mat,int tam)
{
  int i;
  for(i=0;i<tam;i++)
    free(mat[i]);
}

void ver_balance(int p_reg,partido *gen,int j,int k,int datos[N_INPUT])
{
  //Muestra resultados finales a nivel nacional

  int tot_votos=0;  
  int i;

  printf("\n\nHACEMOS BALANCE:\n\n\n\npartidos_reg:%d\n\n",p_reg);
  printf("Todos los partidos:\n\n");
  for(i=0;i<p_reg;i++){ //resultados por partidos, a nivel nacional
    tot_votos+=(gen+i)->votos_partido; //total votantes en todo el pais
    gen[i].porcentaje=((float)gen[i].votos_partido*100)/(k-datos[5]);
    mostrar_datos_partido(&(gen[i]));
  }
  
  printf("\n\n\nLos partidos que obtuvieron representacion:\n\n");
  //resultados por partidos con escaños, a nivel nacional
  for(i=0;i<p_reg;i++){
    if(gen[i].escanyos) //si tiene escaños...
      mostrar_datos_partido(&(gen[i]));
  }

  printf("\n\n\nControl de los datos de entrada:\n");
  for(i=0;i<N_INPUT;i++)
    printf("dato %d:%d\n",i,datos[i]);

  printf("\nSUMA DE LOS VOTOS A PARTIDOS:\nTotal votos:%d\n\n",tot_votos);

  printf("SUMA DE LOS VALORES QUE SE FACILITAN PARA CADA CIRCUNSCRIPCION:\n");
  printf("Total_votantes=%d\nTotal nulos=%d\n\n",k,j);

  printf("DATOS OFICIALES:\n");
  printf("Total votos nulos:%d\n",datos[5]);

  printf("\nReto elecciones\nUltima revision:24-5-04\nFco.Javier Campos\n");
}

int main(){

  char fnombre[100]="elecciones.txt";
  int datos_ini[N_INPUT]; /*poblacion derecho, num diputados,
		      país , num circunscripciones y total partidos*/
  int i,j,k,num_partido,dip_rest;
  int **matriz_ley_dhont;
  partido *generales;
  int partidos_reg;
  circunscripcion *cir;
  int filas;

  printf("RETO ELECCIONES\n\n");

  //Leo datos de entrada y almaceno en memoria a la que apunta "cir".
  //Reparto escaños entre circunscripciones.
  if (leer_datos(fnombre,&cir,datos_ini,&dip_rest)) 
    return -1;
    
  //reparto los diputados restantes entre las circunscripciones 
  //con mayor resto (1 diputado para cada una).
  if(reparto_dip_restantes(&dip_rest,NUM_CIRCUNSCRIPCIONES,MODO_PAIS,cir)==-1){
    printf("Error. El nº de diputados restantes tras el reparto\n");
    printf(" por CUOTA DE REPARTO es mayor que el nº de circunscripciones.\n");
    printf("Esto no deberia pasar.Algo falla :s\n");
    return -1;
  }

  //reservo espacio para la matriz de la Ley D'Hont
  matriz_ley_dhont=(int **)malloc((MAX_PARTIDOS)*sizeof(int *));
  //recorro todas las circunscripciones repartiendo escaños entre partidos
  for(i=0;i<NUM_CIRCUNSCRIPCIONES+MODO_PAIS;i++)
    {
      filas=reparto_entre_partidos(&(cir[i]),matriz_ley_dhont);
      if(filas==-1)
	return -1;
      liberar_filas(matriz_ley_dhont,filas);
    }
  free(matriz_ley_dhont);

  //reservo espacio para gestionar partidos a nivel nacional
  generales=(partido *)malloc(TOTAL_PARTIDOS*sizeof(partido));
  if(generales==NULL){
    printf("ERROR #4:Memoria insuficiente. Prueba a reiniciar XD\n");
    return -1;
  }
  partidos_reg=0;

  //muestro resultados por circunscripciones
  for(i=0;i<NUM_CIRCUNSCRIPCIONES+MODO_PAIS;i++) 
    {
      //imprime datos generales (nombre,poblacion,num. escaños...)
      mostrar_cabecera_cir(&cir[i]);

      //mientras queden partidos en esa circunscripcion...
      for(num_partido=0;num_partido<(cir+i)->num_partidos_zona;num_partido++)
	{
	  //imprimo datos partido
	  mostrar_datos_partido(&((cir+i)->partidos[num_partido]));

	  //los añado a base de datos a nivel nacional
	  //busco el partido entre los ya registrados
	  j=busco_partido(partidos_reg,generales,(cir+i)->partidos[num_partido]);
	  if(j==-1)//si no encuentro el partido, reservo memoria y almaceno
	    {
	      almaceno_partido(&(generales[partidos_reg]),&((cir+i)->partidos[num_partido]));
	      partidos_reg++;
	    }
	  else //si he encontrado el partido...
	    {
	      //sumo votos y escaños a los que ya tengo almacenados
	      generales[j].votos_partido+=(cir+i)->partidos[num_partido].votos_partido;
	      generales[j].escanyos+=(cir+i)->partidos[num_partido].escanyos;
	    }
	}
    }
  
  //calculo suma "total votantes" y "total nulos"; voy liberando "cir"
  for(i=0,j=0,k=0;i<NUM_CIRCUNSCRIPCIONES+MODO_PAIS;i++){
    k+=(cir+i)->num_votos;
    j+=(cir+i)->votos_nulos;
    free((cir+i)->partidos);
  }
  free(cir);
  
  //muestra datos a nivel nacional (nº votos, escaños,etc...)
  ver_balance(partidos_reg,generales,j,k,datos_ini);

  free(generales); //libero vector partidos de generales

  return 0;
}
