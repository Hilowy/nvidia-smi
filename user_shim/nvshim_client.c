int main(void) {
    int fd = open(NVSHIM_DEV_PATH, O_RDWR);
    if (fd < 0) return 1;

    struct nvshim_ioctl_set_mode setm = {
        .api_version = NVSHIM_API_VERSION,
        .mode = NVSHIM_MODE_TCC,
        .strict_rules = 1
    };
    ioctl(fd, NVSHIM_IOCTL_SET_MODE, &setm);

    while (running) {
        struct nvshim_ioctl_get_sample req = {
            .api_version = NVSHIM_API_VERSION,
            .gpu_bdf = target_bdf
        };

        if (ioctl(fd, NVSHIM_IOCTL_GET_SAMPLE, &req) == 0 && req.status == NVSHIM_OK) {
            struct nvshim_ioctl_get_caps caps = {
                .api_version = NVSHIM_API_VERSION,
                .gpu_bdf = target_bdf
            };
            ioctl(fd, NVSHIM_IOCTL_GET_CAPS, &caps);

            struct nvshim_norm_frame frame;
            nvshim_decode_frame(&req.sample, &caps.profile, NVSHIM_MODE_TCC, &frame);

            /* pass frame -> policy engine inference queue */
            enqueue_policy_frame(&frame);
        }

        usleep(1000); /* tune per mode */
    }

    close(fd);
    return 0;
}
