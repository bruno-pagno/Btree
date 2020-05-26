#include "stdio.h"
#include "stdlib.h"

#define M 3

typedef struct rrn_nusp {
		int nusp;
		long RRN;
} RRN_NUSP;

typedef struct _node {
	int         nElementos; /* n < M No. of keys in node will always less than order of B tree */
	RRN_NUSP    nusps[M - 1]; /*array of keys*/
	struct _node *ponteiros[M]; /* (n+1 pointers will be in use) */
} node;

node *root = NULL;

typedef enum KeyStatus {
	Duplicate,
	SearchFailure,
	Success,
	InsertIt,
	LessKeys,
} KeyStatus;

void insert(int key);
void display(node *root, int);
void DelNode(int x);
void search(RRN_NUSP key);
int ins(node *r, RRN_NUSP x, RRN_NUSP* y, node** u);
int searchPos(RRN_NUSP key,RRN_NUSP *key_arr, int n, long * RRNFound);
KeyStatus del(node *r, int x);
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

int showMenu() {
	printf("\tQual Operação deseja Realizar ?\n");
	printf("\t1 - Inserir novos registros na árvore\n");
	printf("\t2 - Deletar nó\n");
	printf("\t3 - Buscar \n");
	printf("\t4 - Mostrar árvore\n");
	printf("\t5 - Sair do programa\n");
	printf("\tOpção: ");
	int res = 0;
	scanf("%d", &res);
	return res;
}

int main() {
	// Inicializando os arquivos
	FILE *arqIndex = fopen("index.dat" , "w+b");
	FILE *arqDados = fopen("dados.txt" , "w+b");
	if(!arqIndex || !arqDados) {
		printf("A abertura dos arquivos falhou\n");
		exit(0);
	}

	int chave = 0;
        int choice = 0;
	while (1) {
                choice = showMenu();
		switch (choice) {
			case 1: // Inserir nó
				printf("Enter the chave : ");
				scanf("%d", &chave); 
                                insert(chave);
				break;
			case 2:  // Deletar Nó
				printf("Enter the chave : ");
				scanf("%d", &chave); 
			        // DelNode(chave);
				break;
			case 3: // Buscar nó
				printf("Enter the chave : ");
				scanf("%d", &chave); 
                                RRN_NUSP aux;
                                aux.nusp = chave;
                                search(aux);
				break;
			case 4: // Mostrar árvore
				printf("Btree is :\n\n");
				display(root, 0);
				break;
			default:
				printf("Saindo ...\n");
				exit(1);
		}
	}
	
	// Dar free em tudo no fim da execução
	fclose(arqDados);
	fclose(arqIndex);
	printf("Fim da execução do programa ... \n");
	return 0;
}

long RRN = 0;



#define Duplicate 		-1
#define SearchFailure 	        -2
#define Success 		-3
#define InsertIt 		-4
#define LessKeys 		-5

void insert(int key) {
		RRN_NUSP new;
		new.nusp = key;		// nusp do novo aluno
		new.RRN = RRN++;	// rrn do arquivo de dados

		node *newnode;		// novo node, se necessario
		RRN_NUSP upKey;		// chave-pai
		int value = ins(root, new, &upKey, &newnode);
		
		if (value == Duplicate)
				printf("Chave duplicada\n");
		
		if (value == InsertIt) { // Primeiro nó
				node *uproot = root;
				root = (node*) malloc(sizeof(node));
				root->nElementos = 1;
				
				// root->nusps[0] = upKey;
				root->nusps[0] = upKey;
				root->ponteiros[0] = uproot;
				root->ponteiros[1] = newnode;
		}
}


