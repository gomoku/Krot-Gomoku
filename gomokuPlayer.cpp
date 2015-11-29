// gomokuPlayer.cpp

// Klasa gracza

// Gracz może wykonać ruch lub zwrócić do engine'u brak gotowości do wykonanaia ruchu
// na co Engine odpowie przekazaniem wykonanaia do innych części programu,
// np. do interfejsu graficznego.

// Wykonanie ruchu przez przeciwnika automatycznie oznacza oczekiwanie na ruch
// od gracza własnego. Przeciwnik nie może wykonywac wielu ruchów. W tej sytuacji
// bowiem strategia gracza własnego byłaby do niczego.

// W przyszłości można dodać funkcję pozwalającą na wczytanie ustawien planszy
// do gracza własnego. Dzięki temu możnaby np. cofać ruchy, lub grać na
// ustawionej przez siebie planszy.

// gracz mógłby zwracać ruch
// - przez referencję/wskaźnik
// - przez osobną funkcję

// Dla funkcji move widzę ciekawsze zastosowania niż zwracanie wykonanego ruchu
// Ale też przekazywanie ruchu przeciwnika za każdym wywołaniem funkcji testującej wydaje się bezcelowe

//- enemymove(), move(enemy)
//- isready(), check(), think()
//- ownmove(), move()

// Możliwe rozwinięcia:
// - gracz wysyła komunikat - oddzielna funkcja
// - gracz wczytuje konkretny układ - odzielna funkcja lub za pomocą obecnych funkcji

// Engine gwarantuje, że nie będzie dwóch ruchów przeciwnika bez jednego własnego

// A zresztą nie wiem co to szkodzi, by funkcja move nie mogła być wielokrotnie wykonywana.
// W tym przypadku gracz nie może zaufać interfejsowi, i musi sprawdzić, czy podjał już decyzję
// czy nie. W pierwszym przypadku mógł na ślepo wykonać ruch zasugerowany przez Engine.
// W ten sposób ewentualny błąd engine'u, pozwalający na podwójny ruch pozostałby niedostrzeżony.
// Ale za to komplikuje to całość, zmuszając gracza do powielania funkcji engine'u.

#include <limits.h>

class gomokuPlayer {
	public:
		// Inicjacja gracza; id - numer gracza; xysize - rozmiar planszy
		virtual int init(int id, int xysize) {
#if DEBUGCODE
			printf("gomokuPlayer::init(id = %d, xysize = %d)\n", id, xysize);
#endif
		}
		
		// Wykonanie ruchu
		virtual void move(gomokuMove move)=0;
		
		// Myślenie (0 - decyzja; 1 - brak decyzji)
		virtual int think(void)=0;
		
		// Ruch własny
		virtual gomokuMove move(void)=0;
		
		// Zakończenie gracza
		virtual void destroy(void) {
#if DEBUGCODE
			printf("gomokuPlayer::destroy()\n");
#endif
		}
};

class gpRandom : public gomokuPlayer {
	private:
		int id;
		int xysize;
		int turn;
		gomokuBoard *board;
		gomokuMove mymove;
	public:
		int init(int id, int xysize) {
			gomokuPlayer::init(id, xysize);
			this->xysize=xysize;
			this->id=id;
			board=new gomokuBoard(xysize);
			turn=0;
		}
		
		void move(gomokuMove move) {
			// Wykonaj ruch graczem przeciwnika
			if(id==1) board->field(move.x, move.y, 2);
			else board->field(move.x, move.y, 1);
		}
		
		int think() {
			// Wykonaj losowy ruch
			do {
				mymove.x=rand()%xysize;
				mymove.y=rand()%xysize;
			} while (board->field(mymove.x, mymove.y)>0);
			
			// Zaznacz ten ruch na planszy
			board->field(mymove.x, mymove.y, id);
			
			// Zwiększ wskaźnik tury
			turn++;
			
			// Zwróć podjęcie decyzji
			return 0;
		}
		
		gomokuMove move() {
			return mymove;
		}
		
		void destroy() {
			gomokuPlayer::destroy();
			delete board;
		}
};

// gpInterface
// gracz używający interfejsu gomokuInterface
class gpInterface : public gomokuPlayer {
	private:
		int id;
		int xysize;
		gomokuBoard *board;
		gomokuInterface *itf;
	public:
		gpInterface(gomokuInterface *itf) {
			this->itf=itf;
		} 
		
		int init(int id, int xysize) {
			gomokuPlayer::init(id, xysize);
			this->xysize=xysize;
			this->id=id;
			board=new gomokuBoard(xysize);
			
			// Jeżeli id==1, to ten gracz zaczyna, wiec moze odblokowac interfejs
			if(id==1) itf->lock=false;
		}
		
		void move(gomokuMove move) {
			// Wykonaj ruch graczem przeciwnika
			if(id==1) board->field(move.x, move.y, 2);
			else board->field(move.x, move.y, 1);
			// Odblokuj interfejs
			itf->lock=false;
		}
		
		int think() {
			// Interfejs jeszcze nie wykonał ruchu - jeszcze brak decyzji
			if(itf->lock==false) return 1;
			
			// Jeżeli interfejs wykona błędny ruch, to gracz dalej czeka na ruch
			if(board->field(itf->move.x, itf->move.y)!=0) {
#if DEBUGCODE
				printf("gpinterface::think() Interfejs wykonał błędny ruch!\n");
#endif
				itf->lock=false;
				return 1;
			}
			
			// Zaznacz ten ruch na planszy
			board->field(itf->move.x, itf->move.y, id);
			
			// Zwróć podjęcie decyzji
			return 0;
		}
		
		gomokuMove move() {
			// Przenieś ruch do tymczasowej struktury
			// dla bezpieczeństwa wielowątkowości
			gomokuMove tmpmove=itf->move;
			// Odblokuj interfejs
			itf->lock=0;		
			return tmpmove;
		}
		
		void destroy() {
			gomokuPlayer::destroy();
			itf->lock=true;
			delete board;
		}
};

