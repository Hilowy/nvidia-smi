/*
 * Merged kernel module script:
 * - Functional synthetic driver stub implementation.
 * - High-level sampling/ioctl flow retained from previous nvshim_kmod draft.
 */

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/random.h>
#include <linux/uaccess.h>

#include "../library/nvshim_api.h"
#define NVSHIM_NAME "nvshim"
#define NVSHIM_BATCH_LIMIT 8u

static dev_t nvshim_dev;
static struct cdev nvshim_cdev;
static struct class *nvshim_class;

static DEFINE_MUTEX(nvshim_lock);
static enum nv_mode current_mode = NV_MODE_AUTO;
static uint32_t sequence_id;

/*
 * sampler_loop equivalent:
 * - produce synthetic counters
 * - in WDDM mode enforce batched availability
 */
static struct pcie_tensor_sample make_synthetic_sample(void)
{
    struct pcie_tensor_sample s = {0};

    s.timestamp_ns = ktime_get_ns();
    s.gpu_bdf = 0x00010000;
    s.sm_util = (uint32_t)(jiffies % 100);
    s.mem_bw = (uint32_t)(1000 + (jiffies % 500));
    s.pcie_rx = (uint32_t)(100 + (jiffies % 80));
    s.pcie_tx = (uint32_t)(120 + (jiffies % 80));
    s.tensor_ops = (uint64_t)(1000000 + (jiffies % 1000));
    return s;
}

static long nvshim_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    if (cmd == NVSHIM_IOCTL_SET_MODE) {
        struct nvshim_mode_payload payload;

        if (copy_from_user(&payload, (void __user *)arg, sizeof(payload)))
            return -EFAULT;
        if (payload.api_version != NVSHIM_API_VERSION)
            return -EINVAL;
        if (payload.mode > NV_MODE_AUTO)
            return -EINVAL;

         mutex_lock(&nvshim_lock);
        current_mode = (enum nv_mode)payload.mode;
        mutex_unlock(&nvshim_lock);
        return 0;
    }

    if (cmd == NVSHIM_IOCTL_GET_SAMPLE) {
        struct nvshim_sample_payload payload = {0};
        enum nv_mode mode;

        mutex_lock(&nvshim_lock);
        mode = current_mode;
        payload.api_version = NVSHIM_API_VERSION;
        payload.sequence_id = ++sequence_id;
        payload.sample = make_synthetic_sample();

        if (mode == NV_MODE_WDDM && (payload.sequence_id % NVSHIM_BATCH_LIMIT) != 0) {
            mutex_unlock(&nvshim_lock);
            return -EAGAIN;
        }
        mutex_unlock(&nvshim_lock);

        if (copy_to_user((void __user *)arg, &payload, sizeof(payload)))
            return -EFAULT;
        return 0;
    }

    if (cmd == NVSHIM_IOCTL_GET_CAPS) {
        struct nvshim_caps_payload caps = {
            .api_version = NVSHIM_API_VERSION,
            .max_devices = 1,
            .supports_tcc = 1,
            .supports_wddm_emu = 1,
            .min_poll_interval_us = 1000,
        };

        if (copy_to_user((void __user *)arg, &caps, sizeof(caps)))
            return -EFAULT;
        return 0;
    }

    return -ENOTTY;
}

static const struct file_operations nvshim_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = nvshim_ioctl,
};

static int __init nvshim_init(void)
{
    int ret = alloc_chrdev_region(&nvshim_dev, 0, 1, NVSHIM_NAME);
    if (ret)
        return ret;

    cdev_init(&nvshim_cdev, &nvshim_fops);
    ret = cdev_add(&nvshim_cdev, nvshim_dev, 1);
    if (ret)
        goto err_unreg;

    nvshim_class = class_create(NVSHIM_NAME);
    if (IS_ERR(nvshim_class)) {
        ret = PTR_ERR(nvshim_class);
        goto err_cdev;
    }

    if (IS_ERR(device_create(nvshim_class, NULL, nvshim_dev, NULL, "nvshim0"))) {
        ret = -EINVAL;
        goto err_class;
    }

    pr_info("nvshim: loaded, device /dev/nvshim0\n");
    return 0;

err_cdev:
    cdev_del(&nvshim_cdev);
err_unreg:
    unregister_chrdev_region(nvshim_dev, 1);
    return ret;
}

static void __exit nvshim_exit(void)
{
    device_destroy(nvshim_class, nvshim_dev);
    class_destroy(nvshim_class);
    cdev_del(&nvshim_cdev);
    unregister_chrdev_region(nvshim_dev, 1);
}

module_init(nvshim_init);
module_exit(nvshim_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("NV shim synthetic sampling driver stub");
