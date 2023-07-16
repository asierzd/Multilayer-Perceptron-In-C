#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <termios.h>

#define maxCapas 5 // Maximo numero de capas del perceptron
#define maxNeuronas 5 // Maximo numero de neuronas por capa
#define maxElems 5 // Maximo numero de entradas
int capacont;

// Capa de un perceptron
struct capa{
	float matrizPesos[maxElems][maxNeuronas];
	float umbral[maxNeuronas];
	float entrada[maxElems];
	float salida[maxElems];
	int numNeuronas;
};

// Perceptron completo
struct perceptron{
    struct capa capas[maxCapas];
    int numCapas,numEntradas;
    FILE *entrada;
}per;

void getch();
float f(float x);
void multiplica(struct perceptron *, int);
void restaVector(struct perceptron *, int);
void configuracion(struct perceptron *);
void entrada(struct perceptron *, int *, int);

/*CONFIGURACION actual
 *
 * Numero de capas: 2
 * Numero de entradas: 2
 * Numero de neuronas capa 0: 2
 * Matriz de pesos capa 0 (2x2): 1 1 1 1
 * Vector de umbral capa 0: 0.5 0.5
 * Numero de neuronas capa 1: 1
 * Matriz de pesos capa 1 (2x1): 1 -2
 * Vector de umbral capa 1: 0.5
 *
*/

int main()
{

    printf("\n             \t\t\t\t::PERCEPTRON MULTICAPA::\n");
    configuracion(&per);

    printf("\nEjecucion del Perceptron\n========================\n");

    if((per.entrada = fopen("entrada", "r")) == NULL){
        printf("\nError al abrir el fichero de entrada . . .\n");
    }else{ // Abierto fichero de entrada correctamente

        int fin = 0, primera = 1, ultima = 0;
        capacont = 0;

        do{

            if(primera) // Primera entrada
                entrada(&per, &fin, capacont);
            else if(capacont == (per.numCapas - 1)){ // Si termina sigue con siguiente entrada
                ultima=1;
		for(int i = 0; i < per.capas[capacont - 1].numNeuronas; i++) // Entrada capa siguiente=Salida capa anterior
                    per.capas[capacont].entrada[i] = per.capas[capacont - 1].salida[i];
            }else{
                for(int i = 0; i < per.capas[capacont - 1].numNeuronas; i++) // Entrada capa siguiente=Salida capa anterior
                    per.capas[capacont].entrada[i] = per.capas[capacont - 1].salida[i];
            } 
            
            // Si todavia quedan entradas
            if(fin != EOF){

                if(primera){ // Si es primera entrada

                    switch(per.numEntradas){ // Mostrar entrada

                        case 1: printf("Entrada: (%.2f)    ->",per.capas[capacont].entrada[0]);
                                break;
                        case 2: printf("Entrada: (%.2f, %.2f)    ->",per.capas[capacont].entrada[0],per.capas[capacont].entrada[1]);
                                break;
                        case 3: printf("Entrada: (%.2f, %.2f, %.2f)    ->",per.capas[capacont].entrada[0],per.capas[capacont].entrada[1],per.capas[capacont].entrada[2]);
                                break;
                        case 4: printf("Entrada: (%.2f, %.2f, %.2f, %.2f)    ->",per.capas[capacont].entrada[0],per.capas[capacont].entrada[1],per.capas[capacont].entrada[2],per.capas[capacont].entrada[3]);
                                break;
                        case 5: printf("Entrada: (%.2f, %.2f, %.2f, %.2f, %.2f)    ->",per.capas[capacont].entrada[0],per.capas[capacont].entrada[1],per.capas[capacont].entrada[2],per.capas[capacont].entrada[3],per.capas[capacont].entrada[4]);
                                break;
                        default: printf("\n\n**Error al mostrar entradas, numEntradas != { 1 || 2 || 3 || 4 || 5 } . . .\n\n");
                                break;

                    }
                }

                // Multiplicar matriz de pesos por entradas
                multiplica(&per, capacont);

                // Resta umbral a la salida anterior
                restaVector(&per, capacont); 

                // Aplicacion de funcion F a salida anterior
                for(int i = 0; i < per.capas[capacont].numNeuronas; i++) 
                    per.capas[capacont].salida[i] = f(per.capas[capacont].salida[i]);

                // Si estamos en ultima entrada
                if(ultima){
                    switch(per.capas[capacont].numNeuronas){ // Mostrar salida

                        case 1: printf("    Salida: (%f)\n",per.capas[capacont].salida[0]);
                                break;
                        case 2: printf("    Salida: (%f, %f)\n",per.capas[capacont].salida[0],per.capas[capacont].salida[1]);
                                break;
                        case 3: printf("    Salida: (%f, %f, %f)\n",per.capas[capacont].salida[0],per.capas[capacont].salida[1],per.capas[capacont].salida[2]);
                                break;
                        case 4: printf("    Salida: (%f, %f, %f, %f)\n",per.capas[capacont].salida[0],per.capas[capacont].salida[1],per.capas[capacont].salida[2],per.capas[capacont].salida[3]);
                                break;
                        case 5: printf("    Salida: (%f, %f, %f, %f, %f)\n",per.capas[capacont].salida[0],per.capas[capacont].salida[1],per.capas[capacont].salida[2],per.capas[capacont].salida[3],per.capas[capacont].salida[4]);
                                break;
                        default: printf("\n\n**Error al mostrar salidas, numNeuronas != { 1 || 2 || 3 || 4 || 5 } . . .\n\n");
                                break;

                    }
                }

                // Reiniciar control
                if(primera){
                    primera = 0; // Siguiente capa ya no es primera entrada
                    capacont++;
                }else if(ultima){
                    primera = 1; // Para leer nueva entrada
                    ultima = 0; // Ya no es ultima es primera entrada
                    capacont = 0; // Comienza de nuevo con la capa 0
                }else 
                    capacont++; // Siguiente capa

            }else // Se acabaron las entradas
                printf("\nEjecucion para todas las entradas terminada con exito.\n");

        }while(capacont < per.numCapas && fin != EOF);


    }

    // Cerrar fichero entradas
    fclose(per.entrada);

    return 0;
}

