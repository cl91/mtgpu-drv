/*
 * @Copyright   Copyright (c) Moorethreads Technologies Ltd. All Rights Reserved
 * @License     Dual MIT/GPLv2
 */

#ifndef __OS_INTERFACE_H__
#define __OS_INTERFACE_H__

#include "linux-types.h"

#ifndef BIT
#define BIT(nr)		(1ul << (nr))
#endif

#ifndef ALIGN
#define __ALIGN_MASK(x, mask)	(((x) + (mask)) & ~(mask))
#define __ALIGN(x, a)		__ALIGN_MASK(x, (typeof(x))(a) - 1)
#define ALIGN(x, a)		__ALIGN((x), (a))
#define ALIGN_DOWN(x, a)	__ALIGN((x) - ((a) - 1), (a))
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr)	(sizeof(arr) / sizeof((arr)[0]))
#endif

#ifndef likely
#define likely(x)	(x)
#endif

#ifndef unlikely
#define unlikely(x)	(x)
#endif

#ifndef DIV_ROUND_UP
#define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))
#endif

#ifndef DMA_BIT_MASK
#define DMA_BIT_MASK(n)	(((n) == 64) ? ~0ull : ((1ull<<(n))-1))
#endif

#ifndef PATH_MAX
#define PATH_MAX	4096	/* # chars in a path name including nul */
#endif

#ifndef min
#define min(a, b) \
	({__typeof(a) _a = (a); __typeof(b) _b = (b); _a > _b ? _b : _a;})
#endif

#define OS_VAL(index)	(os_value[OS_##index])
#define DECLEAR_OS_VALUE \
	X(PCI_IRQ_LEGACY)\
	X(PCI_IRQ_MSI)\
	X(PCI_EXT_CAP_ID_REBAR)\
	X(PCI_COMMAND)\
	X(PCI_COMMAND_MEMORY)\
	X(PCI_EXT_CAP_ID_SRIOV)\
	X(PCI_D3hot)\
	X(PCI_D0)\
	X(IRQF_SHARED)\
	X(MODE_OK)\
	X(DRM_UT_CORE)\
	X(DISPLAY_FLAGS_HSYNC_LOW)\
	X(DISPLAY_FLAGS_HSYNC_HIGH)\
	X(DISPLAY_FLAGS_VSYNC_LOW)\
	X(DISPLAY_FLAGS_VSYNC_HIGH)\
	X(DISPLAY_FLAGS_PIXDATA_NEGEDGE)\
	X(PAGE_SIZE)\
	X(PAGE_SHIFT)\
	X(FOLL_WRITE)\
	X(O_NONBLOCK)\
	X(EPOLLIN)\
	X(GFP_DMA)\
	X(GFP_KERNEL)\
	X(PIDTYPE_PID)\
	X(VM_EXEC)\
	X(VM_LOCKED)\
	X(IORESOURCE_MEM)\
	X(IORESOURCE_IRQ)\
	X(IORESOURCE_IO)\
	X(IORESOURCE_UNSET)\
	X(PLATFORM_DEVID_AUTO)\
	X(PM_HIBERNATION_PREPARE)\
	X(PM_SUSPEND_PREPARE)\
	X(PM_POST_HIBERNATION)\
	X(PM_POST_RESTORE)\
	X(PM_POST_SUSPEND)\
	X(PM_RESTORE_PREPARE)\
	X(EINVAL)\
	X(ENOMEM)\
	X(EIO)\
	X(ETIME)\
	X(EPERM)\
	X(EEXIST)\
	X(ENODEV)\
	X(ENXIO)\
	X(ETIMEDOUT)\
	X(EBUSY)\
	X(EFAULT)\
	X(ENOTSUPP)\
	X(ENOSPC)\
	X(EACCES)\
	X(EAGAIN)\
	X(IRQ_HANDLED)\
	X(IRQ_NONE)\
	X(DMA_BIDIRECTIONAL)\
	X(DMA_TO_DEVICE)\
	X(DMA_FROM_DEVICE)\
	X(ION_HEAP_TYPE_CUSTOM)

enum {
#define X(VALUE) OS_##VALUE,
	DECLEAR_OS_VALUE
#undef X
	OS_VALUE_MAX,
};

#define __os_round_mask(x, y) ((__typeof__(x))((y) - 1))
#define os_round_up(x, y) ((((x) - 1) | __os_round_mask((x), (y))) + 1)

#define os_dma_mmap_coherent(d, v, c, h, s) os_dma_mmap_attrs(d, v, c, h, s, 0)

/*
 * Used to create numbers.
 */

#ifndef _IOC_NONE

#define _IOC_NONE  0U
#define _IOC_WRITE 1U
#define _IOC_READ  2U

#define _IOC_DIRBITS  2
#define _IOC_NRBITS   8
#define _IOC_TYPEBITS 8
#define _IOC_SIZEBITS 14

#define _IOC_NRMASK  ((1 << _IOC_NRBITS) - 1)
#define _IOC_NRSHIFT 0
#define _IOC_NR(nr)  (((nr) >> _IOC_NRSHIFT) & _IOC_NRMASK)