int ins(node *root, RRN_NUSP key, RRN_NUSP *upKey, node **newnode) {
	node *newPtr, *lastPtr;
	int pos, i, nElementos, splitPos;
	RRN_NUSP newKey, lastKey;
	
		int value;

	if (root == NULL) {		// node mandado eh vazio
		*newnode = NULL;
		*upKey = key;
		return InsertIt;
	}

	nElementos = root->nElementos;

		long RRNaux;

	pos = searchPos(key, root->nusps, nElementos, &RRNaux); // Encontra a posição ideal para o NUSP dentro do nó

	if (pos < nElementos && key.nusp == root->nusps[pos].nusp) // Nesse caso já existe
		return Duplicate;

	value = ins(root->ponteiros[pos], key, &newKey, &newPtr); // Desce para o próximo nível recursivamente
	
	if (value != InsertIt) return value; // nao insere nesse node

	/*Se o número de NUSPS do NODE for menor que M - 1*/
	if (nElementos < M - 1) {
		pos = searchPos(newKey, root->nusps, nElementos, &RRNaux);
		/*Shifting the key and pointer right for inserting the new key*/
		for (i = nElementos; i > pos; i--) {
			root->nusps[i] = root->nusps[i - 1];
			root->ponteiros[i + 1] = root->ponteiros[i];
		}
		
		/*Key is inserted at exact location*/
		root->nusps[pos] = newKey;
		root->ponteiros[pos + 1] = newPtr;
		root->nElementos++; /*incrementing the number of keys in node*/
		return Success;
	}

	 /*Se o nó está cheio e a posição do elemento que deseja inserir é a ultima*/
	if (pos == M - 1) {
		lastKey = newKey;
		lastPtr = newPtr;
	} else { 
		/* Neste caso o nó está cheio, mas a posição que deseja inserir não é a ultima*/   
		lastKey = root->nusps[M - 2];
		lastPtr = root->ponteiros[M - 1];

		for (i = M - 2; i > pos; i--) {
			root->nusps[i] = root->nusps[i - 1];
			root->ponteiros[i + 1] = root->ponteiros[i];
		}
		root->nusps[pos] = newKey;
		root->ponteiros[pos + 1] = newPtr;
	}


	splitPos = (M - 1) / 2;                                   /* Posição do nó médio */
	(*upKey) = root->nusps[splitPos];
	(*newnode) = (node*) malloc(sizeof(node));               /* Right node after split */
	root->nElementos = splitPos;                              /* No. of keys for left splitted node */
	(*newnode)->nElementos = M - 1 - splitPos;               /*No. of keys for right splitted node*/
	
	for (i = 0; i < (*newnode)->nElementos; i++) {
		(*newnode)->ponteiros[i] = root->ponteiros[i + splitPos + 1];	
		if (i < (*newnode)->nElementos - 1){
			(*newnode)->nusps[i] = root->nusps[i + splitPos + 1];
		}
		else {
			(*newnode)->nusps[i] = lastKey;       
		}
	}
	(*newnode)->ponteiros[(*newnode)->nElementos] = lastPtr;
	return InsertIt;
}

void display(node *ptr, int blanks) {
	if (ptr) {
		int i;
		for (i = 1; i <= blanks; i++)
			printf(" ");
		for (i = 0; i < ptr->nElementos; i++)
			printf("%d(%ld) ", ptr->nusps[i].nusp, ptr->nusps[i].RRN);
		printf("\n");
		for (i = 0; i <= ptr->nElementos; i++)
			display(ptr->ponteiros[i], blanks + 10);
	}
}

void search(RRN_NUSP key) {
	int pos, i, n;
	node *ptr = root;
	printf("Search path:\n");
	while (ptr) {
		n = ptr->nElementos;
		for (i = 0; i < ptr->nElementos; i++)
			printf(" %d", ptr->nusps[i].nusp);
		printf("\n");

				long RRNaux;
		pos = searchPos(key, ptr->nusps, n, &RRNaux);
		if (pos < n && key.nusp == ptr->nusps[pos].nusp) {
			printf("Nusp %d achado na posição %d, com RRN %ld do ultimo nó mostrado\n", key.nusp, i, RRNaux);
			return;
		}
		ptr = ptr->ponteiros[pos];
	}
	printf("Key %d is not available\n", key.nusp);
}

int searchPos(RRN_NUSP key,RRN_NUSP *key_arr, int n, long * RRNFound) {
	int pos = 0;
	while (pos < n && key.nusp > key_arr[pos].nusp)
		pos++;
	if(pos < n){
		*RRNFound = key_arr[pos].RRN;
		} else {
		*RRNFound = -1;
		}
	return pos;
}

// void DelNode(int key) {
//         node *uproot;
//         KeyStatus value;
//         value = del(root, key);
//         switch (value) {
//         case SearchFailure:
//                 printf("Key %d is not available\n", key);
//                 break;
//         case LessKeys:
//                 uproot = root;
//                 root = root->ponteiros[0];
//                 free(uproot);
//                 break;
//         default:
//                 return;
//         }/*End of switch*/
// }/*End of delnode()*/

// KeyStatus del(node *ptr, int key) {
//         int pos, i, pivot, n, min;
//         int *key_arr;
//         KeyStatus value;
//         node **p, *lptr, *rptr;

