/* decode_workload.h */
#ifndef NVSHIM_DECODE_WORKLOAD_H
#define NVSHIM_DECODE_WORKLOAD_H

#include "nvshim_api.h"

struct nvshim_norm_frame {
    uint64_t timestamp_ns;
    uint32_t gpu_bdf;
    uint32_t node_id;

    float tensor_util_norm;   /* 0..1 */
    float pcie_pressure_norm; /* 0..1 */
    float confidence;         /* 0..1 */
    uint32_t flags;
};

int nvshim_decode_frame(
    const struct nvshim_pcie_tensor_sample *s,
    const struct nvshim_hw_profile *p,
    nvshim_mode_t mode,
    struct nvshim_norm_frame *out);

#endif
