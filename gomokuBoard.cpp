// gomokuBoard.cpp

// Klasa planszy do gomoku
class gomokuBoard {
	private:
		char *pboard;
		int xysize;
		
#if DEBUGCODE
		int mscf;	// Flaga modyfikacji od ostatniego clean
		int lastx;
		int lasty;
#endif
	public:
		// Tworzy nową planszę o wymiarach size*size, domyślnie 19x19
		gomokuBoard(int n_xysize=19) {
			xysize=n_xysize;
			pboard=(char*)malloc(sizeof(char)*xysize*xysize);
#if DEBUGCODE
			mscf=1;
#endif
			clear();
		}
		
		~gomokuBoard() {
			free(pboard);
		}
		
		// Ustawia konkretną wartość pola planszy
		inline void field(int x, int y, char v) {
			pboard[xysize*y+x]=v;
#if DEBUGCODE
			mscf=1;
			lastx=x;
			lasty=y;
#endif
		}
		
		// Sprawdza wartość pola planszy
		inline char field(int x, int y) {return pboard[xysize*y+x];}
		
		// Zwraca rozmiar planszy
		inline int getxysize() {return xysize;}
		
		// Sprawdza, kto wygrał
		int winner() {
			int x,y,c;
			int p;
			
			// Liniami poziomymi
			for(y=0; y<xysize; y++) {
				c=0; p=0;
				for(x=0; x<xysize; x++) {
					if(field(x,y)!=0) {
						if(field(x,y)==p) c++;
						else {p=field(x,y); c=1;}
					} else c=0;
				
					if(c>4) return p;
				}
			}
			
			// Liniami pionowymi
			for(x=0; x<xysize; x++) {
				c=0; p=0;
				for(y=0; y<xysize; y++) {
					if(field(x,y)!=0) {
						if(field(x,y)==p) c++;
						else {p=field(x,y); c=1;}
					} else c=0;

					if(c>4) return p;
				}
			}
			
			int d, i;
			
			// Liniami ukośnymi
			for(d=0; d<2; d++) {
				for(i=0; i<xysize; i++) {
					if(d==0) {x=0; y=i;}
					else {x=i; y=xysize-1;}
					
					p=0; c=0;
					while(x<xysize && y>=0) {
						if(field(x,y)!=0) {
							if(field(x,y)==p) c++;
							else {p=field(x,y); c=1;}
						} else c=0;
	
						if(c>4) return p;
						
						x++; y--;
					}
					
					if(d==0) {x=0; y=i;}
					else {x=i; y=0;}
					
					p=0; c=0;
					while(x<xysize && y<xysize) {
						if(field(x,y)!=0) {
							if(field(x,y)==p) c++;
							else {p=field(x,y); c=1;}
						} else c=0;
						
						if(c>4) return p;
						
						x++; y++;
					}
				}
			}
			
			return 0;
		}
		
		// Zeruje planszę (zmienia wszystkie pola na puste)
		void clear() {
			int i,v;
#if DEBUGCODE
			if(mscf==0)
				printf("gomokuBoard::clear(): Trying to clear clean board!!\n");
			mscf=0;
#endif
			v=xysize*xysize;
			for(i=0; i<v; i++) pboard[i]=0;
		}

#if DEBUGCODE
		// Wyświetla planszę na ekranie
		void show() {
			int x,y;
			
			putchar(' '); putchar(' ');
			for(x=0; x<xysize; x++) putchar('0'+x%10);
			putchar('\n');
			
			for(y=0; y<xysize; y++) {
				putchar('0'+y%10); putchar(' ');
				for(x=0; x<xysize; x++) {
					if(x==lastx && y==lasty) printf("\e[1m\e[32m");
					
					if(field(x,y)==0) putchar(' ');
					else if(field(x,y)==1) putchar('B');
					else if (field(x,y)==2) putchar('W');
					else putchar('?');
					if(x==lastx && y==lasty) printf("\e[0m");
				}
				putchar('\n');
			}
			printf("winner: %d\n", winner());
		}
#endif
};
