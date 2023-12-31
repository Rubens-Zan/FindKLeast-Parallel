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
#define MAX_K_ELEMENTS 2048

#define MIN(a, b) ((a < b ? a:b))

#if TYPE == FLOAT
   #define MAX_TOTAL_ELEMENTS (2000*1000*1000)  // 200 mi of elements

#endif   

/**GLOBALS**/
int nTotalElements, k, nThreads; 
pthread_barrier_t parallelFindKLeast_barrier;

pthread_t parallelFindKLeast_Thread[MAX_THREADS];
float *Input;
pair_t *Output;
pair_t parallelFindKLeast_partialOutput[MAX_THREADS][MAX_K_ELEMENTS]; 
int *parallelFindKLeast_thread_id;
int parallelFindKLeast_nTotalElements;
int parallelFindKLeast_nThreads;
/**GLOBALS**/



int cmpfunc (const void * a, const void * b) {
    float fa = *(const float*) a;
    float fb = *(const float*) b;
    return (fa > fb) - (fa < fb);
}


void verifyOutput( 
    float *Input, 
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
        for (int j = 0;j < k;++j)
            if (Input[i] == Output[j].key)
                ++foundElements;
    }
    if( foundElements == k )
       printf( "\nOutput set verifyed correctly %d.\n",foundElements );
    else
       printf( "\nOutput set DID NOT compute correctly  %d !!!\n",foundElements );   
}



void findKLeastProgram(
    int myIndex
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
    int heapSize;
    heapSize = 0;
    int nElements = nTotalElements / nThreads;
    
    int firstFromRange = myIndex * nElements;
    int lastFromRange = MIN( (myIndex+1) * nElements, nTotalElements ) - 1;

    
    // insert firstFromRange k elements from the range
    for( int i=firstFromRange; i< firstFromRange + k; i++ ) {
        pair_t inputTuple; 
        inputTuple.key = Input[i];
        inputTuple.val = i;

        insert( (pair_t *) parallelFindKLeast_partialOutput[myIndex], &heapSize, inputTuple );
        
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
    for (int i = firstFromRange + k;i <= lastFromRange;++i){
        pair_t inputTuple; 
        inputTuple.key = Input[i];
        inputTuple.val = i;

        decreaseMax(parallelFindKLeast_partialOutput[myIndex], heapSize, inputTuple); 

        #ifdef DEBUG 
        printf("------Max-Heap Tree------ ");
        if( isMaxHeap( parallelFindKLeast_partialOutput[myIndex], heapSize ) )
            printf( "is a heap!\n" );
        else
            printf( "is NOT a heap!\n" );
        #endif   
    }

}

void *findKLeastPartialElmts(void *ptr)
{
    int myIndex = *((int *)ptr);

    // store my result in the array of partial found k least elements
    findKLeastProgram(myIndex);     
    pthread_barrier_wait( &parallelFindKLeast_barrier );    
    
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
    pthread_barrier_init( &parallelFindKLeast_barrier, NULL, nThreads );

    static int initialized = 0;
    int parallelFindKLeast_nTotalElements = nTotalElmts;
    int parallelFindKLeast_nThreads = nThreads;
    

    // cria todas as outra threds trabalhadoras
    parallelFindKLeast_thread_id[0] = 0;
    for( int i=1; i < nThreads; i++ ) {
        parallelFindKLeast_thread_id[i] = i;
        pthread_create( &parallelFindKLeast_Thread[i], NULL, 
                    findKLeastPartialElmts, &parallelFindKLeast_thread_id[i]);
    }


    // above, int this version, all other worker threads from 1 to nThreads will 
    //   start working imediatelly (no barriers to start working)
    
    // caller thread will be thread 0, and will start working on its chunk
    findKLeastPartialElmts( &parallelFindKLeast_thread_id[0] ); 
        
    // chegando aqui todas as threads sincronizaram, 
    //  na barreira no final da funçao findKLeastPartialElmts (até a 0)
    //  entao as outras heaps estao prontas
    concatenateOutputPortions(); 
}


void concatenateOutputPortions(){
    int heapSize = 0;


    for (int i=0;i < k;++i){
        insert( (pair_t *) Output, &heapSize, parallelFindKLeast_partialOutput[ 0 ][i] );
    }

    for (int idx=1;idx < nThreads;++idx){ // faz o decreasce a partir da 2a thread

        for (int j=0;j < k;++j){ // cada thread tera uma porção de K menores considerando as porções que cada uma é responsável
            decreaseMax((pair_t *) Output, heapSize, parallelFindKLeast_partialOutput[ idx][j]); 
        }
    }

    #ifdef DEBUG

    for (int  i = 0;i < nThreads;++i){
        printf("Heap parciais finais: \n"); 
        drawHeapTree( parallelFindKLeast_partialOutput[ i], heapSize, k );
    }

    printf("Heap global final: \n"); 
    drawHeapTree( Output, heapSize, k );

    #endif


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
            printf( "<nTotalElements> must be up to %d\n", MAX_TOTAL_ELEMENTS );
            return 0;
        }
        if (k > MAX_K_ELEMENTS){
            printf( "<k> must be up to %d\n", MAX_K_ELEMENTS );
            return 0;
        }
        k = atoi( argv[2] ); 
    }

    // usage: ./acharKMenores <nTotalElements> <k> <nThreads>
    // k elementos
   
    
    Input = malloc(nTotalElements * sizeof(pair_t));
    Output = malloc(k * sizeof(pair_t));
    parallelFindKLeast_thread_id = malloc(nThreads * sizeof(int));




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

    
    chrono_reset( &runningTime );
    chrono_start( &runningTime );

    #ifdef DEBUG

    for( int n = 0 ; n < nTotalElements; n++ ) {   
      printf("%f ", Input[n]);
    }
    printf("\n\n");
    
    #endif
    
    parallel_findKLeast(Input, Output, nTotalElements, k, nThreads); 


    // Measuring time after threads finished...
    
    chrono_stop( &runningTime );
    
    verifyOutput(Input, Output, nTotalElements, k);

    chrono_reportTime( &runningTime, "runningTime" );
    
    // calcular e imprimir a VAZAO (numero de operacoes/s)

    //  A vazão deve ser reportada em MOPs (Mega Operacoes por segundo)
    //  que é o numero de operações de insersão+decreaseMax
    //  feitas por segundo no Max-Heap pelo seu algoritmo paralelo para
    //  uma dada quantidade de threads.

    double total_time_in_seconds = (double) chrono_gettotal( &runningTime ) /
                                      ((double)1000*1000*1000);
    printf( "\nTotal time in seconds: %lf s\n", total_time_in_seconds );
    
          
    double OPS = (nTotalElements)/total_time_in_seconds;
    printf( "Throughput: %lf MOPs/s\n", OPS );
    


    return 1; 
}