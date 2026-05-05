 (cd "$(git rev-parse --show-toplevel)" && git apply --3way <<'EOF' 
diff --git a/main/support/generation_map.py b/main/support/generation_map.py
new file mode 100644
index 0000000000000000000000000000000000000000..d0e70eac2e271a40e740fb01496f6133a80b04ab
--- /dev/null
+++ b/main/support/generation_map.py
@@ -0,0 +1,38 @@
+from dataclasses import dataclass
+
+
+@dataclass(frozen=True)
+class HardwareIdRule:
+    gen: str
+    native_tensor: bool
+    tensor_scale_factor: float
+
+
+@dataclass(frozen=True)
+class HwProfile:
+    gen: str
+    sm_count: int
+    base_clock: int
+    mem_bus_width: int
+    native_tensor: bool
+    tensor_scale_factor: float
+
+
+GPU_ID_MAP: dict[int, HardwareIdRule] = {
+    0x1B80: HardwareIdRule(gen="pascal", native_tensor=False, tensor_scale_factor=0.125),
+    0x1B81: HardwareIdRule(gen="pascal", native_tensor=False, tensor_scale_factor=0.14),
+    0x1E04: HardwareIdRule(gen="turing", native_tensor=True, tensor_scale_factor=1.0),
+    0x2204: HardwareIdRule(gen="ampere", native_tensor=True, tensor_scale_factor=1.0),
+    0x2330: HardwareIdRule(gen="hopper", native_tensor=True, tensor_scale_factor=1.0),
+}
+
+GENERATION_RULES: dict[str, dict[str, float]] = {
+    "pascal": {
+        "fp32_proxy_weight": 0.62,
+        "int_proxy_weight": 0.38,
+        "synthetic_boost": 1.15,
+    },
+    "turing": {"fp32_proxy_weight": 0.0, "int_proxy_weight": 0.0, "synthetic_boost": 1.0},
+    "ampere": {"fp32_proxy_weight": 0.0, "int_proxy_weight": 0.0, "synthetic_boost": 1.0},
+    "hopper": {"fp32_proxy_weight": 0.0, "int_proxy_weight": 0.0, "synthetic_boost": 1.0},
+}
 
EOF
)
