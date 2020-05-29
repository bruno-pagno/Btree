#include <stdio.h>
#include <stdlib.h>
#include <btree.h>

NODE *root = NULL;	/* raiz da árvore B */
long RRN = 0;		/* contador global para indicar o RRN atual */

int main() {
	/* inicializando os arquivos */
	FILE *arqIndex = fopen("index.dat" , "w+b");
	FILE *arqDados = fopen("dados.txt" , "w+b");
	if(!arqIndex || !arqDados) {
		printf("A abertura dos arquivos falhou\n");
		exit(0);
	}

	/* Variaveis auxiliares */
	int opcao = 0;
	while(opcao != 5) {
		opcao = showMenu();
		switch(opcao) {
			case 1: /* inserir aluno */
				insertAluno(arqDados);
				break;
			case 2:  /* remover aluno */
				removeAluno();
				break;
			case 3: /* buscar aluno */
				searchAluno(arqDados);				
				break;
			case 4: /* mostrar a arvore */
				printf("A Btree está assim:\n\n");
				display(root, 0);
				break;
		}
	}
	
	printf("Saindo...\n");
	
	/* dar free em tudo no fim da execução */
	fclose(arqDados);
	fclose(arqIndex);
	printf("Fim da execução do programa...\n");
	return 0;
}

/* exibe o menu com as opcoes */
int showMenu() {
	int res = 0;
	while(res < 1 || res > 5) {		/* evita opcoes invalidas */
		printf("\n");
		printf("|======================================|\n");
		printf("|    Qual Operação deseja realizar?    |\n");
		printf("|1 - Inserir novo aluno                |\n");
		printf("|2 - Remover aluno                     |\n");
		printf("|3 - Buscar por NUSP                   |\n");
		printf("|4 - Mostrar árvore                    |\n");
		printf("|5 - Sair do programa                  |\n");
		printf("|======================================|\n");
		printf("|Opção: ");
		scanf("%d", &res);
	}
	return res;
}

/* le os dados de um aluno */
ALUNO scanAluno() {
	ALUNO newAluno;

	printf("\n\tDigite os dados do aluno que deseja Inserir:\n");
	printf("\tNUSP do aluno: ");
	scanf("%d", &newAluno.NUSP);

	printf("\tNome do aluno: ");
	scanf(" %[^\n]s", newAluno.nome);

	printf("\tSobrenome do aluno: ");
	scanf(" %[^\n]s", newAluno.sobrenome);

	printf("\tCurso do aluno: ");
	scanf(" %[^\n]s", newAluno.curso);

	printf("\tNota do aluno: ");
	scanf("%f", &newAluno.nota);

	return newAluno;
}

/* das inicio a insercao */
void insertAluno(FILE *arqDados) {
	ALUNO newAluno = scanAluno();
	insert(newAluno.NUSP);

	/* Escrevendo o aluno na árvore */
	fwrite(&newAluno, 1, sizeof(ALUNO), arqDados);
}

/* da inicio a busca */
void searchAluno(FILE *arqDados) {
	int chave = 0;
	long rrnAux = 0;
	ALUNO newAluno;

	printf("\n\tDigite o NUSP que deseja buscar: ");
	scanf("%d", &chave);
	RRN_NUSP aux;
	aux.nusp = chave;
	rrnAux = search(aux);
	if(rrnAux != -1) {					
		printf("\n\tAluno encontrado no RRN %ld\n\n", rrnAux);
		/* Fazendo a leitura do aluno no arquivo de dados */
		fseek(arqDados, (rrnAux * sizeof(ALUNO)), SEEK_SET);
		fread(&newAluno, 1, sizeof(ALUNO), arqDados);
		printAluno(newAluno);

	} else
		printf("O Nusp buscado não existe na árvore\n");
}

/* exibe o aluno */
void printAluno(ALUNO aluno) {
	printf("\t    INFORMAÇÕES DO ALUNO\n");
	printf("\tNUSP: %d\n", aluno.NUSP);
	printf("\tNome Completo: %s %s\n", aluno.nome, aluno.sobrenome);
	printf("\tCurso: %s\n", aluno.curso);
	printf("\tNota: %.2f\n", aluno.nota);
}

/* recebe o nusp do aluno e insere */
void insert(int nusp) {
	RRN_NUSP novo;
	novo.nusp = nusp;	/* nusp do novo aluno */
	novo.RRN = RRN++;	/* rrn do arquivo de dados */

	NODE *novoNode = NULL;		/* novo node, se necessario */
	RRN_NUSP chaveCima;			/* chave-pai */
	int retorno = _insert(root, novo, &chaveCima, &novoNode);
	
	if (retorno == CHAVE_DUPLICADA)
		printf("Chave duplicada\n");
	
	if (retorno == REALIZAR_INSERCAO) {	/* Primeiro nó */
		NODE *upRoot = root;
		root = (NODE*) malloc(sizeof(NODE));
		root->nElementos = 1;
		root->rrn_nusps[0] = chaveCima;
		root->ponteiros[0] = upRoot;
		root->ponteiros[1] = novoNode;
	}
}

