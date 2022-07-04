#include "device.h"
#include <stdio.h>
#include <stdlib.h>

char buffer[128]={};

const char *deviceInfo() {
	FILE* ptr;
	system("curl host.docker.internal:1080/device-info>/tmp/device-info");
    	ptr = fopen("/tmp/device-info", "r");
	fscanf(ptr, "%s", buffer);
	fclose(ptr);
	return buffer;
}

