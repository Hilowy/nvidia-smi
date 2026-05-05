from collections.abc import Mapping

from main.support import GENERATION_RULES, GPU_ID_MAP, HwProfile


def decode_hw_profile(pci_id: int) -> HwProfile:
    if pci_id not in GPU_ID_MAP:
        raise KeyError(f"Unsupported PCI device ID: 0x{pci_id:04X}")

    rule = GPU_ID_MAP[pci_id]

    defaults = {
        "pascal": (20, 1531, 256),
        "turing": (40, 1545, 256),
        "ampere": (68, 1710, 320),
        "hopper": (120, 1980, 384),
    }
    sm_count, base_clock, mem_bus_width = defaults[rule.gen]
    return HwProfile(
        gen=rule.gen,
        sm_count=sm_count,
        base_clock=base_clock,
        mem_bus_width=mem_bus_width,
        native_tensor=rule.native_tensor,
        tensor_scale_factor=rule.tensor_scale_factor,
    )


def compose_tensor_metric(raw_counters: Mapping[str, float], hw_profile: HwProfile, mode: str) -> dict[str, float | str | bool]:
    if hw_profile.native_tensor:
        tensor_val = raw_counters.get("tensor", 0.0)
        is_emulated = False
        agg_factor = 1.0
    else:
        rules = GENERATION_RULES[hw_profile.gen]
        fp_proxy = raw_counters.get("fp32", 0.0) * rules["fp32_proxy_weight"]
        int_proxy = raw_counters.get("int", 0.0) * rules["int_proxy_weight"]
        node_multiplier = raw_counters.get("node_multiplier", 1.0)
        agg_factor = node_multiplier
        tensor_val = (fp_proxy + int_proxy) * hw_profile.tensor_scale_factor * rules["synthetic_boost"] * node_multiplier
        is_emulated = True

    if mode == "normalized":
        tensor_val = tensor_val / max(hw_profile.sm_count, 1)

    return {
        "tensor_metric": round(tensor_val, 4),
        "is_emulated_tensor": is_emulated,
        "source_gpu_gen": hw_profile.gen,
        "node_aggregation_factor": agg_factor,
    }
