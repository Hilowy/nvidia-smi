/* policy_rules.h */
#ifndef NVSHIM_POLICY_RULES_H
#define NVSHIM_POLICY_RULES_H

#include <stdint.h>
#include "nvshim_api.h"

int nvshim_validate_mode_transition(nvshim_mode_t prev, nvshim_mode_t next);
int nvshim_validate_sample_window(uint64_t prev_ts_ns, uint64_t cur_ts_ns, nvshim_mode_t mode);
int nvshim_validate_tensor_delta(uint64_t prev_ops, uint64_t cur_ops, uint64_t delta_ns, nvshim_mode_t mode);

#endif
