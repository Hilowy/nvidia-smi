 (cd "$(git rev-parse --show-toplevel)" && git apply --3way <<'EOF' 
diff --git a/main/decode/__init__.py b/main/decode/__init__.py
new file mode 100644
index 0000000000000000000000000000000000000000..6ba357fe3aa12a90c17c5097ccd042386f92cd31
--- /dev/null
+++ b/main/decode/__init__.py
@@ -0,0 +1,4 @@
+from .hardware import compose_tensor_metric, decode_hw_profile
+from .node import EmuNode
+
+__all__ = ["compose_tensor_metric", "decode_hw_profile", "EmuNode"]
 
EOF
)
