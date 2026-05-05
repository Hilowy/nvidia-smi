# linux nvidia-smi

Cuda 6.0 compute simulation layer from Pascal to limited-tensors 7.5.

# nvidia-smi shim scaffold

+This repository now contains a Linux-oriented shim architecture split by layer:

- `main/library/`: core shared interfaces and ABI-safe payload types.
- `main/decode/`: function parameter and decode logic docs.
- `main/exceptions/`: validation and mode-rule definitions.
- `main/support/`: hardware ID and capability mapping table.
- `kernel_driver/`: kernel-facing `/dev/nvshim0` sampling stub.
- `user_shim/`: user-space decoder into policy-friendly workload frames.

+## End-to-end flow

+1. **Kernel sampler** (`kernel_driver/nvshim_kmod_stub.c`) emits synthetic PCIe + tensor counters at a fixed cadence.

+2. **Device/ioctl boundary** (`main/library/nvshim_api.h`) exposes fixed-size, versioned payloads via:
+   - `NVSHIM_IOCTL_SET_MODE`
+   - `NVSHIM_IOCTL_GET_SAMPLE`
+   - `NVSHIM_IOCTL_GET_CAPS`
      
+3. **User shim decode** (`user_shim/nvshim_decode.c`) validates BDF, sequence IDs, and monotonic timestamps.
      
+4. **Policy input** receives normalized workload frames with stable mode labels:
+   - `tcc-low-latency`
+   - `wddm-emu-batched`
+   - `auto`
+
+## Mode behavior
+
+- **TCC**: low-latency direct sample path (`GET_SAMPLE` returns each poll).

+- **WDDM-emu**: batched reads and stricter rate behavior (`-EAGAIN` until batch boundary).


## Build & Test

```bash
make lint
make test
