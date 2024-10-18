#include "ch32v003fun.h"
int main(void)
{
    while(1) {
        goto SKIP1;
		SKIP1:
        goto SKIP2;
		SKIP2:
		;
    }
    int a = 0;
    switch(a) {
        case 0: 
            break;
        case 1: 
    }

	return 0;
}