#include <sys/mman.h>
#include <inttypes.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>

off_t BASE_REG = 0xfe200000;
const char* PATCH = "/dev/mem";

int plik_do_pamieci = 0;
uint8_t* mmap_res;

//Najpierw trzeba wywolac gpio_init() potem mozna aktywowac poszczegolne piny

void gpio_init(){
        plik_do_pamieci = open(PATCH, O_RDWR);
        assert(plik_do_pamieci > 0);
        mmap_res = (uint8_t*)mmap(nullptr, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, plik_do_pamieci, BASE_REG);
        assert(mmap_res != (uint8_t*)-1);
}

void bin_print(uint32_t a){
	for(int8_t i = 31; i >= 0; i--){
		printf("%u", (a >> i) & 1);
	}
	printf("\n");
}

struct pin_do_przekaznika{
	off_t mmap_offset;
	uint8_t number;
	bool stan = 0;
	uint8_t bits_pos = 32;

	pin_do_przekaznika(uint8_t number)
	: number(number) {
		assert(number <= 27);
		mmap_offset = znajdz_offset();
		//printf("Aktywowano pin %u.\n", number);
		//printf("test1:%u\n", bits_pos);
        	bits_pos = 3 * (number%10);
		//printf("test2:%u\n", bits_pos);
		print();
	}

	pin_do_przekaznika() : number(0) {;}

	pin_do_przekaznika& operator=(const pin_do_przekaznika& other){
		mmap_offset = other.mmap_offset;
		number = other.number;
		stan = other.stan;
		bits_pos = other.bits_pos;
		return *this;
	}

	void ustaw_jako_output(){
        	uint32_t gpfselx = *(volatile uint32_t*)(mmap_res + mmap_offset);
		//printf("number:%u, bits_pos:%u\n", number, bits_pos);
		gpfselx &= ~(0x7 << bits_pos);
        	*(volatile uint32_t*)(mmap_res + mmap_offset) = gpfselx;
        	gpfselx |= (0x1 << bits_pos);
        	*(volatile uint32_t*)(mmap_res + mmap_offset) = gpfselx;
		//bin_print((uint32_t)gpfselx);
		ustaw_stan(stan);
	}

	void ustaw_stan(bool nowy_stan){
		//if(nowy_stan == stan){
		//	break;
		//}
		//logi.write("Ustawiam stan:" );
		stan = nowy_stan;
		//printf("Ustawiono stan %u na %u.\n", stan, number);
		if(!nowy_stan){
			*(volatile uint32_t*)(mmap_res + 0x1c) = 1 << number; //Ustaw stan wysoki
		} else {
			*(volatile uint32_t*)(mmap_res + 0x28) = 1 << number; //Ustaw stan niski
		}
	}

	void print(){
		printf("Ustawienia %u to:\n mmap_offset = %8.8x\n stan = %u\n", number, mmap_offset, stan);
	}

	off_t znajdz_offset(){
		assert(number <= 27);
		return 0x00 + 0x04 * ((number)/10);
	}
};

void testuje();

struct detektor_zmiany_stanu{
	uint8_t number = 0;

	detektor_zmiany_stanu(uint8_t number)
	: number(number){
		pinMode(number, INPUT);
		pullUpDnControl(number, PUD_DOWN);
	}

	detektor_zmiany_stanu(){}
};

int test_f(){
        plik_do_pamieci = open(PATCH, O_RDWR);
        assert(plik_do_pamieci > 0);
        uint8_t* mmap_res = (uint8_t*)mmap(nullptr, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, plik_do_pamieci, BASE_REG);
        assert(mmap_res != (uint8_t*)-1);
        uint32_t gpfsel2 = *(volatile uint32_t*)(mmap_res + 0x08);
        printf("%8.8x\n", gpfsel2);
        gpfsel2 &= ~(0x7 << 3);
        *(volatile uint32_t*)(mmap_res + 0x08) = gpfsel2;
        gpfsel2 |= (0x1 << 3);
        *(volatile uint32_t*)(mmap_res + 0x08) = gpfsel2;
        while(true){
                printf("Wysoki\n");
                *(volatile uint32_t*)(mmap_res + 0x1c) = 1 << 21; //Ustaw stan wysoki
                sleep(3);
                printf("Niski\n");
                *(volatile uint32_t*)(mmap_res + 0x28) = 1 << 21; //Ustaw stan niski
                sleep(3);
        }
        //sleep(2000);
        return 0;
}

