# Homework 3

## Individual Problems:

(1) Explain, in your own words, the mechanisms depicted in the diagrams in
slides 26 and 27 of Lecture #9.

slide 26:
KVM is hypervisor tpe 2, write to Linux on Linux on BTX.
Implement as a kernel module 
The KVM found in the kernel takes care of the virtualization of the CPU and memory.
The mange of  BIOS, devices and all other mange do delegation to userspace.
Use QEMU emulator code to implement the part that is not related to CPU and memory, (use for the part they need to implement in usermode ).
In the kernel, the KVM does the Virtualization.

3 parts:
1.	User mode 
2.	Kernel mode 
3.	Guest

User node and kernel-mode run as root. Guest is not root.

In user mode, we do the initialization.
Open the device /dev/KVM, allocate new VM and CPUs, map the memory in user space (for the guest).
Run VM – it is a system call that runs on the kernel – see it connected to KVM, go to the code of the KVM (we now in the kernel mode ), in the kernel-mode its do some preparing, and in the end do VM launch (actual instruction).
from the kernel mode its goes to Guest mode.
In the guest mode when the happening call of IO or signals (example to signal is ctrl+c), in some IO port the KVM doesn’t know how to handle this so, happen trap to kernel mode (return the system call with information why return ), the kernel-mode understand that he knows to handle this in usermode, handle this in usermode and back to run VM (again) call to system call that does resume and goes to guest mode.
If the trap is not IO (like page fault) he stays in kernel mode and goes back to guest.

slide 27:

How the KVM decide what to do:
Vmexit – some event that exists us from the guest mode and inserts us to root mode (can be some exit conditions), according to exit condition we call to the function handle_*.
The handle_* can be page fault, divided by zero, general protection area, etc.. – the guest doesn’t know to handle that and go to vmentry.
The handle_* can be also what exactly the guest wants to do.
First, we do 
1. fetch go memory we have guest memory address that map to physical address host, if it's unable we get PF\ general protection.
In fetch, we duplicated the MMU to software.
2. decode (what the gust wants to do), if it's unable we get undefined behavior 
In decode, we duplicated the function of CPU.
3. privcheck (the guest unable to do what he want to do ) if its unable we get undefined behavior 
4.	read mem
if we succeed to do all these steps we do emulation (can be some different emulation, (em_* in the slide)) and write to the memory (if necessary ), final update the registers, and update the extraction point after getting to vmentry.


(2) When the OS runs out of memory it resorts to swapping pages to storage.
Consider a system with QEMU/KVM hypervisor running several guests:

* (a) What happens if the guest runs out of memory and starts swapping? How
  does it affect the host? Describe in (low-level) detail what happens when
  a guest-userspace process tries to access a guest-swapped page).

  The guest related to his memory as a physical memory he doesn't aware he runs on virtual memory.
  So when he does swapping he will change the PTE to will be not present - swapped out and give some number that told where the PT in the swap.
  when the guest tries to access this page he will get PF (on the guest), and handle the PF by going to the page ->  in the PTE entry, this is a page that swaps out and he bring the page from the map of swap.
  
  The host not aware of any of these actions.  


* (b) What happens when the host runs out of the memory, and swaps out pages
  that are used by the hypervisor? How does it affect the guest? Describe in
  (low-level) details what happens when a guest-userspace process tries to
  access a host-swapped page.

  The host made a swapping to the page (change the PTE to will be not present - swapped out and give some number that told where the PT in the swap).
  When the guest-userspace tries to access this page (page that swap), the guest gets PF, it is not EPT violation just a page not found, in host-level, not guest level.
  So the host does a regular process to find the page (the handaling of swap done in the kernel) - in the PTE entry, this is a page that swaps out, and he brings the page from the map of swap and comes back with the page to the guest. 
  
  From the perspective of the guest was a little delay to get the page (he don’t aware of the swapping and PF...)

(3) One difference between plain virtualization and nested virtualization is
that the former can leverage EPT/NPT hardware extensions, while the latter
cannot do so directly.

* (a) What are the pros and cons of adding another EPT/NPT layer for nested
  virtualization?

  pros:
    - Guest has full control over its page tables
    - No VM exits due to (guest) page faults, INVLPG, or CR3 changes (Reduced volume of VM exits)
    - HW support to notify VMM

  cons:
    - (N x M) + N + M memory accesses upon TLB miss (many access):
      N - depth of host page tables
      M - depth of guest page tables
      References = (N x M) + N + M

* (b) In the _Turtles Project_, nested virtalization uses "EPT compression"
  to optimize memory virtualization. To some extent, this technique reminds
  of the optimization used in the _L3 µ-kernel_ to reduce TLB flushed. Explain
  the common theme to both. In a hindsight, could the _L3 µ-kernel_ benefit
  from today's EPT? If so, how?

(4) The _Antfarm_ project relied on shadow page-tables for introspection, to
learn about processes inside the guest. It was built before the introduction
of EPT/NPT. How would the use of EPT affect the techniques describes in the
paper? What changes (if any) would you propose to adapt their system to EPT,
and how would such changes affect its performance?

Process Creation - 
In Antfarm_ project we see CR3 that we don't see before, in shadow page table it happen immediately, get PF when he tries to change him.
In the nested page table (EPT/NPT), we don't get immediately PF we get him when the process tries to access something, not exists (we get PF when will have a miss-match between the EPT to the thing the process want to do - it get some time..)
In this case, the performance in EPT/NPT will be less good than Antfarm_ project

Process Termination - 
In Antfarm project, if we recognize the CR3 is clean (CR3 = 0 ) in the user space  + invalidate to Page table or we don't see the process a lot of time we can infer that the process terminated.
In EPT/NPT:
If we don't see specific address spaces for a long time we can infer that the process associated with the address space terminated.
In this case, the performance in EPT/NPT will be better than Antfarm_ project (in the table in slide 15 on lacture 10 we can seee the performance for process terminate is not good)