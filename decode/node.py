from dataclasses import dataclass


@dataclass
class EmuNode:
    node_id: str
    attached_gpu_ids: list[int]
    virtual_tensor_budget: float
    overcommit_policy: str
    root_complex_id: str

    @property
    def node_aggregation_factor(self) -> int:
        return len(self.attached_gpu_ids)
