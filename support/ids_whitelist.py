 (cd "$(git rev-parse --show-toplevel)" && git apply --3way <<'EOF' 
diff --git a/main/support/ids_whitelist.py b/main/support/ids_whitelist.py
new file mode 100644
index 0000000000000000000000000000000000000000..d7ef6f5a15da1ccb936d83c4804eb2ba6d398e98
--- /dev/null
+++ b/main/support/ids_whitelist.py
@@ -0,0 +1,5 @@
+"""Explicitly allowed unknown device IDs for staged bring-up."""
+
+EXPLICIT_ID_WHITELIST: set[int] = {
+    0x13C2,
+}
 
EOF
)
