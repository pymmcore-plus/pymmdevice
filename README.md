# pymmdevice

[![License](https://img.shields.io/pypi/l/pymmdevice.svg?color=green)](https://github.com/tlambert03/pymmdevice/raw/main/LICENSE)
[![PyPI](https://img.shields.io/pypi/v/pymmdevice.svg?color=green)](https://pypi.org/project/pymmdevice)
[![Python Version](https://img.shields.io/pypi/pyversions/pymmdevice.svg?color=green)](https://python.org)
[![CI](https://github.com/tlambert03/pymmdevice/actions/workflows/ci.yml/badge.svg)](https://github.com/tlambert03/pymmdevice/actions/workflows/ci.yml)
[![codecov](https://codecov.io/gh/tlambert03/pymmdevice/branch/main/graph/badge.svg)](https://codecov.io/gh/tlambert03/pymmdevice)

wrapper around MMDevice

## Development

### Clone the repo and initialize the submodules

```sh
git clone https://github.com/tlambert03/pymmdevice.git --recurse-submodules
cd pymmdevice
```

### Create a virtual environment

You can do this however you prefer:

```sh
# e.g
mamba create -n pymmdevice python=3.11
mamba activate pymmdevice

# or
uv venv
source .venv/bin/activate
```

### Install in editable mode *without* build isolation

Meson-python is used to build the pybind11 extension, and editable installs in
meson-python currently require disabling build isolation.  See [meson-python
docs](https://meson-python.readthedocs.io/en/latest/how-to-guides/editable-installs.html)
for more information.

This means you must also install the build dependencies manually
before running `pip install -e .`

```sh
pip install meson-python meson ninja pybind11  # build deps
pip install -e ".[dev]" --no-build-isolation
```

### Run tests

#### One time setup

The tests require a compiled `libmmgr_dal_DemoCamera` library from
Micro-Manager.  I do this by using the build-dev command from
[pymmcore-plus](https://github.com/pymmcore-plus/pymmcore-plus) (which will have
been installed in the `[dev]` extra above.)

```sh
mmcore build-dev DemoCamera
```

However, if you would like to point to a different location, you can set the
`MM_LIB_DIR` environment variable to the directory containing the compiled
libraries. (This directory must minimally contain `libmmgr_dal_DemoCamera`)

Then you can run the tests:

```sh
pytest
```

Note that compiled files end up in `build/` directory and are dynamically loaded
at import time and test time.  This means you can make changes to the pybind11
wrapper and simply re-run the tests without re-installing.

### Troubleshooting

After the initial install, one problem that can arise occasionally is a stale
`build/` directory.  If you encounter errors during the build process, try
removing the `build/` directory and re-installing.

```sh
rm -rf build && pip install -e . --no-build-isolation --force-reinstall
```

### Generating type stubs

```sh
pybind11-stubgen pymmdevice._pymmdevice -o src
```
