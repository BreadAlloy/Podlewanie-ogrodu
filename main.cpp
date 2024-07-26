#include <cstdio>
#include <iostream>
#include <functional>
#include <thread>
#include <wiringPi.h>
#include <cassert>
using namespace std;

const uint8_t LICZBA_SEKCJI = 15;
const uint8_t PIN_DO_WODOMIERZA = 21;
const std::vector<uint8_t> PINY_DO_SEKCJI = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

const uint32_t OKOLO_LITRY_NA_MINUTE = 60;

bool end(bool);

#include "time_funcs.h"

czas globalny_czas = czas();

#include "misc.h"

loger logi("log.log", globalny_czas);
loger errory("errory.log", globalny_czas);

#include "simple_gpio_control.h"
#include "podlewanie.h"

bool wyslij_update = 0;
#include "kontroler.h"

kontroler zarzadca = kontroler(PINY_DO_SEKCJI, PIN_DO_WODOMIERZA); 
// zarzadca zajmuje sie hardwerem

#include "wydarzenia.h"
#include "super_kontroler.h"

super_kontroler szef;
// szef zajmuje sie softwarem

#include "end.h"

std::basic_string<uint8_t> spakuj_stan(){
        uint32_t a = 0;
        for(uint8_t i = 0; i < LICZBA_SEKCJI; i++){
	      if(zarzadca.elektrozawory[i].stan) printf("TAK\n");
              a |= (uint32_t)zarzadca.elektrozawory[i].stan << i;
        }
        uint8_t temp[] = {(uint8_t)(a >> 0), (uint8_t)(a >> 8), (uint8_t)(a >> 16), (uint8_t)(a >> 24),
		(uint8_t)((uint32_t)wodomierz_l() >> 0), (uint8_t)((uint32_t)wodomierz_l() >> 8),
		(uint8_t)((uint32_t)wodomierz_l() >> 16), (uint8_t)((uint32_t)wodomierz_l() >> 24)};
        std::basic_string<uint8_t> res(temp, 8);
        return res;
}


#include "rzeczy_socketowe.h"


void testing(std::basic_string<uint8_t> s){
	szef.z_char(s);
	printf("Przyszlo:%s\n", s.c_str());
}


dt tt;

uint32_t liczba = 0;
void plus_jeden(){
	liczba++;
	double temp = tt.pomiar();
	if(temp < 0.005){
		printf("Moze jest zle: %f\n", temp);
	}
	printf("%lu\n", liczba);
}

void testuje(){
	//testowy.update_time();
	printf("BEEP\n");
	printf("%.6lf\n", tt.pomiar());
}

int main(){
	assert(end(PINY_DO_SEKCJI.size() == LICZBA_SEKCJI));
	printf("TEAts\n");
	logi.write("Uruchomiono program.");
	zarzadca.wylacz_wszystkie();
#if 0
	wiringPiSetupGpio();
	wodomierz_konstruktor(21);
	//test.ustaw_funkcje();
	//wiringPiISR(test.number, INT_EDGE_FALLING, plus_jeden);
	//std::string zmienna;
	while(true){
		//cin >> zmienna;
		//testuje();
		sleep(1);
		printf("%lf\n", wodomierz_l());
	}
#endif
#if 0
	pin_do_przekaznika *piny = new pin_do_przekaznika[liczba_sekcji + 1]; //piny[0] jest nieuzywany
	gpio_init();
	for(uint8_t i = 1; i <= liczba_sekcji; i++){
		piny[i] = pin_do_przekaznika(i);
		piny[i].ustaw_jako_output();
		printf("----------------------------------------\n");
	}
	//pin_do_przekaznika pin21(21);
	//pin21.ustaw_jako_output();

	std::string pin_do_zmiany;
	//test_f();
	czas test;


	while(true){
		std::cin >> pin_do_zmiany;
		//stan = !stan;
		test.update_time();
		uint8_t n = (uint8_t)(std::stoi(pin_do_zmiany));
		printf("%s : %u\n", pin_do_zmiany.c_str(), n);
		assert(n <= 27);
		//if(n == 21){
			//pin21.ustaw_stan(!pin21.stan);
		//} else {
			piny[n].ustaw_stan(!piny[n].stan);
		//}
		//pin21.print();
	}
#endif
#if 0
	wydarzenie test;
	test.default3();
	std::string in;
	std::basic_string<uint8_t> szyfr;
	while(true){
		cin >> in;
        	//printf("%s\n", globalny_czas.str().c_str());
        	//szyfr = globalny_czas.do_char();
        	//globalny_czas.z_char(szyfr);
        	//printf("%s\n", globalny_czas.str().c_str());
		//globalny_czas.update_time();
		printf("%s\n", test.str().c_str());
		szyfr = test.do_char();
		test.z_char(szyfr);
		printf("%s\n", test.str().c_str());
		printf("%s\n", szyfr.c_str());

		//logi.write(in);
	}
#endif
#if 1
	char buf[1024];
	polaczenie eth;
	eth.connection_setup();
	//test.reciever(testing);
	printf("Pointer eth %p\n", &eth);
	std::thread odbieracz(&polaczenie::reciever, &eth, testing);
	//odbieracz.detach();
	while(true){
		globalny_czas.update_time();
		//if(eth.nowy_klient){
			//buf[0] = 't';
			//buf[1] = 'e';
			//buf[2] = 's';
			//buf[3] = 't';
			//eth.emiter(buf);
		//}
		szef.tick();
		if(wyslij_update){
			uint8_t temp[] = {(uint8_t)1};
			eth.emiter(std::basic_string<uint8_t>(temp, 1) + spakuj_stan());
			wyslij_update = false;
		}
		if(eth.reciever_running){
			usleep(100);
		} else {
			sleep(1);
		}
	}


#endif
#if 0
	char buf[1024];
	polaczenie eth;
	eth.connection_setup();
	//test.reciever(testing);
	eth.connection();
	printf("Polaczono\n");
	std::thread odbieracz(&polaczenie::reciever, eth, testing);
	odbieracz.detach();
	while(true){
		printf("Wiadomosc: ");
		cin >> buf;
		eth.emiter(buf);
	}
#endif
	return 0;
}
