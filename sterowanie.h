#include <vector>

struct sterowanie{
	bool reczne = true;
	pin_do_przekaznika* zwory;
	liczba_sekcji = 0;

	sterowanie(std::vector<uint8_t> piny_gpio){
		liczba_sekcji = piny_gpio.size();
		zawory = new pin_do_przekaznika[liczba_sekcji];
		for(uint8_t i = 0; i < liczba_sekcji; i++){
			zawory[i] = pin_do_przekaznika(piny_gpio[i]);
		}
	}


};
