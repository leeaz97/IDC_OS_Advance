#include "mpage.h"
#include <linux/string.h>
#include <linux/syscalls.h>
#include <linux/errno.h>
#include <linux/spinlock.h>
#include <linux/printk.h>
#include <linux/uaccess.h>

mapspages_func global_mapspages_func = NULL;

SYSCALL_DEFINE3(mpage, unsigned long start, unsigned long end, char __user *buf, size_t size) // Not sure about the first parameter 
{
	const long SUCCESS = 0;
	long return_value = SUCCESS; 
	size_t buffer_length = size;
	unsigned long address_start = start;
	unsigned long address_end = end;
	//struct prinfo * process_tree_data = NULL;
	
	if (!global_mapspages_func) 
	{
		return_value = request_module("mpage_loadable_module");
		if (return_value != SUCCESS)
		{
			pr_err("kernel/mpage.c request_module faild, returned %ld", return_value);
			
			return -ENOSYS;
		}
	}
	
	if (start > end)
	{
		pr_err("the start address bigger than the end");
			
			return -EINVAL;
	}
	
	//-EFAULT: if buf is outside the accessible address space.
	if (!access_ok(buf, sizeof(char) * buffer_length))
	{
		return -EFAULT;
	}

	buf_data = (char *)kmalloc(buffer_length * sizeof(char), GFP_KERNEL);
	if (!buf_data)
	{
		pr_err("kernel/mpage.c buf_data malloc failed");

		return -EFAULT;
	}

	return_value = global_mapspages_func(address_start ,address_end , buf_data, buffer_length); // sent the paramters of mapspages

	if (return_value != SUCCESS)
	{
		 pr_err("kernel/mpage.c function failed");

		 return return_value;
	}

	return return_value;
}