#ifndef B_TREE
#define B_TREE

#define M 3

typedef struct rrn_nusp { /* Descrição                       Peso em Bytes */
	int nusp;			  /* NUSP Do Aluno                           4     */
	long RRN;			  /* RRN do aluno no arquivo de Dados        8     */
} RRN_NUSP;
/* 								Peso total: 12 Bytes					   */

typedef struct _node {			/* Descrição                               Peso em Bytes */
	int	nElementos;				/* numero de elementos inseridos < M              4      */
	RRN_NUSP rrn_nusps[M - 1];	/* nusps e rrn dos elementos                   12*(M-1)  */
	
	// struct _node *ponteiros[M];	/* ponteiros dos filhos                        8 * M */
	
	int ponteiros[M];			/* ponteiros dos filhos                        8 * M     */
} NODE;
/* 								Peso total: 12 Bytes					   				 */


typedef struct aluno {			/* Struct do Aluno */
	int NUSP;
	char nome[15];
	char sobrenome[15];
	char curso[15];
	float nota;
} ALUNO;

/* Defines utilizadas na insercao */
#define CHAVE_DUPLICADA		-1
#define SUCESSO				-2
#define REALIZAR_INSERCAO	-3
#define ULTIMAS_CHAVES		-4
#define FALHA_BUSCA			-5

int showMenu();
ALUNO scanAluno();
void insertAluno(FILE *);
void searchAluno(FILE *);
void removeAluno();
void printAluno(ALUNO a);

void insert(int);
int _insert(NODE *, RRN_NUSP, RRN_NUSP *, NODE **);
void display(NODE *, int);
long search(RRN_NUSP);
int searchPos(RRN_NUSP,RRN_NUSP *, int, long *);
/*
void delNode(int);
KeyStatus del(NODE *r, int x);
void eatline();
void inorder(NODE *);
int totalKeys(NODE *);
void printTotal(NODE *);
int getMin(NODE *);
int getMax(NODE *);
void getMinMax(NODE *);
int max(int, int, int);
int maxLevel(NODE *);
void printMaxLevel(NODE *);
*/

#endif