 (cd "$(git rev-parse --show-toplevel)" && git apply --3way <<'EOF' 
diff --git a/main/exceptions/topology.py b/main/exceptions/topology.py
new file mode 100644
index 0000000000000000000000000000000000000000..bdee7ba492de570feede9565bfcda2a6194b8bd0
--- /dev/null
+++ b/main/exceptions/topology.py
@@ -0,0 +1,20 @@
+from main.decode.node import EmuNode
+from main.support import EXPLICIT_ID_WHITELIST, GPU_ID_MAP
+
+
+def validate_attachment(node: EmuNode, gpu_to_root: dict[int, str]) -> None:
+    for gpu_id in node.attached_gpu_ids:
+        root_id = gpu_to_root.get(gpu_id)
+        if root_id is None:
+            raise ValueError(f"GPU 0x{gpu_id:04X} has no root-complex assignment")
+        if root_id != node.root_complex_id:
+            raise ValueError(
+                f"GPU 0x{gpu_id:04X} belongs to root complex {root_id}, "
+                f"cannot attach to node root complex {node.root_complex_id}"
+            )
+
+
+def validate_supported_gpu_ids(gpu_ids: list[int]) -> None:
+    for gpu_id in gpu_ids:
+        if gpu_id not in GPU_ID_MAP and gpu_id not in EXPLICIT_ID_WHITELIST:
+            raise ValueError(f"Unsupported GPU ID 0x{gpu_id:04X}; add it to main/support/ids_* whitelist")
 
EOF
)
