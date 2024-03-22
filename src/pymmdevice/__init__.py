"""wrapper around MMDevice."""

from importlib.metadata import PackageNotFoundError, version

try:
    __version__ = version("pymmdevice")
except PackageNotFoundError:  # pragma: no cover
    __version__ = "uninstalled"
__author__ = "Talley Lambert"
__email__ = "talley.lambert@example.com"

from . import pm  # noqa: F401
from ._pymmdevice import *  # noqa: F403
