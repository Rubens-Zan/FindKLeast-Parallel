#include <stdio.h>
#include <pthread.h>

#include <unistd.h>
#include <sys/syscall.h>

#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>

#include "chrono.h"
#include "max-heap.h"



#include "findKLeast.h"
#define MAX_THREADS 64


#define MIN(a, b) ((a < b ? a:b))


/**GLOBALS**/
pthread_t parallelFindKLeast_Thread[ MAX_THREADS ];
int nTotalElements, k, nThreads; 
float Input[MAX_SIZE];
pair_t Output[MAX_SIZE];
pair_t parallel_output[MAX_SIZE];
pthread_barrier_t parallelFindKLeast_barrier;

pair_t parallelFindKLeast_partial[MAX_THREADS]; 
int parallelFindKLeast_thread_id[ MAX_THREADS ];
int parallelFindKLeast_nTotalElements;
int parallelFindKLeast_nThreads;
/**GLOBALS**/

#if TYPE == FLOAT
   #define MAX_TOTAL_ELEMENTS (500*1000*1000)  // if each float takes 4 bytes
                                            // will have a maximum 500 million FLOAT elements
                                            // which fits in 2 GB of RAM
#endif   

int cmpfunc (const void * a, const void * b) {
    float fa = *(const float*) a;
    float fb = *(const float*) b;
    return (fa > fb) - (fa < fb);
}


void verifyOutput( 
    const float *Input, 
    const pair_t *Output,   // pair_t é o tipo de um par (v,p)
    int nTotalElmts,
    int k
)
{
    qsort(Input, nTotalElements, sizeof(float), cmpfunc);
    #ifdef DEBUG
    for (int i=0;i < k;++i){
        printf("INPUT[%d]: %f \n", i,Input[i]);
        printf("OUTPUT[%d]: %f \n", Output[i].val,Output[i].key);
    }
    #endif
    int foundElements = 0;
    for (int i=0;i < k;++i){
        for (int j = 0 ;j < k;++j)
            if (Input[i] == Output[j].key)
                ++foundElements;
    }
    if( foundElements == k )
       printf( "\nOutput set verifyed correctly %d.\n",foundElements );
    else
       printf( "\nOutput set DID NOT compute correctly  %d !!!\n",foundElements );   
}





pair_t * findKLeastProgram(
    const float *Input, 
    const pair_t *Output,   // pair_t é o tipo de um par (v,p)
    int nTotalElmts,
    int k
){
//     Podemos aplicar uma estrutura de dados do tipo Max-Heap
//      e a operação decreaseMax no seguinte problema:
//      Dado um conjunto C de n valores, queremos obter o
//      subconjunto S com os k menores elementos de C.
// ● Obs: supondo k <= n
//       Um algoritmo possível para resolver o problema acima
//      seria:
//          ● Extrair k elementos de C e criar um Max-Heap h com
//          esses elementos.
    // pair_t * heap = malloc(sizeof(pair_t) * k);
    int heapSize;
    pair_t *outputPortion = malloc( sizeof(pair_t) * k ); 
    heapSize = 0;
    
    for( int i=0; i< k; i++ ) {
      printf("inserting %f\n", Input[i]);
      pair_t inputTuple; 
      inputTuple.key = Input[i];
      inputTuple.val = i;
      insert( (pair_t *) outputPortion, &heapSize, inputTuple );
      
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
        decreaseMax(outputPortion, heapSize, inputTuple); 

        printf("decreaseMax %f \n", Input[i]);

        #ifdef DEBUG 
        printf("------Max-Heap Tree------ ");
        if( isMaxHeap( outputPortion, heapSize ) )
            printf( "is a heap!\n" );
        else
            printf( "is NOT a heap!\n" );
        #endif   
    }


   

    //          ● Ao final, o Max-Heap h contém o subconjunto de k
    //          menores elementos de C
    drawHeapTree( outputPortion, heapSize, k );
    if( !isMaxHeap( outputPortion, heapSize ) )
        printf("NÃO É UMA HEAP");
    else 
        printf("É UMA HEAP");


    return outputPortion; 
}

void *findKLeastPartialElmts(void *ptr)
{
    int myIndex = *((int *)ptr);
    int nElements = nTotalElements / nThreads;
    
    // assume que temos pelo menos 1 elemento por thhread
    int first = myIndex * nElements;
    int last = MIN( (myIndex+1) * nElements, nTotalElements ) - 1;

    #if DEBUG == 1
      printf("thread %d here! first=%d last=%d\n", myIndex, first, last );
    #endif
    
//    if( myIndex != 0 )
    while( 1 ) {
    
        // all worker threads will be waiting here for the caller thread
        pthread_barrier_wait( &parallelFindKLeast_barrier );    
        
        for (int i=0; i<k ; i++){
            // insert first k elements
        }
       for( int i=first+k; i<=last ; i++ ){
        //    DECREASE with the elements in range
       }

       // store my result in the array of partial found k least elements

       parallelFindKLeast_partial[ myIndex ] = findKLeastProgram();     
        
       pthread_barrier_wait( &parallelFindKLeast_barrier );    
       if( myIndex == 0 )
          return NULL;           // return to caller thread
          
    }
    
    // NEVER HERE!
    if( myIndex != 0 )
          pthread_exit( NULL );
}


