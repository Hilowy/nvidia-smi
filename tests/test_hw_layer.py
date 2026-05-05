import pytest

from main.decode import EmuNode, compose_tensor_metric, decode_hw_profile
from main.exceptions import validate_attachment, validate_supported_gpu_ids


def test_decode_hw_profile_pascal():
    profile = decode_hw_profile(0x1B80)
    assert profile.gen == "pascal"
    assert profile.native_tensor is False


def test_compose_tensor_metric_pascal_emulated():
    profile = decode_hw_profile(0x1B80)
    metric = compose_tensor_metric(
        {"fp32": 100.0, "int": 30.0, "node_multiplier": 2.0},
        profile,
        "raw",
    )
    assert metric["is_emulated_tensor"] is True
    assert metric["source_gpu_gen"] == "pascal"
    assert metric["node_aggregation_factor"] == 2.0
    assert metric["tensor_metric"] > 0


def test_topology_reject_cross_root_complex():
    node = EmuNode("node-a", [0x1B80, 0x1B81], 1000.0, "throttle", "rc0")
    with pytest.raises(ValueError):
        validate_attachment(node, {0x1B80: "rc0", 0x1B81: "rc1"})


def test_validate_supported_ids():
    validate_supported_gpu_ids([0x1B80, 0x13C2])
    with pytest.raises(ValueError):
        validate_supported_gpu_ids([0xDEAD])
