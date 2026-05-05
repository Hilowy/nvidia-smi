#ifndef NVSHIM_API_H
#define NVSHIM_API_H

#include <stdint.h>
#include <linux/ioctl.h>

#define NVSHIM_API_VERSION 0x00010000u
#define NVSHIM_DEV_PATH "/dev/nvshim0"

#define NVSHIM_MAX_GPU_PER_NODE 16u
#define NVSHIM_NAME_MAX 32u

/* -------------------------
 * Mode model (TCC / WDDM-emu)
 * ------------------------- */
typedef enum nvshim_mode_e {
    NVSHIM_MODE_AUTO      = 0,
    NVSHIM_MODE_TCC       = 1,  /* low-latency pull */
    NVSHIM_MODE_WDDM_EMU  = 2   /* batched + strict validation */
} nvshim_mode_t;

/* -------------------------
 * Error/status codes
 * ------------------------- */
typedef enum nvshim_status_e {
    NVSHIM_OK                         = 0,
    NVSHIM_ERR_UNSUPPORTED_GPU        = -1,
    NVSHIM_ERR_TOPOLOGY_INVALID       = -2,
    NVSHIM_ERR_MODE_TRANSITION_BLOCK  = -3,
    NVSHIM_ERR_SYNTHETIC_OVERFLOW     = -4,
    NVSHIM_ERR_INVALID_ARG            = -5,
    NVSHIM_ERR_STALE_SAMPLE           = -6
} nvshim_status_t;

/* -------------------------
 * Hardware generation
 * ------------------------- */
typedef enum nvshim_gpu_gen_e {
    NVSHIM_GEN_UNKNOWN = 0,
    NVSHIM_GEN_PASCAL  = 1,
    NVSHIM_GEN_TURING  = 2,
    NVSHIM_GEN_AMPERE  = 3,
    NVSHIM_GEN_ADA     = 4
} nvshim_gpu_gen_t;

/* -------------------------
 * Device capability profile
 * ------------------------- */
struct nvshim_hw_profile {
    uint32_t pci_device_id;
    uint32_t sm_count;
    uint32_t base_clock_mhz;
    uint32_t mem_bus_width_bits;
    uint32_t native_tensor;       /* 0/1 */
    uint32_t tensor_scale_q16;    /* emulation scale factor in Q16.16 */
    nvshim_gpu_gen_t gen;
};

/* -------------------------
 * Raw GPU sample (kernel)
 * ------------------------- */
struct nvshim_pcie_tensor_sample {
    uint64_t timestamp_ns;
    uint32_t gpu_bdf;             /* packed bus:dev:function */
    uint32_t seq_id;

    uint32_t sm_util_mpermil;     /* 0..100000 */
    uint32_t mem_bw_mib_s;
    uint32_t pcie_rx_mib_s;
    uint32_t pcie_tx_mib_s;

    uint64_t tensor_ops_raw;      /* native or proxy-derived */
    uint32_t flags;               /* bit0:emulated_tensor, bit1:batched */
    uint32_t reserved0;
};

/* -------------------------
 * Emulated node aggregation
 * ------------------------- */
struct nvshim_emu_node_sample {
    uint32_t node_id;
    uint32_t gpu_count;
    uint32_t gpu_bdf[NVSHIM_MAX_GPU_PER_NODE];

    uint64_t tensor_accum_ops;
    uint32_t agg_factor_q16;      /* Q16.16 */
    uint32_t latency_us;
};

/* -------------------------
 * IOCTL payloads
 * ------------------------- */
struct nvshim_ioctl_set_mode {
    uint32_t api_version;
    nvshim_mode_t mode;
    uint32_t strict_rules;        /* 0/1 */
    uint32_t reserved0;
};

struct nvshim_ioctl_get_caps {
    uint32_t api_version;
    uint32_t gpu_bdf;
    struct nvshim_hw_profile profile;
    int32_t status;
};

struct nvshim_ioctl_get_sample {
    uint32_t api_version;
    uint32_t gpu_bdf;
    struct nvshim_pcie_tensor_sample sample;
    int32_t status;
};

struct nvshim_ioctl_get_node_sample {
    uint32_t api_version;
    uint32_t node_id;
    struct nvshim_emu_node_sample sample;
    int32_t status;
};

#define NVSHIM_IOC_MAGIC 'N'
#define NVSHIM_IOCTL_SET_MODE       _IOW(NVSHIM_IOC_MAGIC, 0x01, struct nvshim_ioctl_set_mode)
#define NVSHIM_IOCTL_GET_CAPS       _IOWR(NVSHIM_IOC_MAGIC, 0x02, struct nvshim_ioctl_get_caps)
#define NVSHIM_IOCTL_GET_SAMPLE     _IOWR(NVSHIM_IOC_MAGIC, 0x03, struct nvshim_ioctl_get_sample)
#define NVSHIM_IOCTL_GET_NODE_SAMPLE _IOWR(NVSHIM_IOC_MAGIC, 0x04, struct nvshim_ioctl_get_node_sample)

#endif /* NVSHIM_API_H */
