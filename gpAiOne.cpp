// gpAiOne.cpp

// Algorytm jest wciaż zbyt wolny.
// Problem z funkcją losową polega na tym, że przy cięciach alfa-beta nie da się przewidzieć,
// czy przypadkiem rand()%1024 nie zmieni wartości bierzącego ruchu tak, że okaże się on lepszy...
// W modelowym rozwiązaniu losowość powinno się stosować na poziomie, na którym rzeczywiście coś oceniamy.
// Zmiana poprzedniej oceny może być niebezpieczna.

class gpAiOne : public gomokuPlayer {
	private:
		int id;
		int xysize;
		gomokuBoard *board;
		
		typedef struct XYPos {int x; int y;};
		// Kolejne elementy danej linii sfield[kierunek][numer porządkowy]
		static const XYPos sfield[4][8];
		
		// Przeciwnik danego gracza
		static const int enemyof[3];
		
		// Używane przez funkcję solve
		gomokuMove mmove;
		
	public:
		int init(int id, int xysize) {
			gomokuPlayer::init(id, xysize);
			this->id=id;
			this->xysize=xysize;
			board=new gomokuBoard(xysize);
		}
		
		void destroy() {
			delete board;
			gomokuPlayer::destroy();
		}
		
		void move(gomokuMove move) {
			board->field(move.x, move.y, enemyof[id]);
		}
		
		// Funkcja sprawdza użyteczność ruchu z punktu widzenia możliwości jego oceny
		// 1 - użyteczny
		// 0 - nieużyteczny
		inline bool isuseful(int &ol, int &el, int x, int y, int cid, int depth, int maxdepth) {
			el=0;
			ol=0;
			
			for(int d=0; d<4; d++)
				for(int i=0; i<5; i++) {
					int tl;	// tymczasowo obliczona długość
					int k;
					// Gracz własny
					tl=0;
					for(k=0; k<4; k++) {
						int nx=x+sfield[d][i+k].x;
						int ny=y+sfield[d][i+k].y;
						if(nx<0 || nx>=xysize || ny<0 || ny>=xysize) continue;
						if(board->field(nx, ny)==enemyof[cid]) break;
						if(board->field(nx, ny)==cid) tl++;
					}
							
					if(k==4 && tl>ol) ol=tl;
							
					// Gracz przeciwnika
					tl=0;
					for(k=0; k<4; k++) {
						int nx=x+sfield[d][i+k].x;
						int ny=y+sfield[d][i+k].y;
						if(nx<0 || nx>=xysize || ny<0 || ny>=xysize) continue;
						if(board->field(nx, ny)==cid) break;
						if(board->field(nx, ny)==enemyof[cid]) tl++;
					}
							
					if(k==4 && tl>el) el=tl;
				}
			//	Jeżeli ruch nieuzasadniony, to nie wykonuj go
			if((ol<5-(maxdepth-depth+1)/2) && (el<5-(maxdepth-depth)/2)) return false;
			else return true;
		}
		
		// Co funkcja robi każdy widzi...
		void space(int n) {
			for(int i=0; i<n; i++) putchar(' ');
		}

		// Funkcja zwraca ocenę najlepszego ruchu gracza cid
		// depth - głębokość w drzewie gry; zazwyczaj 0
		// cid - bierzący gracz
		// !! Wszystkie ruchy mogą się okazać nieuzasadnione. Wtedy funkcja ich nie sprawdzi
		// i nie wybierze żadnego
		int solve(int cid, int depth=0, int pq=0) {
			static const int maxdepth=4;
			
			bool usefulmoves=false;
			bool keeploop=true;
			
			// Pamiętana ocena ruchu
			int mq;
			if(cid==id) mq=INT_MIN;
			else mq=INT_MAX;
			
			// Maksymalne długości ciągów poszczególnych graczy
			// ol - gracz własny względem cis
			// el - gracz przecinwika względem cid
			int ol;
			int el;
			
			for(int x=0; x<xysize && keeploop; x++)
				for(int y=0; y<xysize && keeploop; y++) {
					// Czy pole jest wolne?
					if(board->field(x,y)!=0) continue;
					
					// Czy ruch jest uzasadniony?
					if(!isuseful(ol, el, x, y, cid, depth, maxdepth)) continue;
					
					usefulmoves=true;
					
					// Wykonaj ruch
					board->field(x,y,cid);
					
					// Spróbuj stwierdzić, czy ktoś wygrywa
					int winner=0;
					if(ol==4) winner=cid;
					
					int q=9999;		// ocena (wartość tu nieistotna, tylko dla wysypania się przy błędzie)
					
					// Oceń ruch
					if(winner==0) {
						depth++;
						if(depth<maxdepth) q=solve(enemyof[cid], depth, mq);
						else q=rand()%1024-512;
						depth--;
					} else if(winner==id) q=4096;
					else q=-4096;
					
					// !! Możnaby tu dodać trochę losowości
					
					// Wybierz najlepszą ocenę (gracz własny wybiera najwyższą, gracz przeciwnika - najniższą)
					if((cid==id && q>mq) || (cid!=id && q<mq)) {
						mq=q;
						if(depth==0) {
							mmove.x=x; mmove.y=y;
						}
					}
						
					// Cofnij wykonany ruch
					board->field(x,y,0);
					
					// Jeżeli wygrał bierzący gracz, i poziom jest niezerowy, to wyjdź z pętli
					if(depth>0 && winner==cid) keeploop=false;
					
					// Jeżeli bierzący gracz jest przeciwnikiem dla id, i mq<pq, to skończ
					if(depth>0 && cid!=id && mq<pq) keeploop=false;
					
					// Jeżeli bierzący gracz jest przyjacielem dla id, i mq>pq, to skończ
					if(depth>0 && cid==id && mq>pq) keeploop=false;
				}
			
			if(usefulmoves==false) mq=rand()%1024-512;
			return mq;
		}
		
		int think() {
			// Przygotowanie się na wypadek, gdyby żaden ruch nie został wybrany
			do {
				mmove.x=rand()%xysize;
				mmove.y=rand()%xysize;
			} while(board->field(mmove.x, mmove.y)!=0);
			printf("punktacja: %d\n", solve(id));
			return 0;
		}
		
		gomokuMove move() {
			// Zaznacz własny ruch
			board->field(mmove.x, mmove.y, id);
			// Zwróć ruch
			return mmove;
		}
};

const gpAiOne::XYPos gpAiOne::sfield[4][8]={
	{{0, -4}, {0, -3}, {0, -2}, {0, -1}, {0, 1}, {0, 2}, {0, 3}, {0, 4}},
	{{4, -4}, {3, -3}, {2, -2}, {1, -1}, {-1, 1}, {-2, 2}, {-3, 3}, {-4, 4}},
	{{-4, 0}, {-3, 0}, {-2, 0}, {-1, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}},
	{{4, 4}, {3, 3}, {2, 2}, {1, 1}, {-1, -1}, {-2, -2}, {-3, -3}, {-4, -4}}
};

const int gpAiOne::enemyof[3]={0,2,1};
