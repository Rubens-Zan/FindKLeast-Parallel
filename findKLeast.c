#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>

#include "chrono.h"
#include "max-heap.h"



/**GLOBALS**/
int nTotalElements, k, nThreads; 
float Input[MAX_SIZE];
/**GLOBALS**/

#if TYPE == FLOAT
   #define MAX_TOTAL_ELEMENTS (500*1000*1000)  // if each float takes 4 bytes
                                            // will have a maximum 500 million FLOAT elements
                                            // which fits in 2 GB of RAM
#endif   


void verifyOutput( const float *Input, 
                   const pair_t *Output,   // pair_t é o tipo de um par (v,p)
                      int nTotalElmts,
                      int k )
{
    // codigo da verificacao a ser incluido por voce
    // voce deve verificar se o conjunto de pares de saida está correto
    // e imprimir uma das mensagens abaixo
    int ok = 1;
    
    // inserir aqui o codigo da verificacao
    // uma implementação possível para o verificador seria
    // (nao precisa seguir essa descrição, voce pode fazer outro método
    //  de verificação)
    //
    // 1) Criar um vetor I de pares (chave, valor) 
    //    os os elementos de I devem ser copias
    //    de cada valor (e,p) do vetor de entrada Input
    //    (ou seja, cada valor e que veio da posição p da entrada)
    // 2) Ordenar o vetor I em ordem crescente, 
    //    obtendo-se um outro vetor Is (ordenado em ordem crescente de chaves)
    //    usando um algoritmo de ordenação do tipo (chave, valor)
    //    (por exemplo ordenação da stdlib, caso exista)
    // 3) Para cada par (ki,vi) pertencente ao vetor Output
    //      procurar a chave ki dentre K primeiros elementos
    //      de Is.
    //      Se a chave Ki estiver em Is com valor val==vi continue
    //      senão faça ok = 0 e reporte o erro abaixo
    
    if( ok )
       printf( "\nOutput set verifyed correctly.\n" );
    else
       printf( "\nOutput set DID NOT compute correctly!!!\n" );   
}





pair_t * findKLeastProgram(){
//     Podemos aplicar uma estrutura de dados do tipo Max-Heap
//      e a operação decreaseMax no seguinte problema:
//      Dado um conjunto C de n valores, queremos obter o
//      subconjunto S com os k menores elementos de C.
// ● Obs: supondo k <= n
//       Um algoritmo possível para resolver o problema acima
//      seria:
//          ● Extrair k elementos de C e criar um Max-Heap h com
//          esses elementos.
    pair_t * heap = malloc(sizeof(pair_t) * k);
    int heapSize;

    heapSize = 0;
    
    for( int i=0; i< k; i++ ) {
      printf("inserting %f\n", Input[i]);
      pair_t inputTuple; 
      inputTuple.key = Input[i];
      inputTuple.val = i;
      insert( heap, &heapSize, inputTuple );
      
      #ifdef DEBUG 
      printf("------Max-Heap Tree------ ");
      if( isMaxHeap( heap, heapSize ) )
        printf( "is a heap!\n" );
      else
        printf( "is NOT a heap!\n" );
        #endif   

    }  

    
//          OBS: note que o Max-Heap tem tamanho k
//          ● Para cada elemento e do conjunto C restante, aplicar
//          a operação decreaseMax( h, e ) no heap.
    for (int i = k;i < nTotalElements;++i){
        pair_t inputTuple; 
        inputTuple.key = Input[i];
        inputTuple.val = i;
        decreaseMax(heap, heapSize, inputTuple); // Decreasing the maximum value to 5

        printf("decreaseMax %f\n", Input[i]);

        #ifdef DEBUG 
        printf("------Max-Heap Tree------ ");
        if( isMaxHeap( heap, heapSize ) )
            printf( "is a heap!\n" );
        else
            printf( "is NOT a heap!\n" );
        #endif   
    }

    //          ● Ao final, o Max-Heap h contém o subconjunto de k
    //          menores elementos de C
    drawHeapTree( heap, heapSize, k );


    return heap; 
}

int main (int argc, char *argv[]) {
    int inputSize = 0;
    chronometer_t runningTime; 


    if( argc != 4 ) {
            printf( "usage: %s <nTotalElements> <k> <nThreads>\n" ,
                argv[0] ); 
        return 0;
    } else {
        nThreads = atoi( argv[3] );
        if( nThreads == 0 ) {
            printf( "usage: %s <nTotalElements> <k> <nThreads>\n" ,
                argv[0] );
            printf( "<nThreads> can't be 0\n" );
            return 0;
        }     
        nTotalElements = atoi( argv[1] ); 
        if( nTotalElements > MAX_TOTAL_ELEMENTS ) {  
            printf( "usage: %s <nTotalElements> <k> <nThreads>\n" ,
                argv[0] );
            printf( "<nTotalElements> must be up to %d\n", MAX_TOTAL_ELEMENTS );
            return 0;
        }

        k = atoi( argv[2] ); 
    }
    // usage: ./acharKMenores <nTotalElements> <k> <nThreads>
    // k elementos
    
    chrono_reset( &runningTime );

    for( int i = 0; i < nTotalElements; i++ ){
     
        int a = rand();  // Returns a pseudo-random integer
                        //    between 0 and RAND_MAX.
        int b = rand();  // same as above
        
        float v = a * 100.0 + b;
        int p = inputSize;

            // inserir o valor v na posição p
        Input[ p ] = v;
        ++inputSize;
    }


    findKLeastProgram();
    // Measuring time after threads finished...
    chrono_stop( &runningTime );




    chrono_reportTime( &runningTime, "runningTime" );
    
    // calcular e imprimir a VAZAO (numero de operacoes/s)

    //  A vazão deve ser reportada em MOPs (Mega Operacoes por segundo)
    //  que é o numero de operações de insersão+decreaseMax
    //  feitas por segundo no Max-Heap pelo seu algoritmo paralelo para
    //  uma dada quantidade de threads.

    double total_time_in_seconds = (double) chrono_gettotal( &runningTime ) /
                                      ((double)1000*1000*1000);
    printf( "total_time_in_seconds: %lf s\n", total_time_in_seconds );
                                  
    double OPS = (nTotalElements)/total_time_in_seconds;
    printf( "Throughput: %lf OP/s\n", OPS );

    // verifyOutput( Input, Output, nTotalElements, k );
    return 1; 
}