/* funcao interna que insere na arvore */
int _insert(NODE *root, RRN_NUSP rrn_nusp, RRN_NUSP *chaveCima, NODE **novoNode) {
	if (!root) {		/* node mandado eh vazio */
		*novoNode = NULL;
		*chaveCima = rrn_nusp;
		return REALIZAR_INSERCAO;
	}

	int nElementos = root->nElementos;
	long RRNaux = 0;
	int pos = searchPos(rrn_nusp, root->rrn_nusps, nElementos, &RRNaux);	/* Encontra a posição ideal para o NUSP dentro do nó */

	if(pos < nElementos && rrn_nusp.nusp == root->rrn_nusps[pos].nusp)	/* Nesse caso já existe */
		return CHAVE_DUPLICADA;

	RRN_NUSP novaChave;
	NODE *novoPtr = NULL;
	int retorno = _insert(root->ponteiros[pos], rrn_nusp, &novaChave, &novoPtr);		/* Desce para o próximo nível recursivamente */
	
	if (retorno != REALIZAR_INSERCAO) return retorno;	/* nao insere nesse node */

	/*Se o número de NUSPS do NODE for menor que M - 1*/
	int i = 0;
	if (nElementos < M - 1) {
		pos = searchPos(novaChave, root->rrn_nusps, nElementos, &RRNaux);
		/* shift dos elementos */
		for (i = nElementos; i > pos; i--) {
			root->rrn_nusps[i] = root->rrn_nusps[i - 1];
			root->ponteiros[i + 1] = root->ponteiros[i];
		}
		
		/* chave inserida na posicao */
		root->rrn_nusps[pos] = novaChave;
		root->ponteiros[pos + 1] = novoPtr;
		root->nElementos++;		/* incrementa o contador */
		return SUCESSO;
	}

	/*Se o nó está cheio e a posição do elemento que deseja inserir é a ultima*/
	RRN_NUSP ultimaChave;
	NODE *ultimoPtr = NULL;
	if (pos == M - 1) {
		ultimaChave = novaChave;
		ultimoPtr = novoPtr;
	} else {
		/* Neste caso o nó está cheio, mas a posição que deseja inserir não é a ultima*/  
		ultimaChave = root->rrn_nusps[M - 2];
		ultimoPtr = root->ponteiros[M - 1];

		for (i = M - 2; i > pos; i--) {
			root->rrn_nusps[i] = root->rrn_nusps[i - 1];
			root->ponteiros[i + 1] = root->ponteiros[i];
		}
		root->rrn_nusps[pos] = novaChave;
		root->ponteiros[pos + 1] = novoPtr;
	}

	int splitPos = (M - 1) / 2;							/* Posição do nó médio */
	(*chaveCima) = root->rrn_nusps[splitPos];
	(*novoNode) = (NODE*) malloc(sizeof(NODE));		/* node da direita */
	root->nElementos = splitPos;					/* numero de chaves (code esquerdo) */
	(*novoNode)->nElementos = M - 1 - splitPos;		/* numero de chaves (code direito) */
	
	for (i = 0; i < (*novoNode)->nElementos; i++) {
		(*novoNode)->ponteiros[i] = root->ponteiros[i + splitPos + 1];	
		if (i < (*novoNode)->nElementos - 1)
			(*novoNode)->rrn_nusps[i] = root->rrn_nusps[i + splitPos + 1];

		else (*novoNode)->rrn_nusps[i] = ultimaChave;
	}
	(*novoNode)->ponteiros[(*novoNode)->nElementos] = ultimoPtr;
	return REALIZAR_INSERCAO;
}

/* exibe a arvore na tela */
void display(NODE *node, int espacos) {
	if (node) {
		int i;
		for (i = 1; i <= espacos; i++)
			printf(" ");
		
		for (i = 0; i < node->nElementos; i++)
			printf("%d(%ld) ", node->rrn_nusps[i].nusp, node->rrn_nusps[i].RRN);
		printf("\n");

		for (i = 0; i <= node->nElementos; i++)
			display(node->ponteiros[i], espacos + 10);
	}
}

/* busca o node que pode conter o aluno */
long search(RRN_NUSP chave) {
	NODE *node = root;
	/* printf("Search path:\n"); */
	while (node) {
		int n = node->nElementos;
		long RRNaux = 0;
		int pos = searchPos(chave, node->rrn_nusps, n, &RRNaux);
		if (pos < n && chave.nusp == node->rrn_nusps[pos].nusp) {
			/* printf("Nusp %d achado na posição %d, com RRN %ld do ultimo nó mostrado\n", chave.nusp, i, RRNaux); */
			return RRNaux;
		}
		node = node->ponteiros[pos];
	}
	return -1;
}

/* busca internamente o nusp */
int searchPos(RRN_NUSP chave, RRN_NUSP *chaves, int n, long *rrnEncontrado) {
	int pos = 0;
	while (pos < n && chave.nusp > chaves[pos].nusp)
		pos++;
		
	*rrnEncontrado = (pos < n) ? chaves[pos].RRN : -1;
	return pos;
}

/* da inicio a remocao */
void removeAluno() {
	int chave = 0;
	printf("Digite o número USP que deseja remover: ");
	scanf("%d", &chave);
	/* delNode(chave); */
}