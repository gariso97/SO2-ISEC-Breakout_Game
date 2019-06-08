#include "server.h"

//variaveis globais do servidor
int contador;
int id = 0;
int tempo = 5;
int flag_start = 0;

//estruturas e vetores de estruturas
cliente* vetor;
threadsID tid;
Hthread ht;
FilesM fm;
sinc s;
baseDados bd;
bola b_inicial;


DWORD WINAPI Divulga(LPVOID param) {
	TCHAR buf[256];
	HANDLE* ptr = (HANDLE*)param;
	DWORD n = 0;
	do {
		_tprintf(TEXT("[ESCRITOR] Frase: "));
		_fgetts(buf, 256, stdin);
		buf[_tcslen(buf) - 1] = '\0';
		for (int j = 0; j < NLEITORES; j++)
			if (ptr[j] != INVALID_HANDLE_VALUE && !WriteFile(ptr[j], buf, _tcslen(buf) * sizeof(TCHAR), &n, NULL)) {
				_tprintf(TEXT("[ERRO] Escrever no pipe! (WriteFile)\n"));
				_tprintf(TEXT("[ESCRITOR] Desligar o pipe (DisconnectNamedPipe)\n"));
				if (!DisconnectNamedPipe(ptr[j])) {
					_tprintf(TEXT("[ERRO] Desligar o pipe! (DisconnectNamedPipe)"));
					exit(-1);
				}
				CloseHandle(ptr[j]);
				ptr[j] = INVALID_HANDLE_VALUE;
				contador--;
			}
		_tprintf(TEXT("[ESCRITOR] Enviei %d bytes aos leitores... (WriteFile)\n"), n);
	} while (_tcscmp(buf, TEXT("fim")));
	return 0;
}

DWORD WINAPI ThreadBroad(LPVOID bolas) {
	TCHAR buf[256];
	int i = 0;
	HANDLE hThread2;
	HANDLE hPipe[NLEITORES], hPipeTemp;
	for (; i < NLEITORES; i++)
		hPipe[i] = INVALID_HANDLE_VALUE;
	hThread2 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Divulga, (LPVOID)hPipe, 0, NULL);
	if (hThread2 == NULL) {
		_tprintf(TEXT("[ERRO] Lançar Thread! (CreateNamedPipe)"));
		return -1;
	}
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = FALSE;
	ConvertStringSecurityDescriptorToSecurityDescriptor(szSD, SDDL_REVISION_1, &(sa.lpSecurityDescriptor), NULL);

	do {
		//_tprintf(TEXT("[ESCRITOR] Criar uma cópia do pipe '%s' ... (CreateNamedPipe)\n"), PIPE_NAME);

		do {
			i = 0;
			while (i < NLEITORES && hPipe[i] != INVALID_HANDLE_VALUE)
				i++;
			if (i == NLEITORES) {
				Sleep(5000);
				continue;
			}
			hPipeTemp = CreateNamedPipe(PIPE_NAME, PIPE_ACCESS_OUTBOUND, PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, NLEITORES, sizeof(buf), sizeof(buf), 1000, &sa); ///&sasa);
			if (hPipeTemp == INVALID_HANDLE_VALUE) {
				_tprintf(TEXT("[ERRO] Criar Named Pipe! (CreateNamedPipe)"));
				break;
			}

			//_tprintf(TEXT("[ESCRITOR] Esperar ligação de um leitor... (ConnectNamedPipe). %d\n"), contador);
			if (!ConnectNamedPipe(hPipeTemp, NULL)) {
				if (GetLastError() != ERROR_PIPE_CONNECTED) {
					_tprintf(TEXT("[ERRO] Ligação ao leitor! (ConnectNamedPipe\n"));
					exit(-1);
				}
			}
			contador++;
			hPipe[i] = hPipeTemp;
		} while (i <= NLEITORES);

		WaitForSingleObject(hThread2, INFINITE);
	} while (1);
}

DWORD WINAPI ThreadRecebePipe(LPVOID param) {
	TCHAR buf[256];
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
	Sleep(200);
	return 0;
}

/////////////^^^^^ Pipes ^^^^^^
////////////vvvvv Funcoes vvvvv

void preenche_vetor(int id, TCHAR* nome) {
	vetor[id].id = id;
	_tcscpy_s(vetor[id].nome, nome);
	vetor[id].pontos = 0;
	vetor[id].vidas = MAX_VIDAS;
}

void verificacao_nome(TCHAR *nome) {
	for (int i = 0; i < NUMBER_PLAYERS; i++)
		if (_tcscmp(nome, vetor[i].nome) == 0) {
			flag_start = 1;
			return;
		}
	
	bd.msg.cliente_id = id;
	preenche_vetor(id, nome);
	id++;
	flag_start = 2;
}

