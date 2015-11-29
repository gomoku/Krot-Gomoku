// ggui.cpp

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Box.H>

class gomokuFlBoard : public Fl_Window {
	private:
		int xysize;
		gomokuBoard *board;
		gomokuInterface *itf;
		
		inline int xlinepos(int i) {
			return w()*(i+1)/(xysize+1);
		}
		
		inline int ylinepos(int i) {
			return h()*(i+1)/(xysize+1);
		}
	public:
		gomokuFlBoard(gomokuInterface *itf, gomokuBoard *board, int w, int h, const char *title) : Fl_Window(w, h, title) {
			this->itf=itf;
			xysize=19;
			this->board=board;
			color(fl_rgb_color(210,199,78));
			resizable(this);
		}
		
		~gomokuFlBoard() {
			delete board;
		}
		
		void update() {
			// Uaktualnij swoją zawartość
			this->redraw();
			
			// Sprawdź, czy ktoś nie wygrał
			if(board->winner()!=0) {
				// Komunikat
				static char msg[20];
				snprintf(msg, 20, "Player %d wins!\0", board->winner());
				// Wyświetl komunikat
				Fl_Window *window;
				window=new Fl_Window(256, 128, msg);
				window->set_modal();
				Fl_Box *box=new Fl_Box(8,8, 256-8, 128-8, msg);
				box->labelsize(40);
				window->end();
				window->show();
			}
		}
		
		void draw() {
			Fl_Window::draw();

			// Rysowanie siatki
			fl_color(0,0,0);
			int i;
			for(i=0; i<xysize; i++) {
				fl_line(xlinepos(i),0, xlinepos(i), h());
				fl_line(0,ylinepos(i),w(),ylinepos(i));
			}
			
			// Rysowanie pionków
			for(int x=0; x<xysize; x++) {
				for(int y=0; y<xysize; y++) {
					int t=board->field(x,y);
					
					int r=w()/(xysize+1)/2;
					if(h()/(xysize+1)/2<r) r=h()/(xysize+1)/2;
					
					if(t==1 || t==2) {
						// Rysowanie cienia
						
						if(r>5) {
							fl_color(128,128,128);
							fl_pie(xlinepos(x)-r+1, ylinepos(y)-r+1, 2*r+1, 2*r+1, 0, 360);
						}
					
						if(t==2) fl_color(255,255,255);
						else fl_color(0,0,0);
						fl_pie(xlinepos(x)-r, ylinepos(y)-r, 2*r, 2*r, 0, 360);
					}
				}
			}
		}
		
		// !! Co to zwraca?!
		int handle(int e) {
			Fl_Window::handle(e);
			if(e==FL_PUSH) {	// Jeżeli ruch myszki
				
				if(itf->lock==false) {				
					// Oblicz nowe linie punktu aktywnego
					itf->move.x=(Fl::event_x()+w()/(xysize+1)/2)*(xysize+1)/w()-1;
					itf->move.y=(Fl::event_y()+h()/(xysize+1)/2)*(xysize+1)/h()-1;
				
					// Czy zawieraja sie w przedziale?
					if(itf->move.x>=0 && itf->move.x<xysize && itf->move.y>=0 && itf->move.y<xysize) {
					
						// Wypisz ruch
#if DEBUGCODE
						printf("gomokuFlBoard: chosen move: (%d, %d)\n", itf->move.x, itf->move.y);
#endif						
						// Zablokuj
						itf->lock=true;
					}
				}
			}
			
		}
};