#define _IOC_TYPESHIFT    (_IOC_NRSHIFT + _IOC_NRBITS)
#define _IOC_SIZESHIFT    (_IOC_TYPESHIFT + _IOC_TYPEBITS)
#define _IOC_DIRSHIFT     (_IOC_SIZESHIFT + _IOC_SIZEBITS)
#define _IOC_TYPECHECK(t) (sizeof(t))

#define _IOC(dir, type, nr, size) \
    (((dir) << _IOC_DIRSHIFT) | ((type) << _IOC_TYPESHIFT) | ((nr) << _IOC_NRSHIFT) | ((size) << _IOC_SIZESHIFT))

#define __IO(type, nr)        _IOC(_IOC_NONE, (type), (nr), 0)
#define _IOR(type, nr, size)  _IOC(_IOC_READ, (type), (nr), (_IOC_TYPECHECK(size)))
#define _IOW(type, nr, size)  _IOC(_IOC_WRITE, (type), (nr), (_IOC_TYPECHECK(size)))
#define _IOWR(type, nr, size) _IOC(_IOC_READ | _IOC_WRITE, (type), (nr), (_IOC_TYPECHECK(size)))
#endif /* _IOC_NONE */

struct mutex;
struct semaphore;
typedef struct spinlock spinlock_t;
struct wait_queue_head;
typedef struct wait_queue_head wait_queue_head_t;
struct wait_queue_entry;
struct work_struct;
typedef void (*work_func_t)(struct work_struct *work);
struct workqueue_struct;
struct platform_device;
struct platform_device_info;
struct device;
struct msi_msg;
struct msi_desc;
struct pci_bus;
struct pci_dev;
struct pci_saved_state;
struct pci_device_id;
struct resource;
struct file;
struct firmware;
struct timer_list;
struct mt_work;
struct sg_table;
struct dma_buf;
struct blocking_notifier_head;
struct notifier_block;
struct kref;
struct miscdevice;
struct inode;
struct poll_table_struct;
struct mm_struct;
struct vm_area_struct;
struct poll_table_struct;
struct ida;
struct task_struct;
struct pid;
struct path;
struct mtgpu_resource;
struct attribute;
struct device_attribute;
struct kobject;
struct seq_file;

#if defined(SUPPORT_ION)
struct ion_heap;
struct ion_platform_heap;
struct ion_device;
struct ion_heap_data;
#endif

typedef void (*dr_release_t)(struct device *dev, void *res);
typedef int (*notifier_fn_t)(struct notifier_block *nb,
			     unsigned long action, void *data);
typedef int mt_kref;
typedef int pci_power_t;
typedef unsigned long kernel_ulong_t;

struct mt_file_operations {
	int (*open)(struct inode *, struct file *);
	ssize_t (*read)(struct file *, char __user *, size_t, loff_t *);
	ssize_t (*write)(struct file *, const char __user *, size_t, loff_t *);
	long (*unlocked_ioctl)(struct file *, unsigned int, unsigned long);
	int (*mmap)(struct file *, struct vm_area_struct *);
	__poll_t (*poll)(struct file *, struct poll_table_struct *);
	loff_t (*llseek)(struct file *, loff_t, int);
	int (*release)(struct inode *, struct file *);
};

#define mt_typeof_member(T, m)	typeof(((T*)0)->m)

#define os_container_of(ptr, type, member)		\
	(type *)((uintptr_t)(ptr) - offsetof(type, member))

#define os_for_each_set_bit(bit, addr, size)		\
	for ((bit) = os_find_first_bit((addr), (size));	\
	     (bit) < (size);				\
	     (bit) = os_find_next_bit((addr), (size), (bit) + 1))

#define os_list_entry(ptr, type, member)		\
	os_container_of(ptr, type, member)

#define os_list_first_entry(ptr, type, member)		\
	os_list_entry((ptr)->next, type, member)

#define os_list_next_entry(pos, member)			\
	os_list_entry((pos)->member.next, typeof(*(pos)), member)

#define os_list_for_each_entry_safe(pos, n, head, member)			\
	for (pos = os_list_first_entry(head, typeof(*pos), member),		\
	     n = os_list_next_entry(pos, member);				\
	     &pos->member != (head); 						\
	     pos = n, n = os_list_next_entry(n, member))

#define os_plist_for_each_entry_safe(pos, n, head, m)				\
	os_list_for_each_entry_safe(pos, n, &(head)->node_list, m.node_list)

/**
 * The macros are defined for os struct.
 * Take encoder for example (type: drm_encoder):
 *      struct mt_drm_encoder {
 *              struct drm_encoder obj;
 *              void *data;
 *      };
 *      struct drm_encoder *os_create_drm_encoder(void);
 *      void os_destroy_drm_encoder(struct drm_encoder *obj);
 *      void *os_get_drm_encoder_drvdata(struct drm_encoder *obj);
 *      void os_set_drm_encoder_drvdata(struct drm_encoder *obj, void *data);
 */
