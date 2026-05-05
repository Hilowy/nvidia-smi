/* policy_rules.c concrete thresholds */

int nvshim_validate_mode_transition(nvshim_mode_t prev, nvshim_mode_t next) {
    if (next < NVSHIM_MODE_AUTO || next > NVSHIM_MODE_WDDM_EMU)
        return NVSHIM_ERR_INVALID_ARG;

    /* Example: disallow direct WDDM_EMU -> TCC if strict lock active elsewhere */
    /* if (strict_lock && prev==WDDM_EMU && next==TCC) return BLOCK; */

    return NVSHIM_OK;
}

int nvshim_validate_sample_window(uint64_t prev_ts_ns, uint64_t cur_ts_ns, nvshim_mode_t mode) {
    if (cur_ts_ns <= prev_ts_ns) return NVSHIM_ERR_STALE_SAMPLE;
    uint64_t d = cur_ts_ns - prev_ts_ns;

    uint64_t min_ns = (mode == NVSHIM_MODE_TCC) ? 500000ull : 2000000ull;
    uint64_t max_ns = (mode == NVSHIM_MODE_TCC) ? 50000000ull : 200000000ull;

    if (d < min_ns || d > max_ns) return NVSHIM_ERR_STALE_SAMPLE;
    return NVSHIM_OK;
}

int nvshim_validate_tensor_delta(uint64_t prev_ops, uint64_t cur_ops, uint64_t delta_ns, nvshim_mode_t mode) {
    if (cur_ops < prev_ops) return NVSHIM_ERR_SYNTHETIC_OVERFLOW;
    if (delta_ns == 0) return NVSHIM_ERR_INVALID_ARG;

    uint64_t dop = cur_ops - prev_ops;
    long double ops_per_s = ((long double)dop * 1e9L) / (long double)delta_ns;

    long double cap = (mode == NVSHIM_MODE_TCC) ? 2.5e14L : 1.6e14L;
    if (ops_per_s > cap) return NVSHIM_ERR_SYNTHETIC_OVERFLOW;
    return NVSHIM_OK;
}
