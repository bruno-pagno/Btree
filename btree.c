/*
Nome: Bruno de Sousa Pagno		NUSP: 11366872
Nome: João Vitor de Mello Gomes	NUSP: 11218622
Professor: Elaine Parros Machado de Sousa
Disciplina: SCC0503
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <btree.h>
#include <time.h>

long rrnRoot = -1;				/* raiz da árvore B */
long lastRRN = -1;				/* contador global para indicar o RRN atual */
long lastRRNIndex =	-1;			/* rrn do arquivo de indice */
int numInitialInserts = 1024;	/* Numero de alunos inseridos inicialmente */

int main() {
	/* inicializando os arquivos */
	FILE *arqIndex = fopen("index.dat" , "w+b");
	FILE *arqDados = fopen("dados.txt" , "w+b");
	if(!arqIndex || !arqDados) {
		printf("A abertura dos arquivos falhou\n");
		exit(0);
	}

	printf("Tamanho da Struct RRN_NUSP: \t%ld\n", sizeof(RRN_NUSP));
	printf("Tamanho da Struct NODE: \t%ld\n", sizeof(NODE));
	printf("Tamanho da Struct ALUNO: \t%ld\n", sizeof(ALUNO));

	/* Inserindo alunos na arvore */
	fillBTree(arqDados, arqIndex, numInitialInserts);
	printf("Número de alunos inseridos inicialmente: \t%d\n", numInitialInserts);

	/* Variaveis auxiliares */
	int opcao = 1;
	while(opcao >= 1 && opcao <= 3) {
		opcao = showMenu();
		switch(opcao) {
			case 1: /* inserir aluno */
				insertAluno(arqDados, arqIndex);
				break;
			case 2: /* buscar aluno */
				searchAluno(arqDados, arqIndex);
				break;
			case 3: /* mostrar a arvore */
				printf("\n A Btree está assim:\n\n");
				display(arqIndex, rrnRoot, 0);
				break;
		}
	}
	
	printf("Fechando arquivos e saindo...\n");
	
	/* dar free em tudo no fim da execução */
	fclose(arqDados);
	fclose(arqIndex);
	printf("Fim da execução do programa...\n");
	return 0;
}

/* exibe o menu com as opcoes */
int showMenu() {
	int res = 0;
	printf("\n");
	printf("|======================================|\n");
	printf("|    Qual Operação deseja realizar?    |\n");
	printf("|1 - Inserir novo aluno                |\n");
	printf("|2 - Buscar por NUSP                   |\n");
	printf("|3 - Mostrar árvore                    |\n");
	printf("|4 - Sair do programa                  |\n");
	printf("|======================================|\n");
	printf("|Opção: ");
	scanf("%d", &res);
	return res;
}

/* faz a leitura dados de um aluno */
ALUNO scanAluno() {
	ALUNO novoAluno;

	printf("\n\tDigite os dados do aluno que deseja inserir:\n");
	printf("\tNUSP do aluno: ");
	scanf("%d", &novoAluno.NUSP);

	printf("\tNome do aluno: ");
	scanf(" %[^\n]s", novoAluno.nome);

	printf("\tSobrenome do aluno: ");
	scanf(" %[^\n]s", novoAluno.sobrenome);

	printf("\tCurso do aluno: ");
	scanf(" %[^\n]s", novoAluno.curso);

	printf("\tNota do aluno: ");
	scanf("%f", &novoAluno.nota);

	return novoAluno;
}

NODE newNode() {
	NODE novoNode;
	novoNode.nElementos = 0;

	int i = 0;
	for(i = 0; i < M; i++) {
		novoNode.ponteiros[i] = -1;
		if(i < M - 1) {
			novoNode.rrnNUSPs[i].nusp = -1;
			novoNode.rrnNUSPs[i].RRN = -1;
		}
	}

	return novoNode;
}

/* das inicio a insercao */
void insertAluno(FILE *arqDados, FILE *arqIndex) {
	ALUNO novoAluno = scanAluno();
	/* escrevendo o aluno na arvore de dados*/
	fseek(arqDados, 0, SEEK_END);
	fwrite(&novoAluno, sizeof(ALUNO), 1, arqDados);

	if(insert(novoAluno.NUSP, arqIndex) != CHAVE_DUPLICADA)
		lastRRN++;
}

