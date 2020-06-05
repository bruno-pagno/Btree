/*
Nome: Bruno de Sousa Pagno		NUSP: 11366872
Nome: João Vitor de Mello Gomes	NUSP: 11218622
Professor: Elaine Parros Machado de Sousa
Disciplina: SCC0503
*/

#ifndef B_TREE
#define B_TREE

#define M 171	/* Ordem da arvore */
/* A ordem da arvore será 171, para que as páginas contenham 4096 bytes */

typedef struct rrn_nusp {	/* Struct de cada elemento				Peso em Bytes	*/
	int nusp;				/* NUSP Do Aluno								4		*/
	long RRN;				/* RRN do aluno no arquivo de Dados				8		*/
} RRN_NUSP;					/* Peso total:								16 bytes	*/

typedef struct _node {			/* Struct de cada pagina				Peso em Bytes	*/
	int nElementos;				/* numero de elementos inseridos < M		4			*/
	RRN_NUSP rrnNUSPs[M - 1];	/* nusps e rrn dos elementos				16*(M-1)	*/
	long ponteiros[M];			/* ponteiros dos filhos						8*M 		*/
} NODE;							/* Peso total:           					24*M-8		*/

typedef struct aluno {		/* Struct do Aluno		Peso em Bytes	*/
	int NUSP;				/* NUSP do aluno				04		*/
	char nome[15];			/* Nome do aluno				15		*/
	char sobrenome[15];		/* Sobrenome do aluno			15		*/
	char curso[15];			/* Curso do aluno				15		*/
	float nota;				/* Nota do aluno				04		*/
} ALUNO;					/* Peso total: 				56 Bytes	*/

/* Defines utilizadas na insercao e busca */
#define CHAVE_DUPLICADA		-1
#define SUCESSO				-2
#define REALIZAR_INSERCAO	-3
#define ULTIMAS_CHAVES		-4
#define FALHA_BUSCA			-5

/* Headers das funções*/
int showMenu();
ALUNO scanAluno();
void insertAluno(FILE *,  FILE *);
void searchAluno(FILE *, FILE *);
void fillBTree(FILE *, FILE *, int);
void printAluno(ALUNO);
int insert(int, FILE *);
int _insert(FILE *, int , RRN_NUSP , RRN_NUSP *, int *);
void display(FILE *, long, int);
long search(RRN_NUSP, FILE *);
int searchPos(RRN_NUSP,RRN_NUSP *, int, long *);

#endif