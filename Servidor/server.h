#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include "..\\DLL\\DLL.h"
#include <process.h>
#include <conio.h>
#include <ctype.h>

#define NUMBER_PLAYERS 10
#define MAX_VIDAS 3
#define NLEITORES 20
#define PIPE_NAME TEXT("\\\\.\\pipe\\teste")


// Estrutura de IDs das threads criadas
typedef struct IDThreads {
	DWORD threadIdLogin;
	DWORD threadIdJogo;
	DWORD threadIdBarra;
	DWORD threadIdBola;
	DWORD threadIdJogoMSG;
	DWORD threadIdEnviaMSG;
	DWORD threadIdBrindes;
	DWORD threadIdbroad;
	DWORD threadRecebePipe;
}threadsID;

// Estrutura de handles das threads criadas
typedef struct HThreads {
	HANDLE hThreadLogin;
	HANDLE hThreadJogo;
	HANDLE hThreadBarra;
	HANDLE hThreadBola;
	HANDLE ThreadEscutaMSG;
	HANDLE hThreadEnviaMSG;
	HANDLE hThreadBrinde;
	HANDLE Hthreadbroad;
	HANDLE hThreadRecebe;
}Hthread;
