#include "Cliente.h"
#include "resource.h"


//estruturas
bipm bipMaps;
cliente jogador;
hdcParaImagens hPI;
baseDados bd;
tecla t;

//variaveis e handles
TCHAR szProgName[] = TEXT("Base");
HINSTANCE hInstanceGlobal;
HDC memdc;

///////////////////////////////////////////////////////////////

void carregaBitMaps() {
	bipMaps.Barra = (HBITMAP)LoadImage(NULL, TEXT("../../Imagens/barra.bmp"), IMAGE_BITMAP, 70, 50, LR_LOADFROMFILE);
	bipMaps.Explosao = (HBITMAP)LoadImage(NULL, TEXT("../../Imagens/explosao.bmp"), IMAGE_BITMAP, 40, 40, LR_LOADFROMFILE);
	bipMaps.Tijolo1 = (HBITMAP)LoadImage(NULL, TEXT("../../Imagens/tijolo1.bmp"), IMAGE_BITMAP, 50, 25, LR_LOADFROMFILE);
	bipMaps.Tijolo2 = (HBITMAP)LoadImage(NULL, TEXT("../../Imagens/tijolo2.bmp"), IMAGE_BITMAP, 50, 25, LR_LOADFROMFILE);
	bipMaps.Tijolo3 = (HBITMAP)LoadImage(NULL, TEXT("../../Imagens/tijolo3.bmp"), IMAGE_BITMAP, 50, 25, LR_LOADFROMFILE);
	bipMaps.Tijolo4 = (HBITMAP)LoadImage(NULL, TEXT("../../Imagens/tijolo4.bmp"), IMAGE_BITMAP, 50, 25, LR_LOADFROMFILE);
	bipMaps.Bola = (HBITMAP)LoadImage(NULL, TEXT("../../Imagens/bola.bmp"), IMAGE_BITMAP, 15, 15, LR_LOADFROMFILE);
	bipMaps.Bonus = (HBITMAP)LoadImage(NULL, TEXT("../../Imagens/bonus.bmp"), IMAGE_BITMAP, 80, 80, LR_LOADFROMFILE);
	bipMaps.Background = (HBITMAP)LoadImage(NULL, TEXT("../../Imagens/fundo.bmp"), IMAGE_BITMAP, DIMMAPA_X, DIMMAPA_Y, LR_LOADFROMFILE);
	bipMaps.Wallpaper = (HBITMAP)LoadImage(NULL, TEXT("../../Imagens/wallpaper.bmp"), IMAGE_BITMAP, DIMMAPA_X + 200, DIMMAPA_Y, LR_LOADFROMFILE);
	bipMaps.lvd = (HBITMAP)LoadImage(NULL, TEXT("../../Imagens/limite_vertical_d.bmp"), IMAGE_BITMAP, 10, DIMMAPA_Y + 10, LR_LOADFROMFILE);
	bipMaps.lve = (HBITMAP)LoadImage(NULL, TEXT("../../Imagens/limite_vertical_e.bmp"), IMAGE_BITMAP, 10, DIMMAPA_Y + 10, LR_LOADFROMFILE);
	bipMaps.lh = (HBITMAP)LoadImage(NULL, TEXT("../../Imagens/limite_horizontal.bmp"), IMAGE_BITMAP, DIMMAPA_X + 10, 10, LR_LOADFROMFILE);
	bipMaps.logo = (HBITMAP)LoadImage(NULL, TEXT("../../Imagens/logo.bmp"), IMAGE_BITMAP, 150, 70, LR_LOADFROMFILE);
	bipMaps.vida = (HBITMAP)LoadImage(NULL, TEXT("../../Imagens/vida.bmp"), IMAGE_BITMAP, 40, 50, LR_LOADFROMFILE);
	bipMaps.go = (HBITMAP)LoadImage(NULL, TEXT("../../Imagens/gameOver.bmp"), IMAGE_BITMAP, 400, 400, LR_LOADFROMFILE);
	bipMaps.bonus1 = (HBITMAP)LoadImage(NULL, TEXT("../../Imagens/bonus1.bmp"), IMAGE_BITMAP, 40, 40, LR_LOADFROMFILE);
	bipMaps.bonus2 = (HBITMAP)LoadImage(NULL, TEXT("../../Imagens/bonus2.bmp"), IMAGE_BITMAP, 40, 40, LR_LOADFROMFILE);
	bipMaps.bonus3 = (HBITMAP)LoadImage(NULL, TEXT("../../Imagens/bonus3.bmp"), IMAGE_BITMAP, 40, 40, LR_LOADFROMFILE);

}

