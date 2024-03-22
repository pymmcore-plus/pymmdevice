import pytest

import pymmdevice as pmmd


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