void definicoes_jogo() {
	bd.tab.vida = MAX_VIDAS;
	//mapa
	bd.tab.n_elementos = 96;
	bd.tab.pontos = 0;
	//barreira
	bd.bar.dim.alt = 10;
	bd.bar.dim.larg = 70;
	bd.bar.velocidade = 10;
	bd.bar.pos.x = 375;
	bd.bar.pos.y = 550;
	//bola
	bd.b.velocidade = 1;		//nao mexer
	bd.b.dim = 15;
	bd.b.sentido = 0;
	bd.b.pos.x = bd.bar.pos.x + (bd.bar.dim.larg / 2) + 1;
	bd.b.pos.y = bd.bar.pos.y - 5;
	bd.b.comeco = 1;	//flag começo do jogo

	b_inicial.comeco = 1;
	b_inicial.dim = bd.b.dim;
	b_inicial.sentido = bd.b.sentido;
	b_inicial.pos.x = bd.b.pos.x;
	b_inicial.pos.y = bd.b.pos.y;

}

void definicoes_jogo_ficheiro(TCHAR* nomeFich) {
	FILE* f;
	_tfopen_s(&f, L"config.txt", TEXT("r"));
	if (f == NULL) {
		_tprintf(L" [ERRO] Abertura o ficheiro...\n");
	}
	//mapa
	_ftscanf_s(f, TEXT("N Elementos: %d"), &bd.tab.n_elementos);
	bd.tab.pontos = 0;
	//barreira
	_ftscanf_s(f, TEXT("Altura da barreira: %d\nLargura da barreira: %d\nVelocidade da barreira: %d\nPosicao da barreira-X: %d\nPosicao da barreira-Y: %d"), &bd.bar.dim.alt, &bd.bar.dim.larg, &bd.bar.velocidade, &bd.bar.pos.x, &bd.bar.pos.y);
	//bola
	_ftscanf_s(f, TEXT("Velocidade da bola: %d"), &tempo);
	bd.b.velocidade = 1;
	_ftscanf_s(f, TEXT("Dimensão da bola: %d"), &bd.b.dim);
	_ftscanf_s(f, TEXT("Sentido da bola: %d"), &bd.b.sentido);
	_ftscanf_s(f, TEXT("Posicao da bola-X: %d"), &bd.b.pos.x);
	_ftscanf_s(f, TEXT("Posicao da bola-Y: %d"), &bd.b.pos.y);
	bd.b.comeco = 1;
	//vidas
	_ftscanf_s(f, TEXT("Vidas: %d"), &bd.tab.vida);
	fclose(f);

	b_inicial.comeco = 0;
	b_inicial.dim = bd.b.dim;
	b_inicial.sentido = bd.b.sentido;
	b_inicial.pos.x = bd.b.pos.x;
	b_inicial.pos.y = bd.b.pos.y;

	//_tprintf(TEXT("\n[INFO] Posicao barreira: [%d,%d] Posicao bola: [%d,%d]\n"), bd.bar.pos.x, bd.bar.pos.y, bd.b.pos.x, bd.b.pos.y);
	//_tprintf(TEXT("\n[INFO] Bar Alt: %d | Bar larg: %d | N_ele: %d | Tempo: %d\n"), bd.bar.dim.alt, bd.bar.dim.larg, bd.tab.n_elementos, tempo);
}

int CriaTopRegistry() {
	HKEY chave;
	DWORD existe, tamanho;
	TCHAR str[TAM], autor[TAM];

	//Criar/abrir uma chave em HKEY_CURRENT_USER\Software\Tp_SO2
	if (RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Tp_SO2"), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &chave, &existe) != ERROR_SUCCESS) {
		_tprintf(TEXT("Erro ao criar/abrir chave (%d)\n"), GetLastError());
		return -1;
	}
	else {

		if (existe == REG_CREATED_NEW_KEY) //Se a chave foi criada, inicializar os valores
		{
			for (int i = 1; i <= 10; i++) {
				_stprintf_s(str, TEXT("Top%d"), i);
				_stprintf_s(autor, TEXT("Melhor%d"), i);
				RegSetValueEx(chave, str, 0, REG_SZ, (LPBYTE)autor, _tcslen(autor) * sizeof(TCHAR));
			}
			_tprintf(TEXT("Registo do TOP 10 criado!! Tente novamente!!\n"));
		}
		//Se a chave foi aberta, ler os valores lá guardados
		else if (existe == REG_OPENED_EXISTING_KEY) {

			for (int i = 1; i <= 10; i++) {
				_stprintf_s(str, TEXT("Top%d"), i);
				tamanho = 20;
				RegQueryValueEx(chave, str, NULL, NULL, (LPBYTE)autor, &tamanho);
				if (tamanho > 0)
					_tprintf(TEXT(" %s: %s\n"), str, autor);
			}
		}
		RegCloseKey(chave);
	}
	return 0;
}

void cria_mapa() {
	int tipoRand;
	int x = 5, y = 5;
	int aux = 16, aux2 = 0;
	for (int i = 0; i < bd.tab.n_elementos; i++) {
		if (i == aux) {
			y = y + bd.tab.tijolos[i - 1].dim.alt;
			x = 5;
			aux = aux + 16;// -aux2;
			/*if(aux2 < 5)
				aux2++;*/
		}
		tipoRand = 0;
		bd.tab.tijolos[i].id = i;
		tipoRand = rand() % 4;
		bd.tab.tijolos[i].tipo = tipoRand;
		tipoRand = rand() % 1;
		bd.tab.tijolos[i].bonus = tipoRand;

		bd.tab.tijolos[i].dim.larg = 50;
		bd.tab.tijolos[i].pos.x = x;
		x = x + bd.tab.tijolos[i].dim.larg;

		bd.tab.tijolos[i].pos.y = y;
		bd.tab.tijolos[i].dim.alt = 25;
		bd.tab.tijolos[i].vida = 1;
	}
}

