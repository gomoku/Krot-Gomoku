// gomokuGame.cpp

class gomokuGame {
	private:
		gomokuBoard *board;
		gomokuPlayer *gp[2];
		gomokuMove move;
		int turn;
	public:
		gomokuGame(gomokuPlayer *gp1, gomokuPlayer *gp2) {
			// Przygotowanie planszy
			board=new gomokuBoard(19);
			
			// Przygotowanie tablicy graczy
			gp[0]=gp1;
			gp[1]=gp2;
			
			// Inicjacja graczy
			gp[0]->init(1, 19); gp[1]->init(2, 19);
			
			turn=0;
		}
		
		~gomokuGame(void) {
			delete board;
		}
		
		// Funkcja zwraca wskaźnik do tablicy
		gomokuBoard *getboard() {return board;}
		
		// Funkcja gry
		// Zwraca:
		// 0 - wykonano zwykły ruch
		// 1 - żaden z graczy nie wykonał ruchu
		// 2 i więcej - coś się stało
		int play() {
			int state;
			
			if(!(turn<19*19)) return 2;		
			
			// Jeżeli gracz wykonał ruch
			if((state=gp[turn%2]->think())==0) {
				// Pobierz wykonany ruch od gracza
				move=gp[turn%2]->move();
				
				// Zaznacz ruch, jeżeli jest poprawny
				if(board->field(move.x, move.y)==0) {
					board->field(move.x, move.y, turn%2+1);
				} else {
					printf("Incorrect move (%d, %d)!\n", move.x, move.y);
					return 2;
				}

#if DEBUGCODE				
				// Pokaż tablicę
				board->show();
#endif

				if(board->winner()) return 2;
				
				// Zwiększ wskaźnik tury
				turn++;
				
				// Powiedz drugiemu graczowi, że wykonano ruch
				// !! To może być pewnego rodzaju błąd, ponieważ na jedno wywołanie play()
				// powinno przypadać wywołanie tylko jednego gracza
				gp[turn%2]->move(move);				
			}
			
			// Jeżeli żaden z graczy nie wykonał ruchu, to powiedz o tym
			if(state==1) return 1;
						
			return 0;
		}
};
