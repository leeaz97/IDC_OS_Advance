#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/prinfo.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/export.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/cred.h>

#define SUCCESS 0
#define ERROR -1

MODULE_DESCRIPTION ("mapspages loadable module");
MODULE_LICENSE ("GPL");
MODULE_INFO(intree, "Y");

int mapspages(unsigned long start, unsigned long end, char __user *buf, size_t size)
{
	
}


static int mapspages_module_init(void)
{
	//int result = register_ptree(&getptree);
	if (!result) 
	{
		pr_info("mapspages module loaded\n");
	}

	return result;
}

static void mapspages_module_exit (void)
{
	//unregister_ptree(&getptree);
	pr_info("module unloaded\n");
}

module_init (mapspages_module_init);
module_exit (mapspages_module_exit);