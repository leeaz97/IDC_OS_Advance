#ifndef _MPAGE_H_
#define _MPAGE_H_

typedef int (*mapspages_func)(unsigned long start, unsigned long end, char __user *buf, size_t size);


#endif