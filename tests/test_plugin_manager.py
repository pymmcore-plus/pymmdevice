from __future__ import annotations

from typing import TYPE_CHECKING

import pytest

import pymmdevice as pmmd

if TYPE_CHECKING:
    from .conftest import Di


def _getenv(varname: str) -> str:
    # because c++ setenv is not visible to python
    import ctypes

    libc = ctypes.CDLL(None)
    getenv = libc.getenv
    getenv.restype = ctypes.c_char_p
    return getenv(varname.encode()).decode()  # type: ignore


def test_plugin_manager(mm_lib_dir: str) -> None:
    pm = pmmd.PluginManager()
    assert not pm.GetAvailableDeviceAdapters()
    with pytest.raises(RuntimeError, match='Failed to load device adapter "FooCamera"'):
        pm.GetDeviceAdapter("FooCamera")

    pm.SetSearchPaths([mm_lib_dir])
    assert "DemoCamera" in pm.GetAvailableDeviceAdapters()
    assert isinstance(pm.GetDeviceAdapter("DemoCamera"), pmmd.LoadedDeviceAdapter)
    pm.UnloadPluginLibrary("DemoCamera")


def test_set_search_paths_updates_environment_path() -> None:
    path = "some_path"
    assert path not in _getenv("PATH")
    pm = pmmd.PluginManager()
    pm.SetSearchPaths([path])
    assert path in _getenv("PATH")


def test_global_pm(mm_lib_dir: str) -> None:
    from pymmdevice import pm

    assert not pm.get_available_device_adapters()
    pm.set_search_paths([])
    assert not pm.get_search_paths()

    pm.set_search_paths([mm_lib_dir])
    assert "DemoCamera" in pm.get_available_device_adapters()
    assert isinstance(pm.get_device_adapter("DemoCamera"), pmmd.LoadedDeviceAdapter)

    pm.unload_plugin_library("DemoCamera")


def test_all_devices(pm: pmmd.PluginManager, device_info: Di) -> None:
    module = pm.GetDeviceAdapter(device_info.library)
    with module.LoadDevice(device_info.name, f"My{device_info.name}") as dev:
        assert dev


# def test_loaded_module_from_file(mm_lib_dir: str) -> None:
#     democam_file = sorted(Path(mm_lib_dir).glob("libmmgr_dal_DemoCamera.*"))[0]
#     module = pmmd.LoadedDeviceAdapter.from_file(str(democam_file))
#     assert module.LoadDevice("DCam", "MyDemoCamera").GetName() == "DCam"
