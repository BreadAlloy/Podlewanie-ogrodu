



struct sub_wydarzenie{
        //uint8_t w_wypadku_konfliktu = 0;
        // 0 skasuj wydarzenie
        // 1 dodaj do kolejki
        // 2 dodaj do szczytu kolejki
        // 3 skasuj bizace i wykonaj

        uint8_t sekcja = 0;
        uint16_t ilosc = 0;

	static const uint32_t CHAR_ROZMIAR_SUB_WYDARZENIE = 3;

	std::basic_string<uint8_t> do_char(){
		uint8_t temp[] = {(char)sekcja, (char)ilosc, (char)(ilosc >> 8)};
		std::basic_string<uint8_t> res(temp, CHAR_ROZMIAR_SUB_WYDARZENIE);
		return res;
	}

	void z_char(std::basic_string<uint8_t> &in){
		sekcja = (uint8_t)in[0];
		ilosc = 0;
		ilosc|=(uint16_t)in[1];
		ilosc|=((uint16_t)in[2] >> 8);
		in = in.substr(CHAR_ROZMIAR_SUB_WYDARZENIE);
	}
};

struct wydarzenie{
        uint8_t id;
        bool enabled;
        std::vector<sub_wydarzenie> sub_wydarzenia;
	uint8_t index_ostatniego = 0;
        bool wybieracz_sterowania;
        // 0 czas
        // 1 objetosc
        czas poczatek;
	czas koniec;
	std::vector<czas> przejsciowe;//[LICZBA_SEKCJI + 1];
        uint8_t co_ile_dni;
        std::vector<bool> dozwolone_dnie = {1,1,1,1,1,1,1};

        wydarzenie(uint8_t id, std::vector<uint16_t> ilosci, std::vector<uint8_t> sekcje,
        bool enabled = true, czas poczatek = czas(), bool sterowanie = 0,
         uint8_t co_ile_dni = 1, std::vector<bool> dozwolone_dnie = {1,1,1,1,1,1,1})
        : poczatek(poczatek), wybieracz_sterowania(sterowanie), co_ile_dni(co_ile_dni),
        dozwolone_dnie(dozwolone_dnie), id(id), enabled(enabled){
                //assert(LICZBA_SEKCJI == ilosci.size());
                //assert(LICZBA_SEKCJI == sekcje.size());
		assert(ilosci.size() == sekcje.size());
                sub_wydarzenia.resize(ilosci.size());
		for(uint8_t i = 0; i < sub_wydarzenia.size(); i++){
                        sub_wydarzenia[i].sekcja = sekcje[i];
                        sub_wydarzenia[i].ilosc = ilosci[i];
                }
                assert(dozwolone_dnie.size() == 7);
                assert(dozwolone_dnie[0] || dozwolone_dnie[1] || dozwolone_dnie[2] ||
                       dozwolone_dnie[3] || dozwolone_dnie[4] || dozwolone_dnie[5] ||
                       dozwolone_dnie[6]);
        }

	wydarzenie(){}

        bool czy_nalezy_wykonac(czas& zewnetrzny){
		//printf("%s\n\n", str().c_str());
		if(koniec < zewnetrzny) znajdz_nastepne(zewnetrzny);
                return zewnetrzny > poczatek && enabled && zewnetrzny < koniec;
        }

        void znajdz_nastepne(czas& zewnetrzny){
		index_ostatniego = 0;
                poczatek.mday = poczatek.mday + co_ile_dni;
                poczatek.wday = poczatek.wday + co_ile_dni;
                poczatek.normalizacja();
		uint8_t i = 0;
		bool termin_w_przeszlosci = poczatek < zewnetrzny;
                while(!(dozwolone_dnie[poczatek.wday] && !termin_w_przeszlosci) && i < 8){
			if(!termin_w_przeszlosci || poczatek > zewnetrzny){
				i++;
				termin_w_przeszlosci = false;
			}
                        poczatek.mday = poczatek.mday + co_ile_dni;
                        poczatek.wday = poczatek.wday + co_ile_dni;
                        poczatek.normalizacja();
                }
		if(i >= 8){
                	enabled = false;
			errory.write("Nie znaleziono mozliwego terminu. Wylaczam ten program: " + to_string(id) + "."); //W pewnych warunkach jest powtarzane
			return;
		}
		oblicz_czasy();
		logi.write("Program " + to_string(id) + " znalazl kolejny poczatek o " + poczatek.str());
        }