/* implementation the os struct functions */
#define IMPLEMENT_OS_STRUCT_COMMON_FUNCS(type)					\
	struct mt_##type {							\
		struct type obj;						\
		void *data;							\
	};									\
	struct type *os_create_##type(void)					\
	{									\
		return kzalloc(sizeof(struct mt_##type), GFP_KERNEL);		\
	}									\
	void os_destroy_##type(struct type *obj)				\
	{									\
		kfree(obj);							\
	}									\
	void *os_get_##type##_drvdata(struct type *obj)				\
	{									\
		struct mt_##type *mt_obj = (struct mt_##type *)obj;		\
		return mt_obj->data;						\
	}									\
	void os_set_##type##_drvdata(struct type *obj, void *data)		\
	{									\
		struct mt_##type *mt_obj = (struct mt_##type *)obj;		\
		mt_obj->data = data;						\
	}

/* declarations the os struct functions */
#define DECLARE_OS_STRUCT_COMMON_FUNCS(type)					\
	struct type *os_create_##type(void);					\
	void os_destroy_##type(struct type *obj);				\
	void *os_get_##type##_drvdata(struct type *obj);			\
	void os_set_##type##_drvdata(struct type *obj, void *data)

#define os_for_each_sg(sglist, sg, nr, __i)	\
	for (__i = 0, sg = (sglist); __i < (nr); __i++, sg = os_sg_next(sg))

/* get member of the structure */
#define IMPLEMENT_GET_OS_MEMBER_FUNC(type, member)					\
mt_typeof_member(struct type, member) os_get_##type##_##member(struct type *ptr)	\
{											\
	return ptr->member;								\
}
extern const u64 os_value[];

#if defined(SUPPORT_ION)
size_t os_ion_query_heaps_kernel(struct ion_device *idev, struct ion_heap_data *hdata,
				 size_t size);
void os_ion_dev_lma_heap_destroy(struct ion_device *dev, const char *name);
void os_ion_platform_heap_init(struct ion_platform_heap *heap, u32 type, unsigned int id,
			       const char *name, phys_addr_t vram_base);
struct ion_platform_heap *os_ion_platform_heap_create(void);
int os_ion_device_add_heap(struct ion_device *idev, struct ion_heap *heap);
struct ion_device *os_ion_device_create(void);
int os_ion_device_destroy(struct ion_device *idev);
const char *os_ion_get_dev_name(struct ion_device *idev);
struct ion_heap_data *os_alloc_ion_heap_data_array(u32 count);
char *os_get_ion_heap_name(struct ion_heap_data *data, int i);
u32 os_get_ion_heap_id(struct ion_heap_data *data, int i);
#endif /*SUPPORT_ION*/

/**
 * Interface for get members of the structure.
 */
struct scatterlist *os_get_sg_table_sgl(struct sg_table *sgt);
unsigned int os_get_sg_table_nents(struct sg_table *sgt);
unsigned int os_get_sg_table_orig_nents(struct sg_table *sgt);

#define OS_SG_TABLE_MEMBER(ptr, member)	(os_get_sg_table_##member(ptr))

struct pci_dev *os_to_pci_dev(struct device *dev);
struct kobject *os_get_device_kobj(struct device *dev);
bool os_is_power_of_2(unsigned long n);
void *os_dev_get_drvdata(const struct device *dev);
void os_dev_set_drvdata(struct device *dev, void *data);
struct device *os_get_device(struct device *dev);
void os_put_device(struct device *dev);
int os_device_attach(struct device *dev);

int os_find_first_bit(const unsigned long *p, unsigned int size);
int os_find_next_bit(const unsigned long *p, int size, int offset);
void *os_kmalloc(size_t size);
void *os_kmalloc_atomic(size_t size);
void *os_kzalloc(size_t size);
void os_kfree(const void *ptr);
void *os_devm_kzalloc(struct device *dev, size_t size);
void *os_vmalloc(unsigned long size);
void *os_vzalloc(unsigned long size);
void *os_kvzalloc(size_t size);
void *os_kcalloc(size_t n, size_t size);
void os_kvfree(const void *addr);
void os_vfree(const void *addr);
struct page *os_vmalloc_to_page(const void *vmalloc_addr);
struct apertures_struct *os_alloc_apertures(unsigned int max_num);
int os_sg_table_create(struct sg_table **sgt);
void os_sg_table_destroy(struct sg_table *sgt);
struct scatterlist *os_sg_next(struct scatterlist *sg);
dma_addr_t os_sg_dma_address(struct scatterlist *sg);
unsigned int os_sg_dma_len(struct scatterlist *sg);
int os_sg_alloc_table_from_pages(struct sg_table *sgt, struct page **pages,
				 unsigned int n_pages, unsigned int offset,
				 unsigned long size);
void os_sg_free_table(struct sg_table *sgt);
int os_dma_map_sg(struct device *dev, struct scatterlist *sg,
		  int nents, u64 dir);
void os_dma_unmap_sg(struct device *dev, struct scatterlist *sg,
		     int nents, u64 dir);
void os_dma_sync_sg_for_device(struct device *dev, struct scatterlist *sg,
			       int nelems, u64 dir);
void os_dma_sync_sg_for_cpu(struct device *dev, struct scatterlist *sg,
			    int nelems, u64 dir);

struct pid *os_find_vpid(int nr);
struct task_struct *os_pid_task(struct pid *pid, int type);
struct mm_struct *os_get_task_mm(struct task_struct *p);
void os_task_lock(struct task_struct *p);
void os_task_unlock(struct task_struct *p);
char *os_d_path(const struct path *p, char *param, int size);

struct vm_area_struct *os_get_mm_mmap(const struct mm_struct *mm);

unsigned long os_get_vm_area_struct_vm_start(struct vm_area_struct *vma);
unsigned long os_get_vm_area_struct_vm_end(struct vm_area_struct *vma);
unsigned long os_get_vm_area_struct_vm_flags(struct vm_area_struct *vma);
struct vm_area_struct *os_get_vm_area_struct_vm_next(struct vm_area_struct *vma);
struct file *os_get_vm_area_struct_vm_file(struct vm_area_struct *vma);
void os_set_vm_area_struct_vm_flags(struct vm_area_struct *vma, unsigned long flag);

void *os_memset(void *s, int c, size_t count);
void *os_memcpy(void *dst, const void *src, size_t size);
void os_memcpy_fromio(void *dst, const void __iomem *src, size_t size);
void os_memcpy_toio(void __iomem *dst, const void *src, size_t size);
void os_memset_io(void __iomem *addr, int value, size_t size);
unsigned long os_copy_from_user(void *to, const void *from, unsigned long n);
unsigned long os_copy_to_user(void __user *to, const void *from, unsigned long n);

void os_poll_wait(struct file *filp, struct wait_queue_head *wait_address,
		  struct poll_table_struct *p);

void os_seq_puts(struct seq_file *m, const char *s);
void os_seq_putc(struct seq_file *m, char c);
void os_seq_put_decimal_ll(struct seq_file *m, const char *delimiter, long long num);
ssize_t os_seq_read(struct file *file, char __user *buf, size_t size, loff_t *ppos);
loff_t os_seq_lseek(struct file *file, loff_t offset, int whence);
int os_single_open(struct file *file, int (*show)(struct seq_file *, void *), void *data);
int os_single_release(struct inode *inode, struct file *file);
void *os_get_seq_file_private(struct seq_file *seq);
struct proc_dir_entry *os_proc_create_data(const char *name, umode_t mode,
					   struct proc_dir_entry *parent,
					   const void *proc_fops, void *data);
struct proc_dir_entry *os_proc_create_single_data(const char *name, umode_t mode,
						  struct proc_dir_entry *parent,
						  int (*show)(struct seq_file *, void *),
						  void *data);
struct proc_dir_entry *os_proc_mkdir(const char *name, struct proc_dir_entry *parent);
void os_proc_remove(struct proc_dir_entry *de);
void os_remove_proc_entry(const char *name, struct proc_dir_entry *parent);
int os_atomic_read_this_module_refcnt(void);
void *os_pde_data(const struct inode *inode);
void os_seq_printf(struct seq_file *m, const char *f, ...);

void *os_miscdevice_create(void);
void os_miscdevice_destroy(struct miscdevice *misc);
int os_miscdevice_init(struct miscdevice *misc, struct device *parent,
		       const char *name, int mode, const struct mt_file_operations *ops);
void os_miscdevice_deinit(struct miscdevice *misc);
int os_misc_register(struct miscdevice *misc);
void os_misc_deregister(struct miscdevice *misc);
void *os_get_miscdevice_drvdata(struct miscdevice *misc);
void os_set_miscdevice_drvdata(struct miscdevice *misc, void *data);
int os_set_miscdevice_name(struct miscdevice *misc, const char *data);
void os_clear_miscdevice_name(struct miscdevice *misc);
const char *os_get_miscdevice_name(struct miscdevice *misc);
struct device *os_get_miscdevice_parent(struct miscdevice *misc);

void os_set_file_private_data(struct file *file, void *private_data);
void *os_get_file_private_data(struct file *file);
void *os_get_file_node_private_data(struct file *file);
unsigned int os_get_file_flags(struct file *file);
struct path *os_get_file_path(struct file *file);

void os_kref_init(mt_kref *kref);
int os_kref_put(mt_kref *kref, void (*release)(mt_kref *kref));
void os_kref_get(mt_kref *kref);

int os_ida_create(struct ida **ida);
void os_ida_destroy(struct ida *ida);
int os_ida_alloc(struct ida *ida);
void os_ida_free(struct ida *ida, unsigned long id);
bool os_ida_is_empty(const struct ida *ida);

int os_mutex_create(struct mutex **lock);
void os_mutex_lock(struct mutex *lock);
void os_mutex_unlock(struct mutex *lock);
void os_mutex_destroy(struct mutex *lock);

int os_spin_lock_create(spinlock_t **lock);
void os_spin_lock(spinlock_t *lock);
void os_spin_unlock(spinlock_t *lock);
void os_spin_lock_irqsave(spinlock_t *lock, unsigned long *flags);
void os_spin_unlock_irqrestore(spinlock_t *lock, unsigned long flags);
void os_spin_lock_destroy(spinlock_t *lock);

int os_sema_create(struct semaphore **sem, int val);
void os_up(struct semaphore *sem);
int os_down_timeout(struct semaphore *sem, long timeout);
void os_sema_destroy(struct semaphore *sem);

unsigned long os_get_jiffies(void);
unsigned long os_msecs_to_jiffies(const unsigned int m);

void *os_get_work_drvdata(struct work_struct *work);
void os_set_work_drvdata(struct work_struct *work, void *data);
void *os_create_work(void);
void os_destroy_work(struct work_struct *work);
bool os_queue_work(struct workqueue_struct *wq, struct work_struct *work);
void os_destroy_workqueue(struct workqueue_struct *wq);
struct workqueue_struct *os_alloc_workqueue(const char *fmt, int max_active);
void os_wake_up(struct wait_queue_head *wq_head);
void os_wake_up_interruptible(struct wait_queue_head *wq_head);
void os_wake_up_all(struct wait_queue_head *wq_head);
int os_create_waitqueue_head(struct wait_queue_head **wq_head);
void os_destroy_waitqueue_head(struct wait_queue_head *wq_head);
void os_might_sleep(void);
long os_schedule_timeout(long timeout);
void os_init_wait_entry(struct wait_queue_entry *wq_entry, int flags);
void os_finish_wait(struct wait_queue_head *wq_head, struct wait_queue_entry *wq_entry);
long os_prepare_to_wait_event_uninterruptible(struct wait_queue_head *wq_head,
					      struct wait_queue_entry *wq_entry);
struct wait_queue_entry *os_create_wait_queue_entry(void);

void os_blocking_init_notifier_head(struct blocking_notifier_head *nh);
void os_notifier_block_set_notifier_call(struct notifier_block *nb, notifier_fn_t cb);
int os_blocking_notifier_call_chain(struct blocking_notifier_head *nh,
				    unsigned long val, void *v);
int os_blocking_notifier_chain_register(struct blocking_notifier_head *nh,
					struct notifier_block *n);
int os_blocking_notifier_chain_unregister(struct blocking_notifier_head *nh,
					  struct notifier_block *n);
int os_blocking_notifier_head_create(struct blocking_notifier_head **nh);
void os_blocking_notifier_head_destroy(struct blocking_notifier_head *nh);
int os_notifier_block_create(struct notifier_block **nb);
void os_notifier_block_destroy(struct notifier_block *nb);

struct attribute *os_get_device_attr_attr(struct device_attribute *dev_attr);
int os_sysfs_create_file(struct kobject *kobj, const struct attribute *attr);
void os_sysfs_remove_file(struct kobject *kobj, const struct attribute *attr);

#define os_wait_cond_timeout(condition, ret)					\
({										\
	bool cond = (condition);						\
	if (cond && !ret)							\
		ret = 1;							\
	cond || !ret;								\
})

#define os_wait_event_timeout(wq_head, condition, timeout)					\
({												\
	long ret1 = timeout;									\
	os_might_sleep();									\
	if (!os_wait_cond_timeout(condition, ret1)) {						\
		ret1 =										\
		({										\
			struct wait_queue_entry *wq_entry = os_create_wait_queue_entry();	\
			long ret2 = timeout;	/* explicit shadow */				\
												\
			os_init_wait_entry(wq_entry, 0);					\
			for (;;) {								\
				os_prepare_to_wait_event_uninterruptible(wq_head, wq_entry);	\
												\
				if (os_wait_cond_timeout(condition, ret2))			\
					break;							\
												\
				ret2 = os_schedule_timeout(ret2);				\
			}									\
			os_finish_wait(wq_head, wq_entry);					\
			os_kfree(wq_entry);							\
			ret2;									\
		});										\
	}											\
	ret1;											\
})

void os_init_work(struct work_struct *work, work_func_t func);
bool os_schedule_work(struct work_struct *work);
bool os_cancel_work_sync(struct work_struct *work);
struct task_struct *os_kthread_create(int (*threadfn)(void *data),
				      void *data, const char *namefmt, ...);
int os_kthread_stop(struct task_struct *k);
bool os_kthread_should_stop(void);

void os_wmb(void);
void os_mb(void);

int os_arch_io_reserve_memtype_wc(resource_size_t base, resource_size_t size);
void os_arch_io_free_memtype_wc(resource_size_t base, resource_size_t size);
int os_arch_phys_wc_add(unsigned long base, unsigned long size);
void os_arch_phys_wc_del(int handle);

void __iomem *os_ioremap(phys_addr_t phys_addr, size_t size);
void __iomem *os_ioremap_cache(resource_size_t offset, unsigned long size);
void os_iounmap(void __iomem *io_addr);

int os_get_user_pages_fast(unsigned long start, int nr_pages,
			   unsigned int gup_flags, struct page **pages);
void os_put_page(struct page *page);

unsigned int os_ioread32(void __iomem *addr);
u32 os_readl(const void __iomem *addr);
void os_iowrite32(u32 b, void __iomem *addr);
void os_writel(u32 value, void __iomem *addr);

int os_pci_domain_nr(struct pci_dev *pdev);
int os_request_pci_io_addr(struct pci_dev *pdev, u32 index,
			   resource_size_t offset, resource_size_t length);
unsigned int os_pci_slot(unsigned int devfn);
unsigned int os_pci_func(unsigned int devfn);
unsigned int os_get_pci_dev_virfn(struct pci_dev *pdev);
struct device *os_get_pci_device_base(struct pci_dev *pdev);
unsigned short os_get_pci_device_vendor(struct pci_dev *pdev);
unsigned short os_get_pci_device_id(struct pci_dev *dev);
struct pci_bus *os_get_pci_bus(struct pci_dev *pdev);
struct resource *os_get_pci_resource(struct pci_dev *pdev);
pci_power_t os_get_pci_current_state(struct pci_dev *pdev);
kernel_ulong_t os_get_pci_device_data(const struct pci_device_id *id);
unsigned int os_get_pci_devfn(struct pci_dev *pdev);
unsigned char os_get_pci_bus_number(struct pci_dev *pdev);
unsigned int os_get_pci_irq(struct pci_dev *pdev);
resource_size_t os_pci_resource_start(struct pci_dev *pdev, int bar);
resource_size_t os_pci_resource_end(struct pci_dev *pdev, int bar);
unsigned long os_pci_resource_flags(struct pci_dev *pdev, int bar);
resource_size_t os_pci_resource_len(struct pci_dev *pdev, int bar);
int os_pci_irq_vector(struct pci_dev *dev, unsigned int nr);
int os_pci_alloc_irq_vectors(struct pci_dev *dev, unsigned int min_vecs,
			     unsigned int max_vecs, unsigned int flags);
int os_pci_enable_device(struct pci_dev *dev);
void os_pci_disable_device(struct pci_dev *dev);
void os_pci_set_master(struct pci_dev *dev);
void os_pci_clear_master(struct pci_dev *dev);
int os_pci_load_and_free_saved_state(struct pci_dev *dev, struct pci_saved_state **state);
int os_pci_load_saved_state(struct pci_dev *dev, struct pci_saved_state *state);
void os_pci_restore_state(struct pci_dev *dev);
int os_pci_save_state(struct pci_dev *dev);
int os_pci_set_power_state(struct pci_dev *dev, pci_power_t state);
struct pci_saved_state *os_pci_store_saved_state(struct pci_dev *dev);

int os_pci_find_ext_capability(struct pci_dev *dev, int cap);
int os_pci_read_config_byte(const struct pci_dev *dev, int where, u8 *val);
int os_pci_read_config_word(const struct pci_dev *dev, int where, u16 *val);
int os_pci_write_config_word(const struct pci_dev *dev, int where, u16 val);
void os_pci_release_resource(struct pci_dev *dev, int resno);
int os_pci_resize_resource(struct pci_dev *dev, int resno, int size);
void os_pci_assign_unassigned_bus_resources(struct pci_bus *bus);
int os_pci_enable_pcie_error_reporting(struct pci_dev *pdev);
int os_pci_disable_pcie_error_reporting(struct pci_dev *pdev);
struct resource *os_pci_bus_resource_n(const struct pci_bus *bus, int n);

void os_pci_release_related_resources(struct pci_dev *pdev, int DDR_BAR_NUM);
void os_check_root_pcibus(struct pci_bus *root, struct resource **res);
struct pci_dev *os_find_parent_pci_dev(struct device *dev);

struct resource *os_request_region(resource_size_t start,
				   resource_size_t n,
				   const char *name);
void os_release_region(resource_size_t start, resource_size_t n);
struct resource *os_request_mem_region(resource_size_t start,
				       resource_size_t n,
				       const char *name);
void os_release_mem_region(resource_size_t start, resource_size_t n);
struct resource *os_create_resource(struct mtgpu_resource *mtgpu_res, u32 num_res);
void os_destroy_resource(struct resource *resource);
int os_release_resource(struct resource *new);
resource_size_t os_resource_size(const struct resource *res);
resource_size_t os_resource_start(const struct resource *res);

struct platform_device_info *os_create_platform_device_info(struct device *dev,
							    const char *name,
							    int id,
							    const struct resource *res,
							    unsigned int num_res,
							    const void *data,
							    size_t size_data,
							    u64 dma_mask);
void os_destroy_platform_device_info(struct platform_device_info *pdev_info);
void *os_platform_get_drvdata(struct platform_device *pdev);
void os_platform_set_drvdata(struct platform_device *pdev, void *data);
struct platform_device *
os_platform_device_register_full(const struct platform_device_info *pdevinfo);
void os_platform_device_unregister(struct platform_device *pdev);
struct resource *os_platform_get_resource(struct platform_device *dev,
					  unsigned int type,
					  unsigned int num);
struct device *os_get_platform_device_base(struct platform_device *pdev);
struct platform_device *os_to_platform_device(struct device *dev);

u64 os_roundup_pow_of_two(u64 size);
u32 os_order_base_2(u64 size);

resource_size_t os_get_system_available_ram_size(void);
void os_get_cached_msi_msg(unsigned int irq, struct msi_msg *msg);
struct msi_desc *os_irq_get_msi_desc(unsigned int irq);
struct msi_msg *os_msi_msg_alloc(void);
int os_get_msi_message(struct msi_msg *msiptr, u32 *addr_lo, u32 *addr_hi, u32 *data);
int os_get_msi_message_from_desc(struct msi_desc *desc, u32 *addr_lo, u32 *addr_hi, u32 *data);
int os_request_irq(unsigned int irq, void *handler, unsigned long flags,
		   const char *name, void *dev);
const void *os_free_irq(unsigned int irq, void *dev_id);
void os_pci_free_irq_vectors(struct pci_dev *pdev);

void os_msleep(unsigned int msecs);
void os_udelay(unsigned long secs);
void os_mdelay(unsigned long secs);
void os_usleep_range(unsigned long min, unsigned long max);

int os_register_pm_notifier(struct notifier_block *nb);
int os_unregister_pm_notifier(struct notifier_block *nb);

void os_dma_buf_put(struct dma_buf *dmabuf);
struct dma_buf *os_dma_buf_get(int fd);
int os_dma_set_mask_and_coherent(struct device *dev, u64 mask);
void *os_dma_alloc_coherent(struct device *dev, size_t size, dma_addr_t *dma_handle, gfp_t gfp);
void os_dma_free_coherent(struct device *dev, size_t size, void *cpu_addr, dma_addr_t dma_handle);
int os_dma_mmap_attrs(struct device *dev, struct vm_area_struct *vma, void *cpu_addr,
		      dma_addr_t dma_addr, size_t size, unsigned long attrs);

int os_request_firmware(const struct firmware **fw, const char *name, struct device *device);
const u8 *os_get_firmware_data(const struct firmware *fw);
size_t os_get_firmware_size(const struct firmware *fw);

unsigned long os_nsecs_to_jiffies(u64 n);
int os_mod_timer(struct timer_list *timer, unsigned long expires);
int os_del_timer_sync(struct timer_list *timer);
void os_timer_setup(struct timer_list *timer, void (*function)(struct timer_list *),
		    unsigned int flags);
int os_create_timer(struct timer_list **timer);
void os_destroy_timer(struct timer_list *timer);

struct inode *os_file_inode(const struct file *f);

int os_ilog2(u64 n);

/*About dev print*/
void _os_dev_emerg(const struct device *dev, const char *fmt, ...);
void _os_dev_crit(const struct device *dev, const char *fmt, ...);
void _os_dev_alert(const struct device *dev, const char *fmt, ...);
void _os_dev_err(const struct device *dev, const char *fmt, ...);
void _os_dev_warn(const struct device *dev, const char *fmt, ...);
void _os_dev_notice(const struct device *dev, const char *fmt, ...);
void _os_dev_info(const struct device *dev, const char *fmt, ...);
void _os_dev_dbg(const struct device *dev, const char *fmt, ...);

int os_snprintf(char *buf, size_t size, const char *fmt, ...);
int os_printk(const char *fmt, ...);
void os_printk_ratelimited(const char *fmt, ...);

void *os_devres_alloc(dr_release_t release, size_t size, gfp_t gfp);
void *os_devres_find(struct device *dev, void *release, void *match, void *match_data);
void *os_devres_open_group(struct device *dev, void *id, gfp_t gfp);
int os_devres_release_group(struct device *dev, void *id);
void os_devres_add(struct device *dev, void *res);
void os_devres_remove_group(struct device *dev, void *id);

bool OS_IS_ERR(const void *ptr);
bool OS_IS_ERR_OR_NULL(__force const void *ptr);
long OS_PTR_ERR(__force const void *ptr);
void *OS_ERR_PTR(long error);
int OS_READ_ONCE(int *val);

int os_sscanf(const char *str, const char *fmt, ...);
size_t os_strlen(const char *s);
int os_strcmp(const char *cs, const char *ct);
int os_strncmp(const char *cs, const char *ct, size_t count);
char *os_strcpy(char *dest, const char *src);
char *os_strncpy(char *dest, const char *src, size_t count);
char *os_strstr(const char *cs, const char *ct);
int os_sprintf(char *buf, const char *fmt, ...);

struct device *os_get_dev_parent_parent(struct device *dev);
void *os_get_device_driver_data(struct device *dev);

char *os_get_current_comm(void);
u64 os_get_current_pid(void);

char *os_get_utsname_version(void);

DECLARE_OS_STRUCT_COMMON_FUNCS(notifier_block);

#ifndef KERN_SOH
#define KERN_SOH	"\001"		/* ASCII Start Of Header */
#define KERN_SOH_ASCII	'\001'

#define KERN_EMERG	KERN_SOH "0"	/* system is unusable */
#define KERN_ALERT	KERN_SOH "1"	/* action must be taken immediately */
#define KERN_CRIT	KERN_SOH "2"	/* critical conditions */
#define KERN_ERR	KERN_SOH "3"	/* error conditions */
#define KERN_WARNING	KERN_SOH "4"	/* warning conditions */
#define KERN_NOTICE	KERN_SOH "5"	/* normal but significant condition */
#define KERN_INFO	KERN_SOH "6"	/* informational */
#define KERN_DEBUG	KERN_SOH "7"	/* debug-level messages */

#define KERN_DEFAULT	""		/* the default kernel loglevel */
#endif

#define os_pr_fmt(fmt) fmt

#define os_pr_emerg(fmt, ...)							\
	os_printk(KERN_EMERG os_pr_fmt(fmt), ##__VA_ARGS__)
#define os_pr_alert(fmt, ...)							\
	os_printk(KERN_ALERT os_pr_fmt(fmt), ##__VA_ARGS__)
#define os_pr_crit(fmt, ...)							\
	os_printk(KERN_CRIT os_pr_fmt(fmt), ##__VA_ARGS__)
#define os_pr_err(fmt, ...)							\
	os_printk(KERN_ERR os_pr_fmt(fmt), ##__VA_ARGS__)
#define os_pr_warning(fmt, ...)							\
	os_printk(KERN_WARNING os_pr_fmt(fmt), ##__VA_ARGS__)
#define os_pr_warn os_pr_warning
#define os_pr_notice(fmt, ...)							\
	os_printk(KERN_NOTICE os_pr_fmt(fmt), ##__VA_ARGS__)
#define os_pr_info(fmt, ...)							\
	os_printk(KERN_INFO os_pr_fmt(fmt), ##__VA_ARGS__)
#define os_pr_cont(fmt, ...)							\
	os_printk(KERN_CONT os_pr_fmt(fmt), ##__VA_ARGS__)

#define os_pr_debug(fmt, ...)							\
	os_printk(KERN_DEBUG os_pr_fmt(fmt), ##__VA_ARGS__)

#define os_pr_emerg_ratelimited(fmt, ...)					\
	os_printk_ratelimited(KERN_EMERG os_pr_fmt(fmt), ##__VA_ARGS__)
#define os_pr_alert_ratelimited(fmt, ...)					\
	os_printk_ratelimited(KERN_ALERT os_pr_fmt(fmt), ##__VA_ARGS__)
#define os_pr_crit_ratelimited(fmt, ...)					\
	os_printk_ratelimited(KERN_CRIT os_pr_fmt(fmt), ##__VA_ARGS__)
#define os_pr_err_ratelimited(fmt, ...)						\
	os_printk_ratelimited(KERN_ERR os_pr_fmt(fmt), ##__VA_ARGS__)
#define os_pr_warn_ratelimited(fmt, ...)					\
	os_printk_ratelimited(KERN_WARNING os_pr_fmt(fmt), ##__VA_ARGS__)
#define os_pr_notice_ratelimited(fmt, ...)					\
	os_printk_ratelimited(KERN_NOTICE os_pr_fmt(fmt), ##__VA_ARGS__)
#define os_pr_info_ratelimited(fmt, ...)					\
	os_printk_ratelimited(KERN_INFO os_pr_fmt(fmt), ##__VA_ARGS__)

#define os_dev_fmt(fmt) fmt

#define os_dev_emerg(dev, fmt, ...)						\
	_os_dev_emerg(dev, os_dev_fmt(fmt), ##__VA_ARGS__)
#define os_dev_crit(dev, fmt, ...)						\
	_os_dev_crit(dev, os_dev_fmt(fmt), ##__VA_ARGS__)
#define os_dev_alert(dev, fmt, ...)						\
	_os_dev_alert(dev, os_dev_fmt(fmt), ##__VA_ARGS__)
#define os_dev_err(dev, fmt, ...)						\
	_os_dev_err(dev, os_dev_fmt(fmt), ##__VA_ARGS__)
#define os_dev_warn(dev, fmt, ...)						\
	_os_dev_warn(dev, os_dev_fmt(fmt), ##__VA_ARGS__)
#define os_dev_notice(dev, fmt, ...)						\
	_os_dev_notice(dev, os_dev_fmt(fmt), ##__VA_ARGS__)
#define os_dev_info(dev, fmt, ...)						\
	_os_dev_info(dev, os_dev_fmt(fmt), ##__VA_ARGS__)
#define os_dev_dbg(dev, fmt, ...)						\
	_os_dev_dbg(dev, os_dev_fmt(fmt), ##__VA_ARGS__)

#endif /* __OS_INTERFACE_H__ */