/* da inicio a busca */
void searchAluno(FILE *arqDados, FILE * arqIndex) {
	int chave = 0;
	long rrnAux = 0;
	ALUNO novoAluno;

	printf("\n\tDigite o NUSP que deseja buscar: ");
	scanf("%d", &chave);
	RRN_NUSP aux;
	aux.nusp = chave;
	
	rrnAux = search(aux, arqIndex);

	if(rrnAux != -1) {
		/* Fazendo a leitura do aluno no arquivo de dados */
		fseek(arqDados, (rrnAux * sizeof(ALUNO)), SEEK_SET);
		fread(&novoAluno, sizeof(ALUNO), 1, arqDados);
		printAluno(novoAluno);

	} else
		printf("\tO Número USP buscado não existe na árvore\n");
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
int insert(int nusp, FILE *arqIndex) {
	RRN_NUSP novo;
	novo.nusp = nusp;			/* nusp do novo aluno */
	novo.RRN = lastRRN + 1;		/* rrn do arquivo de dados */

	int novoNode = -1;				/* novo node, se necessario */
	RRN_NUSP chavePromovida;		/* chave que vai subir */
	int retorno = _insert(arqIndex, rrnRoot, novo, &chavePromovida, &novoNode);
	
	if(retorno == CHAVE_DUPLICADA){
		printf("\n\tChave duplicada, aluno não inserido!\n");
		return retorno;

	} else if(retorno == REALIZAR_INSERCAO) {	/* Primeiro nó */
		long rootAnterior = rrnRoot;

		NODE novoRoot = newNode();
		novoRoot.nElementos = 1;
		novoRoot.rrnNUSPs[0] = chavePromovida;
		novoRoot.ponteiros[0] = rootAnterior;
		novoRoot.ponteiros[1] = novoNode;

		rrnRoot = ++lastRRNIndex;	/* um novo node e escrito */

		fseek(arqIndex, 0, SEEK_END);
		fwrite(&novoRoot, sizeof(NODE), 1, arqIndex);

		return SUCESSO;
	}
	return retorno;
}

/* funcao interna que insere na arvore */
int _insert(FILE *arqIndex, int rootRRN, RRN_NUSP novoRRNUSP, RRN_NUSP *chavePromovida, int *rrnNovoNode) {
	if(rootRRN == -1) {		/* node mandado eh vazio */
		*rrnNovoNode = -1;
		*chavePromovida = novoRRNUSP;
		return REALIZAR_INSERCAO;
	}

	NODE *rootNode = (NODE *) malloc(sizeof(NODE));
	fseek(arqIndex, (rootRRN * sizeof(NODE)), SEEK_SET);
	fread(rootNode, sizeof(NODE), 1, arqIndex);

	int nElementos = rootNode->nElementos;
	long RRNaux = 0;
	int pos = searchPos(novoRRNUSP, rootNode->rrnNUSPs, nElementos, &RRNaux);	/* Encontra a posição ideal para o NUSP dentro do nó */

	if(pos < nElementos && novoRRNUSP.nusp == rootNode->rrnNUSPs[pos].nusp)		/* Nesse caso já existe */
		return CHAVE_DUPLICADA;

	RRN_NUSP novaChave;
	int novoPtr = -1;
	/* Desce para o próximo nível recursivamente */
	int retorno = _insert(arqIndex, rootNode->ponteiros[pos], novoRRNUSP, &novaChave, &novoPtr);
	
	if(retorno != REALIZAR_INSERCAO) return retorno;	/* nao insere nesse node */

	/*Se o número de NUSPS do NODE for menor que M - 1*/
	int i = 0;
	if(nElementos < M - 1) {
		pos = searchPos(novaChave, rootNode->rrnNUSPs, nElementos, &RRNaux);
		/* shift dos elementos e ponteiros */
		for(i = nElementos; i > pos; i--) {
			rootNode->rrnNUSPs[i] = rootNode->rrnNUSPs[i - 1];
			rootNode->ponteiros[i + 1] = rootNode->ponteiros[i];
		}
		
		/* chave inserida na posicao */
		rootNode->rrnNUSPs[pos] = novaChave;
		rootNode->ponteiros[pos + 1] = novoPtr;
		rootNode->nElementos++;		/* incrementa o contador */

		/* sobrescreve o node */
		fseek(arqIndex, (rootRRN * sizeof(NODE)), SEEK_SET);
		fwrite(rootNode, sizeof(NODE), 1, arqIndex);

		return SUCESSO;
	}

	/*Se o nó está cheio e a posição do elemento que deseja inserir é a ultima*/
	RRN_NUSP ultimaChave;
	int ultimoPtr = 0;
	if(pos == M - 1) {
		ultimaChave = novaChave;
		ultimoPtr = novoPtr;
	} else {
		/* Neste caso o nó está cheio, mas a posição que deseja inserir não é a ultima*/
		ultimaChave = rootNode->rrnNUSPs[M - 2];
		ultimoPtr = rootNode->ponteiros[M - 1];

		for(i = M - 2; i > pos; i--) {
			rootNode->rrnNUSPs[i] = rootNode->rrnNUSPs[i - 1];
			rootNode->ponteiros[i + 1] = rootNode->ponteiros[i];
		}
		rootNode->rrnNUSPs[pos] = novaChave;
		rootNode->ponteiros[pos + 1] = novoPtr;
	}

	int splitPos = (M - 1) / 2;							/* Posição do node médio */
	*chavePromovida = rootNode->rrnNUSPs[splitPos];
	NODE novoNode = newNode();							/* node da direita */
	rootNode->nElementos = splitPos;					/* numero de chaves (node esquerdo) */
	novoNode.nElementos = M - 1 - splitPos;				/* numero de chaves (node direito) */
	
	/* copia dos elementos */
	for(i = 0; i < novoNode.nElementos; i++) {
		novoNode.ponteiros[i] = rootNode->ponteiros[i + splitPos + 1];	
		rootNode->ponteiros[i + splitPos + 1] = -1;

		if(i < novoNode.nElementos - 1) {
			novoNode.rrnNUSPs[i] = rootNode->rrnNUSPs[i + splitPos + 1];
			rootNode->rrnNUSPs[i + splitPos + 1].nusp = -1;
			rootNode->rrnNUSPs[i + splitPos + 1].RRN = -1;
		}

		else novoNode.rrnNUSPs[i] = ultimaChave;
	}

	/* escreve o novo node */
	novoNode.ponteiros[novoNode.nElementos] = ultimoPtr;
	fseek(arqIndex, 0, SEEK_END);
	fwrite(&novoNode, sizeof(NODE), 1, arqIndex);
	*rrnNovoNode = ++lastRRNIndex;

	/* reecreve o node root, depois de alterar */
	fseek(arqIndex, rootRRN * sizeof(NODE), SEEK_SET);
	fwrite(rootNode, sizeof(NODE), 1, arqIndex);

	return REALIZAR_INSERCAO;
}

/* exibe a arvore na tela */
void display(FILE *arqIndex, long rrnNode, int espacos) {
	if(rrnNode >= 0) {
		NODE *nodeAux = (NODE *) malloc(sizeof(NODE));
		fseek(arqIndex, (rrnNode * sizeof(NODE)), SEEK_SET);
		fread(nodeAux, sizeof(NODE), 1, arqIndex);

		int i;
		for(i = 1; i <= espacos; i++)
			printf(" ");
		
		for(i = 0; i < nodeAux->nElementos; i++)
			printf("%d(%ld) ", nodeAux->rrnNUSPs[i].nusp, nodeAux->rrnNUSPs[i].RRN);
		printf("\n");

		for(i = 0; i <= nodeAux->nElementos; i++)
			display(arqIndex, nodeAux->ponteiros[i], espacos + 10);
	}
}

/* busca o node que pode conter o aluno */
long search(RRN_NUSP chave, FILE * arqIndex) {
	if(rrnRoot >= 0) {
		NODE *node = (NODE *) malloc(sizeof(NODE));
		int rrnAux = rrnRoot;

		while(rrnAux >= 0) {
			fseek(arqIndex, (rrnAux * sizeof(NODE)), SEEK_SET);
			fread(node, sizeof(NODE), 1, arqIndex);
			int n = node->nElementos;
			long RRNaux = 0;
			int pos = searchPos(chave, node->rrnNUSPs, n, &RRNaux);
			if(pos < n && chave.nusp == node->rrnNUSPs[pos].nusp)
				return RRNaux;
			
			rrnAux = node->ponteiros[pos];
		}
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

void fillBTree(FILE *arqDados, FILE *arqIndex, int num) {
	char nomes[26][20] = {
		"André", "Bruna", "Carlos", "Daniela", "Eugênio", "Flávia", "Guilherme",
		"Hosana", "Ivan", "Joana", "Kaio", "Luana", "Marcos", "Natália", "Osmar",
		"Paula", "Quelônio", "Rosana", "Sebastião", "Tatiana", "Ulisses", "Valdirene",
		"Walace", "Xuxa", "Yago", "Zuleica"
	};

	char sobrenomes[26][20] = {
		"Almeida", "Barbosa", "Carvalho", "Degenário", "Escobar", "Firmino", "Godoy",
		"Holmes", "Inácio", "Januário", "Kastein", "Louzada", "Marostegan", "Nascimento",
		"Oliveira", "Pedreira", "Quaresma", "Rossi", "Souza", "Teixeira", "Ulrich",
		"Vasquez", "Wilbert", "Xeque", "York", "Zaganelli"
	};

	char cursos[11][20] = {
		"Informática", "Engenharia", "Medicina", "Direito", "Pedagogia", "Química", "Física",
		"Computação", "Estatística", "Odontologia", "Mecânica"
	};

	srand(time(NULL));
	ALUNO novoAluno;
	int i;
	for(i = 1; i <= num; i++) {
		novoAluno.NUSP = i;
		strcpy(novoAluno.nome, nomes[rand() % 26]);
		strcpy(novoAluno.sobrenome, sobrenomes[rand() % 26]);
		strcpy(novoAluno.curso, cursos[rand() % 11]);
		novoAluno.nota = (float) (rand() % 100) / 10;

		/* escrevendo o aluno na arvore de dados*/
		fseek(arqDados, 0, SEEK_END);
		fwrite(&novoAluno, sizeof(ALUNO), 1, arqDados);
		
		if(insert(novoAluno.NUSP, arqIndex) != CHAVE_DUPLICADA)
			lastRRN++;
	}
}