void encerra_cliente() {
	DeleteObject(hPI.JWallpaper);
	DeleteObject(hPI.JBackground);
	DeleteObject(hPI.JBarra);
	DeleteObject(hPI.JBola);
	DeleteObject(hPI.JBonus);
	DeleteObject(hPI.JExplosao);
	DeleteObject(hPI.JTijolo1);
	DeleteObject(hPI.JTijolo2);
	DeleteObject(hPI.JTijolo3);
	DeleteObject(hPI.JTijolo4);
	DeleteObject(hPI.JLVE);
	DeleteObject(hPI.JLVD);
	DeleteObject(hPI.JLH);
	DeleteObject(hPI.JLogo);
	DeleteObject(hPI.JVida);
	DeleteObject(hPI.JGO);
	DeleteObject(hPI.JBonus1);
	DeleteObject(hPI.JBonus2);
	DeleteObject(hPI.JBonus3);
	DeleteDC(memdc);
	exit(0);
}

DWORD WINAPI escutaPacote(LPVOID param) {
	TCHAR str[TAM];
	int aux;
	do {
		if (jogador.start == 1) {
			//recebe bola
			bd.b = recebeBola(bd.b);
			//recebe mapa
			bd.tab = recebeJogo(bd.tab);
			//recebe barra
			bd.bar = recebeBarra(bd.bar);

			//cria uma copia
			SelectObject(memdc, GetStockObject(BLACK_BRUSH));
			PatBlt(memdc, 0, 0, DIMMAPA_X + 200, DIMMAPA_Y + 100, PATCOPY);

			//Imprime mapa
			BitBlt(memdc, 5, 10, DIMMAPA_X, DIMMAPA_Y, hPI.JBackground, 0, 0, SRCCOPY);
			for (int i = 0; i < bd.tab.n_elementos; i++) {
				if (bd.tab.tijolos[i].vida == 0) {
					PlaySoundA((LPCSTR) "../../Sons/shot.wav", NULL, SND_FILENAME | SND_ASYNC);
					TransparentBlt(memdc, bd.tab.tijolos[i].pos.x, bd.tab.tijolos[i].pos.y, 40, 40, hPI.JExplosao, 0, 0, 40, 40, RGB(255, 255, 255));
				}
				else if (bd.tab.tijolos[i].vida > 0) {
					if (bd.tab.tijolos[i].tipo == 0) {
						TransparentBlt(memdc, bd.tab.tijolos[i].pos.x, bd.tab.tijolos[i].pos.y, 50, 25, hPI.JTijolo1, 0, 0, 50, 25, RGB(255, 255, 255));
					}
					else if (bd.tab.tijolos[i].tipo == 1) {
						TransparentBlt(memdc, bd.tab.tijolos[i].pos.x, bd.tab.tijolos[i].pos.y, 50, 25, hPI.JTijolo2, 0, 0, 50, 25, RGB(255, 255, 255));
					}
					else if (bd.tab.tijolos[i].tipo == 2) {
						TransparentBlt(memdc, bd.tab.tijolos[i].pos.x, bd.tab.tijolos[i].pos.y, 50, 25, hPI.JTijolo3, 0, 0, 50, 25, RGB(255, 255, 255));
					}
					else {
						TransparentBlt(memdc, bd.tab.tijolos[i].pos.x, bd.tab.tijolos[i].pos.y, 50, 25, hPI.JTijolo4, 0, 0, 50, 25, RGB(255, 255, 255));
					}
				}
			}
			//Imprime Brinde
			if (bd.tab.brinde.existe == 1) {
				if (bd.tab.brinde.tipo == 0) {
					TransparentBlt(memdc, bd.tab.brinde.pos.x-20, bd.tab.brinde.pos.y-20, 40, 40, hPI.JBonus1, 0, 0, 40, 40, RGB(255, 255, 255));
				}
				else if (bd.tab.brinde.tipo == 1) {
					TransparentBlt(memdc, bd.tab.brinde.pos.x-20, bd.tab.brinde.pos.y-20, 40, 40, hPI.JBonus2, 0, 0, 40, 40, RGB(255, 255, 255));
				}
				else {
					TransparentBlt(memdc, bd.tab.brinde.pos.x-20, bd.tab.brinde.pos.y-20, 40, 40, hPI.JBonus3, 0, 0, 40, 40, RGB(255, 255, 255));
				}
			}
			TransparentBlt(memdc, bd.bar.pos.x, bd.bar.pos.y - 5, 70, 50, hPI.JBarra, 0, 0, 70, 50, RGB(255, 255, 255));
			//Imprime limites e extras
			TransparentBlt(memdc, 0, 0, 10, DIMMAPA_Y + 10, hPI.JLVE, 0, 0, 10, DIMMAPA_Y + 10, RGB(255, 255, 255));	//limite esquerdo
			TransparentBlt(memdc, DIMMAPA_X + 1, 0, 10, DIMMAPA_Y + 10, hPI.JLVD, 0, 0, 10, DIMMAPA_Y + 10, RGB(255, 255, 255));	//limite direito
			TransparentBlt(memdc, 0, 0, DIMMAPA_X + 10, 10, hPI.JLH, 0, 0, DIMMAPA_X + 10, 10, RGB(255, 255, 255));	//limite cima
			TransparentBlt(memdc, DIMMAPA_X + 12, 0, 150, 70, hPI.JLogo, 0, 0, 150, 70, RGB(255, 255, 255));	//logo
			_stprintf_s(str, TEXT("Jogador: %s"), jogador.nome);
			TextOut(memdc, DIMMAPA_X + 20, 210, (LPCWSTR)str, _tcslen(str));
			_stprintf_s(str, TEXT("Pontos: %d"), bd.tab.pontos);
			TextOut(memdc, DIMMAPA_X + 20, 230, (LPCWSTR)str, _tcslen(str));
			aux = 0;
			for (int i = 0; i < bd.tab.vida; i++) {
				TransparentBlt(memdc, DIMMAPA_X + 15 + aux, DIMMAPA_Y - 50, 40, 50, hPI.JVida, 0, 0, 40, 50, RGB(255, 255, 255));	//vida
				aux += 30;
			}
			//Imprime Barra
			TransparentBlt(memdc, bd.bar.pos.x, bd.bar.pos.y - 5, 70, 50, hPI.JBarra, 0, 0, 70, 50, RGB(255, 255, 255));
			//Imprime bola
			TransparentBlt(memdc, bd.b.pos.x, bd.b.pos.y, 15, 15, hPI.JBola, 0, 0, 15, 15, RGB(255, 255, 255));

			InvalidateRect(hWnd, NULL, 0);
			//O NULL do invalidrect representa o retangulo que vai ser desenhado e é so preciso mudar o bit blt nas coordenadas a desenhar
			
			if (bd.tab.alteracoes == 1) {
				PlaySoundA((LPCSTR) "../../Sons/perdeu.wav", NULL, SND_FILENAME | SND_ASYNC);
			}
			if (bd.tab.alteracoes == 2) {
				PlaySoundA((LPCSTR) "../../Sons/fundo.wav", NULL, SND_FILENAME | SND_ASYNC);
				if (MessageBox(hWnd, TEXT("GANHOU o jogo!!\nDeseja começar de novo?"), TEXT("Vitória"), MB_YESNO | MB_ICONINFORMATION | MB_APPLMODAL) == IDYES) {
					bd.msg.tecla = 's';
					enviaMensagem(bd.msg);
					MessageBox(hWnd, TEXT("Prepar-se! O jogo vai reiniciar..."), TEXT("Espere"), MB_OK);
				}
				else {
					encerra_cliente();
				}
			}
			else if (bd.tab.alteracoes == 3) {
				TransparentBlt(memdc, 200, 100, 400, 400, hPI.JGO, 0, 0, 400, 400, RGB(255, 255, 255));
				PlaySoundA((LPCSTR) "../../Sons/gameOver.wav", NULL, SND_FILENAME | SND_ASYNC);
				if (MessageBox(hWnd, TEXT("PERDEU o jogo!!\nDeseja começar de novo?"), TEXT("Derrota"), MB_YESNO | MB_ICONINFORMATION | MB_APPLMODAL) == IDYES) {
					bd.msg.tecla = 's';
					enviaMensagem(bd.msg);
					MessageBox(hWnd, TEXT("Prepar-se! O jogo vai reiniciar..."), TEXT("Espere"), MB_OK);
				}
				else {
					encerra_cliente();
				}
			}
		}
	} while (1);
}

