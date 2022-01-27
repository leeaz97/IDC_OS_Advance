#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAPPAGE_SYSCALL_NUM 449 //change to the relevant num
#define DEFAULT_SIZE 3
#define SUCCESS 0
#define FAIL -1
#define ARGS_TOTAL 2

int main(int argc, char **argv)
{
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	int buf_data_length = 0;
	int original_length = 0;
	long result = 0;
	int should_continue = 0;

	char * buf_data= (char *)malloc(DEFAULT_SIZE * sizeof(char));
	if (!buf_data)
	{
		return FAIL;	
	}

	buf_data_length = original_length = DEFAULT_SIZE;
	
	FILE* file=fopen("/proc/self/maps","r");
	if(!file){
		fprintf(stderr,"pmparser : cannot open the memory maps, %s\n",strerror(errno));
		return NULL;
	}
	
	while ((read = getline(&line, &len, file)) != -1) {
        //printf("Retrieved line of length %zu:\n", read);
        //printf("%s", line);
		// not sure if need to read only the stack segment
		if(strstr(line, "stack") != NULL) {
			int n = sscanf(line, "%lX-%lX", &address_start, &address_end);
			if(n != 2){
				printf("Involid line read from proc/self/maps\n");
				continue;
			}
			printf("%lu",address_start);
			printf("%lu",address_end);
			
			// test if the buff that return not full if full need to run again with bigger buffer
			do {
				original_length = buf_data_length;
				// call the syscall 
				result = syscall(MAPPAGE_SYSCALL_NUM ,address_start, address_end , buf_data, original_length);
				if (result != SUCCESS)
				{
					printf("syscall failed %ld\n", result);

					return FAIL;
				}

				if (buf_data_length == original_length)
				{
					free(buf_data);
					buf_data_length = original_length * 2;
					buf_data = (char *)malloc(buf_data_length * sizeof(char));
					if (!buf_data)
					{
						return FAIL;
					}
					
					should_continue = 1;
				} 
				else
				{
					should_continue = 0;
				}
			} while(should_continue);
		
		}

    free(buf_data);
    fclose(file);
    if (line) {free(line);}
	return SUCCESS;
}
