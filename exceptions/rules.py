"""Policy mode rules, validations, and deterministic error handling."""

from dataclasses import dataclass
from typing import Any, Dict, List, Optional

ERR_UNSUPPORTED_GPU = "ERR_UNSUPPORTED_GPU"
ERR_TOPOLOGY_INVALID = "ERR_TOPOLOGY_INVALID"
ERR_MODE_TRANSITION_BLOCKED = "ERR_MODE_TRANSITION_BLOCKED"
ERR_SYNTHETIC_OVERFLOW = "ERR_SYNTHETIC_OVERFLOW"

MODE_SWITCH = "MODE_SWITCH"
POLICY_CLAMP = "POLICY_CLAMP"
EMU_NODE_REJECT = "EMU_NODE_REJECT"

MODE_RULES = {
    "TCC": {
        "sample_frequency_hz_max": 500,
        "batch_threshold": 8,
        "batch_window_ms": None,
        "tensor_growth_bound": 0.20,
    },
    "WDDM-emu": {
        "sample_frequency_hz_max": 120,
        "batch_threshold": 32,
        "batch_window_ms": 25,
        "tensor_growth_bound": 0.05,
    },
}

ALLOWED_TRANSITIONS = {
    "TCC": {"TCC", "WDDM-emu"},
    "WDDM-emu": {"WDDM-emu", "TCC"},
}


@dataclass(frozen=True)
class ValidationResult:
    ok: bool
    code: Optional[str] = None
    tag: Optional[str] = None


def validate_mode_transition(prev_mode: str, next_mode: str) -> ValidationResult:
    if prev_mode not in ALLOWED_TRANSITIONS or next_mode not in MODE_RULES:
        return ValidationResult(False, ERR_MODE_TRANSITION_BLOCKED, MODE_SWITCH)

    if next_mode not in ALLOWED_TRANSITIONS[prev_mode]:
        return ValidationResult(False, ERR_MODE_TRANSITION_BLOCKED, MODE_SWITCH)

    return ValidationResult(True, None, MODE_SWITCH)


def validate_sample_window(ts_prev: int, ts_cur: int) -> ValidationResult:
    if ts_cur <= ts_prev:
        return ValidationResult(False, ERR_TOPOLOGY_INVALID, POLICY_CLAMP)
    return ValidationResult(True)


def validate_tensor_delta(prev: float, cur: float, max_step_per_ms: float) -> ValidationResult:
    delta = cur - prev
    if delta > max_step_per_ms:
        return ValidationResult(False, ERR_SYNTHETIC_OVERFLOW, EMU_NODE_REJECT)
    return ValidationResult(True)


def apply_mode_rules(mode: str, sample_frequency_hz: int, batch_threshold: int, tensor_growth: float) -> Dict[str, Any]:
    if mode not in MODE_RULES:
        return {
            "ok": False,
            "code": ERR_UNSUPPORTED_GPU,
            "tag": EMU_NODE_REJECT,
            "rules": None,
        }

    rules = MODE_RULES[mode]
    clamped_sample_frequency = min(sample_frequency_hz, rules["sample_frequency_hz_max"])
    enforced_batch_threshold = min(batch_threshold, rules["batch_threshold"])

    overflow = tensor_growth > rules["tensor_growth_bound"]

    return {
        "ok": not overflow,
        "code": ERR_SYNTHETIC_OVERFLOW if overflow else None,
        "tag": POLICY_CLAMP if (clamped_sample_frequency != sample_frequency_hz or enforced_batch_threshold != batch_threshold) else MODE_SWITCH,
        "rules": {
            "sample_frequency_hz": clamped_sample_frequency,
            "batch_threshold": enforced_batch_threshold,
            "batch_window_ms": rules["batch_window_ms"],
            "tensor_growth_bound": rules["tensor_growth_bound"],
        },
    }


def normalize_policy_output(
    mode: str,
    gpu_id: str,
    node_id: str,
    tensor_util_norm: float,
    confidence: float,
    flags: Optional[List[str]] = None,
) -> Dict[str, Any]:
    return {
        "mode": mode,
        "gpu_id": gpu_id,
        "node_id": node_id,
        "tensor_util_norm": max(0.0, min(1.0, tensor_util_norm)),
        "confidence": max(0.0, min(1.0, confidence)),
        "flags": flags or [],
    }
