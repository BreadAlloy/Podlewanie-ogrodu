#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

struct loger{
	int fd;
	czas& tttt;

	loger(std::string path, czas& ttt) : tttt(ttt)
	{
		fd = open(path.c_str(), O_WRONLY|O_APPEND|O_CREAT, 00666);
		assert(fd != 0);
	}

	void write(std::string text){
		printf("%s\n", text.c_str());
		tttt.update_time();
		std::string output = tttt.str() + "	" + text + "\n";
		printf("%lu\n", output.length());
		assert(0 < ::write(fd, output.c_str(), output.length()));
	}

};