void valida_pos_bola() {
	bd.tab.alteracoes = 0;	//jogador esta normal
	int ganhou = 1;

	//verificacoes se ganhou o jogo
	for (int i = 0; i < bd.tab.n_elementos; i++) {
		if (bd.tab.tijolos[i].vida != 0) {
			ganhou = 0;
			break;
		}
	}
	if (ganhou == 1) {
		bd.tab.alteracoes = 2;	// jogador ganhou o jogo
		return;
	}

	//verificacoes da bola
	switch (bd.b.sentido) {
	case 0:  //cima e direira
		//tijolos
		for (int i = 0; i < bd.tab.n_elementos; i++) {
			if (bd.tab.tijolos[i].vida == 0) {
				bd.tab.tijolos[i].vida = -1;
				bd.tab.pontos += 10;
			}
			if (bd.b.pos.x + bd.b.velocidade >= bd.tab.tijolos[i].pos.x && bd.b.pos.x + bd.b.velocidade <= bd.tab.tijolos[i].pos.x + bd.tab.tijolos[i].dim.larg && bd.b.pos.y - bd.b.velocidade == bd.tab.tijolos[i].pos.y + bd.tab.tijolos[i].dim.alt) {
				if (bd.tab.tijolos[i].vida > 0) {
					bd.b.sentido = 2;
					bd.b.pos.y += bd.b.velocidade;
					bd.b.pos.x += bd.b.velocidade;
					bd.tab.tijolos[i].vida--;
					return;
				}
			}
			if (bd.b.pos.y - bd.b.velocidade >= bd.tab.tijolos[i].pos.y && bd.b.pos.y - bd.b.velocidade <= bd.tab.tijolos[i].pos.y + bd.tab.tijolos[i].dim.alt && bd.b.pos.x + bd.b.velocidade == bd.tab.tijolos[i].pos.x) {
				if (bd.tab.tijolos[i].vida > 0) {
					bd.b.sentido = 1;
					bd.b.pos.y -= bd.b.velocidade;
					bd.b.pos.x -= bd.b.velocidade;
					bd.tab.tijolos[i].vida--;
					return;
				}
			}
		}
		if ((bd.b.pos.x + bd.b.velocidade) >= DIMMAPA_X - 5) {
			bd.b.sentido = 1;
			bd.b.pos.x -= bd.b.velocidade;
		}
		else {
			bd.b.pos.x += bd.b.velocidade;
		}
		if ((bd.b.pos.y - bd.b.velocidade) <= 5) {
			bd.b.sentido = 2;
			bd.b.pos.y += bd.b.velocidade;
		}
		else {
			bd.b.pos.y -= bd.b.velocidade;
		}
		break;
	case 1: //cima esquerda
		//tijolos
		for (int i = 0; i < bd.tab.n_elementos; i++) {
			if (bd.tab.tijolos[i].vida == 0) {
				bd.tab.tijolos[i].vida = -1;
				bd.tab.pontos += 10;
			}
			if (bd.b.pos.x - bd.b.velocidade >= bd.tab.tijolos[i].pos.x && bd.b.pos.x - bd.b.velocidade <= bd.tab.tijolos[i].pos.x + bd.tab.tijolos[i].dim.larg && bd.b.pos.y - bd.b.velocidade == bd.tab.tijolos[i].pos.y + bd.tab.tijolos[i].dim.alt) {
				if (bd.tab.tijolos[i].vida > 0) {
					bd.b.sentido = 3;
					bd.b.pos.y += bd.b.velocidade;
					bd.b.pos.x -= bd.b.velocidade;
					bd.tab.tijolos[i].vida--;
					return;
				}
			}
			if (bd.b.pos.y - bd.b.velocidade >= bd.tab.tijolos[i].pos.y && bd.b.pos.y - bd.b.velocidade <= bd.tab.tijolos[i].pos.y + bd.tab.tijolos[i].dim.alt && bd.b.pos.x - bd.b.velocidade == bd.tab.tijolos[i].pos.x + bd.tab.tijolos[i].dim.larg) {
				if (bd.tab.tijolos[i].vida > 0) {
					bd.b.sentido = 0;
					bd.b.pos.y -= bd.b.velocidade;
					bd.b.pos.x += bd.b.velocidade;
					bd.tab.tijolos[i].vida--;
					return;
				}
			}
		}
		if ((bd.b.pos.x - bd.b.velocidade) <= 5) {
			bd.b.sentido = 0;
			bd.b.pos.x += bd.b.velocidade;
		}
		else {
			bd.b.pos.x -= bd.b.velocidade;
		}
		if ((bd.b.pos.y - bd.b.velocidade) <= 5) {
			bd.b.sentido = 3;
			bd.b.pos.y += bd.b.velocidade;
		}
		else {
			bd.b.pos.y -= bd.b.velocidade;
		}
		break;
	case 2: //baixo e direira
		//perde jogada
		if (bd.b.pos.y + bd.b.velocidade == DIMMAPA_Y - 10) {
			bd.tab.vida--;
			bd.tab.alteracoes = 1;	//perde uma vida
			b_inicial.comeco = 1;
			return;
		}
		//tijolos
		for (int i = 0; i < bd.tab.n_elementos; i++) {
			if (bd.tab.tijolos[i].vida == 0) {
				bd.tab.tijolos[i].vida = -1;
				bd.tab.pontos += 10;
			}
			if (bd.b.pos.x + bd.b.velocidade >= bd.tab.tijolos[i].pos.x && bd.b.pos.x + bd.b.velocidade <= bd.tab.tijolos[i].pos.x + bd.tab.tijolos[i].dim.larg && bd.b.pos.y + bd.b.velocidade == bd.tab.tijolos[i].pos.y) {
				if (bd.tab.tijolos[i].vida > 0) {
					bd.b.sentido = 0;
					bd.b.pos.y -= bd.b.velocidade;
					bd.b.pos.x += bd.b.velocidade;
					bd.tab.tijolos[i].vida--;
					return;
				}
			}
			if (bd.b.pos.y + bd.b.velocidade >= bd.tab.tijolos[i].pos.y && bd.b.pos.y + bd.b.velocidade <= bd.tab.tijolos[i].pos.y + bd.tab.tijolos[i].dim.alt && bd.b.pos.x + bd.b.velocidade == bd.tab.tijolos[i].pos.x) {
				if (bd.tab.tijolos[i].vida > 0) {
					bd.b.sentido = 3;
					bd.b.pos.y += bd.b.velocidade;
					bd.b.pos.x -= bd.b.velocidade;
					bd.tab.tijolos[i].vida--;
					return;
				}
			}
		}
		//barra
		if (bd.b.pos.x + bd.b.velocidade >= bd.bar.pos.x && bd.b.pos.x + bd.b.velocidade <= bd.bar.pos.x + bd.bar.dim.larg && bd.b.pos.y + bd.b.velocidade == bd.bar.pos.y) {
			bd.b.sentido = 0;
			bd.b.pos.y -= bd.b.velocidade;
			bd.b.pos.x += bd.b.velocidade;
			return;
		}
		else if (bd.b.pos.y + bd.b.velocidade >= bd.bar.pos.y && bd.b.pos.y + bd.b.velocidade <= bd.bar.pos.y + bd.bar.dim.alt && bd.b.pos.x + bd.b.velocidade == bd.bar.pos.x) {
			bd.b.sentido = 3;
			bd.b.pos.y += bd.b.velocidade;
			bd.b.pos.x -= bd.b.velocidade;
			return;
		}
		if ((bd.b.pos.x + bd.b.velocidade) >= DIMMAPA_X - 5) {
			bd.b.sentido = 3;
			bd.b.pos.x -= bd.b.velocidade;
		}
		else {
			bd.b.pos.x += bd.b.velocidade;
		}
		if ((bd.b.pos.y + bd.b.velocidade) >= DIMMAPA_Y - 5) {
			bd.b.sentido = 0;
			bd.b.pos.y -= bd.b.velocidade;
		}
		else {
			bd.b.pos.y += bd.b.velocidade;
		}
		break;
	case 3: //baixo e esquerda
		//perde jogada
		if (bd.b.pos.y + bd.b.velocidade == DIMMAPA_Y - 10) {
			bd.tab.vida--;
			bd.tab.alteracoes = 1;
			b_inicial.comeco = 1;
			return;
		}
		//tijolos
		for (int i = 0; i < bd.tab.n_elementos; i++) {
			if (bd.tab.tijolos[i].vida == 0) {
				bd.tab.tijolos[i].vida = -1;
				bd.tab.pontos += 10;
			}
			if (bd.b.pos.x - bd.b.velocidade >= bd.tab.tijolos[i].pos.x && bd.b.pos.x - bd.b.velocidade <= bd.tab.tijolos[i].pos.x + bd.tab.tijolos[i].dim.larg && bd.b.pos.y + bd.b.velocidade == bd.tab.tijolos[i].pos.y) {
				if (bd.tab.tijolos[i].vida > 0) {
					bd.b.sentido = 1;
					bd.b.pos.y -= bd.b.velocidade;
					bd.b.pos.x -= bd.b.velocidade;
					bd.tab.tijolos[i].vida--;
					return;
				}
			}
			if (bd.b.pos.y + bd.b.velocidade >= bd.tab.tijolos[i].pos.y && bd.b.pos.y + bd.b.velocidade <= bd.tab.tijolos[i].pos.y + bd.tab.tijolos[i].dim.alt && bd.b.pos.x - bd.b.velocidade == bd.tab.tijolos[i].pos.x + bd.tab.tijolos[i].dim.larg) {
				if (bd.tab.tijolos[i].vida > 0) {
					bd.b.sentido = 2;
					bd.b.pos.y += bd.b.velocidade;
					bd.b.pos.x += bd.b.velocidade;
					bd.tab.tijolos[i].vida--;
					return;
				}
			}
		}
		//barra
		if (bd.b.pos.x + bd.b.velocidade >= bd.bar.pos.x && bd.b.pos.x + bd.b.velocidade <= bd.bar.pos.x + bd.bar.dim.larg && bd.b.pos.y + bd.b.velocidade == bd.bar.pos.y) {
			bd.b.sentido = 1;
			bd.b.pos.y -= bd.b.velocidade;
			bd.b.pos.x -= bd.b.velocidade;
			return;
		}
		else if (bd.b.pos.y + bd.b.velocidade >= bd.bar.pos.y && bd.b.pos.y + bd.b.velocidade <= bd.bar.pos.y + bd.bar.dim.alt && bd.b.pos.x + bd.b.velocidade == bd.bar.pos.x + bd.bar.dim.larg) {
			bd.b.sentido = 2;
			bd.b.pos.y += bd.b.velocidade;
			bd.b.pos.x += bd.b.velocidade;
			return;
		}
		if ((bd.b.pos.x - bd.b.velocidade) <= 5) {
			bd.b.sentido = 2;
			bd.b.pos.x += bd.b.velocidade;
		}
		else {
			bd.b.pos.x -= bd.b.velocidade;
		}
		if ((bd.b.pos.y + bd.b.velocidade) >= DIMMAPA_Y - 5) {
			bd.b.sentido = 1;
			bd.b.pos.y -= bd.b.velocidade;
		}
		else {
			bd.b.pos.y += bd.b.velocidade;
		}
		break;
	}
}

