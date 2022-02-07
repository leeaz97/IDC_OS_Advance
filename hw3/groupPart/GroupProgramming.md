## Group programming

### (1) KVM: hypervisor, guest(s), and hypercalls

We using this [git](https://github.com/purplewall1206/kvm-hello-world). and update the code to answers the homework.

To run the hypervisor in _"long"_ mode,
make kvm-hello-world
./kvm-hellow-world -l

**(a) Study an example: "Hello, World!"**

- (a.1) What is the size of the guest (physical) memory? How and where in the code does the hypervisor allocate it? At what host (virtual) address is this memory mapped?

    - 0x200000 == 2097152 == pow(2,21) == 2 MB bytes of memory is allocated as the RAM for the guest. print in kvm-hello-world.c line 107. (in function `vm_init`)
    - in kvm-hello-world.c line 109, the hypervisor allocates ocate the guest (physical) address:
    - `vm->mem = mmap(NULL, mem_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);`
    - The host (virtual) address is this memory mapped print in kvm-hello-world.c line 113. (in function `vm_init`)

- (a.2) Besides the guest memory, what additional memory is allocated? What is stored in that memory? Where in the code is this memory allocated? At what host/guest? (virtual/physical?) address is it located?
    
    - The additionally allocated memory is vCPU. vCPU is the abbreviation for the virtual centralized processing unit. vCPU represents a portion or share of the underlying, physical CPU that is assigned to a particular VM.
    - memory allocated in the host as a virtual address:
    - `vcpu->kvm_run = mmap(NULL, vcpu_mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, vcpu->fd, 0);` in kvm-hello-world.c line 154 (in function `vcpu_init`)
    - Print the virtual address in kvm-hello-world.c line 157. (in function `vcpu_init`)

From here on, assume _"long"_ mode.

- (a.3) The guest memory area is setup to contain the guest code, the guest page table, and a stack. For each of these, identify where in the code it is setup, and the address range it occupies (both guest-physical and host-virtual).

    - The guest memory area is setup `memreg.guest_phys_addr = 0`. in kvm-hello-world.c line 123
    - Kernel stack is setup `regs.rsp = 2 << 20`; in kvm-hello-world.c line 535 
    - Print the range (both guest-physical and host-virtual) in kvm-hello-world.c line 493-497 (in function `setup_long_mode`).

- (a.4) Examine the guest page table. How many levels does it have? How many pages does it occupy? Describe the guest virtual-to-physical mappings: what part(s) of the guest virtual address space is mapped, and to where?

    - 4 Levels:
    - 1. Page Map Level 4 (PML4) - 8 KB == 8192 byte position
    - 2. Page Directory Pointer Table (PDPT) - 12 KB == 12288 byte position
    - 3. Page Directory Table (PDT) - 16 KB == 16384 byte position
    - 4. Page Table (PT)
    - the physical page is found.


For both (a.3) and (a.4), illustrate/visualize the hypervisor memory layout
and the guest page table structure and address translation. (Preferably in
text form).

- (a.5) At what (guest virtual) address does the guest start execution? Where is this address configured?

    - The guest virtual address start execution in `memreg.guest_phys_addr = 0` (in kvm-hello-world.c line 123 (in function `vm_init`))
    - this config `regs.rip = 0` in in kvm-hello-world.c line 532 in function `run_long_mode`.

- (a.6) What port number does the guest use? How can the hypervisor know the port number, and read the value written? Which memory buffer is used for this value? How many exits occur during this print?

    - The port number is `0xE9 = 233`.
    - When the hypervisor runs recognize the direction is output by the port (that define also in the guest) and then print the value.
    - The memory buffer `p + vcpu->kvm_run->io.data_offset`.
    - 14 exits occur during "Hello, world!\n". 15 is printed because we call print() to print the occur 14+1. 

- (a.7) At what guest virtual (and physical?) address is the number 42 written? And how (and where) does the hypervisor read it?

    - Number 42 written in the guest physical address vm->mem + 0x400 and the physical is 0x400 (in kvm-hello-world.c line 301)

**(b) Extend with new hypercalls**

In the code that found in directory Part A+B+C.
To run in _"long"_ mode:

make kvm-hello-world
./kvm-hellow-world -l

**(c) Filesystem para-virtualization**

In the code that found in directory Part A+B+C.
To run in _"long"_ mode:

make kvm-hello-world
./kvm-hellow-world -l

**(d) Bonus: hypercall in KVM**

Not implemant.

**(e) Multiple vCPUs**

(e.1) + (e.2)
In the code that found in directory Part E.
To run in _"long"_ mode:

make kvm-hello-world
./kvm-hellow-world -l
### (2) Containers and namespaces

1. Create user namespace; remap the UIDs/GIDs in the new _userns_
2. Create uts namespaces; change hostname in the new _utsns_
3. Create ipc namespace
4. Create net namespace; create and configure veth interface pair
5. Create pid namespace
6. Create mnt namespace; mount /proc inside the new _mntns_

(Note that the process would run in an isolated environment, but would share
the same root filesystem as the parent, just in a separate _mntns_).

These steps can be done in userspace as follows:

            Parent shell                     Child shell
            -------------------------------  -----------------------------  
          1                                  # (1) create (privileged) userns
          2   
          3                                  $ unshare -U --kill-child /bin/bash
          4                                  $ echo "my-user-ns" > /proc/$$/comm
          5                                  $ id
          6                                  uid=65534(nobody) gid=65534(nogroup) groups=65534(nogroup)
          7   
          8   
          9   $ ps -e -o pid,comm | grep my-user-ns
         10   22310,my-user-ns?
         11   
         12   $ sudo bash -c 'echo "0 1000 1000" > /proc/22310/uid_map'
         13   $ sudo bash -c 'echo "0 1000 1000" > /proc/22310/gid_map'
         14   
         15                                  $ id
         16                                  uid=0(root) gid=0(root) groups=0(root),65534(nogroup)
         17                                  
         18                                  # (2,3) create utsns and ipcns
         19   
         20                                  $ unshare --ipc --uts --kill-child /bin/bash
         21                                  $ hostname isolated
         22                                  $ hostname
         23                                  isolated
         24   
         25                                  # (4) create netns
         26                                  $ unshare --net --kill-child /bin/bash
         27                                  $ echo "my-net-ns" > /proc/$$/comm
         28   
         29   $ ps -e -o pid,comm | grep my-user-ns
         30   22331,my-net-ns?
         31   
         32   $ sudo ip link add veth0 type veth peer name peer0
         33   $ sudo ip link set veth0 up
         34   $ sudo ip addr add 10.11.12.13/24 dev veth0
         35   
         36   $ sudo ip link set peer0 netns /proc/22331/ns/net
         37   
         38                                  $ ip link
         39                                  1: lo: <LOOPBACK> mtu 65536 qdisc noop state DOWN mode DEFAULT group default qlen 1000
         40                                      link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
         41                                  9: peer0@if10: <BROADCAST,MULTICAST> mtu 1500 qdisc noop state DOWN mode DEFAULT group default qlen 1000
         42                                      link/ether 76:8d:bb:61:1b:f5 brd ff:ff:ff:ff:ff:ff link-netnsid 0
         43                                  $ ip link set lo up
         44                                  $ ip link set peer0 up
         45                                  $ ip addr add 10.11.12.14/24 dev peer0
         46   
         47                                  $ ping -c 1 10.11.12.13
         48                                  PING 10.11.12.13 (10.11.12.13) 56(84) bytes of data.
         49                                  64 bytes from 10.11.12.13: icmp_seq=1 ttl=64 time=0.066 ms
         50   
         52                                  # (5,6) create pidns, mntns
         53                                  $ unshare --pid --mount --fork --kill-child /bin/sh
         54                                  $ mount -t proc proc /proc
         55                                  $ ps

- (a) Describe the process hierarchy produced by the sequence of commands in the
"child shell" column. How can it be minimized, and what would the hierarchy
look like?

   - unshare -U --kill-child /bin/bash 
   unshare moves the current process inside a new set of namespaces,
   In this case, create new user namespace (-U) and running bash the paramter --kill-child mean when the parent process dying will kill the child process (bash). 
   - $ echo "my-user-ns" > /proc/$$/comm
   /proc/[pid]/comm - This file exposes the process's comm value—that is, the command name associated with the process. 
   rename the process bash to `my-user-ns`.
   - $ id
   Print user and group information for the specified USER, or (when USER omitted) for the current user.
   print the information of the corrent process bash
   - $ id - the paret change to the child process (bash) the uid, gid, groups to root. (the parent change the child to root user ) 
   - $ unshare --ipc --uts --kill-child /bin/bash
   --ipc - Unshare the IPC namespace. If file is specified, then a persistent namespace is created by a bind mount.
   --uts - Unshare the UTS namespace. If file is specified, then a persistent namespace is created by a bind mount.
   In this case, create new IPC and UTS namespace, from the process my-user-ns, running bash and when the parent process (my-user-ns) dying will kill the child process (bash). 
   - $ hostname isolated - change the name of namespace to isolated
   - $ hostname - print the hoatname
   - $ unshare --net --kill-child /bin/bash
   --net - Unshare the network namespace. If file is specified, then a persistent namespace is created by a bind mount.
   In this case, create new network namespace from the namespace isolated , running bash and when the parent process (bash) dying will kill the child process (bash). 
   - $ echo "my-net-ns" > /proc/$$/comm -  rename the process bash to `my-net-ns`.
   - $ ip link
   ip link - Show information for all interfaces
   the loopback interface which is also down.
   - $ ip link set lo up
   Change the state of the lo interface to UP.
   - $ ip link set peer0 up
   Change the state of the peer0 interface to UP.
   - $ ip addr add 10.11.12.14/24 dev peer0
   Add address 10.11.12.14 with netmask 24 to device peer0
   - $ ping -c 1 10.11.12.13
   ping to the parent shell for test the communication from child to parent
   - $ unshare --pid --mount --fork --kill-child /bin/sh
   --pid - Unshare the PID namespace. If file is specified, then a persistent namespace is created by a bind mount.
    --fork - Fork the specified program as a child process of unshare rather than running it directly. This is useful when creating a new PID namespace. 
    The following command creates a PID namespace, using --fork to ensure that the executed command is performed in a child process that (being the first process in the namespace) has PID 1. The --mount-proc option ensures that a new mount namespace is also simultaneously created and that a new proc(5) filesystem is mounted that contains information corresponding to the new PID namespace. And running bash and when the parent process (bash) dying will kill the child process (bash).   
   - $ mount -t proc proc /proc 
    Mounting a new procfs inside.
   - $ ps
   report a snapshot of the current processes.
   will report the processes on the corrent namespace.

?????????????
It can be minimized in child shell:
   1. unshare -U --ipc --uts --net --pid --kill-child /bin/bash
   2. ip link set lo up
   3. ip link set peer0 up
   4. ip addr add 10.11.12.14/24 dev peer0
   5. mount -t proc proc /proc
   6. unshare --mount --fork --kill-child /bin/bash
   7. ps
   create 2 namespace and dont 4 namespace but the 2 new namespace will be with same configuration of all the 4.
?????????????

Hierarchy look like :
user namespace, child process:my-user-ns(bash) -> 
isolated (IPC & UTS) namespace, child process:bash -> 
network namespace, child process:my-net-ns(bash) -> 
PID namespace, child process:bash.

(b) What would happen if you change the order of namespace creation, e.g. run
`unshare --ipc` first? And what would happen if you defer lines 12-13 until
a later time?

According to LWN.net - Namespaces in operation, part 6: more on user namespaces: "Creating namespaces other than user namespaces requires the CAP_SYS_ADMIN capability. On the other hand, creating a user namespace requires (since Linux 3.8) no capabilities, and the first process in the namespace gains a full set of capabilities (in the new user namespace). This means that that process can now create any other type of namespace using a second call to clone()." 
(This information can be gathered from man7 documentation, but it is much more simplified by LWN.net, I have read both sources, but choosing to explain by using the source LWN.net).

First of all, if we change the order of namespace creation we need to make sure to create the other namespaces with the required capabilities (e.g. by calling sudo), otherwise, we will get "unshare failed: Operation not permitted".

Second, if we run the creation of a non-user-namespace (e.g. unshare --ipc) with `sudo`, and after creating the new user namespace, then the mapping of uid\gid in lines 12-13 will not be valid mapping, the user root is left unknown for the child user namespace, since we running under root and mapping ID-outside-ns for 1000 (The user).  

If we defer lines 12-13 until a later time (for instance after line 20), any creation of a new namespace will fail ("Operation not permitted") because the shell has no capabilities inside the new user namespace (as can be seen by running the commands: `id` and `cat /proc/$$/status | egrep 'Cap(Inh|Prm|Eff)'`). 
The route cause of this problem is at the execve() call that executed the bash shell: when a process with non-zero user IDs performs an execve(), the process's capability sets are cleared. 
To avoid this problem, it is necessary to create a user ID mapping inside the user namespace before performing the execve(), or in our scenario, before creating new namespaces.

(c) What is the purpose of line 4 and lines 9-10 (and similarly, line 27 and
lines 29-30)? Why are they needed?

   - We rename the process names inside the namespaces, because we want to recognize the specific process in the parent process to change his uid,gid,groups/ network.
   in the process parent  run many instances of some pid and bash (as a process name ) ,we want to connect each instance of bash that we create in the namespace to how he presents in the parent, if we rename the process it will rename in the parent, and we can identify the process in the parent.

(d) Describe how to undo and cleanup the commands above. (Note: there is more
than one way; try to find the minimal way). Make sure there are no resources
left dangling around.

   - need to exits in each namespace , because the paramter --kill-child when we exits the namespace (kill the child)  its also kill the parent. in some namespace we need to free the resources.
   In the child shell: 
   1. `unmount /proc`
   2. `exit`
   3. `ip link set lo down`
   4. `ip link set peer0 down`
   5. `ip addr del 10.11.12.14/24 dev peer0`
   6. `exit`
   7. `exit`
   In the child shell:
   1. `sudo ip link set veth0 down`
   2. `sudo ip addr del 10.11.12.13/24 dev veth0`
   3. `sudo ip link del veth0 type veth peer name peer0`


(d) Write a program that would implement the sequence above, whose usage is:

        usage: isolate PROGRAM [ARGS...]

For example, the command:

        isolate ps aux

would execute the command "ps aux" inside an isolated environment.

For this, you may use the skeleton below that uses _clone(2)_ to create new
namespaces:

        #define STACK_SIZE (1024*1024)
        char stack_child[STACK_SIZE];

        int create_namespaces()
        {
            int fds[2];
            int flags;
            pid_t pid;

            pipe(fds);
            
            // recieve signal on child process termination
            flags = SIGCHLD | \
                    CLONE_NEWUSER | CLONE_NEWNET | CLONE_NEWUTS | \
                    CLONE_NEWIPC| CLONE_NEWNS | CLONE_NEWPID;

            // the child stack growns downwards 
            pid = clone(child_func, stack_child + STACK_SIZE, flags, fds);
            if (pid == -1) {
                fprintf(stderr,"clone: %s", strerror(errno));
                exit(1);
            }
            
            setup_userns(pid);
            setup_netns(pid);

            write(c->fd[1], &pid, sizeof(int));
            close(c->fd[1]);
            waitpid(pid, NULL, 0);
        }

        void int child_func(void *args)
        {
            int fds[2] = args;
            pid_t pid;

            read(fds[0], &pid, sizeof(int));
            close(fds[0]);

            setup_mntns();
            setup_utsns();

            write(c->fd[1], &pid, sizeof(int));
            close(c->fd[1]);
            waitpid(pid, NULL, 0);
        }

        void int child_func(void *args)
        {
            int fds[2] = args;
            pid_t pid;

            read(fds[0], &pid, sizeof(int));
            close(fds[0]);

            execvp(...);
        }

Note: you may (and should) use the _system()_ helper to implement the
_setup\_mntns()_ and _setup\_netns()_ functions; but not for the
_setup\_utsns()_ and _setup\_userns()_.

(e) Test your program. Does it require root privileges? If so, then why?
How can it be changed to not require these privileges?

Yes, it does require root privileges, because some of the functionality requires:
* superuser- sethostname, 
* CAP_SETUID capability in its user namespace- setuid.
* CAP_SETGID capability in its user namespace- setgid
* CAP_SYS_ADMIN, CAP_SETGID for writing the file setgroups
* The flags (CLONE_NEWIPC, CLONE_NEWNET, CLONE_NEWUTS, CLONE_NEWPID, CLONE_NEWNS) passed to clone needs CAP_SYS_ADMIN capability.

Theoretically, you can create another isolated environment to run the program, and change the uid/gid mapping in isolate according to the new user namespace uid/gid mapping.

