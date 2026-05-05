#include <stdbool.h>
#include <stdint.h>

#include "../main/library/nvshim_api.h"

struct workload_frame {
    const char *mode_label;
    uint32_t bdf;
    uint32_t sequence_id;
    uint64_t timestamp_ns;
    uint32_t sm_util_pct;
    uint32_t mem_bw_mb_s;
    uint64_t tensor_ops;
};

static bool is_valid_bdf(uint32_t bdf)
{
    return (bdf & 0xFFFF0000u) != 0;
}

static const char *map_mode_label(enum nv_mode mode)
{
    if (mode == NV_MODE_TCC)
        return "tcc-low-latency";
    if (mode == NV_MODE_WDDM)
        return "wddm-emu-batched";
    return "auto";
}

bool nvshim_decode_sample(enum nv_mode mode,
                          const struct nvshim_sample_payload *payload,
                          uint32_t previous_sequence_id,
                          uint64_t previous_timestamp_ns,
                          struct workload_frame *out_frame)
{
    if (!payload || !out_frame)
        return false;
    if (payload->api_version != NVSHIM_API_VERSION)
        return false;
    if (!is_valid_bdf(payload->sample.gpu_bdf))
        return false;
    if (payload->sequence_id <= previous_sequence_id)
        return false;
    if (payload->sample.timestamp_ns <= previous_timestamp_ns)
        return false;

    out_frame->mode_label = map_mode_label(mode);
    out_frame->bdf = payload->sample.gpu_bdf;
    out_frame->sequence_id = payload->sequence_id;
    out_frame->timestamp_ns = payload->sample.timestamp_ns;
    out_frame->sm_util_pct = payload->sample.sm_util;
    out_frame->mem_bw_mb_s = payload->sample.mem_bw;
    out_frame->tensor_ops = payload->sample.tensor_ops;
    return true;
}