	void oblicz_czasy(){
		przejsciowe.resize(sub_wydarzenia.size() + 1);
		przejsciowe[0] = poczatek;
		for(uint8_t i = 0; i < sub_wydarzenia.size(); i++){
			przejsciowe[i + 1] = przejsciowe[i];
			if(wybieracz_sterowania){
				przejsciowe[i + 1].min = przejsciowe[i + 1].min + (uint8_t)(sub_wydarzenia[i].ilosc / OKOLO_LITRY_NA_MINUTE);
			} else {
				przejsciowe[i + 1].min = przejsciowe[i + 1].min + (uint8_t)sub_wydarzenia[i].ilosc;
			}
			przejsciowe[i + 1].normalizacja();
		}
		koniec = przejsciowe[sub_wydarzenia.size()];
	}

	bool akcja(czas& zewnetrzny){

		if(wybieracz_sterowania){
			//sterowanie objetoscia
			if(sub_wydarzenia[index_ostatniego].ilosc <
			   (uint16_t)wodomierz_l()){
				zarzadca.bezpieczne_ustawienie(
					sub_wydarzenia[index_ostatniego].sekcja
					, 0, id);
				index_ostatniego++;
				wodomierz_reset();
				if(index_ostatniego == sub_wydarzenia.size()){
					znajdz_nastepne(zewnetrzny);
					return false;
				}
			}
			zarzadca.bezpieczne_ustawienie(
				sub_wydarzenia[index_ostatniego].sekcja
				, 1, id);

			return true;

		} else {
			//sterowanie czasem

			uint8_t index_czasu = index_ostatniego;
			while(index_czasu < (przejsciowe.size() - 1) &&
			      przejsciowe[index_czasu] < zewnetrzny){
				index_czasu++;
			}
			if(index_ostatniego != index_czasu){
				//printf("BOOP\n");
				zarzadca.bezpieczne_ustawienie(
					sub_wydarzenia[index_ostatniego].sekcja,
					0, id);
			}
			if(index_czasu == (przejsciowe.size() - 1)){
				//index_ostatniego = 0;
				znajdz_nastepne(zewnetrzny);
				return false;
			}
			zarzadca.bezpieczne_ustawienie(
				sub_wydarzenia[index_czasu].sekcja,
				1, id);
			index_ostatniego = index_czasu;

			return true;
		}
		//zarzadca.bezpieczne_ustawienie(1, 1, 1);



		bool res = zewnetrzny > koniec;
		if(res) znajdz_nastepne(zewnetrzny);
		return res;
	}

	void default1(){
		id = 0;
		enabled = 0;
		wybieracz_sterowania = 0;
		co_ile_dni = 1;
		dozwolone_dnie = {1,0,0,0,0,0,0};
		sub_wydarzenia.resize(LICZBA_SEKCJI);
		for(uint8_t i = 0; i < LICZBA_SEKCJI; i++){
			sub_wydarzenia[i].sekcja = i;
			sub_wydarzenia[i].ilosc = 0;
		}
		oblicz_czasy();
	}

	void default2(){
		id = 0;
		enabled = 1;
		wybieracz_sterowania = 0;
		co_ile_dni = 1;
		dozwolone_dnie = {1,1,1,1,1,1,1};
		sub_wydarzenia.resize(LICZBA_SEKCJI);
		for(uint8_t i = 0; i < LICZBA_SEKCJI; i++){
			sub_wydarzenia[i].sekcja = i;
			sub_wydarzenia[i].ilosc = 1;
		}
		oblicz_czasy();
	}

