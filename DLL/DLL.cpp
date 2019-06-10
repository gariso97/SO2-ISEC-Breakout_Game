#include "DLL.h"

char ponteiro[40960];			//Para verificar ao carregar a dll que a aplicaçao irá ocupar mais memória

//variaveis e estruturas necessarias as funcoes da DLL
TCHAR NomeEventoLogin[] = TEXT("Evento Login");
TCHAR NomeSemaforoLogin[] = TEXT("Semáforo Login");
TCHAR NomeFicheiroLogin[] = TEXT("Comunicação Login");
TCHAR NomeFicheiroMapeadoJogo[] = TEXT("Comunição Jogo");
TCHAR NomeEventoJogo[] = TEXT("Evento Jogo");
TCHAR NomeFicheiroMapeadoDLL[] = TEXT("Comunição Mensagem");
TCHAR NomeEventoJogoMensagem[] = TEXT("Evento Mensagem");
TCHAR NomeFicheiroMapeadoBola[] = TEXT("Comunicação Bola");
TCHAR NomeEventoBola[] = TEXT("Evento Bola");
TCHAR NomeFicheiroMapeadoBarra[] = TEXT("Comunicação Barra");
TCHAR NomeEventoBarra[] = TEXT("Evento Barra");
TCHAR NomeFicheiroMapeadoCliente[] = TEXT("Comunicação Cliente Unico");
TCHAR MutexT[] = TEXT("Mutex");
sinc s;
FilesM fm;

// funcoes da DLL
cliente Login(cliente jog) {
	cliente* pClienteLogin;

	pClienteLogin = (cliente*)MapViewOfFile(fm.hMapFileLogin, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(cliente));
	if (pClienteLogin == NULL) {
		MessageBox(hWnd, TEXT("Erro a mapear pClienteLogin..."), TEXT("ERROR"), MB_OK | MB_ICONERROR);
		return jog;
	}

	_tcscpy_s(pClienteLogin->nome, jog.nome);
	SetEvent(s.LoginEvent);
	ReleaseSemaphore(s.LoginSem, 1, NULL);
	ResetEvent(s.LoginEvent);

	return jog;
}

mapa  recebeJogo(mapa m) {		//funçao que recebe posiçao da bola ate o cliente primir uma tecla
	mapa* pJogo;

	pJogo = (mapa*)MapViewOfFile(fm.hMapFile, FILE_MAP_WRITE, 0, 0, sizeof(mapa));
	if (pJogo == NULL) {
		MessageBox(hWnd, TEXT("Erro a mapear pJogo..."), TEXT("ERROR"), MB_OK | MB_ICONERROR);
		return m;
	}

	WaitForSingleObject(s.jEvent, INFINITE);
	m.alteracoes = pJogo->alteracoes;
	m.pontos = pJogo->pontos;
	m.vida = pJogo->vida;
	m.n_elementos = pJogo->n_elementos;
	m.brinde.existe = pJogo->brinde.existe;
	m.brinde.tipo = pJogo->brinde.tipo;
	m.brinde.pos = pJogo->brinde.pos;
	for (int i = 0; i < m.n_elementos; i++)
		m.tijolos[i] = pJogo->tijolos[i];
	UnmapViewOfFile(pJogo);

	return m;
}

void  enviaMensagem(mensagem msg) {
	mensagem* msgDLL = NULL;

	msgDLL = (mensagem*)MapViewOfFile(fm.hMapFileDLL, FILE_MAP_WRITE, 0, 0, sizeof(mensagem));
	if (msgDLL == NULL) {
		MessageBox(hWnd, TEXT("Erro a mapear msg..."), TEXT("ERROR"), MB_OK | MB_ICONERROR);
		return;
	}

	msgDLL->tecla = msg.tecla;
	msgDLL->top = msg.top;
	msgDLL->cliente_id = msg.cliente_id;
	SetEvent(s.msgEvent);
	ResetEvent(s.msgEvent);

	UnmapViewOfFile(msgDLL);
}

