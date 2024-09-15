struct zapisywanie{
	int fd;

	zapisywanie(std::string path){
		fd = open(path.c_str(), O_RDWR|O_CREAT, 00666);
		assert(fd > 0);
	}

	void zapisz(std::basic_string<uint8_t> str){
		licznik_klatek = 0;
		lseek(fd, 0, SEEK_SET);
		if(0 > write(fd, str.c_str(), str.length())){
			printf("Zapisywanie nie dziala.\n");
		} else {
			printf("Zapisano do pliku.\n");
		}
	}

	std::basic_string<uint8_t> przeczytaj(){
		off_t rozmiar_pliku = lseek(fd, 0, SEEK_END);
		lseek(fd, 0, SEEK_SET);
		char* buf = new char[rozmiar_pliku];
		if(0 > read(fd, buf, rozmiar_pliku)){
			printf("Czytanie nie dziala.\n");
		}
		std::basic_string<uint8_t> res((uint8_t*)buf, rozmiar_pliku);
		delete[] buf;
		return res;
	}


};









