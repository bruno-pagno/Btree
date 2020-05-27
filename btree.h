#ifndef B_TREE
#define B_TREE

#define M 3

typedef struct rrn_nusp {
	int nusp;
	long RRN;
} RRN_NUSP;

typedef struct _node {
	int	nElementos;				/* numero de elementos inseridos < M */
	RRN_NUSP rrn_nusps[M - 1];	/* nusps e rrn dos elementos */
	struct _node *ponteiros[M];	/* ponteiros dos filhos */
} NODE;

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
void display(NODE *, int);
void delNode(int);
long search(RRN_NUSP);
int _insert(NODE *, RRN_NUSP, RRN_NUSP *, NODE **);
int searchPos(RRN_NUSP,RRN_NUSP *, int, long *);
/* KeyStatus del(NODE *r, int x); */
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

#endif