void valida_barra() {

	if (bd.msg.tecla == 'd' && (bd.bar.pos.x + bd.bar.dim.larg + bd.bar.velocidade) <= (DIMMAPA_X)) {
		bd.bar.pos.x += bd.bar.velocidade;
	}
	else if (bd.msg.tecla == 'a' && (bd.bar.pos.x - bd.bar.velocidade) >= 5) {
		bd.bar.pos.x -= bd.bar.velocidade;
	}
	bd.msg.tecla = 0;
}

void reinicia_jogo() {
	cria_mapa();
	definicoes_jogo();
	vetor[0].vidas = MAX_VIDAS;
	vetor[0].pontos = 0;
}

DWORD WINAPI ThreadLogin(LPVOID param) {                                //função da thread que recebe o username do cliente
	cliente* pClienteLogin;

	pClienteLogin = (cliente*)MapViewOfFile(fm.hMapFileLogin, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(cliente));
	if (pClienteLogin == NULL) {
		_tprintf(TEXT("Erro a mapear Login...\n"));
		return -1;
	}

	do {
		WaitForSingleObject(s.LoginEvent, INFINITE);
		WaitForSingleObject(s.LoginSem, INFINITE);                                                                //Ler frase do ficheiro
		_tprintf(TEXT("\nPedido de username: %s\n"), pClienteLogin->nome);
		verificacao_nome(pClienteLogin->nome);
		ReleaseSemaphore(s.LoginSem, 1, NULL);
	} while (1);

	return 0;
}

