#include <linux/delay.h>
#include <linux/export.h>
#include <linux/fdtable.h>
#include <linux/fs_struct.h>
#include <linux/init_task.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <uapi/linux/sched.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/module.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");

static unsigned long cr0,cr3;


static void get_pgtable_macro(void)
{
    cr0 = read_cr0();
    cr3 = read_cr3_pa();
     
    printk("cr0 = 0x%lx, cr3 = 0x%lx\n",cr0,cr3);

}


static void vaddr2paddr(struct task_struct *p, unsigned long vaddr)
{
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;
    unsigned long paddr = 0;
    unsigned long page_addr = 0;
    unsigned long page_offset = 0;
    pgd = pgd_offset(p->mm, vaddr);
    printk("pgd_val = 0x%lx, pgd_index = %lu\n", pgd_val(*pgd),pgd_index(vaddr));
    if (pgd_none(*pgd)){
        printk("not mapped in pgd\n");
        return ;
    }

    p4d = p4d_offset(pgd, vaddr);
    printk("p4d_val = 0x%lx, p4d_index = %lu\n", p4d_val(*p4d),p4d_index(vaddr));
    if(p4d_none(*p4d))
    { 
        printk("not mapped in p4d\n");
        return ;
    }

    pud = pud_offset(p4d, vaddr);
    printk("pud_val = 0x%lx, pud_index = %lu\n", pud_val(*pud),pud_index(vaddr));
    if (pud_none(*pud)) {
        printk("not mapped in pud\n");
        return ;
    }
 
    pmd = pmd_offset(pud, vaddr);
    printk("pmd_val = 0x%lx, pmd_index = %lu\n", pmd_val(*pmd),pmd_index(vaddr));
    if (pmd_none(*pmd)) {
        printk("not mapped in pmd\n");
        return ;
    }
 
    pte = pte_offset_kernel(pmd, vaddr);
    printk("pte_val = 0x%lx, ptd_index = %lu\n", pte_val(*pte),pte_index(vaddr));
    if (pte_none(*pte)) {
        printk("not mapped in pte\n");
        return ;
    }
    page_addr = pte_val(*pte) & PAGE_MASK;
    page_offset = vaddr & ~PAGE_MASK;
    paddr = page_addr | page_offset;
    printk("page_addr = %lx, page_offset = %lx\n", page_addr, page_offset);
    printk("vaddr = %lx, paddr = %lx\n", vaddr, paddr);
    // char* poi = (char*)paddr;
    // printk("poi: %x\n", poi);
    // sprintf(poi, "poi\n");
}

//内核模块初始化函数
static int __init print_pid(void)
{
	struct task_struct *task, *p;
	struct list_head *pos;
	unsigned long vaddr = 0x4006a4;
	
	printk("Printf process'message begin:\n");
	task = &init_task;
	
	//遍历进程链表
	list_for_each(pos,&task->tasks)
	{
		p = list_entry(pos,struct task_struct,tasks);
		if (p->pid == 2000) {
			printk("\n\n");
			if((p->mm)!=NULL) {
				printk("total_vm:%ld;",(p->mm)->total_vm);
				printk("pid: %d\n", p->pid);
			}
			get_pgtable_macro();	// cr0
			vaddr2paddr(p, vaddr);
		}
	}	

	return 0;
}



//内核模块退出函数
static void __exit pid_exit(void)
{
	printk("exiting...\n");
}


module_init(print_pid);
module_exit(pid_exit);
