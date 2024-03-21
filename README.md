# pymmdevice

[![License](https://img.shields.io/pypi/l/pymmdevice.svg?color=green)](https://github.com/tlambert03/pymmdevice/raw/main/LICENSE)
[![PyPI](https://img.shields.io/pypi/v/pymmdevice.svg?color=green)](https://pypi.org/project/pymmdevice)
[![Python Version](https://img.shields.io/pypi/pyversions/pymmdevice.svg?color=green)](https://python.org)
[![CI](https://github.com/tlambert03/pymmdevice/actions/workflows/ci.yml/badge.svg)](https://github.com/tlambert03/pymmdevice/actions/workflows/ci.yml)
[![codecov](https://codecov.io/gh/tlambert03/pymmdevice/branch/main/graph/badge.svg)](https://codecov.io/gh/tlambert03/pymmdevice)

wrapper around MMDevice

## Dev

editable install (must be done without build isolation to work with meson)
compiled files end up in `build/` directory

```sh
pip install meson-python ninja pybind11
rm -rf build  # common problem
pip install -e . --no-build-isolation --force-reinstall
pybind11-stubgen pymmdevice._pymmdevice -o src
```