BOOL CALLBACK TrataDlg(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
	TCHAR str[TAM] = TEXT("Username");
	switch (messg) {
	case WM_CLOSE:
		EndDialog(hWnd, 0); //Fechar a caixa de diálogo atual
		return TRUE; //Tratei o evento da caixa de diálogo
	case WM_INITDIALOG:
		SetDlgItemText(hWnd, IDC_EDIT1, str);
		return TRUE;
	case WM_COMMAND: //Tratar apenas para botões - é mais comum
		switch (LOWORD(wParam)) {
		case IDCANCEL:
			EndDialog(hWnd, 0); //Fechar a caixa de diálogo atual
			return TRUE; //Tratei o evento da caixa de diálogo
		case IDOK:
			//Ler da caixa de texto
			GetDlgItemText(hWnd, IDC_EDIT1, jogador.nome, TAM);
			jogador = Login(jogador);
			jogador = recebeMensagem(jogador);
			if (jogador.start == 2) {
				MessageBox(hWnd, TEXT("Login efetuado com sucesso!!\n Prepar-se! O jogo vai iniciar..."), TEXT("Login Sucesso"), MB_OK);
				EndDialog(hWnd, 0);
				jogador.start = 1;
				return TRUE;
			}
			else {
				MessageBox(hWnd, TEXT("Login nao efetuado..."), TEXT("Login Falhado"), MB_OK | MB_ICONWARNING);
				return TRUE;
			}
		case IDOK_remoto:
			GetDlgItemText(hWnd, IDC_EDIT1, str, TAM);
			GetDlgItemText(hWnd, IDC_IPADDRESS1, str, TAM);
			MessageBox(hWnd, str, TEXT("Conteúdo da Caixa de Texto"), MB_OK);
			return TRUE;
		}
	}
	return FALSE; //Não tratei o evento
}

