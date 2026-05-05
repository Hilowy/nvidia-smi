#ifndef NV_TYPES_H
#define NV_TYPES_H

#include <stdint.h>

enum nv_mode {
    NV_MODE_TCC = 0,
    NV_MODE_WDDM = 1,
    NV_MODE_AUTO = 2,
};

struct pcie_tensor_sample {
    uint64_t timestamp_ns;
    uint32_t gpu_bdf;
    uint32_t sm_util;
    uint32_t mem_bw;
    uint32_t pcie_rx;
    uint32_t pcie_tx;
    uint64_t tensor_ops;
};

struct emu_node_sample {
    uint32_t node_id;
    uint32_t linked_gpu_bdf;
    uint64_t tensor_accum;
    uint32_t latency_us;
};

#endif
