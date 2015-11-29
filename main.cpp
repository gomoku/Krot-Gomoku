// main.cpp

// Wartości plaszy:
// 0 - nic, 1 - czarny, 2 - biały

//-----------------------------------------------------------------------------
// Główne opcje konfiguracyjne
//-----------------------------------------------------------------------------

// System docelowy
#define OS_LINUX		1
#define OS_WINDOWS	2
#define TARGETOS	OS_LINUX

// Określa, czy program powinien zawierać dodatkowy kod
// przydatny przy debugowaniu
#define DEBUGCODE 1

#include <cstdio>
#include <cstdlib>

#include "gomokuBoard.cpp"

typedef struct gomokuMove {
	int x;
	int y;
};

#include "gomokuInterface.cpp"

#include "gomokuPlayer.cpp"

#include "gpAiOne.cpp"

#include "gpAiTwo.cpp"

#include "gomokuGame.cpp"

#include "ggui.cpp"

#if TARGETOS==OS_WINDOWS
#include <windows.h>
#endif

#include <time.h>
#include <unistd.h>

//-----------------------------------------------------------------------------
// Główna pętla
//-----------------------------------------------------------------------------

int main() {
	srand((unsigned)time(NULL));

	printf("GOMOKU by krot\n");
	printf("version dev2\n\n");
	
	// Przygotowanie interfejsu
	gomokuInterface *itf;
	itf=new gomokuInterface();
	
	// Przygotowanie graczy
	gomokuPlayer *gp[2];
	gp[0]= new gpInterface(itf);
	//gp[0]= new gpAiOne();
	gp[1]= new gpAiTwo();
	//gp[1]= new gpRandom();
	
	// Przygotowanie gry
	gomokuGame *gg;
	gg=new gomokuGame(gp[0], gp[1]);
	
	// Przygotowanie GUI
	gomokuFlBoard *gfb;
	gfb=new gomokuFlBoard(itf, gg->getboard(), 300, 300, "Gomoku by Janusz Kowalski");
	gfb->show();
	
	int state=0;
	
#if TARGETOS == OS_LINUX
	struct timespec req={0, 10000000};
#endif
	
	while(1) {
		// Jeżeli nie było po prostu oczekiwania
		if(state!=1) gfb->update();
		
		// Przekaż kontrolę do GUI
		if(Fl::check()==0) break;
#if TARGETOS == OS_LINUX
		nanosleep(&req, NULL);
#endif
#if TARGETOS == OS_WINDOWS
		Sleep(10);
#endif
		
		fflush(stdout);
		
		// Jeżeli jakieś zdarzenia, to przerwij grę
		if(state>1) break;
		state=gg->play();
	};
	
	gp[0]->destroy();
	gp[1]->destroy();
	
	Fl::run();
	
	return 0;
}
