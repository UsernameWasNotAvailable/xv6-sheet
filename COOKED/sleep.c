#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
	int sleep_sec;
	if (argc < 2){
		fprintf(2, "Usage: sleep seconds");
		exit(1);
	}

	sleep_sec = atoi(argv[1]);
	if (sleep_sec < 0){
		fprintf(2, "sleep: invalid time %d\n", sleep_sec);
		exit(1);
	}
	else {
		pause(sleep_sec);
		exit(0);
	}

}



// ruien's lame code but hey it works!!!!
// int main (int argc, char*argv[]){
// 	if(argc < 2){
// 		printf("boo");
// 		exit(1);
// 	}

// 	int i;
// 	int timeCount;
// 	for (i=1;i<argc;i++){
// 		timeCount = *argv[i];

// 	}
// 	pause(timeCount);

	

// 	exit(0);
// }