pair_t * parallel_findKLeast( 
    const float *Input, 
    const pair_t *Output,   // pair_t é o tipo de um par (v,p)
    int nTotalElmts,
    int k, 
    int nThreads)
{

    static int initialized = 0;
    int parallelFindKLeast_nTotalElements = nTotalElmts;
    int parallelFindKLeast_nThreads = nThreads;
    
    if( ! initialized ) { 
       pthread_barrier_init( &parallelFindKLeast_barrier, NULL, nThreads );
       // thread 0 will be the caller thread
    
       // cria todas as outra threds trabalhadoras
       parallelFindKLeast_thread_id[0] = 0;
       for( int i=1; i < nThreads; i++ ) {
         parallelFindKLeast_thread_id[i] = i;
         pthread_create( &parallelFindKLeast_Thread[i], NULL, 
                      findKLeastPartialElmts, &parallelFindKLeast_thread_id[i]);
       }

       initialized = 1;
    }

    // above, int this version, all other worker threads from 1 to nThreads will 
    //   start working imediatelly (no barriers to start working)
    
    // caller thread will be thread 0, and will start working on its chunk
    findKLeastPartialElmts( &parallelFindKLeast_thread_id[0] ); 
        
    // chegando aqui todas as threads sincronizaram, 
    //  na barreira no final da funçao findKLeastPartialElmts (até a 0)
    //  entao o vertor de somasPartcias estah pronto
    
    // a thread chamadora faz, entao, a reduçao da soma global
    // EFETUAR CONCATENCAO
    // isso é necessário ?
    //pthread_barrier_destroy( &myBarrier );
    
    // obs: como as threads trabalhadoras sincronizaram e irão terminar,
    //      não é necessário esperar o término delas
    
    // return globalSum; 
    // RETORNAR OBJETO CONCATENADO
}





float parallel_FindKLeastSum( float InputVector[], int nTotalElements, int nThreads )
{

    static int initialized = 0;
    parallelFindKLeast_nTotalElements = nTotalElements;
    parallelFindKLeast_nThreads = nThreads;
    
    if( ! initialized ) { 
       pthread_barrier_init( &parallelFindKLeast_barrier, NULL, nThreads );
       // thread 0 will be the caller thread
    
       // cria todas as outra threds trabalhadoras
       parallelFindKLeast_thread_id[0] = 0;
       for( int i=1; i < nThreads; i++ ) {
         parallelFindKLeast_thread_id[i] = i;
         pthread_create( &parallelFindKLeast_Thread[i], NULL, 
                      findKLeastPartialElmts, &parallelFindKLeast_thread_id[i]);
       }

       initialized = 1;
    }

    // above, int this version, all other worker threads from 1 to nThreads will 
    //   start working imediatelly (no barriers to start working)
    
    // caller thread will be thread 0, and will start working on its chunk
    findKLeastPartialElmts( &parallelFindKLeast_thread_id[0] ); 
        
    // chegando aqui todas as threads sincronizaram, 
    //  na barreira no final da funçao findKLeastPartialElmts (até a 0)
    //  entao o vertor de somasPartcias estah pronto
    
    // a thread chamadora faz, entao, a reduçao da soma global
    
    // CONCATENAR 
    // float globalSum = 0;
    // for( int i=0; i<nThreads ; i++ ) {
    //     //printf( "globalSum = %f\n", globalSum );
    //     globalSum += parallelFindKLeast_partialSum[i];
    // }    
    
    // isso é necessário ?
    //pthread_barrier_destroy( &myBarrier );
    
    // obs: como as threads trabalhadoras sincronizaram e irão terminar,
    //      não é necessário esperar o término delas
    
    // return globalSum;
    float teste;
    return teste; 
}


pair_t * concatenateOutputPortions(
    const pair_t *Output,
    pair_t **outputPortions,
    int nThreads,
    int k
){
    pair_t concatenatedOutput[k];
    int idx = 0;
    int heapSize = 0;
    
    for (int i=0;i < k;++i){
        insert( (pair_t *) concatenatedOutput, &heapSize, outputPortions[0][i] );
    }
    for (int i=1;i < nThreads;++i){ // faz o decreasce a partir da 2a thread
        for (int j=0;j < k;++j){ // cada thread tera uma porção de K menores considerando as porções que cada uma é responsável
            decreaseMax(concatenatedOutput, heapSize, outputPortions[i][j]); 
        }
    }



}

int main (int argc, char *argv[]) {
    int inputSize = 0;
    chronometer_t runningTime; 
    int USAR_PARALELO = 1;

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
        }else if (nThreads == 1){
            USAR_PARALELO = 0; 
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

    pair_t *outputPortions[nThreads];
    int parallelFindKLeast_thread_id[nThreads];
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


    for( int n = 0 ; n < nTotalElements; n++ ) {   
      printf("%f ", Input[n]);
    }

    printf("\n\n");

    for (int i=0;i < nThreads;++i){
        // TODO CHECK IF IS CORRECT
        const float *InputPortion;

        if (USAR_PARALELO){
            outputPortions[i] = parallel_findKLeast(Input, Output, nTotalElements, k, nThreads); 
        } else{
            outputPortions[i] = findKLeastProgram(Input, Output,nTotalElements, k);
        } 
    }


    // jam from the calculation if the number is not multiple from nThreads

    // SE FOR PARALELO FAZER CONCATENACAO E ORDENACAO
    // concatenateOutputPortions(Output,outputPortions, nThreads,k );

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


    // verifyOutput(Input, outputPortions[0], nTotalElements, k);

    return 1; 
}