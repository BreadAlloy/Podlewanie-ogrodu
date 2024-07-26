
struct super_kontroler{
	uint8_t liczba_sekcji = 15;
	bool reczne = 0;
	std::vector<bool> reczne_stany;
	std::vector<wydarzenie> wydarzenia;

	super_kontroler(){
		reczne_stany.resize(liczba_sekcji);
		for(uint8_t i = 0; i < liczba_sekcji; i++){
			reczne_stany[i] = 0;
		}
	}

	void tick(){
		if(zarzadca.token != 0xFF && !wydarzenia[zarzadca.token].akcja(globalny_czas) && !reczne){
			logi.write("Program " + to_string(zarzadca.token) + "oddaje token");
			zarzadca.token = 0xFF;
		}
		if(reczne){
			//printf("AAAAaAA\n");
			for(uint8_t i = 0; i < liczba_sekcji; i++){
				//if(reczne_stany[i]) printf("saasddasd\n");
				zarzadca.bezpieczne_ustawienie(i, reczne_stany[i], 0, 1);
			}
		}
		for(uint8_t i = 0; i < wydarzenia.size(); i++){
			if(wydarzenia[i].czy_nalezy_wykonac(globalny_czas)){
				logi.write("Program " + to_string(i) + " sygnalizuje gotowosc do wykonania");
				if(zarzadca.token == 0xFF){
					zarzadca.token = i;
					logi.write("Program " + to_string(i) + "dostal token");
				} else {
					logi.write("Token jest zajety");
					errory.write("Jest kolizja miedzy dwoma programami" + to_string(i) + " : " + to_string(zarzadca.token) + ".");
					wydarzenia[i].znajdz_nastepne(globalny_czas);
				}
			}
		}
	}

	void reczne_przelacznik(bool stan){
		if(stan != reczne){
			reczne = stan;
			zarzadca.wylacz_wszystkie();
		}
	}

        std::basic_string<uint8_t> do_char() {
            uint32_t reczne_stany_int = 0;
            for (uint8_t i = 0; i < liczba_sekcji; i++) {
                reczne_stany_int |= ((uint32_t)reczne_stany[i] << i);
            }
            uint8_t temp[] = { (uint8_t)liczba_sekcji, (uint8_t)reczne,
            (uint8_t)(reczne_stany_int >> 0), (uint8_t)(reczne_stany_int >> 8),
            (uint8_t)(reczne_stany_int >> 16), (uint8_t)(reczne_stany_int >> 24),
            (uint8_t)wydarzenia.size()};
            std::basic_string<uint8_t> res(temp, 7);
            for (uint8_t i = 0; i < wydarzenia.size(); i++) {
                res += wydarzenia[i].do_char();
            }
            return res;
        }

        void z_char(std::basic_string<uint8_t>& in) {
            liczba_sekcji = (uint8_t)in[0];
            reczne_przelacznik((bool)(in[1] & 1));
            uint32_t reczne_stany_int = 0;
            reczne_stany_int |= ((uint32_t)in[2] << 0);
            reczne_stany_int |= ((uint32_t)in[3] << 8);
            reczne_stany_int |= ((uint32_t)in[4] << 16);
            reczne_stany_int |= ((uint32_t)in[5] << 24);
            wydarzenia.resize((uint8_t)in[6]);
            in = in.substr(7);
            for (uint8_t i = 0; i < wydarzenia.size(); i++) {
                wydarzenia[i].z_char(in);
            }
            for (uint8_t i = 0; i < liczba_sekcji; i++) {
                reczne_stany[i] = (bool)((reczne_stany_int >> i) & 1);
            }
        }

};









