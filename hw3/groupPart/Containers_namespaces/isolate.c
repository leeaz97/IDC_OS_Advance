#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define DEFAULT_LENGTH 100
#define STACK_SIZE (1024*1024)
#define ERROR -1
#define SUCCESS 0
char stack_child[STACK_SIZE];

struct child_process_params
{
    int pipe_fd[2];
    char **argv;
};

void setup_mntns() { 
    system("mount -t proc proc /proc");
}

void setup_utsns() {
    const char HOST_NAME[] = "isolated"; 
    int result = 0;

    result = sethostname(HOST_NAME, strlen(HOST_NAME));
    if (result != 0) {
        fprintf(stderr, "sethostname faild: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void write_file(char path[DEFAULT_LENGTH], char line[DEFAULT_LENGTH]) {
    FILE *file = fopen(path, "w");
    size_t total_written = 0;
    const size_t length = strlen(line);

    if (!file) {
        fprintf(stderr, "Faild open file: %s", path);
        exit(EXIT_FAILURE);
    }
    
    total_written = fwrite(line, sizeof(char), length, file);
    if (total_written != length) {
        fprintf(stderr, "Faild write file: %s", path);
        exit(EXIT_FAILURE);
    }

    if (fclose(file) != SUCCESS) {
        fprintf(stderr, "Faild close file: %s", path);
        exit(EXIT_FAILURE);
    }
}

void setup_userns(int pid) {
    char path[DEFAULT_LENGTH] = {0};
    char line[DEFAULT_LENGTH] = {0};

    int uid = 1000;

    sprintf(path, "/proc/%d/uid_map", pid);
    sprintf(line, "0 %d 1\n", uid);
    write_file(path, line);

    sprintf(path, "/proc/%d/setgroups", pid);
    sprintf(line, "deny");
    write_file(path, line);

    sprintf(path, "/proc/%d/gid_map", pid);
    sprintf(line, "0 %d 1\n", uid);
    write_file(path, line);
}

void setup_netns(int pid) { 
    char command[DEFAULT_LENGTH] = {0};

    system("ip link add veth0 type veth peer name peer0");
    system("ip link set veth0 up");
    system("ip addr add 10.11.12.13/24 dev veth0");
    
    sprintf(command, "ip link set peer0 netns /proc/%d/ns/net", pid);
    system(command);
}

void child_setup_netns() {
    system("ip link set lo up");
    system("ip link set peer0 up");
    system("ip addr add 10.11.12.14/24 dev peer0");
}

void child_setup_ids() {
    int result = ERROR;

    result = setuid(0);
    if (result == ERROR) {
        fprintf(stderr, "setuid faild: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    result = setgid(0);
    if (result == ERROR) {
        fprintf(stderr, "setgid faild: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

int child_func(void *args)
{
    struct child_process_params *params = (struct child_process_params*)args;
    int *fds = params->pipe_fd;
    pid_t pid = 0;
    int result = 0;

    read(fds[0], &pid, sizeof(int));
    close(fds[0]);

    child_setup_ids();
    child_setup_netns();

    setup_mntns();
    setup_utsns();

    result = execvp(params->argv[0], params->argv);
    if (result == ERROR) {
        fprintf(stderr, "execvp faild: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return SUCCESS;
}

int create_namespaces(char **child_argv)
{
    struct child_process_params params = {0};
    int flags = 0;
    pid_t pid = 0;

    params.argv = child_argv;
    pipe(params.pipe_fd);

    // recieve signal on child process termination
    flags = SIGCHLD | \
                CLONE_NEWUSER | CLONE_NEWNET | CLONE_NEWUTS | \
                CLONE_NEWIPC| CLONE_NEWNS | CLONE_NEWPID;

    // the child stack growns downwards 
    pid = clone(child_func, stack_child + STACK_SIZE, flags, &params);
    if (pid == -1) {
        fprintf(stderr,"clone: %s", strerror(errno));
        exit(1);
    }

    setup_userns(pid);
    setup_netns(pid);

    write(params.pipe_fd[1], &pid, sizeof(int));
    close(params.pipe_fd[1]);
    waitpid(pid, NULL, 0);

    return SUCCESS;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("usage: isolate PROGRAM [ARGS...]\n");
        
        return ERROR;
    }
    
    return create_namespaces(argv + 1);
}