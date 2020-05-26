#ifndef B_TREE
#define B_TREE

#define M 3

typedef struct rrn_nusp {
	int nusp;
	long RRN;
} RRN_NUSP;

typedef struct _node {
	int	nElementos;				// numero de elementos inseridos < M
	RRN_NUSP rrn_nusps[M - 1];	// nusps e rrn dos elementos
	struct _node *ponteiros[M];	// ponteiros dos filhos
} node;

// Defines utilizadas na insercao
#define chave_duplicada		-1
#define sucesso				-2
#define realizar_insercao	-3
#define LessKeys			-4
#define SearchFailure		-5

int showMenu();
void insert(int key);
void display(node *root, int);
void DelNode(int x);
void search(RRN_NUSP key);
int ins(node *r, RRN_NUSP x, RRN_NUSP* y, node** u);
int searchPos(RRN_NUSP key,RRN_NUSP *key_arr, int n, long * RRNFound);
// KeyStatus del(node *r, int x);
void eatline(void);
void inorder(node *ptr);
int totalKeys(node *ptr);
void printTotal(node *ptr);
int getMin(node *ptr);
int getMax(node *ptr);
void getMinMax(node *ptr);
int max(int first, int second, int third);
int maxLevel(node *ptr);
void printMaxLevel(node *ptr);

#endif