BOOL CALLBACK TrataDlg2(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
	const TCHAR* str[] = { TEXT("João      100pts"), TEXT("Ana    50pts") };
	TCHAR tecla[1];
	switch (messg) {
	case WM_CLOSE:
		EndDialog(hWnd, 0); //Fechar a caixa de diálogo atual
		return TRUE; //Tratei o evento da caixa de diálogo
	case WM_INITDIALOG:
		tecla[0] = t.esquerda;
		SetDlgItemText(hWnd, IDC_MOVE, tecla);
		tecla[0] = t.direita;
		SetDlgItemText(hWnd, IDC_MOVD, tecla);
		tecla[0] = t.bola_inicio;
		SetDlgItemText(hWnd, IDC_LB, tecla);
		for (int i = 0; i < 2; i++)
			SendDlgItemMessage(hWnd, IDC_LIST2, LB_ADDSTRING, 0, (LPARAM)str[i]);
		return TRUE;
	case WM_COMMAND: //Tratar apenas para botões - é mais comum
		switch (LOWORD(wParam)) {
		case IDOKCONFIG:
			GetDlgItemText(hWnd, IDC_MOVE, tecla, 1);
			t.esquerda = tecla[0];
			GetDlgItemText(hWnd, IDC_MOVD, tecla, 1);
			t.direita = tecla[0];
			GetDlgItemText(hWnd, IDC_LB, tecla, 1);
			t.bola_inicio = tecla[0];
			EndDialog(hWnd, 0);
			return TRUE;
		case IDOK:
			EndDialog(hWnd, 0); //Fechar a caixa de diálogo atual
			return TRUE; //Tratei o evento da caixa de diálogo    
		}
	}
	return FALSE; //Não tratei o evento
}