// Funcion F: f(x) = 1 / (1 + e^(-20*x))
float f(float x){ 
    double resul = 0;
    x *= -20;

    resul = 1 / (1 + pow(M_E, x));

    return resul;
}

// Lectura de fichero de entrada
void entrada(struct perceptron *per, int *fin, int capacont){ 

        // Leemos fila completa dependiendo del numero de entradas
        switch(per->numEntradas){

            case 1: *fin=fscanf(per->entrada,"%f",&per->capas[capacont].entrada[0]);
                    break;
            case 2: *fin=fscanf(per->entrada,"%f %f", &per->capas[capacont].entrada[0], &per->capas[capacont].entrada[1]);
                    break;
            case 3: *fin=fscanf(per->entrada,"%f %f %f",&per->capas[capacont].entrada[0],&per->capas[capacont].entrada[1],&per->capas[capacont].entrada[2]);
                    break;
            case 4: *fin=fscanf(per->entrada,"%f %f %f %f",&per->capas[capacont].entrada[0],&per->capas[capacont].entrada[1],&per->capas[capacont].entrada[2],&per->capas[capacont].entrada[3]);
                    break;
            case 5: *fin=fscanf(per->entrada,"%f %f %f %f %f",&per->capas[capacont].entrada[0],&per->capas[capacont].entrada[1],&per->capas[capacont].entrada[2],&per->capas[capacont].entrada[3],&per->capas[capacont].entrada[4]);
                    break;
            default: printf("\n**Error: No hay entrada valida al perceptron . . .\n");
                    exit(1);
                    break;

        }
}

