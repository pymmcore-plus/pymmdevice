from typing import Sequence

from ._pymmdevice import PluginManager

_GLOBAL_PM: None | PluginManager


def pm() -> PluginManager:
    global _GLOBAL_PM
    if _GLOBAL_PM is None:
        _GLOBAL_PM = PluginManager()
    return _GLOBAL_PM


def set_search_paths(paths: Sequence[str]) -> None:
    pm().SetSearchPaths(list(paths))


def get_search_paths() -> Sequence[str]:
    return pm().GetSearchPaths()
