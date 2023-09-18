ci1316/ci316  - PROGRAMAÇÃO PARALELA - 
2o semestre de 2023    
por W.Zola/UFPR

Lab 1: versão paralela para algoritmo 
       kMenoresElementos de um conjunto   (versão 1.3)  
----------------------------------
Histórico:
- v1.0 a versão inicial
- v1.1 Corrigida a especificação da entrada para o algoritmo,
       melhoradas e aumentadas algumas descrições,
       descrito um possível pseudo-algoritmo para 
       o código de verificaçoes
- v1.2 Pequena correção na especificação da 
       entrada para o algoritmo  
- v1.3 Pequenas correções no nome das variáveis das descrições
       mudando de v para e 
       (para nao confundir com o par (chave, valor)
       
----------------------------------  
Data do enunciado: 11/set/2023
Data da entrega Lab1: 27/set/2023 (será aproximadamente 2semanas, 
                                   um pouco mais!)
Você pode fazer sozinho OU em dupla.
NÃO serão aceitos trabalhos com mais de 2 alunos.
---------------------------------

Objetivos: 
// Obter uma implementação paralela eficiente para 
//   esse algoritmo "acharKMenores"
//   em CPUs multicore usando PThreads

// dada uma uma implementação sequencial inicial do
//   fornecida pelo prof. voce deve produzir uma 
//   implementação paralela eficiente em linguagem C 
//   usando PThreads para 
//   esse algoritmo "acharKMenores"


// Podemos definir o problema "acharKMenores" da 
//   seguinte maneira:
// 
//   Dado um vetor V de n valores, queremos obter o
//   subconjunto S com os k menores elementos de C,
//   bem como obter a posição desses elementos no vetor V.
//   (Obs: supondo k <= n)

// Entrada para o algoritmo:
//   - Trabalharemos com um vetor de entrada Input,
//       de valores float (32 bits)
//   - Esse vetor de entrada deve ser preenchido com valores
//   (pseudo)-aleatórios conforme descrição abaixo.

// O algoritmo acharKMenores deve trabalhar com heaps
// do tipo Max-Heap 
// (ver PDF descrevendo as operações necessárias)
//
// Cada Max-Heap deve sem implementado em estutura
// implícita, ou seja, os heaps NÃO devem ser implementados
// com pointers, mas com uma estrutura em que os nodos
// tem posições implícitas, 
// i.e. 
// dada a posição de um nodo na estrutura é possível 
// determinar a posição dos seus filhos (esquerdo e direito).

// Cada elemento nesses Max-Heaps deve ser um par
// (chave,valor)
// ou seja, cada elemento em um Max-Heap deve ser:
//
//       um par (chave, valor) onde:
//       a chave é um número float (de 32 bits)
//       o valor é um número int   (de 32 bits)

// Assim, ao se tomar um elemento e da posição p 
// do vetor de entrada, devemos inserir
// um par (e, p) em um Max-Heap, 
// ou seja, 
//          e é usado com chave e 
//          p é usado como valor

// OBS: cada nodo de um Max-Heap deve ter,
//      *no mínimo* um par de valores (e,p)
//      voce pode decidir colocar mais coisas
//      nos nodos dos heaps, se achar adequado;

// Voce deve entregar a sua nova versão sequencial,
//   bem como sua versão paralela que deve funcionar
//   criando um número qualquer de threads 
//   (variável nThreads, definida via linha de comando)

// Funcionamento:
//  O programa deve funcionar para 
//   nTotalElements com nThreads 
//   SENDO: nTotalElements e nThreads 
//          obtidos da linha de comando
//  
//  Utilização do programa:
//  usage: ./acharKMenores <nTotalElements> <k> <nThreads>

// ENTRADA para o algoritmo:
// A entrada para a função acharKMenores será: 
//   um vetor (GLOBAL) de floats com nTotalElements, 
//   cada elemento deve ser um valor float
// Esse conjunto de entrada, deve ser chamado Input
//
// Para esse trabalho o conjunto de valores de entrada 
// NÃO será lido, 
//   - o conjunto será preenchido sequencialmente 
//     pela função main
//   - a inicializaçao do conjunto de entrada (em main)
//     deve ser
//
//   // initialize Input vector
     float Input[MAX_SIZE];
     int inputSize = 0;
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
// Note que: ao final da inicialização acima, 
//    o vetor Input deve conter nTotalElements float
// ou seja, inputSize deve ser igual a nTotalElements    

// SAIDA do o algoritmo:
// A saída da função acharKMenores será: 
//   um conjunto de k elementos do tipo (chave, valor)
//   conforme especificado acima,
//   esse conjunto de saída deve ser chamado de Output
//   (OBS: k obtido da linha de comando)
//
// Conforme especificado anteriormente,
// cada par (e, p) inserido um Max-Heap, 
// deve reprensentar 
//     um valor e presente na posição p
//     do vetor de entrada (Input)
 


// o programa deve calcular e imprimir 
//   o tempo e a vazão do algoritmo acharKMenores usando
//   a inicializacao de um Max-Heap de tamanho K
//   a VERSÂO 1 do algoritmo usando a operação decreaseMAX
//   e as idéias descritas
//   no arquivo : 
//     max-heap-descricao-e-examplos.pdf
//
//  A vazão deve ser reportada em MOPs (Mega Operacoes por segundo)
//  que é o numero de operações de insersão+decreaseMax
//  feitas por segundo no Max-Heap pelo seu algoritmo paralelo para
//  uma dada quantidade de threads.
//
//  OBSERVAÇÕES: 
//  Se for conveniente para maior eficiência:
//    - voce pode usar mais de um Max-Heap
//  Voce pode (ou não) usar o que for necessário dos tópicos estudados
//  para implementar, ou seja:
//     operações atômicas, locks, spinlocks, barreiras
//     e/ou outras primitivas de Pthreads que forem adequadas
//     para melhor desempenho.
//
//  A inserçao inicial dos K primeiro elementos no Max-Heap
//  pode ser feita sequencialmente. Voce NÃO precisa paralelizar
//  essas k operações de inserção.
//
//  Note que o Max-Heap deve inserir elementos do tipo
//  (chave, valor) no heap, fazendo as 
//  operações de comparação dos algoritmos usando
//  as chaves (que são valores float) 
//  e "carregando" junto os valores de cada par (chave, valor)


// Verificaçao de correção do programa: 
// ------------------------------------
// 
// o programa main, antes de terminar, deve verificar
//    se seu algoritmo paralelo está gerando corretamente o 
//    conjunto de saída
//    essa verificação DEVE ser feita de forma sequencial 
//    (ao final do main), INCLUINDO-SE O CÓDIGO ABAIXO
//    esse código roda apenas ao final, e NÃO deve influenciar
//    na medida de tempo feita para o algoritmo paralelo e
//    nem deve influenciar no cálculo e resultado de VAZAO reportada

typedef struct {
    float key;   // inserir um valor v float na chave 
                 //  (obtido do vetor de entrada Input)
    int val;     // inserir a posiçao p como valor val
} pair_t;

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


// O código ACIMA DEVE SER chamado ao final de main
   da seguinte maneira para verificar que seus algoritmos funcionam:
   
   verifyOutput( Input, Output, nTotalElements, k );


// rodar o programa 10 vezes obtendo o tempo MÍNIMO e o MÉDIO
//  colocar TODOS os resultados em planilha

// Colocar em uma planilha (tabela) os tempos para
// 1, 2, 3, 4, 5, 6, 7, e 8 threads
// A última coluna da tabela (planilha) deve calcular 
//  a aceleração para 1, 2, 3, 4, 5, 6, 7, e 8 threads
// colocando em uma planilha (tabela)

// Entregar:
//  um tar.gz contendo:
//  -  o fonte da sua implementação em C, 
//  -  os scripts para compilar e rodar as experiências
//  -  a planilha preenchida com dados conforme descrito acima
//  -  Um arquivo com seu relatório descrevendo
//
//       a) como você implementou (descrever seu algoritmo)
//
//       b) a descrição do processador que voce usou, 
//          seu modelo e caracteristicas importantes para o experimento
//          COLOQUE EM APENDICE NO RELATORIO A SAIDA DO COMANDO lscpu

//          de preferência adicione também uma figura da 
//          topologia dos cores do processador obtida pelo programa lstopo

//       c) a descrição dos experimentos e como foram feitas as medidas
//       
//       d) a planilha de resultados sumarizando a vazao e aceleração
//       e) um gráfico (obtido de sua planilha) mostrando:
//            - no eixo X: o número de threads
//            - no eixo Y da esquerda: a vazao para cada número de threads
//                unir os pontos com linhas de uma certa cor C1 de sua preferência
//            - no eixo Y da direita: a aceleração para cada número de threads
//                unir os pontos com linhas de uma OUTRA cor C2 de sua preferência 

ENTREGA:
Em princípio o tar.gz deve ser entregue via upload na ufpr virtual
O professor deverá incluir (em breve) a opção para entrega do lab2 na ufpr virtual

-------------------------------------------------
------- PASSOS PARA FAZER ESSE TRABALHO: -----------
O professor vai fornecer: 
 - Um programa sequencial main com uma implementação 
   sequencial do algoritmo a ser trabalhado
 - o script para rodar os experimentos
 - um Makefile exemplo do professor
 - uma planilha exemplo para voce rodar seus experimentos
   e incluir seus dados na pagina dadosV1
   ao incluir os seus dados a planilha já tem os gráficos
   como incluir?
   * compile com comando make
   * rode o script do professor chamado roda.sh em uma das máquinas do cluster Xeon
     Somente serão aceitos resultados rodados no cluster Xeon
     (durante seus testes voce DEVE ter um mínimo de
      processos rodando para não atrapalhar suas medidas, 
     MAS, 
     para rodar e obter resultados finais você deve
     rodar em uma maquina do Cluster de maneira exclusiva,
     ou seja, 1 usuário por máquina
     Para fazer isso, o professor disponibilizará na 
     UFPR virtual as instrucoes para rodar no 
     sistema de filas do cluster em modo exclusivo)
     assim: 
     
         #rode com 10milhoes de elementos na máquina assim:
         ./roda.sh 10000000 
         
         
     esse script vai produzir a saida das experiencias para
     1 a 8 threads (10 vezes cada)
     Voce deve copiar (contol-C) a saida do script 
       na página dadosV1 na primeira celula dessa pagina
       fazer (control-V nessa célula)
       MARCANDO PARA A PLANILHA USAR COMO SEPARADORES OS CARACTERES []

     Ao fazer isso as tabelas estarão ok, e as cẽlulas também
     certifique-se que está tudo certo
     
     Aí basta voce ajustar as escalas dos gráficos 
     - clicando com mouse os eixos y (esquerdo ou direito) para
     e adequar as escalas, o prof. vai mostra na sala

