 (cd "$(git rev-parse --show-toplevel)" && git apply --3way <<'EOF' 
diff --git a/main/exceptions/__init__.py b/main/exceptions/__init__.py
new file mode 100644
index 0000000000000000000000000000000000000000..2a4d44d30acd5ed683776039396115fa4ba39b1f
--- /dev/null
+++ b/main/exceptions/__init__.py
@@ -0,0 +1,3 @@
+from .topology import validate_attachment, validate_supported_gpu_ids
+
+__all__ = ["validate_attachment", "validate_supported_gpu_ids"]
 
EOF
)
