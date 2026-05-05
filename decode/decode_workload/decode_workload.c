/* decode_workload.c  */

static inline float clampf(float v, float lo, float hi) {
    return (v < lo) ? lo : (v > hi) ? hi : v;
}

static float q16_to_f32(uint32_t q16) {
    return ((float)q16) / 65536.0f;
}

int nvshim_decode_frame(const struct nvshim_pcie_tensor_sample *s,
                        const struct nvshim_hw_profile *p,
                        nvshim_mode_t mode,
                        struct nvshim_norm_frame *out) {
    if (!s || !p || !out) return NVSHIM_ERR_INVALID_ARG;

    float tensor_ops = (float)s->tensor_ops_raw;

    /* Pascal emulation correction */
    if (!p->native_tensor) {
        tensor_ops *= q16_to_f32(p->tensor_scale_q16);
    }

    /* Mode-dependent normalization ceiling */
    float max_ops = (mode == NVSHIM_MODE_TCC) ? 1.5e14f : 1.0e14f;
    float tensor_norm = clampf(tensor_ops / max_ops, 0.0f, 1.0f);

    float pcie_mix = (float)s->pcie_rx_mib_s * 0.45f + (float)s->pcie_tx_mib_s * 0.55f;
    float pcie_norm = clampf(pcie_mix / 32768.0f, 0.0f, 1.0f);

    float confidence = 0.95f;
    if (s->flags & 0x1) confidence -= 0.15f; /* emulated tensor */
    if (mode == NVSHIM_MODE_WDDM_EMU) confidence -= 0.05f; /* batched path */

    out->timestamp_ns = s->timestamp_ns;
    out->gpu_bdf = s->gpu_bdf;
    out->node_id = 0; /* filled by node mapper */
    out->tensor_util_norm = tensor_norm;
    out->pcie_pressure_norm = pcie_norm;
    out->confidence = clampf(confidence, 0.0f, 1.0f);
    out->flags = s->flags;
    return NVSHIM_OK;
}