DWORD WINAPI ThreadJogo(LPVOID param) {
	mapa* pJogo;

	pJogo = (mapa*)MapViewOfFile(fm.hMapFile, FILE_MAP_WRITE, 0, 0, sizeof(mapa));
	if (pJogo == NULL) {
		_tprintf(TEXT("Erro a mapear pJogo...\n"));
	}
	bd.msg.tecla = 0;
	do {
		pJogo->alteracoes = bd.tab.alteracoes;
		pJogo->pontos = bd.tab.pontos;
		pJogo->vida = bd.tab.vida;
		pJogo->n_elementos = bd.tab.n_elementos;
		for (int i = 0; i < bd.tab.n_elementos; i++) {
			pJogo->tijolos[i] = bd.tab.tijolos[i];
		}
		SetEvent(s.jEvent);
		ResetEvent(s.jEvent);
		Sleep(tempo);
	} while (1);
	UnmapViewOfFile(pJogo);
	CloseHandle(fm.hMapFile);
	CloseHandle(s.jEvent);
	return 0;
}

DWORD WINAPI ThreadEscutaMSG(LPVOID param) {
	mensagem* msg;

	msg = (mensagem*)MapViewOfFile(fm.hMapFileCliente, FILE_MAP_WRITE, 0, 0, sizeof(mensagem));
	if (msg == NULL) {
		_tprintf(TEXT("Erro a mapear pJogo...\n"));
	}
	do {
		WaitForSingleObject(s.msgEvent, INFINITE);
		bd.msg.cliente_id = msg->cliente_id;
		bd.msg.tecla = msg->tecla;
		bd.msg.top = msg->top;
	} while (1);
	UnmapViewOfFile(msg);
	CloseHandle(fm.hMapFileCliente);
	CloseHandle(s.msgEvent);
	return 0;
}

