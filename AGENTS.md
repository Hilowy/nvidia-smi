# nvidia-smi
Cuda 6.0 compute simulation layer from Pascal to limited-tensors 7.5.
# tcc-async
Extention by over onnx einstin model trained
on-the-fly code updating single example focused.

""" @codex
#Working step's on point structure.
- kernel driver (strict, minimal).
- user-space driver shim (translation + validation).
- policy engine (model inference).
- hardware abstraction layer (cross-gen mapping).
"""
#Branch structures permament.
- define core layer in 'main->library'
- define parameters of each function 'main->decode'
- define rules in 'main->exceptions'
- define hardware id's in 'main->support'
  ###\###/###\###/###\...
