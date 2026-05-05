 (cd "$(git rev-parse --show-toplevel)" && git apply --3way <<'EOF' 
diff --git a/main/decode/README.md b/main/decode/README.md
new file mode 100644
index 0000000000000000000000000000000000000000..71adf003979a1374f20645fe8396177ede9996ce
--- /dev/null
+++ b/main/decode/README.md
@@ -0,0 +1,4 @@
+# Decode layer
+
+Contains function parameter contracts and decode logic for transforming
+`nvshim` sample payloads into normalized workload frames.
 
EOF
)
