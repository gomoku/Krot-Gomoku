// gpAiTwo.cpp

class gpAiTwo : public gomokuPlayer {
	private:
		int id;
		int xysize;
		gomokuBoard *board;
		int *modf;
		
		typedef struct XYPos {int x; int y;};
		
		// Kolejne elementy danej linii sfield[kierunek][numer porządkowy]
		static const XYPos sfield[4][8];
		
		// Przeciwnik danego gracza
		static const int enemyof[3];
		
		// Używane przez funkcję solve
		gomokuMove mmove;
		
		static const int maxdepth=6;
		
	public:
		int init(int id, int xysize) {
			gomokuPlayer::init(id, xysize);
			this->id=id;
			this->xysize=xysize;
			board=new gomokuBoard(xysize);
			modf=new int[xysize*xysize];
			for(int i=0; i<xysize*xysize; i++) modf[xysize*xysize]=0;
		}
		
		void modfchange(int x, int y, int n) {
			for(int d=0; d<4; d++) {
				for(int i=0; i<8; i++) {
					int nx=x+sfield[d][i].x;
					int ny=y+sfield[d][i].y;
					if(nx<0 || nx>=xysize || ny<0 || ny>=xysize) continue;
					modf[xysize*ny+nx]+=n;
				}
			}
		}
		
		void destroy() {
			delete board;
			delete[] modf;
			gomokuPlayer::destroy();
		}
		
		void move(gomokuMove move) {
			board->field(move.x, move.y, enemyof[id]);
		}
		
		// Funkcja oblicza największą długość ciągów własnych i przeciwnika
		// To, który gracz jest własny, a który przeciwnika, jest określane względem CID
		inline void oelcalc(int &ol, int &el, int x, int y, int cid) {
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
		}
		
		// Co funkcja robi każdy widzi...
		void space(int n) {
			for(int i=0; i<n; i++) putchar(' ');
		}
		
		// Funkcja ocenia dany ruch, i modyfikuje mq, oraz mmove
		inline int checkmove(int x, int y, int cid, int depth, int &mq, int pq);
		
