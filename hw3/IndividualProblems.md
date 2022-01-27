# Homework 3

## Individual Problems:

(1) Explain, in your own words, the mechanisms depicted in the diagrams in
slides 26 and 27 of Lecture #9.

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
