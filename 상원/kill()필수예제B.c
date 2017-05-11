#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//kill()을 통해서 종료될 배경 프로세스
int main(void) {
	while(1) {
		printf("\n[OSLAB]\n");
		sleep(5);
	}
	exit(0);
}
