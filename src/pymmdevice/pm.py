"""Global PluginManager instance."""

from typing import Sequence

from ._pymmdevice import LoadedDeviceAdapter, PluginManager

_GLOBAL_PM: None | PluginManager = None


def global_instance() -> PluginManager:
    """Get the global PluginManager instance."""
    global _GLOBAL_PM
    if _GLOBAL_PM is None:
        _GLOBAL_PM = PluginManager()
    return _GLOBAL_PM


def set_search_paths(paths: Sequence[str]) -> None:
    """Set the search paths for the global PluginManager instance."""
    global_instance().SetSearchPaths(list(paths))


def get_search_paths() -> Sequence[str]:
    """Return the search paths for the global PluginManager instance."""
    return global_instance().GetSearchPaths()


def get_device_adapter(name: str) -> LoadedDeviceAdapter:
    """Return DeviceAdapter instance for the given name."""
    return global_instance().GetDeviceAdapter(name)


def get_available_device_adapters() -> Sequence[str]:
    """Return the list of device adapters available in the set search paths."""
    return global_instance().GetAvailableDeviceAdapters()


def unload_plugin_library(name: str) -> None:
    """Unload the plugin library for the given name."""
    global_instance().UnloadPluginLibrary(name)
