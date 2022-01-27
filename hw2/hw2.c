#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h> 


bool prefix(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}

static inline unsigned long long getcycles(void)
    {
        unsigned long low, high;
        asm volatile ("rdtsc" : "=a" (low), "=d" (high));
        return ((low) | (high) << 32);
}

// answers to A
static inline unsigned long long gethosttime(unsigned long long cycles )
{
	unsigned long ns;
	// get the cpu mhz
	unsigned long cpu_mhz;  
	//long cpu_mhz = system("cat /proc/cpuinfo | grep MHz | grep -Eo -m 1 [0-9].*.[0-9].*");
	size_t len = 0;
	char * line = NULL;
	ssize_t read;
	FILE* file=fopen("/proc/cpuinfo","r");
	if(!file){
		fprintf(stderr,"pmparser : cannot open the memory maps, %s\n",strerror(errno));
		return NULL;
	}
	
	while ((read = getline(&line, &len, file)) != -1) {
        //printf("Retrieved line of length %zu:\n", read);
	if(strstr(line, "MHz") != NULL) {
        //if (prefix(line,"cpu MHZ")){
		//printf("%s", line);
		char * ch = strtok(line, ":");
		ch = strtok(NULL, " ");
		//printf("%s",ch);
		cpu_mhz = strtoul(ch,NULL,10);	
		//printf("%lu",cpu_mhz);
		break;
	}
	}
	
	//printf("%lu",cpu_mhz);	
	//calculate the in nanoseconds , ns = cycles * (10^9 / (cpu_mhz * 10^6))
	ns = cycles * (10^3 / cpu_mhz);
	
	return ns;
	//return 0;
}


int main (int argc, char ** argv) {
	// answers to B 
	 unsigned long cycles;
	 unsigned long start_time_cycles;
	 unsigned long end_time_cycles;
	 cycles = getcycles();
	 //printf("%lu\n",cycles);
	 start_time_cycles = gethosttime(cycles);
	 //printf("%lu\n",start_time_cycles);
	 cycles = getcycles();
	 end_time_cycles = gethosttime(cycles);
	 
	 printf("time take to the func getcycles() %.4lu in nanosecund , tested with gethosttime\n", end_time_cycles-start_time_cycles);
	 
	 struct timespec start_time;
	 struct timespec end_time;

	 gettimeofday(&start_time, NULL);
	 cycles = getcycles();
	 gettimeofday(&end_time, NULL);
	 
	 printf("time take to the func getcycles() %.4lu in nanosecund , tested with gettimeofday\n", end_time.tv_nsec-start_time.tv_nsec);
	 
	 // answers to C
	 int i,j,k =0;

	 cycles = getcycles();
	 start_time_cycles = gethosttime(cycles); 
	 for (i=0; i < 1000; i++) {
		 for (j=0; j < 100; j++) {  /* inner loop starts here */
			 k = i + j;  
		 }                          /* inner loop ends here */
	 }
	 cycles = getcycles();
	 end_time_cycles = gethosttime(cycles);
	 
	 printf("time take to the loops %.4lu in nanosecund , tested with gethosttime\n", end_time_cycles-start_time_cycles);
	 
	 i,j,k =0;
	 gettimeofday(&start_time, NULL);
         for (i=0; i < 1000; i++) {
                 for (j=0; j < 100; j++) {  /* inner loop starts here */
                         k = i + j;
                 }                          /* inner loop ends here */
         }
         gettimeofday(&end_time, NULL);

         printf("time take to the loops %.4lu in nanosecund , tested with gettimeofday\n", end_time.tv_nsec-start_time.tv_nsec);

	 return 0;
}
