 (cd "$(git rev-parse --show-toplevel)" && git apply --3way <<'EOF' 
diff --git a/main/support/__init__.py b/main/support/__init__.py
new file mode 100644
index 0000000000000000000000000000000000000000..37a2d17fe2153d6c9eb34022edbe501d2a44594c
--- /dev/null
+++ b/main/support/__init__.py
@@ -0,0 +1,12 @@
+"""Hardware support maps and identifiers."""
+
+from .generation_map import GPU_ID_MAP, GENERATION_RULES, HardwareIdRule, HwProfile
+from .ids_whitelist import EXPLICIT_ID_WHITELIST
+
+__all__ = [
+    "GPU_ID_MAP",
+    "GENERATION_RULES",
+    "HardwareIdRule",
+    "HwProfile",
+    "EXPLICIT_ID_WHITELIST",
+]
 
EOF
)
