/* hw_ids.h */
#ifndef NVSHIM_HW_IDS_H
#define NVSHIM_HW_IDS_H
#include <stdint.h>
#include "nvshim_api.h"

int nvshim_lookup_profile(uint32_t pci_device_id, struct nvshim_hw_profile *out);

#endif
