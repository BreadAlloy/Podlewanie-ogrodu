//struct miernik_uzytej_wody{
	uint32_t wodomierz_licznik = 0;
	detektor_zmiany_stanu wodomierz_gpio;
	static dt wodomierz_anty_spam;

	void wodomierz_dodaj_jeden(){
		wodomierz_licznik++;
        	double temp = wodomierz_anty_spam.pomiar();
        	if(temp < 0.005){
                	printf("Moze jest zle: %f\n", temp);
        	}
		printf("Licznik:%lu\n", wodomierz_licznik);
	}

	void wodomierz_konstruktor(uint8_t numer_gpio, uint32_t stan_poczatkowy = 0){
		wodomierz_licznik = stan_poczatkowy;
		wodomierz_gpio = detektor_zmiany_stanu(numer_gpio);
		wiringPiISR(wodomierz_gpio.number, INT_EDGE_BOTH, wodomierz_dodaj_jeden);
	}

	double wodomierz_l(){
		return 1.0 * (double)wodomierz_licznik;
	}

	void wodomierz_reset(uint32_t stan_zresetowany = 0){
		wodomierz_licznik = stan_zresetowany;
	}

	double wodomierz_pomiar(uint32_t stan_zresetowany = 0){
		double temp = wodomierz_l();
		wodomierz_reset(stan_zresetowany);
		return temp;
	}
//};

