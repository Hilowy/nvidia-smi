/*
 * Linux kernel module skeleton:
 * - char device /dev/nvshim0
 * - periodic sampler thread
 * - mode-aware behavior (TCC vs WDDM-emu)
 */

global state {
    nvshim_mode_t mode = NVSHIM_MODE_AUTO;
    bool strict_rules = true;

    spinlock sample_lock;
    map<uint32_t /*gpu_bdf*/, nvshim_pcie_tensor_sample> latest_sample;
    map<uint32_t /*gpu_bdf*/, nvshim_hw_profile> hw_caps;
    map<uint32_t /*node_id*/, nvshim_emu_node_sample> latest_node_sample;

    wait_queue sample_wq;
    atomic64 global_seq;
    kthread *sampler_task;
}

function module_init():
    discover_pci_nvidia_devices()
    for each gpu:
        hw_caps[gpu.bdf] = build_hw_profile(gpu.device_id)
    register_chrdev_region()
    cdev_add()
    create_dev_node("/dev/nvshim0")
    sampler_task = kthread_run(sampler_loop)
    return 0

function module_exit():
    stop_kthread(sampler_task)
    destroy_dev_node()
    cdev_del()
    unregister_chrdev_region()

function sampler_loop():
    while not kthread_should_stop():
        interval_us = (mode == NVSHIM_MODE_TCC) ? 1000 : 5000

        for each gpu in discovered:
            raw = read_or_simulate_gpu_counters(gpu)
            profile = hw_caps[gpu.bdf]

            sample.timestamp_ns = ktime_get_ns()
            sample.gpu_bdf = gpu.bdf
            sample.seq_id = atomic64_inc_return(global_seq)
            sample.sm_util_mpermil = clamp(raw.sm_util, 0, 100000)
            sample.mem_bw_mib_s = raw.mem_bw
            sample.pcie_rx_mib_s = raw.pcie_rx
            sample.pcie_tx_mib_s = raw.pcie_tx

            if profile.native_tensor:
                sample.tensor_ops_raw = raw.native_tensor_ops
                sample.flags.bit0_emulated_tensor = 0
            else:
                /* Pascal proxy synthesis */
                sample.tensor_ops_raw = synthesize_tensor_proxy(raw, profile)
                sample.flags.bit0_emulated_tensor = 1

            if mode == NVSHIM_MODE_WDDM_EMU:
                sample = apply_batch_window_and_rate_limits(sample)
                sample.flags.bit1_batched = 1

            lock(sample_lock)
            latest_sample[gpu.bdf] = sample
            unlock(sample_lock)

        rebuild_emu_node_aggregation(latest_sample, latest_node_sample)
        wake_up_interruptible(sample_wq)
        usleep_range(interval_us, interval_us + 250)

function ioctl(file, cmd, arg):
    switch cmd:
      case NVSHIM_IOCTL_SET_MODE:
          req = copy_from_user(arg)
          if req.api_version != NVSHIM_API_VERSION: return -EINVAL
          if !validate_mode_transition(mode, req.mode): return -EPERM
          mode = req.mode
          strict_rules = req.strict_rules
          return 0

      case NVSHIM_IOCTL_GET_CAPS:
          req = copy_from_user(arg)
          prof = hw_caps[req.gpu_bdf]
          if not found: req.status = NVSHIM_ERR_UNSUPPORTED_GPU
          else req.profile = prof, req.status = NVSHIM_OK
          copy_to_user(arg, req)
          return 0

      case NVSHIM_IOCTL_GET_SAMPLE:
          req = copy_from_user(arg)
          lock(sample_lock)
          s = latest_sample[req.gpu_bdf]
          unlock(sample_lock)
          if !s: req.status = NVSHIM_ERR_UNSUPPORTED_GPU
          else req.sample = s, req.status = NVSHIM_OK
          copy_to_user(arg, req)
          return 0

      case NVSHIM_IOCTL_GET_NODE_SAMPLE:
          req = copy_from_user(arg)
          ns = latest_node_sample[req.node_id]
          if !ns: req.status = NVSHIM_ERR_TOPOLOGY_INVALID
          else req.sample = ns, req.status = NVSHIM_OK
          copy_to_user(arg, req)
          return 0
