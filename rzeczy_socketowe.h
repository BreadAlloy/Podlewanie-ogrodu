#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

#define PORT 8080

int server_fd;
struct sockaddr_in address;
int opt = 1;
int addrlen = sizeof(address);
//char *hello = "Hello from server";

struct polaczenie{
	int sock;
	bool reciever_running = false;
	//bool nowy_klient = false;

	void connection_setup() {
    		// Creating socket file descriptor
    		if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    		{
        		perror("socket failed");
       			exit(EXIT_FAILURE);
    		}

    		// Forcefully attaching socket to the port 8080
    		if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    		{
        		perror("setsockopt");
        		exit(EXIT_FAILURE);
    		}
    		address.sin_family = AF_INET;
    		address.sin_addr.s_addr = INADDR_ANY;
    		address.sin_port = htons( PORT );

    		// Forcefully attaching socket to the port 8080
    		if (bind(server_fd, (struct sockaddr *)&address,
                	sizeof(address))<0)
    		{
        	perror("bind failed");
        	exit(EXIT_FAILURE);
    		}
	}

	void connection() {
    		if (listen(server_fd, 3) < 0)
    		{
        		perror("listen");
        		exit(EXIT_FAILURE);
    		}
    		if ((sock = accept(server_fd, (struct sockaddr *)&address,
        		(socklen_t*)&addrlen))<0)
    		{
        	perror("accept");
        	exit(EXIT_FAILURE);
    		}
		logi.write("Ktos sie polaczyl");
		reciever_running = true;
		uint8_t temp[] = {0};
		emiter(std::basic_string<uint8_t>(temp, 1) + szef.do_char());
		temp[0] = 1;
		emiter(std::basic_string<uint8_t>(temp, 1) + spakuj_stan());
	}

	void reciever(std::function<void(std::basic_string<uint8_t>)> f) {
        	char buffer[1024];
		        printf("Pointer eth %p\n", this);
        	while (true) {
			if(reciever_running){
      		        	if (0 == recv(sock, buffer, 2, 0)) {
        		      		reciever_running = false;
          		      		//close(sock);
					logi.write("ktos sie rozlaczyl\n");
          		      		continue;
				}

            			uint16_t ile_przeczytac = 0 | (uint16_t)((uint8_t*)buffer)[0] | ((uint16_t)((uint8_t*)buffer)[1] << 8);
        			if (0 == recv(sock, buffer, ile_przeczytac, 0)) {
       			            	reciever_running = false;
				        //close(sock);
					logi.write("ktos sie rozlaczyl\n");
                			continue;
            			}
           			std::basic_string<uint8_t> do_f((uint8_t*)buffer, ile_przeczytac);
            			f(do_f);
			} else {
				//connection_setup();
				connection();
			}
		}
	}

    void emiter(std::basic_string<uint8_t> buffer) {
	printf("socket adress: %d\n", &sock);
	printf("Wyslano cos\n");
        uint8_t temp[] = {(uint8_t)buffer.size(), (uint8_t)(buffer.size() >> 8)};
	printf("Rozmiar: %d\n", buffer.size());
        buffer = std::basic_string<uint8_t>(temp, 2) + buffer;
	int er = send(sock, (char*)(buffer.c_str()), buffer.size(), 0);
        if(0 > er){
		std::perror("ERRONO: ");
		printf("Nie ma polaczenia | error:%d\n", er);
	}
    }

};
