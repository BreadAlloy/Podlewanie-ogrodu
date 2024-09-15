#include <time.h>
#include <chrono>

time_t t = time(NULL);

struct czas{
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t mday;
	uint8_t mon;
	uint32_t year; // + 1900 do bizacej daty
	uint8_t wday; // +1 w normalnym formacie  // 8 oznacza ze to nic nie znaczy


	czas(int8_t asec = -1, int8_t amin = -1, int8_t ahour = -1, int8_t amday = -1, int8_t amon = -1, int32_t ayear = -1){
		update_time();
		set_time(asec, amin, ahour, amday, amon, ayear);
	}

	void update_time(){
		t = time(NULL);
    		struct tm res;
		localtime_r(&t, &res);
		//printf("%d,%d,%d,%d,%d,%d,%d\n", res.tm_sec, res.tm_min, res.tm_hour, res.tm_mday, res.tm_mon + 1, res.tm_year + 1900, res.tm_wday);
		sec = (uint8_t)res.tm_sec;
		min = (uint8_t)res.tm_min;
		hour = (uint8_t)res.tm_hour;
		mday = (uint8_t)res.tm_mday;
		mon = (uint8_t)res.tm_mon;
		year = (uint32_t)res.tm_year + 1900;
		wday = (uint8_t)res.tm_wday;
	}

	void set_time(int8_t asec = -1, int8_t amin = -1, int8_t ahour = -1, int8_t amday = -1, int8_t amon = -1, int32_t ayear = -1){
		if(asec >= 0){
			sec = (uint8_t)asec;
			wday = 8;
		}
		if(amin >= 0){
			min = (uint8_t)amin;
			wday = 8;
		}
		if(ahour >= 0){
			hour = (uint8_t)ahour;
			wday = 8;
		}
		if(amday >= 0){
			mday = (uint8_t)amday;
			wday = 8;
		}
		if(amon >= 0){
			mon = (uint8_t)amon;
			wday = 8;
		}
		if(ayear >= 0){
			year = (uint32_t)ayear;
			wday = 8;
		}

	}

	czas& operator=(const czas& other){
		sec = other.sec;
		min = other.min;
		hour = other.hour;
		mday = other.mday;
		mon = other.mon;
		year = other.year;
		wday = other.wday;
		return *this;
	}

	std::string str(){
		return std::to_string(hour) + ":" +
		       std::to_string(min) + ":" +
		       std::to_string(sec) + "   " +
		       std::to_string(mday) + "." +
		       std::to_string(mon + 1) + "." +
		       std::to_string(year);
	}

	void normalizacja(){
		min = min + sec / 60;
		sec = sec % 60;

		hour = hour + min / 60;
		min = min % 60;

		mday = mday + hour / 24;
		hour = hour % 24;

		uint8_t temp = ile_dni_w_mon();
		mon = mon + mday / temp;
		mday = mday % temp;

		year = year + (uint32_t)((mon - 1) / 12);
		mon = ((mon - 1) % 12) + 1;

		wday = wday % 7;
	}


	uint8_t ile_dni_w_mon(){
		if(mon == 2){
			return 28 + (uint8_t)is_leap_year();
		}

		uint8_t offset = 0;
		if(mon > 7) offset = 1;

		return (mon + offset) % 2 + 30;
	}

	bool is_leap_year(){
		bool res = false;
		if(year % 4 == 0) res = true;
		if(year % 100 == 0) res = false;
		if(year % 400 == 0) res = true;

		return res;
	}

	static const uint32_t CHAR_ROZMIAR_CZAS = 10;

	std::basic_string<uint8_t> do_char(){
		uint8_t temp[] = {(char)sec, (char)min, (char)hour, (char)mday,
			       (char)mon, (char)wday, (char)year, (char)(year >> 8),
			       (char)(year >> 16), (char)(year >> 24)};
		std::basic_string<uint8_t> res(temp, CHAR_ROZMIAR_CZAS);
		//printf("%s\n", res.c_str());
		return res;
	}

	void z_char(std::basic_string<uint8_t> &in){
		sec = (uint8_t)in[0];
		min = (uint8_t)in[1];
		hour = (uint8_t)in[2];
		mday = (uint8_t)in[3];
		mon = (uint8_t)in[4];
		wday = (uint8_t)in[5];
		year = 0;
		year|=((uint32_t)in[6]);
		year|=((uint32_t)in[7]) << 8;
		year|=((uint32_t)in[8]) << 16;
		year|=((uint32_t)in[9]) << 24;
		in = in.substr(CHAR_ROZMIAR_CZAS);
	}
};


 // Udaje ze dwa czasy nie moga byc takie same

bool operator>(const czas& a, const czas& b){
	if (a.year > b.year) return true;
	if (a.year < b.year) return false;

	if (a.mon > b.mon) return true;
	if (a.mon < b.mon) return false;

	if (a.mday > b.mday) return true;
	if (a.mday < b.mday) return false;

	if (a.hour > b.hour) return true;
	if (a.hour < b.hour) return false;

	if (a.min > b.min) return true;
	if (a.min < b.min) return false;

	if (a.sec > b.sec) return true;
	if (a.sec < b.sec) return false;
	return true;
}

bool operator<(const czas& a, const czas& b){
	return !(a>b);
}


struct dt{
	//using namespace std::chrono;

	std::chrono::high_resolution_clock::time_point t1;

	dt(){
		t1 = std::chrono::high_resolution_clock::now();
	}

	double pomiar(){
		std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
		t1 = t2;
		return time_span.count();
	}

};

