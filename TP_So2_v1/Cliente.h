#include "..\\DLL\\DLL.h"
#include "windowsx.h"

typedef struct img {
	HBITMAP Tijolo1;
	HBITMAP Tijolo2;
	HBITMAP Tijolo3;
	HBITMAP Tijolo4;
	HBITMAP Bola;
	HBITMAP Background;
	HBITMAP Wallpaper;
	HBITMAP Bonus;
	HBITMAP Explosao;
	HBITMAP Barra;
	HBITMAP lve;
	HBITMAP lvd;
	HBITMAP lh;
	HBITMAP logo;
	HBITMAP vida;
	HBITMAP go;
	HBITMAP bonus1;
	HBITMAP bonus2;
	HBITMAP bonus3;
}bipm;

typedef struct JanelasHCD {
	HDC JTijolo1;
	HDC JTijolo2;
	HDC JTijolo3;
	HDC JTijolo4;
	HDC JBola;
	HDC JBackground;
	HDC JWallpaper;
	HDC JBonus;
	HDC JExplosao;
	HDC JBarra;
	HDC JLVE;
	HDC JLVD;
	HDC JLH;
	HDC JLogo;
	HDC JVida;
	HDC JGO;
	HDC JBonus1;
	HDC JBonus2;
	HDC JBonus3;
}hdcParaImagens;

typedef struct teclas {
	TCHAR direita;
	TCHAR esquerda;
	TCHAR bola_inicio;
}tecla;