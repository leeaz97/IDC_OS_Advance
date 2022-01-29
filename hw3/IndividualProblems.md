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

* (b) What happens when the host runs out of the memory, and swaps out pages
  that are used by the hypervisor? How does it affect the guest? Describe in
  (low-level) details what happens when a guest-userspace process tries to
  access a host-swapped page.

(3) One difference between plain virtualization and nested virtualization is
that the former can leverage EPT/NPT hardware extensions, while the latter
cannot do so directly.

* (a) What are the pros and cons of adding another EPT/NPT layer for nested
  virtualization?

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
