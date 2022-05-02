#include <linux/gfp.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/types.h>

#define DEFAULT_ORDER 5

LIST_HEAD(fragment_list);

int create_fragments(void) {
  struct page* page;
  int next;
  while ((page = alloc_pages(GFP_KERNEL, DEFAULT_ORDER))) {
    split_page(page, DEFAULT_ORDER);
    list_add(&page->lru, &fragment_list);
    for (next = 1; next < (1 << DEFAULT_ORDER); next++)
      __free_page(page + next);
  }
  return 0;
}

int fragmenter_init(void) {
  printk(KERN_INFO "Starting fragmenter\n");
  create_fragments();
  return 0;
}

int release_fragments(void) {
  struct page *page, *next;
  list_for_each_entry_safe(page, next, &fragment_list, lru) {
    __free_page(page);
  }
  return 0;
}

void fragmenter_exit(void) {
  printk(KERN_INFO "Releasing all fragments\n");
  release_fragments();
  printk(KERN_INFO "Released all fragments\n");
}

module_init(fragmenter_init);
module_exit(fragmenter_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION(
    "Physical Memory Fragmenter, exhausts contiguous physical memory (from a particular order)");
MODULE_AUTHOR("Saravanan D");