LRESULT CALLBACK TrataEventos(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	HBITMAP screenshot;
	PAINTSTRUCT ps;
	static int x;

	switch (messg) {
	case WM_CLOSE:
		//PlaySoundA((LPCSTR) "../../Sons/inicio.wav", NULL, SND_FILENAME | SND_ASYNC);
		if (MessageBox(hWnd, TEXT("Deseja mesmo sair?"), TEXT("Sair"), MB_YESNO | MB_ICONEXCLAMATION) == IDYES) {
			encerra_cliente();
		}
		break;
	case WM_CREATE:
		////Acontece uma vez, na criação da janela
		hdc = GetDC(hWnd);
		memdc = CreateCompatibleDC(hdc);
		screenshot = CreateCompatibleBitmap(hdc, DIMMAPA_X + 200, DIMMAPA_Y + 100);
		SelectObject(memdc, screenshot);
		SelectObject(memdc, GetStockObject(BLACK_BRUSH));

		// Cria uma "janela"( memory device compatible ) para cada bipmap
		hPI.JBackground = CreateCompatibleDC(memdc);
		hPI.JWallpaper = CreateCompatibleDC(memdc);
		hPI.JBola = CreateCompatibleDC(memdc);
		hPI.JBonus = CreateCompatibleDC(memdc);
		hPI.JExplosao = CreateCompatibleDC(memdc);
		hPI.JTijolo1 = CreateCompatibleDC(memdc);
		hPI.JTijolo2 = CreateCompatibleDC(memdc);
		hPI.JTijolo3 = CreateCompatibleDC(memdc);
		hPI.JTijolo4 = CreateCompatibleDC(memdc);
		hPI.JBarra = CreateCompatibleDC(memdc);
		hPI.JLVE = CreateCompatibleDC(memdc);
		hPI.JLVD = CreateCompatibleDC(memdc);
		hPI.JLH = CreateCompatibleDC(memdc);
		hPI.JLogo = CreateCompatibleDC(memdc);
		hPI.JVida = CreateCompatibleDC(memdc);
		hPI.JGO = CreateCompatibleDC(memdc);
		hPI.JBonus1 = CreateCompatibleDC(memdc);
		hPI.JBonus2 = CreateCompatibleDC(memdc);
		hPI.JBonus3 = CreateCompatibleDC(memdc);

		//colocar o bipMap para dentro do Device Contexts
		SelectObject(hPI.JBackground, bipMaps.Background);
		SelectObject(hPI.JWallpaper, bipMaps.Wallpaper);
		SelectObject(hPI.JBola, bipMaps.Bola);
		SelectObject(hPI.JBonus, bipMaps.Bonus);
		SelectObject(hPI.JExplosao, bipMaps.Explosao);
		SelectObject(hPI.JBarra, bipMaps.Barra);
		SelectObject(hPI.JTijolo1, bipMaps.Tijolo1);
		SelectObject(hPI.JTijolo2, bipMaps.Tijolo2);
		SelectObject(hPI.JTijolo3, bipMaps.Tijolo3);
		SelectObject(hPI.JTijolo4, bipMaps.Tijolo4);
		SelectObject(hPI.JLVD, bipMaps.lvd);
		SelectObject(hPI.JLVE, bipMaps.lve);
		SelectObject(hPI.JLH, bipMaps.lh);
		SelectObject(hPI.JLogo, bipMaps.logo);
		SelectObject(hPI.JVida, bipMaps.vida);
		SelectObject(hPI.JGO, bipMaps.go);
		SelectObject(hPI.JBonus1, bipMaps.bonus1);
		SelectObject(hPI.JBonus2, bipMaps.bonus2);
		SelectObject(hPI.JBonus3, bipMaps.bonus3);

		PatBlt(memdc, 0, 0, DIMMAPA_X + 200, DIMMAPA_Y + 100, PATCOPY);  //cria uma copia
		ReleaseDC(hWnd, hdc);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_LOGIN_LOCAL:
			if (jogador.start == 0) {
				DialogBox(hInstanceGlobal, MAKEINTRESOURCE(IDD_DIALOG2), hWnd, (DLGPROC)TrataDlg); //Modal
			}
			break;
		case  ID_LOGIN_REMOTO:
			if (jogador.start == 0) {
				DialogBox(hInstanceGlobal, MAKEINTRESOURCE(IDD_DIALOG3), hWnd, (DLGPROC)TrataDlg); //Modal
			}
			break;
		case ID_TOP10:
			DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)TrataDlg2); //Não Modal
			break;
		case ID_TECLA:
			DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG4), NULL, (DLGPROC)TrataDlg2); //Não Modal
			break;
		case ID_NOVOJOGO:
			if (jogador.start == 1) {
				if (MessageBox(hWnd, TEXT("Deseja mesmo reiniciar o jogo?"), TEXT("Reiniciar Jogo"), MB_YESNO | MB_ICONQUESTION) == IDYES) {
					bd.msg.tecla = 's';
					enviaMensagem(bd.msg);
					MessageBox(hWnd, TEXT("Prepar-se! O jogo vai reiniciar..."), TEXT("Espere"), MB_OK);
				}
			}
			break;
		case ID_EXIT:
			//PlaySoundA((LPCSTR) "../../Sons/inicio.wav", NULL, SND_FILENAME | SND_ASYNC);
			if (MessageBox(hWnd, TEXT("Deseja mesmo sair?"), TEXT("Sair"), MB_YESNO | MB_ICONEXCLAMATION) == IDYES) {
				encerra_cliente();
			}
			break;
		}
		break;
	case WM_KEYDOWN:
		if (wParam == VK_RIGHT) {
			if (jogador.start == 1) {
				bd.msg.tecla = 'd';
				enviaMensagem(bd.msg);
			}
		}
		if (wParam == VK_LEFT) {
			if (jogador.start == 1) {
				bd.msg.tecla = 'a';
				enviaMensagem(bd.msg);
			}
		}
		if (wParam == ' ') {
			if (jogador.start == 1) {
				bd.msg.tecla = ' ';
				enviaMensagem(bd.msg);
			}
		}
		if (wParam == t.direita) {
			if (jogador.start == 1) {
				bd.msg.tecla = 'd';
				enviaMensagem(bd.msg);
			}
		}
		else if (wParam == t.esquerda) {
			if (jogador.start == 1) {
				bd.msg.tecla = 'a';
				enviaMensagem(bd.msg);
			}
		}
		else if (wParam == t.bola_inicio) {
			if (jogador.start == 1) {
				bd.msg.tecla = ' ';
				enviaMensagem(bd.msg);
			}
		}
		break;
	case WM_MOUSEMOVE:
		if (jogador.start == 1) {
			x = GET_X_LPARAM(lParam);
			if (x - 35 < bd.bar.pos.x)
				bd.msg.tecla = 'a';
			else if (x - 35 > bd.bar.pos.x)
				bd.msg.tecla = 'd';
			else
				break;
			enviaMensagem(bd.msg);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		BitBlt(hdc, 0, 0, DIMMAPA_X + 200, DIMMAPA_Y + 100, memdc, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		break;
	default:
		return DefWindowProc(hWnd, messg, wParam, lParam);
	}
	return(0);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
	MSG lpMsg;
	WNDCLASSEX wcApp;
	hInstanceGlobal = hInst;

	//funcoes de inicializacao do cliente
	carregaBitMaps();					//carrega os bitmaps
	cria_sincronizacao_DLL();           //inicializa a sincronizacao do cliente na DLL
	cria_ficheiros_mapeados_abertos();  //inicializa os ficheiros mapeados do lado cliente na DLL

	//Thread para escutar o mapa do servidor
	DWORD IDEscutaPacote;
	HANDLE HEscutaPacote = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)escutaPacote, NULL, 0, &IDEscutaPacote);
	if (HEscutaPacote == NULL) {
		MessageBox(hWnd, TEXT("[ERRO] Criação da Thread para escuta de pacotes..."), TEXT("ERROR"), MB_OK | MB_ICONWARNING);
		return -1;
	}

	//configuracao da janela
	wcApp.cbSize = sizeof(WNDCLASSEX);
	wcApp.hInstance = hInstanceGlobal;
	wcApp.lpszClassName = szProgName;
	wcApp.lpfnWndProc = TrataEventos;
	wcApp.style = CS_HREDRAW | CS_VREDRAW;
	wcApp.hIcon = LoadIcon(hInstanceGlobal, MAKEINTRESOURCE(IDI_ICON1));
	wcApp.hIconSm = NULL;
	wcApp.hCursor = LoadCursor(hInstanceGlobal, MAKEINTRESOURCE(IDC_CURSOR1));
	wcApp.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wcApp.cbClsExtra = 0;
	wcApp.cbWndExtra = 0;
	wcApp.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

	if (!RegisterClassEx(&wcApp))
		return(0);

	hWnd = CreateWindow(
		szProgName, TEXT("Arkanoid@ISEC"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, DIMMAPA_X + 200, DIMMAPA_Y + 65,
		(HWND)HWND_DESKTOP, (HMENU)NULL, (HINSTANCE)hInst, 0);

	//Mostrar a janela
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	BitBlt(memdc, 0, 0, DIMMAPA_X + 200, DIMMAPA_Y, hPI.JWallpaper, 0, 0, SRCCOPY);
	InvalidateRect(hWnd, NULL, 0);
	//PlaySoundA((LPCSTR) "../../Sons/inicio.wav", NULL, SND_FILENAME | SND_ASYNC);

	while (GetMessage(&lpMsg, NULL, 0, 0)) {
		TranslateMessage(&lpMsg);
		DispatchMessage(&lpMsg);
	}
	return((int)lpMsg.wParam);
}