	void default3(){
		id = 0;
		enabled = 1;
		wybieracz_sterowania = 1;
		co_ile_dni = 1;
		dozwolone_dnie = {1,1,1,1,1,1,1};
		sub_wydarzenia.resize(LICZBA_SEKCJI);
		for(uint8_t i = 0; i < LICZBA_SEKCJI; i++){
			sub_wydarzenia[i].sekcja = i;
			sub_wydarzenia[i].ilosc = 61;
		}
		oblicz_czasy();
	}

	std::string str(){
		std::string res = "id: " + to_string(id) + " | enabled: " + to_string(enabled) + "\n" +
		"sterowanie: " + to_string(wybieracz_sterowania) + " | co_ile_dni: " + to_string(co_ile_dni) + "\n" +
		"Dozwolone dni: " + to_string(dozwolone_dnie[0]) + " | " + to_string(dozwolone_dnie[1]) + " | " +
		to_string(dozwolone_dnie[2]) + " | " + to_string(dozwolone_dnie[3]) + " | " +
		to_string(dozwolone_dnie[4]) + " | " + to_string(dozwolone_dnie[5]) + " | " +
		to_string(dozwolone_dnie[6]) + " | " + "\n";
		for(uint8_t i = 0; i < sub_wydarzenia.size(); i++){
			res = res + przejsciowe[i].str() + "\n" +
			"Sekcja:" + to_string(sub_wydarzenia[i].sekcja) +
			" | Ilosc :" + to_string(sub_wydarzenia[i].ilosc) + " min/l \n\n";
		}
		res = res + koniec.str();
		return res;
	}

	std::basic_string<uint8_t> do_char(){
		uint16_t kompresor = 0;
		kompresor |= ((uint16_t)enabled << 0);
		kompresor |= ((uint16_t)wybieracz_sterowania << 1);
		kompresor |= ((uint16_t)dozwolone_dnie[0] << 2);
		kompresor |= ((uint16_t)dozwolone_dnie[1] << 3);
		kompresor |= ((uint16_t)dozwolone_dnie[2] << 4);
		kompresor |= ((uint16_t)dozwolone_dnie[3] << 5);
		kompresor |= ((uint16_t)dozwolone_dnie[4] << 6);
		kompresor |= ((uint16_t)dozwolone_dnie[5] << 7);
		kompresor |= ((uint16_t)dozwolone_dnie[6] << 8);
		uint8_t temp[] = {(char)kompresor, (char)(kompresor >> 8), (char)id,
		(char)co_ile_dni, (char)sub_wydarzenia.size()};
		std::basic_string<uint8_t> res(temp, 5);
		res += poczatek.do_char();
		for(uint8_t i = 0; i < sub_wydarzenia.size(); i++){
			res+=sub_wydarzenia[i].do_char();
		}
		return res;
	}

	void z_char(std::basic_string<uint8_t> &in){
		uint16_t rekompresor = 0;
		rekompresor |= (uint16_t)in[0];
		rekompresor |= ((uint16_t)in[1]) << 8;
		enabled = (rekompresor >> 0) & 1;
		wybieracz_sterowania = (rekompresor >> 1) & 1;
		dozwolone_dnie.resize(7);
		dozwolone_dnie[0] = (rekompresor >> 2) & 1;
		dozwolone_dnie[1] = (rekompresor >> 3) & 1;
		dozwolone_dnie[2] = (rekompresor >> 4) & 1;
		dozwolone_dnie[3] = (rekompresor >> 5) & 1;
		dozwolone_dnie[4] = (rekompresor >> 6) & 1;
		dozwolone_dnie[5] = (rekompresor >> 7) & 1;
		dozwolone_dnie[6] = (rekompresor >> 8) & 1;
		id = (uint8_t)in[2];
		co_ile_dni = (uint8_t)in[3];
		sub_wydarzenia.resize((uint8_t)in[4]);
		in = in.substr(5);
		poczatek.z_char(in);
		for(uint8_t i = 0; i < sub_wydarzenia.size(); i++){
			sub_wydarzenia[i].z_char(in);
		}
		oblicz_czasy();
	}

};
