DWORD WINAPI ThreadEnviaMSG(LPVOID param) {
	cliente* pCliente;

	pCliente = (cliente*)MapViewOfFile(fm.hMapFileDLL, FILE_MAP_WRITE, 0, 0, sizeof(cliente));
	if (pCliente == NULL) {
		_tprintf(TEXT("Erro a mapear pCliente...\n"));
		return -1;
	}

	do {
		WaitForSingleObject(s.Mutex, INFINITE);
		if (flag_start == 1) {
			pCliente->start = 0;
			pCliente->id = -1;
			flag_start = 0;
		}
		else if(flag_start == 2) {
			pCliente->start = 2;
			pCliente->id = bd.msg.cliente_id;
			flag_start = 0;
		}
		for (int i = 0; i < NUMBER_PLAYERS; i++) {
			if (bd.msg.cliente_id == vetor[i].id) {
				for (int j = 0; j < 10; j++) {
					pCliente->top[j] = vetor[i].top[j];
				}
				pCliente->id = bd.msg.cliente_id;
			}
		}
		ReleaseMutex(s.Mutex);
	} while (1);
	CloseHandle(s.Mutex);
	UnmapViewOfFile(pCliente);
	CloseHandle(fm.hMapFileDLL);
	return 0;
}

DWORD WINAPI ThreadBrindes(LPVOID param) {
	bonus* pBrinde;

	pBrinde = (bonus*)MapViewOfFile(fm.hMapFileBrinde, FILE_MAP_WRITE, 0, 0, sizeof(bonus));
	if (pBrinde == NULL) {
		_tprintf(TEXT("Erro a mapear pBrindes...\n"));
	}
	do {
		//  valida_barra();
		//  /*pBarra->pos.x = bd.bar.pos.x;
		//  pBarra->pos.y = bd.bar.pos.y;
		//  pBarra->dim.alt = bd.bar.dim.alt;
		//  pBarra->dim.larg = bd.bar.dim.larg;*/
		//  SetEvent(BrindeEvent);
		//  ResetEvent(BrindeEvent);
	} while (1);

	UnmapViewOfFile(pBrinde);
	CloseHandle(fm.hMapFileBrinde);
	return 0;
}

DWORD WINAPI ThreadBola(LPVOID param) {
	bola* pBola;

	pBola = (bola*)MapViewOfFile(fm.hMapFileBola, FILE_MAP_WRITE, 0, 0, sizeof(bola));
	if (pBola == NULL) {
		_tprintf(TEXT("Erro a mapear pBola...\n"));
	}
	int aux = 0;
	do {
		if (bd.msg.tecla == 's') {
			reinicia_jogo();
			aux = 1;
		}
		if (aux == 1)
			bd.tab.alteracoes = 0;
		if (b_inicial.comeco == 1 && bd.tab.alteracoes != 3) {
			aux = 0;
			aux++;
		}
		if (bd.msg.tecla == ' ' || aux == 3) {
			b_inicial.comeco = 0;
			valida_pos_bola();
			//verificacoes se perdeu o jogo
			if (bd.tab.vida == 0) {
				bd.tab.alteracoes = 3; // jogador perdeu o jogo
			}
			aux = 3;
		}
		pBola->pos.x = bd.b.pos.x;
		pBola->pos.y = bd.b.pos.y;
		pBola->dim = bd.b.dim;
		SetEvent(s.bEvent);
		ResetEvent(s.bEvent);
		Sleep(tempo);
	} while (1);
	UnmapViewOfFile(pBola);
	CloseHandle(fm.hMapFileBola);
	return 0;
}

DWORD WINAPI ThreadBarra(LPVOID param) {
	barra* pBarra;

	pBarra = (barra*)MapViewOfFile(fm.hMapFileBarra, FILE_MAP_WRITE, 0, 0, sizeof(barra));
	if (pBarra == NULL) {
		_tprintf(TEXT("Erro a mapear pBola...\n"));
	}
	do {
		valida_barra();
		if(b_inicial.comeco == 1) {
			bd.b.sentido = b_inicial.sentido;
			bd.b.pos.x = bd.bar.pos.x + (bd.bar.dim.larg / 2) - 7;
			bd.b.pos.y = bd.bar.pos.y - 5;
		}
		pBarra->pos.x = bd.bar.pos.x;
		pBarra->pos.y = bd.bar.pos.y;
		pBarra->dim.alt = bd.bar.dim.alt;
		pBarra->dim.larg = bd.bar.dim.larg;
		SetEvent(s.barraEvent);
		ResetEvent(s.barraEvent);
		Sleep(tempo);
	} while (1);

	UnmapViewOfFile(pBarra);
	CloseHandle(fm.hMapFileBarra);
	return 0;
}

cliente* criaVetor(int tam) {
	cliente* v;
	v = (cliente*)malloc(sizeof(cliente) * tam);
	return v;
}

void ListaJogadores() {
	int i;
	_tprintf(TEXT("\n\tLista de Jogadores:\n\n"));
	for (i = 0; i < id; i++)
		_tprintf(TEXT("[ID: %d] Jogador: %s -> pontos = %d / vidas = %d\n"), vetor[i].id, vetor[i].nome, vetor[i].pontos, vetor[i].vidas);
}