// Lectura de fichero de configuracion
void configuracion(struct perceptron *per){

    FILE *configuracion;
    // Comprobar que fichero existe y se puede abrir
    if((configuracion = fopen("configuracion", "r")) == NULL)
        printf("\n**Error al abrir el fichero de configuracion del perceptron . . .\n");
    else{ // Fichero correctamente abierto

        // Obtengo Numero de Capas
        fscanf(configuracion, "%d", &per->numCapas); 
        if(per->numCapas > maxCapas){
            printf("\n**Error: Sobrepasa el maximo de capas que permite este perceptron . . .\n");
            exit(1);
        }

        // Obtengo Numero de Entradas
        fscanf(configuracion, "%d", &per->numEntradas); 
        if(per->numEntradas > maxElems){
            printf("\n**Error: Sobrepasa el maximo de entradas que permite este perceptron . . .\n");
            exit(1);
        }

        // Configurar cada capa
        for(int i = 0; i < per->numCapas; i++){
            //fflush(stdin);

            // Obtengo Numero de Neuronas
            fscanf(configuracion, "%d", &per->capas[i].numNeuronas); 
            if(per->capas[i].numNeuronas > maxNeuronas){
                printf("\n**Error: Sobrepasa el maximo de neuronas que permite este perceptron . . .\n");
                exit(1);
            }

            // Obtengo matriz de pesos
            for(int j = 0; j < per->numEntradas; j++){
                for(int k = 0; k < per->capas[i].numNeuronas; k++){
                        fscanf(configuracion, "%f ", &per->capas[i].matrizPesos[j][k]);
                }
            }

            // Para leer valores float de un fichero en formato string usamos
            // funcion strof para convertir string a float con un puntero char
            char *pEnd; // string->float
            char umbral[10];
            fgets(umbral, 9, configuracion);

            // Para multicapa de hasta 5
            switch(per->capas[i].numNeuronas){ 
                case 1: per->capas[i].umbral[0] = strtof(umbral, &pEnd);
                        break;
                case 2: per->capas[i].umbral[0] = strtof(umbral, &pEnd);
                        per->capas[i].umbral[1] = strtof(pEnd, NULL);
                        break;
                case 3: per->capas[i].umbral[0] = strtof(umbral, &pEnd);
                        per->capas[i].umbral[1] = strtof(pEnd, &pEnd);
                        per->capas[i].umbral[2] = strtof(pEnd, NULL);
                        break;
                case 4: per->capas[i].umbral[0] = strtof(umbral, &pEnd);
                        per->capas[i].umbral[1] = strtof(pEnd, &pEnd);
                        per->capas[i].umbral[2] = strtof(pEnd, &pEnd);
                        per->capas[i].umbral[3] = strtof(pEnd, NULL);
                        break;
                case 5: per->capas[i].umbral[0] = strtof(umbral, &pEnd);
                        per->capas[i].umbral[1] = strtof(pEnd, &pEnd);
                        per->capas[i].umbral[2] = strtof(pEnd, &pEnd);
                        per->capas[i].umbral[3] = strtof(pEnd, &pEnd);
                        per->capas[i].umbral[4] = strtof(pEnd, NULL);
                        break;
                default:printf("\n**Error al leer vector umbral . . .\n");
                        exit(1);
                        break;
            }

        }

        // Cerrar fichero configuracion
        fclose(configuracion);

        // Carga de la configuracion del perceptron multicapa
        printf("\nCarga del Perceptron\n====================\n");
        //getch();
        printf("Numero de Capas: %d",per->numCapas);
        printf("\nNumero de Entradas: %d",per->numEntradas);

        // Mostrar configuracion del perceptron multicapa
        for(int i = 0; i < per->numCapas; i++){

            printf("\n\nCargando capa . . .\n");
            //getch();
            printf("\n Numero de entradas: %d", per->numEntradas);
            printf("\n Numero de neuronas: %d\n", per->capas[i].numNeuronas);

            for(int j = 0; j < per->numEntradas; j++)// Matriz de Pesos
                for(int k = 0; k < per->capas[i].numNeuronas; k++)
                    printf("w[%d][%d]=%f\n", j, k, per->capas[i].matrizPesos[j][k]);

            printf("\nUmbrales\n");

            for(int j = 0; j < per->capas[i].numNeuronas; j++)

                printf("umbral[%d]=%f\n", j, per->capas[i].umbral[j]);
        }
    }
}

// Funcion para multiplicar matriz de pesos por las entradas
void multiplica(struct perceptron *per, int capacont){
	float suma = 0;
	for(int i = 0; i < per->capas[capacont].numNeuronas; i++){
		for(int j = 0; j < per->numEntradas; j++){
			suma += per->capas[capacont].matrizPesos[j][i] * per->capas[capacont].entrada[j];
		}
		per->capas[capacont].salida[i] = suma;
		suma = 0;
	}
}

// Funcion para restar al vector de salida el vector umbral
void restaVector(struct perceptron *per, int capacont){

    for(int i = 0; i < per->capas[capacont].numNeuronas; i++){
        per->capas[capacont].salida[i] = per->capas[capacont].salida[i] - per->capas[capacont].umbral[i];
    }

}

// Used to pause execution until pressed Enter button, only for debugging
void getch(){
    char buf=0;
    struct termios old={0};
    fflush(stdout);
    if(tcgetattr(0, &old)<0)
        perror("tcsetattr()");
    old.c_lflag&=~ICANON;
    old.c_lflag&=~ECHO;
    old.c_cc[VMIN]=1;
    old.c_cc[VTIME]=0;
    if(tcsetattr(0, TCSANOW, &old)<0)
        perror("tcsetattr ICANON");
    if(read(0,&buf,1)<0)
        perror("read()");
    old.c_lflag|=ICANON;
    old.c_lflag|=ECHO;
    if(tcsetattr(0, TCSADRAIN, &old)<0)
        perror ("tcsetattr ~ICANON");
    //return buf;
 }

