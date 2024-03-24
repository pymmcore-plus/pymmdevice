# pymmdevice

[![License](https://img.shields.io/pypi/l/pymmdevice.svg?color=green)](https://github.com/pymmcore-plus/pymmdevice/raw/main/LICENSE)
[![PyPI](https://img.shields.io/pypi/v/pymmdevice.svg?color=green)](https://pypi.org/project/pymmdevice)
[![Python Version](https://img.shields.io/pypi/pyversions/pymmdevice.svg?color=green)](https://python.org)
[![CI](https://github.com/pymmcore-plus/pymmdevice/actions/workflows/ci.yml/badge.svg)](https://github.com/pymmcore-plus/pymmdevice/actions/workflows/ci.yml)
[![codecov](https://codecov.io/gh/pymmcore-plus/pymmdevice/branch/main/graph/badge.svg)](https://codecov.io/gh/pymmcore-plus/pymmdevice)

Direct, low-level control of Micro-Manager device adapters in Python.

## Rationale

For most use cases, if you want to use Micro-Manager devices directly in Python,
you will install [`pymmcore`](https://github.com/micro-manager/pymmcore)
(or [`pymmcore-plus`](https://github.com/pymmcore-plus/pymmcore-plus))
and instantiate an instance of `pymmcore.CMMCore` (or
`pymmcore_plus.CMMCorePlus`).  Then, to load a specific device adapter, you
would call `loadDevice('MyLabel', 'ModuleName', 'DeviceName')`.  All control of
the device is routed through the `CMMCore` instance, and is therefore subject to
the `CMMCore` model and API.  This is a safe and robust way to do this.

This library is an **experimental** lower-level wrapper around the MMDevice API that
gives you *direct* control over device adapter libraries, as python wrappers around
the various subclasses of
[`MMCore/Devices/DeviceInstance.h`](https://github.com/micro-manager/mmCoreAndDevices/tree/4441b057e65fed8914c58c33e64123b17eeb6b25/MMCore/Devices).
This means that you could have nothing more than a single compiled device
adapter (i.e. one of the `libmmgr_dal_` libraries that you find inside of the
Micro-Manager directory) and control it from Python, without any restrictions of
the intermediate CMMCore API.  This absolutely opens the possibility of using
a device incorrectly, so an understanding of the MMDevice API is important,
and caution is advised.  Here be dragons.

## Installation

```sh
pip install git+https://github.com/pymmcore-plus/pymmdevice.git
```

## Usage

```python
import numpy as np

import pymmdevice as pmmd

pm = pmmd.PluginManager()
lib_dir = (
    "/Users/talley/Library/Application Support/pymmcore-plus/mm/Micro-Manager-80d5ac1"
)
pm.SetSearchPaths([lib_dir])
module = pm.GetDeviceAdapter("DemoCamera")
assert "DCam" in module.GetAvailableDeviceNames()

with module.load_camera("DCam", "MyCamera") as cam:
    cam.SetBinning(2)
    print(cam.GetExposure())
    cam.SnapImage()
    img = cam.GetImageArray()
    assert isinstance(img, np.ndarray)
    assert img.shape == (256, 256)
```

## Development

### Clone the repo and initialize the submodules

```sh
git clone https://github.com/pymmcore-plus/pymmdevice.git --recurse-submodules
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
pip install meson-python meson ninja pybind11
pip install -e ".[dev]" --no-build-isolation
```

#### Source the micro-manager libraries somewhere

The tests require a compiled `libmmgr_dal_DemoCamera` library from
Micro-Manager.  I do this by using the build-dev command from
[pymmcore-plus](https://github.com/pymmcore-plus/pymmcore-plus) (which will have
been installed in the `[dev]` extra above.)

```sh
mmcore build-dev DemoCamera

# or, to install all libraries from the latest release
mmcore install
```

However, if you would like to point to a different location, you can set the
`MM_LIB_DIR` environment variable to the directory containing the compiled
libraries. (This directory must minimally contain `libmmgr_dal_DemoCamera`
for tests to pass.)

### Run tests

Then you can run the tests:

```sh
pytest
```

Note that compiled files end up in `build/` directory and are dynamically loaded
at import time and test time.  This means you can make changes to the pybind11
wrapper and simply re-run the tests without re-installing.

### Troubleshooting

- `ERROR: File  does not exist.` when building or running `pip install -e .`

    You likely didn't initialize the submodules when you cloned the repo.  Run:

    ```sh
    git submodule update --init --recursive
    ```

- General Reset

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