int lanca_threads() {

	ht.hThreadLogin = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadLogin, NULL, 0, &tid.threadIdLogin);
	if (ht.hThreadLogin == NULL) {
		_tprintf(TEXT("Erro ao criar Thread\n"));
		return -1;
	}

	ht.hThreadJogo = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadJogo, NULL, 0, &tid.threadIdJogo);
	if (ht.hThreadJogo == NULL) {
		_tprintf(TEXT("Erro ao criar Thread\n"));
		return -1;
	}

	ht.hThreadBarra = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadBarra, NULL, 0, &tid.threadIdBarra);
	if (ht.hThreadBarra == NULL) {
		_tprintf(TEXT("Erro ao criar Thread\n"));
		return -1;
	}

	ht.hThreadBola = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadBola, NULL, 0, &tid.threadIdBola);
	if (ht.hThreadBola == NULL) {
		_tprintf(TEXT("Erro ao criar Thread\n"));
		return -1;
	}

	ht.ThreadEscutaMSG = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadEscutaMSG, NULL, 0, &tid.threadIdJogoMSG);
	if (ht.ThreadEscutaMSG == NULL) {
		_tprintf(TEXT("Erro ao criar Thread\n"));
		return -1;
	}

	ht.hThreadEnviaMSG = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadEnviaMSG, NULL, 0, &tid.threadIdEnviaMSG);
	if (ht.hThreadEnviaMSG == NULL) {
		_tprintf(TEXT("Erro ao criar Thread\n"));
		return -1;
	}

	ht.hThreadBrinde = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadBrindes, NULL, 0, &tid.threadIdBrindes);
	if (ht.hThreadBrinde == NULL) {
		_tprintf(TEXT("Erro ao criar Thread brindes\n"));
		return -1;
	}

	ht.Hthreadbroad = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadBroad, NULL, 0, &tid.threadIdbroad);
	if (ht.Hthreadbroad == NULL) {
		_tprintf(TEXT("Erro ao criar Thread\n"));
		return -1;
	}

	ht.hThreadRecebe = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadRecebePipe, NULL, 0, &tid.threadRecebePipe);
	if (ht.Hthreadbroad == NULL) {
		_tprintf(TEXT("Erro ao criar Thread\n"));
		return -1;
	}
	return 1;
}

int cria_ficheiros_mapeados_server() {
	
	fm.hMapFileLogin = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(cliente), NomeFicheiroLogin);
	if (fm.hMapFileLogin == NULL) {
		_tprintf(TEXT("\nErro a criar FileMapp Login %s na memoria Partilhada!\n"), NomeFicheiroLogin);
		return -1;
	}
	else if (GetLastError() == ERROR_ALREADY_EXISTS) {
		_tprintf(TEXT("\nEste  %s ja foi criado!\n"), NomeFicheiroLogin);
		return -1;
	}

	fm.hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(mapa), NomeFicheiroMapeadoJogo);
	if (fm.hMapFile == NULL) {
		_tprintf(TEXT("\nErro a criar pJogo %s na memoria Partilhada!\n"), NomeFicheiroMapeadoJogo);
		return -1;
	}
	else if (GetLastError() == ERROR_ALREADY_EXISTS) {
		_tprintf(TEXT("\nEste pJogo -> %s ja foi criado!\n"), NomeFicheiroMapeadoJogo);
		return -1;
	}

	fm.hMapFileCliente = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(mensagem), NomeFicheiroMapeadoDLL);
	if (fm.hMapFileCliente == NULL) {
		_tprintf(TEXT("\nErro a criar msg %s na memoria Partilhada!\n"), NomeFicheiroMapeadoDLL);
		return -1;
	}
	else if (GetLastError() == ERROR_ALREADY_EXISTS) {
		_tprintf(TEXT("\nEste msg -> %s ja foi criado!\n"), NomeFicheiroMapeadoDLL);
		return -1;
	}

	fm.hMapFileDLL = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(cliente), NomeFicheiroMapeadoCliente);
	if (fm.hMapFileDLL == NULL) {
		_tprintf(TEXT("\nErro a criar pCliente %s na memoria Partilhada!\n"), NomeFicheiroMapeadoCliente);
		return -1;
	}
	else if (GetLastError() == ERROR_ALREADY_EXISTS) {
		_tprintf(TEXT("\nEste %s ja foi criado!\n"), NomeFicheiroMapeadoCliente);
		return -1;
	}

	fm.hMapFileBrinde = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(bonus), NomeFicheiroMapeadoBrindes);
	if (fm.hMapFileBrinde == NULL) {
		_tprintf(TEXT("\nErro a criar pBrinde %s na memoria Partilhada!\n"), NomeFicheiroMapeadoBrindes);
		return -1;
	}
	else if (GetLastError() == ERROR_ALREADY_EXISTS) {
		_tprintf(TEXT("\nEste  %s ja foi criado!\n"), NomeFicheiroMapeadoBrindes);
		return -1;
	}

	fm.hMapFileBola = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(bola), NomeFicheiroMapeadoBola);
	if (fm.hMapFileBola == NULL) {
		_tprintf(TEXT("\nErro a criar pBola %s na memoria Partilhada!\n"), NomeFicheiroMapeadoBola);
		return -1;
	}
	else if (GetLastError() == ERROR_ALREADY_EXISTS) {
		_tprintf(TEXT("\nEste pBola -> %s ja foi criado!\n"), NomeFicheiroMapeadoBola);
		return -1;
	}

	fm.hMapFileBarra = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(barra), NomeFicheiroMapeadoBarra);
	if (fm.hMapFileBarra == NULL) {
		_tprintf(TEXT("\nErro a criar pBarra %s na memoria Partilhada!\n"), NomeFicheiroMapeadoBarra);
		return -1;
	}
	else if (GetLastError() == ERROR_ALREADY_EXISTS) {
		_tprintf(TEXT("\nEste pBarra -> %s ja foi criado!\n"), NomeFicheiroMapeadoBarra);
		return -1;
	}

	return 1;
}