//         if (ptr == NULL)
//                 return SearchFailure;
//         /*Assigns values of node*/
//         n = ptr->n;
//         key_arr = ptr->nusps;
//         p = ptr->ponteiros;
//         min = (M - 1) / 2;/*Minimum number of keys*/

//                                           //Search for key to delete
//         pos = searchPos(key, key_arr, n);
//         // p is a leaf
//         if (p[0] == NULL) {
//                 if (pos == n || key < key_arr[pos])
//                         return SearchFailure;
//                 /*Shift keys and pointers left*/
//                 for (i = pos + 1; i < n; i++)
//                 {
//                         key_arr[i - 1] = key_arr[i];
//                         p[i] = p[i + 1];
//                 }
//                 return --ptr->n >= (ptr == root ? 1 : min) ? Success : LessKeys;
//         }/*End of if */

//          //if found key but p is not a leaf
//         if (pos < n && key == key_arr[pos]) {
//                 node *qp = p[pos], *qp1;
//                 int nkey;
//                 while (1) {
//                         nkey = qp->n;
//                         qp1 = qp->ponteiros[nkey];
//                         if (qp1 == NULL)
//                                 break;
//                         qp = qp1;
//                 }/*End of while*/
//                 key_arr[pos] = qp->nusps[nkey - 1];
//                 qp->nusps[nkey - 1] = key;
//         }/*End of if */
//         value = del(p[pos], key);
//         if (value != LessKeys)
//                 return value;

//         if (pos > 0 && p[pos - 1]->n > min) {
//                 pivot = pos - 1; /*pivot for left and right node*/
//                 lptr = p[pivot];
//                 rptr = p[pos];
//                 /*Assigns values for right node*/
//                 rptr->ponteiros[rptr->n + 1] = rptr->ponteiros[rptr->n];
//                 for (i = rptr->n; i>0; i--) {
//                         rptr->nusps[i] = rptr->nusps[i - 1];
//                         rptr->ponteiros[i] = rptr->ponteiros[i - 1];
//                 }
//                 rptr->n++;
//                 rptr->nusps[0] = key_arr[pivot];
//                 rptr->ponteiros[0] = lptr->ponteiros[lptr->n];
//                 key_arr[pivot] = lptr->nusps[--lptr->n];
//                 return Success;
//         }/*End of if */
//          //if (posn > min)
//         if (pos < n && p[pos + 1]->n > min) {
//                 pivot = pos; /*pivot for left and right node*/
//                 lptr = p[pivot];
//                 rptr = p[pivot + 1];
//                 /*Assigns values for left node*/
//                 lptr->nusps[lptr->n] = key_arr[pivot];
//                 lptr->ponteiros[lptr->n + 1] = rptr->ponteiros[0];
//                 key_arr[pivot] = rptr->nusps[0];
//                 lptr->n++;
//                 rptr->n--;
//                 for (i = 0; i < rptr->n; i++) {
//                         rptr->nusps[i] = rptr->nusps[i + 1];
//                         rptr->ponteiros[i] = rptr->ponteiros[i + 1];
//                 }/*End of for*/
//                 rptr->ponteiros[rptr->n] = rptr->ponteiros[rptr->n + 1];
//                 return Success;
//         }/*End of if */

//         if (pos == n)
//                 pivot = pos - 1;
//         else
//                 pivot = pos;

//         lptr = p[pivot];
//         rptr = p[pivot + 1];
//         /*merge right node with left node*/
//         lptr->nusps[lptr->n] = key_arr[pivot];
//         lptr->ponteiros[lptr->n + 1] = rptr->ponteiros[0];
//         for (i = 0; i < rptr->n; i++) {
//                 lptr->nusps[lptr->n + 1 + i] = rptr->nusps[i];
//                 lptr->ponteiros[lptr->n + 2 + i] = rptr->ponteiros[i + 1];
//         }
//         lptr->n = lptr->n + rptr->n + 1;
//         free(rptr); /*Remove right node*/
//         for (i = pos + 1; i < n; i++) {
//                 key_arr[i - 1] = key_arr[i];
//                 p[i] = p[i + 1];
//         }
//         return --ptr->n >= (ptr == root ? 1 : min) ? Success : LessKeys;
// }/*End of del()*/

void eatline(void) {
	char c;
	while ((c = getchar()) != '\n');
}
