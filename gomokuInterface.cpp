// gomokuInterface.cpp

// interfejs
class gomokuInterface {
	public:
		gomokuInterface(void) {
			lock=true;
		}
		// Blokada. Jeżeli zablokowana, to zmiana move jest zabroniona
		bool lock;
		// Ruch
		gomokuMove move;
};