cliente recebeMensagem(cliente cli) {
	cliente* pCliente;

	pCliente = (cliente*)MapViewOfFile(fm.hMapFileCliente, FILE_MAP_WRITE, 0, 0, sizeof(cliente));
	if (pCliente == NULL) {
		MessageBox(hWnd, TEXT("Erro a mapear pCliente..."), TEXT("ERROR"), MB_OK | MB_ICONERROR);
		return cli;
	}
	ReleaseMutex(s.Mutex);
	WaitForSingleObject(s.Mutex, INFINITE);

	cli.start = pCliente->start;
	for (int i = 0; i < 10; i++) {
		_tcscpy_s(cli.top[i].jogador, pCliente->top[i].jogador);
	}
	cli.id = pCliente->id;
	ReleaseMutex(s.Mutex);

	UnmapViewOfFile(pCliente);
	return cli;
}

bola recebeBola(bola bl) {

	bola* pBola;

	pBola = (bola*)MapViewOfFile(fm.hMapFileBola, FILE_MAP_WRITE, 0, 0, sizeof(bola));
	if (pBola == NULL) {
		MessageBox(hWnd, TEXT("Erro a mapear pBola..."), TEXT("ERROR"), MB_OK | MB_ICONERROR);
		return bl;
	}

	WaitForSingleObject(s.bEvent, INFINITE);
	bl.pos.x = pBola->pos.x;
	bl.pos.y = pBola->pos.y;
	bl.dim = pBola->dim;

	UnmapViewOfFile(pBola);
	return bl;
}

barra recebeBarra(barra br) {

	barra* pBarra;

	pBarra = (barra*)MapViewOfFile(fm.hMapFileBarra, FILE_MAP_WRITE, 0, 0, sizeof(barra));
	if (pBarra == NULL) {
		MessageBox(hWnd, TEXT("Erro a mapear pBarra..."), TEXT("ERROR"), MB_OK | MB_ICONERROR);
		return br;
	}

	WaitForSingleObject(s.barraEvent, INFINITE);
	br.pos.x = pBarra->pos.x;
	br.pos.y = pBarra->pos.y;
	br.dim.alt = pBarra->dim.alt;
	br.dim.larg = pBarra->dim.larg;

	UnmapViewOfFile(pBarra);
	return br;
}

int cria_sincronizacao_DLL() {
	TCHAR str[TAM];

	s.LoginSem = CreateSemaphore(NULL, 0, 1, NomeSemaforoLogin);
	if (s.LoginSem == NULL) {
		_stprintf_s(str, TEXT("Erro ao criar Semaforo %s..."), NomeSemaforoLogin);
		MessageBox(hWnd, str, TEXT("ERROR"), MB_OK | MB_ICONERROR);
		return -1;
	}
	s.LoginEvent = CreateEvent(NULL, TRUE, FALSE, NomeEventoLogin);
	if (s.LoginEvent == NULL) {
		_stprintf_s(str, TEXT("Erro ao criar Evento %s..."), NomeEventoLogin);
		MessageBox(hWnd, str, TEXT("ERROR"), MB_OK | MB_ICONERROR);
		return -1;
	}

	s.bEvent = CreateEvent(NULL, TRUE, FALSE, NomeEventoBola);
	if (s.bEvent == NULL) {
		_stprintf_s(str, TEXT("Erro ao criar Evento %s..."), NomeEventoBola);
		MessageBox(hWnd, str, TEXT("ERROR"), MB_OK | MB_ICONERROR);
		return -1;
	}

	s.msgEvent = CreateEvent(NULL, TRUE, FALSE, NomeEventoJogoMensagem);
	if (s.msgEvent == NULL) {
		_stprintf_s(str, TEXT("Erro ao criar Evento %s..."), NomeEventoJogoMensagem);
		MessageBox(hWnd, str, TEXT("ERROR"), MB_OK | MB_ICONERROR);
		return -1;
	}

	s.jEvent = CreateEvent(NULL, TRUE, FALSE, NomeEventoJogo);
	if (s.jEvent == NULL) {
		_stprintf_s(str, TEXT("Erro ao criar Evento %s..."), NomeEventoJogo);
		MessageBox(hWnd, str, TEXT("ERROR"), MB_OK | MB_ICONERROR);
		return -1;
	}

	s.barraEvent = CreateEvent(NULL, TRUE, FALSE, NomeEventoBarra);
	if (s.barraEvent == NULL) {
		_stprintf_s(str, TEXT("Erro ao criar Evento %s..."), NomeEventoBarra);
		MessageBox(hWnd, str, TEXT("ERROR"), MB_OK | MB_ICONERROR);
		return -1;
	}

	s.Mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, MutexT);
	if (s.Mutex == NULL) {
		_stprintf_s(str, TEXT("Erro ao criar o Mutex  %s..."), MutexT);
		MessageBox(hWnd, str, TEXT("ERROR"), MB_OK | MB_ICONERROR);
		return -1;
	}
	return 0;
}

