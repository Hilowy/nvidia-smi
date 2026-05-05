/* hw_ids.c - concrete C style */

static const struct nvshim_hw_profile k_profiles[] = {
    /* Pascal examples (native_tensor=0, emulated scale > 1.0) */
    { .pci_device_id=0x1B80, .sm_count=20, .base_clock_mhz=1607, .mem_bus_width_bits=256,
      .native_tensor=0, .tensor_scale_q16=(uint32_t)(0.65 * 65536), .gen=NVSHIM_GEN_PASCAL },

    { .pci_device_id=0x1B06, .sm_count=30, .base_clock_mhz=1531, .mem_bus_width_bits=384,
      .native_tensor=0, .tensor_scale_q16=(uint32_t)(0.72 * 65536), .gen=NVSHIM_GEN_PASCAL },

    /* Turing+ example (native tensor available) */
    { .pci_device_id=0x1E04, .sm_count=36, .base_clock_mhz=1365, .mem_bus_width_bits=256,
      .native_tensor=1, .tensor_scale_q16=(1u << 16), .gen=NVSHIM_GEN_TURING },
};

int nvshim_lookup_profile(uint32_t pci_device_id, struct nvshim_hw_profile *out) {
    if (!out) return -1;
    for (size_t i = 0; i < ARRAY_SIZE(k_profiles); ++i) {
        if (k_profiles[i].pci_device_id == pci_device_id) {
            *out = k_profiles[i];
            return 0;
        }
    }
    return -1;
}