int cria_sincronizacao_server() {

	s.LoginSem = CreateSemaphore(NULL, N, N, NomeSemaforoLogin);
	if (s.LoginSem == NULL) {
		_tprintf(TEXT("Erro ao criar Semaforo %s\n"), NomeSemaforoLogin);
		return -1;
	}

	s.LoginEvent = CreateEvent(NULL, TRUE, FALSE, NomeEventoLogin);
	if (s.LoginEvent == NULL) {
		_tprintf(TEXT("Erro ao criar Evento %s\n"), NomeEventoLogin);
		return -1;
	}

	s.jEvent = CreateEvent(NULL, TRUE, FALSE, NomeEventoJogo);
	if (s.jEvent == NULL) {
		_tprintf(TEXT("Erro ao criar Evento %s\n"), NomeEventoJogo);
		return -1;
	}

	s.msgEvent = CreateEvent(NULL, TRUE, FALSE, NomeEventoJogoMensagem);
	if (s.msgEvent == NULL) {
		_tprintf(TEXT("Erro ao criar Evento %s\n"), NomeEventoJogoMensagem);
		return -1;
	}

	s.Mutex = CreateMutex(NULL, FALSE, MutexT);
	if (s.Mutex == NULL) {
		_tprintf(TEXT("Erro ao criar o Mutex de Mensagens!!!\n"));
		return -1;
	}

	s.BrindeEvent = CreateEvent(NULL, TRUE, FALSE, NomeEventoBrinde);
	if (s.BrindeEvent == NULL) {
		_tprintf(TEXT("Erro ao criar Evento %s\n"), NomeEventoBrinde);
		return -1;
	}

	s.bEvent = CreateEvent(NULL, TRUE, FALSE, NomeEventoBola);
	if (s.bEvent == NULL) {
		_tprintf(TEXT("Erro ao criar Evento %s\n"), NomeEventoBola);
		return -1;
	}

	s.barraEvent = CreateEvent(NULL, TRUE, FALSE, NomeEventoBarra);
	if (s.barraEvent == NULL) {
		_tprintf(TEXT("Erro ao criar Evento %s\n"), NomeEventoBarra);
		return -1;
	}

	return 1;
}

int _tmain(int argc, LPTSTR argv[]) {
	TCHAR menu;
	TCHAR nomeFich[TAM];
	int valid;

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif

	if (argc != 2) {
		definicoes_jogo();
		_tprintf(TEXT("\n[INFO] Foram usadas as definições por padrão!!!!\n"));
	}
	if (argc == 2) {
		_tcscpy_s(nomeFich, argv[1]);
		definicoes_jogo_ficheiro(nomeFich);
	}

	_tprintf(TEXT("\n=$||Bem vindo ao Servidor!||$=\n"));

	srand(time(NULL));

	vetor = criaVetor(NUMBER_PLAYERS);		//Alocar memoria para o numero maximo de clientes

	cria_mapa();

	//cria sincronizacao do server
	valid = cria_sincronizacao_server();
	if (valid == -1)
		exit(0);

	//cria ficheiros mapeados do server
	valid = cria_ficheiros_mapeados_server();
	if (valid == -1)
		exit(0);

	//cria as threads
	valid = lanca_threads();
	if (valid == -1)
		exit(0);


	do {
		_tprintf(TEXT("\n\tMenu:\n\n1 - Lista de Jogadores\n2 - Top10\n3 - Sair\n\nOpção:"));
		menu = _getch();
		fflush(stdin);
		switch (menu) {
		case '1':
			ListaJogadores();
			break;
		case'2':
			CriaTopRegistry();
			break;
		case'3':
			break;
		default:
			_tprintf(TEXT("Opção inválida!\n"));
			break;
		}
	} while (menu != '3');
	CloseHandle(ht.hThreadLogin);
	CloseHandle(ht.hThreadJogo);
	CloseHandle(ht.hThreadBola);
	CloseHandle(ht.ThreadEscutaMSG);
	CloseHandle(ht.hThreadBarra);
	CloseHandle(ht.hThreadBrinde);
	CloseHandle(ht.hThreadEnviaMSG);
	return 0;
}