		// Funkcja zwraca ocenę najlepszego ruchu gracza cid
		// depth - głębokość w drzewie gry; zazwyczaj 0
		// cid - bierzący gracz
		// !! Wszystkie ruchy mogą się okazać nieuzasadnione. Wtedy funkcja ich nie sprawdzi
		// i nie wybierze żadnego
		int solve(int cid, int depth=0, int pq=0);
		
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

const gpAiTwo::XYPos gpAiTwo::sfield[4][8]={
	{{0, -4}, {0, -3}, {0, -2}, {0, -1}, {0, 1}, {0, 2}, {0, 3}, {0, 4}},
	{{4, -4}, {3, -3}, {2, -2}, {1, -1}, {-1, 1}, {-2, 2}, {-3, 3}, {-4, 4}},
	{{-4, 0}, {-3, 0}, {-2, 0}, {-1, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}},
	{{4, 4}, {3, 3}, {2, 2}, {1, 1}, {-1, -1}, {-2, -2}, {-3, -3}, {-4, -4}}
};

const int gpAiTwo::enemyof[3]={0,2,1};

#define gpAiTwo_USELESS	0		// brak użytecznych ruchów
#define gpAiTwo_USEFUL	1		// użyteczne ruchy (tylko)
#define gpAiTwo_BREAK	2		// zerwanie pętli (tylko)

// Statystyka - symulowane ruchy
static int stat_simmoves;
// Statystyka - wszystkie ruchy
static int stat_allmoves;

int gpAiTwo::solve(int cid, int depth, int pq) {
	bool usefulmoves=false;
	bool keeploop=true;
	
	if(depth==0) {
		stat_simmoves=0;
		stat_allmoves=0;
	}
	
	// Pamiętana ocena ruchu
	int mq;
	if(cid==id) mq=INT_MIN;
	else mq=INT_MAX;			
	
	if(depth==0) {
		static XYPos movelist[19*19];
		// Przygotowanie listy ruchów
		for(int x=0; x<xysize; x++)
			for(int y=0; y<xysize; y++) {
				movelist[y*xysize+x].x=x;
				movelist[y*xysize+x].y=y;
			}
		
		for(int i=0; i<xysize*xysize; i++) {
			int src, dest;
			src=rand()%(xysize*xysize);
			dest=rand()%(xysize*xysize);
			XYPos t;
			t=movelist[src]; movelist[src]=movelist[dest]; movelist[dest]=t;
		}
		
		// Ruch na podstawie listy
		for(int i=0; i<xysize*xysize && keeploop; i++) {
			int x=movelist[i].x;
			int y=movelist[i].y;
			int status=checkmove(x,y,cid,depth,mq,pq);
			if(status & gpAiTwo_USEFUL) usefulmoves=true;
			if(status & gpAiTwo_BREAK) keeploop=false;
		}
		
		if(!usefulmoves) {
			// poszukaj ruchu heurystycznie
			int maxol=0;
			for(int i=0; i<xysize*xysize; i++) {
				int x=movelist[i].x;
				int y=movelist[i].y;
				if(board->field(x,y)!=0) continue;
				int ol=0, el=0;
				oelcalc(ol, el, x, y, cid);
				if(ol>maxol) {
					maxol=ol;
					mmove.x=x;
					mmove.y=y;
					usefulmoves=true;
				}	
			}
		}
	} else {
		for(int x=0; x<xysize && keeploop; x++)
			for(int y=0; y<xysize && keeploop; y++) {
				int status=checkmove(x,y,cid,depth,mq,pq);
				if(status & gpAiTwo_USEFUL) usefulmoves=true;
				if(status & gpAiTwo_BREAK) keeploop=false;
			}
	}
	if(depth==0) {
		printf("stat_simmoves: %d\n", stat_simmoves);
		printf("stat_allmoves: %d\n", stat_allmoves);
	}
	
	if(!usefulmoves) mq=0;
	return mq;
}

int gpAiTwo::checkmove(int x, int y, int cid, int depth, int &mq, int pq) {
	int ol=0;
	int el=0;
	
	stat_allmoves++;
	
	// Czy pole jest wolne?
	if(board->field(x,y)!=0) return gpAiTwo_USELESS;
	
	if(depth>1 && modf[xysize*y+x]<1) return gpAiTwo_USELESS;

	// Czy ruch jest uzasadniony?
	oelcalc(ol, el, x, y, cid);
	if((ol<5-(maxdepth-depth+1)/2) && (el<5-(maxdepth-depth)/2)) return gpAiTwo_USELESS;
	
	stat_simmoves++;
	
	// Wykonaj ruch
	board->field(x,y,cid);
	modfchange(x,y,1);
	
	// Spróbuj stwierdzić, czy ktoś wygrywa
	int winner=0;
	if(ol==4) winner=cid;
	
	int q=0;	// Ocena
	
	// Oceń ruch
	if(winner==0) {
		depth++;
		if(depth<maxdepth) q=solve(enemyof[cid], depth, mq);
		else q=0;
		depth--;
	} else if(winner==id) q=4096*maxdepth/(depth+1);
	else q=-4096*maxdepth/(depth+1);
	
	// Wybierz najlepszą ocenę (gracz własny wybiera najwyższą, gracz przeciwnika - najniższą)
	if((cid==id && q>mq) || (cid!=id && q<mq)) {
		mq=q;
		if(depth==0) {
			mmove.x=x; mmove.y=y;
		}
	}
		
	// Cofnij wykonany ruch
	board->field(x,y,0);
	modfchange(x,y,-1);
	
	// !! Powinno się także sprawdzać, czy bierzący gracz wygrał pośrednio (odpowiednia punktacja)	
	// Jeżeli bierzący gracz osiągnął swoje maksimum...
	//if(depth>0 && cid==id && mq>2048) return gpAiTwo_BREAK | gpAiTwo_USEFUL;
	
	// Jeżeli gracz przeciwnika osiągnął swoje maksimum...
	//if(depth>0 && cid==enemyof[id] && mq<-2048) return gpAiTwo_BREAK | gpAiTwo_USEFUL;
	
	// Jeżeli bierzący gracz jest przeciwnikiem dla id, i mq<=pq, to skończ
	if(depth>0 && cid!=id && mq<=pq) return gpAiTwo_BREAK | gpAiTwo_USEFUL;
	
	// Jeżeli bierzący gracz jest przyjacielem dla id, i mq>=pq, to skończ
	if(depth>0 && cid==id && mq>=pq) return gpAiTwo_BREAK | gpAiTwo_USEFUL;
	
	return gpAiTwo_USEFUL;
}
