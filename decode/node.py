 (cd "$(git rev-parse --show-toplevel)" && git apply --3way <<'EOF' 
diff --git a/main/decode/node.py b/main/decode/node.py
new file mode 100644
index 0000000000000000000000000000000000000000..c12c8040bf5acda2c82c2473df0c87a87776f69c
--- /dev/null
+++ b/main/decode/node.py
@@ -0,0 +1,14 @@
+from dataclasses import dataclass
+
+
+@dataclass
+class EmuNode:
+    node_id: str
+    attached_gpu_ids: list[int]
+    virtual_tensor_budget: float
+    overcommit_policy: str
+    root_complex_id: str
+
+    @property
+    def node_aggregation_factor(self) -> int:
+        return len(self.attached_gpu_ids)
 
EOF
)
