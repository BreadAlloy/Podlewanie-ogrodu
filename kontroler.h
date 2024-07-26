struct kontroler{
	uint8_t token = 0xFF; //token jest to wziecia
	std::vector<pin_do_przekaznika> elektrozawory;

	kontroler(std::vector<uint8_t> piny_do_zaworow, uint8_t pin_do_licznika_wody){
		assert(end(piny_do_zaworow.size() == LICZBA_SEKCJI));
		gpio_init();
		wiringPiSetupGpio();
		wodomierz_konstruktor(pin_do_licznika_wody);
		elektrozawory.resize(LICZBA_SEKCJI);
		for(uint8_t i = 0; i < LICZBA_SEKCJI; i++){
			elektrozawory[i] = pin_do_przekaznika(piny_do_zaworow[i]);
		}
	}

	void bezpieczne_ustawienie(uint8_t sekcja, bool nowy_stan, uint8_t id, bool unsafe = 0){
		//printf("AAS: %u\n", nowy_stan);
		assert(end(id == token || unsafe));
		for(uint8_t i = 0; i < elektrozawory.size(); i++){
			if(i == sekcja) continue;
			if(elektrozawory[i].stan && !unsafe){
				logi.write("Zawor innej sekcji " + to_string(i) + "jest wlaczony.");
				assert(end(0));
			}
		}
		//if(elektrozawory[sekcja].stan == nowy_stan){
			//logi.write("Sekcja " + to_string(sekcja) + " jest juz taka.");
			//assert(end(0));
		//}
		if(elektrozawory[sekcja].stan != nowy_stan){
			logi.write("Ustawiam stan: " + to_string(nowy_stan) + " na sekcji: " + to_string(sekcja));
			wyslij_update = true;
			elektrozawory[sekcja].ustaw_stan(nowy_stan);
		}
	}

	void wylacz_wszystkie(){
		for(uint8_t i = 0; i < elektrozawory.size(); i++){
			elektrozawory[i].ustaw_stan(0);
		}
		logi.write("Wylaczam wszystkie sekcje.");
	}

};