int cria_ficheiros_mapeados_abertos() {
	TCHAR str[TAM];

	fm.hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, NomeFicheiroMapeadoJogo);
	if (fm.hMapFile == NULL) {
		_stprintf_s(str, TEXT("Erro a criar pJogo %s na memoria partilhada!"), NomeFicheiroMapeadoJogo);
		MessageBox(hWnd, str, TEXT("ERROR"), MB_OK | MB_ICONERROR);
		return -1;
	}

	fm.hMapFileDLL = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, NomeFicheiroMapeadoDLL);
	if (fm.hMapFileDLL == NULL) {
		_stprintf_s(str, TEXT("Erro a criar msg %s na memoria partilhada!"), NomeFicheiroMapeadoDLL);
		MessageBox(hWnd, str, TEXT("ERROR"), MB_OK | MB_ICONERROR);
		return -1;
	}

	fm.hMapFileBola = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, NomeFicheiroMapeadoBola);
	if (fm.hMapFileBola == NULL) {
		_stprintf_s(str, TEXT("Erro a criar pBola %s na memoria Partilhada!"), NomeFicheiroMapeadoBola);
		MessageBox(hWnd, str, TEXT("ERROR"), MB_OK | MB_ICONERROR);
		return -1;
	}

	fm.hMapFileBarra = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, NomeFicheiroMapeadoBarra);
	if (fm.hMapFileBarra == NULL) {
		_stprintf_s(str, TEXT("Erro a criar pBarra %s na memoria partilhada!"), NomeFicheiroMapeadoBarra);
		MessageBox(hWnd, str, TEXT("ERROR"), MB_OK | MB_ICONERROR);
		return -1;
	}

	fm.hMapFileCliente = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, NomeFicheiroMapeadoCliente);
	if (fm.hMapFileCliente == NULL) {
		_stprintf_s(str, TEXT("Erro a criar pCliente %s na memoria partilhada!"), NomeFicheiroMapeadoCliente);
		MessageBox(hWnd, str, TEXT("ERROR"), MB_OK | MB_ICONERROR);
		return -1;
	}

	fm.hMapFileLogin = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, NomeFicheiroLogin);
	if (fm.hMapFileLogin == NULL) {
		_stprintf_s(str, TEXT("Erro a criar pJogo %s na memoria partilhada!"), NomeFicheiroLogin);
		MessageBox(hWnd, str, TEXT("ERROR"), MB_OK | MB_ICONERROR);
		return -1;
	}

	return 0;
}

DWORD WINAPI recebePipe(LPVOID bolas) {
	TCHAR buf[TAM];
	HANDLE hPipe;
	int i = 0;
	BOOL ret;
	DWORD n;

	_tprintf(TEXT("[LEITOR] Esperar pelo pipe '%s' (WaitNamedPipe)\n"), PIPE_NAME);
	if (!WaitNamedPipe(PIPE_NAME, NMPWAIT_WAIT_FOREVER)) {
		_tprintf(TEXT("[ERRO] Ligar ao pipe '%s'! (WaitNamedPipe)\n"), PIPE_NAME);
		exit(-1);
	}
	_tprintf(TEXT("[LEITOR] Ligação ao pipe do escritor... (CreateFile)\n"));
	hPipe = CreateFile(PIPE_NAME, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hPipe == NULL) {
		_tprintf(TEXT("[ERRO] Ligar ao pipe '%s'! (CreateFile)\n"), PIPE_NAME);
		exit(-1);
	}
	_tprintf(TEXT("[LEITOR] Liguei-me...\n"));

	while (1)
	{
		ret = ReadFile(hPipe, buf, sizeof(buf), &n, NULL);
		buf[n / sizeof(TCHAR)] = '\0';
		if (!ret || !n) {
			_tprintf(TEXT("[LEITOR] %d %d... (ReadFile)\n"), ret, n);
			break;
		}
		_tprintf(TEXT("[LEITOR] Recebi %d bytes: '%s'... (ReadFile)\n"), n, buf);
	}
	CloseHandle(hPipe);
	return 0;

}
