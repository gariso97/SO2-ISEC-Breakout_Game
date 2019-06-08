#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <conio.h>
#include <ctype.h>
#include "sddl.h"


#define TAM 200
#define N_TIJOLOS 100
#define N 1
#define DIMMAPA_X 800
#define DIMMAPA_Y 600
#define PIPE_NAME TEXT("\\\\.\\pipe\\teste")

HWND hWnd;	// hWnd é o handler da janela principal

SECURITY_ATTRIBUTES sa;
TCHAR szSD[] = TEXT("D:")
TEXT("(A;OICI;GA;;;BG)")
TEXT("(A;OICI;GA;;;AN)")
TEXT("(A;OICI;GA;;;AU)")
TEXT("(A;OICI;GA;;;BA)");


typedef struct FicheirosMapeados {
	HANDLE hMapFile;
	HANDLE hMapFileDLL;
	HANDLE hMapFileBola;
	HANDLE hMapFileBarra;
	HANDLE hMapFileBrinde;
	HANDLE hMapFileCliente;
	HANDLE hMapFileLogin;
}FilesM;

typedef struct sincronizacao {
	HANDLE LoginEvent;
	HANDLE LoginSem;
	HANDLE bEvent;
	HANDLE msgEvent;
	HANDLE jEvent;
	HANDLE barraEvent;
	HANDLE BrindeEvent;
	HANDLE Mutex;
}sinc;

typedef struct Top10 {
	TCHAR name[10];
	int	pontuacao;
}top10;

typedef struct client {
	TCHAR nome[TAM];
	int id;
	int vidas;
	int pontos;
	int start;
	top10 top[10];
}cliente;

typedef struct Posicoes {
	int x;
	int y;
}posicao;

typedef struct Dimensoes {
	int larg;
	int alt;
}dimensao;

typedef struct Brinde {
	posicao pos;
	int tipo;
}bonus;

typedef struct ObjetoMapa {
	int id;
	int tipo;
	dimensao dim;
	int vida;
	int bonus;
	posicao pos;
}objeto, * pObj;

typedef struct Barreiras {
	dimensao dim;
	posicao pos;
	int velocidade;
}barra;

typedef struct Bolas {
	int comeco;
	int sentido;	//0 - direita/cima	|  1 - esquerda/cima  |  2 - direita/baixo  | 3 - esquerda/baixo
	posicao pos;
	int dim;
	int velocidade;
}bola;

typedef struct Pacote_Mapa {
	int alteracoes;
	int pontos;
	int vida;
	int n_elementos;
	objeto tijolos[N_TIJOLOS];
}mapa;

typedef struct Mensagem {
	int cliente_id;
	int tecla;
	int top;
}mensagem;

typedef struct BaseDeDados {
	mensagem msg;
	mapa tab;
	barra bar;
	bola b;
}baseDados;

//Esta macro é definida pelo sistema caso estejamos na DLL (<DLL_IMP>_EXPORTS definida)
//ou na app (<DLL_IMP>_EXPORTS não definida) onde DLL_IMP é o nome deste projeto
#ifdef DLL_EXPORTS
#define DLL_IMP_API __declspec(dllexport)
#else
#define DLL_IMP_API __declspec(dllimport)
#endif
extern "C"
{
	//Variaveis globais a serem exportadas
	extern DLL_IMP_API TCHAR NomeFicheiroMapeadoJogo[];
	extern DLL_IMP_API TCHAR NomeEventoLogin[];
	extern DLL_IMP_API TCHAR NomeSemaforoLogin[];
	extern DLL_IMP_API TCHAR NomeFicheiroLogin[];
	extern DLL_IMP_API TCHAR NomeEventoJogo[];
	extern DLL_IMP_API TCHAR NomeFicheiroMapeadoDLL[];
	extern DLL_IMP_API TCHAR NomeEventoJogoMensagem[];
	extern DLL_IMP_API TCHAR NomeFicheiroMapeadoBola[];
	extern DLL_IMP_API TCHAR NomeEventoBola[];
	extern DLL_IMP_API TCHAR NomeFicheiroMapeadoBarra[];
	extern DLL_IMP_API TCHAR NomeEventoBarra[];
	extern DLL_IMP_API TCHAR NomeEventoBrinde[];
	extern DLL_IMP_API TCHAR NomeFicheiroMapeadoBrindes[];
	extern DLL_IMP_API TCHAR NomeFicheiroMapeadoCliente[];
	extern DLL_IMP_API TCHAR MutexT[];

	//Funções a serem exportadas
	DLL_IMP_API cliente Login(cliente jog);
	DLL_IMP_API int cria_sincronizacao_DLL();
	DLL_IMP_API int cria_ficheiros_mapeados_abertos();
	DLL_IMP_API cliente recebeMensagem(cliente cli);
	DLL_IMP_API mapa  recebeJogo(mapa m);
	DLL_IMP_API barra recebeBarra(barra br);
	DLL_IMP_API bola recebeBola(bola bl);
	DLL_IMP_API bonus recebeBrinde(bonus bon);
	DLL_IMP_API void enviaMensagem(mensagem msg);
}
