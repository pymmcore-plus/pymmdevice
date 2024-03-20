import ctypes
import os
import sys
from ctypes import CDLL, POINTER, byref, c_bool, c_char_p, c_int, c_long, c_void_p, cdll
from pathlib import Path
from typing import TYPE_CHECKING, Sequence

if TYPE_CHECKING:
    from typing import TypeVar

    from typing_extensions import ParamSpec

    P = ParamSpec("P")
    R = TypeVar("R")


MODULE_INTERFACE_VERSION = 10
DEVICE_INTERFACE_VERSION = 71


class DeviceInstance: ...


class LoadedDeviceAdapter:
    """Wrapper for a loaded device adapter."""

    def __init__(self, name: str, filename: str) -> None:
        self.name = name
        self.filename = filename
        try:
            self._module = self.load_mm_module(str(filename))
        except OSError as e:
            raise ValueError(f"Could not load MM module {filename!r}") from e

        # CheckInterfaceVersion
        # self.check_interface_version()
        self.initialize_module_data()

    @staticmethod
    def load_mm_module(filename: str | Path) -> CDLL:
        """Load a Micro-Manager module."""
        lib = cdll.LoadLibrary(str(filename))

        # InitializeModuleData
        lib.InitializeModuleData.argtypes = []
        lib.InitializeModuleData.restype = None

        # CreateDevice
        lib.CreateDevice.argtypes = [c_char_p]
        # Assuming MM::Device* is returned as a void pointer
        lib.CreateDevice.restype = c_void_p

        # DeleteDevice
        # Assuming MM::Device* is passed as a void pointer
        lib.DeleteDevice.argtypes = [c_void_p]
        lib.DeleteDevice.restype = None

        # GetModuleVersion
        lib.GetModuleVersion.argtypes = []
        lib.GetModuleVersion.restype = c_long

        # GetDeviceInterfaceVersion
        lib.GetDeviceInterfaceVersion.argtypes = []
        lib.GetDeviceInterfaceVersion.restype = c_long

        # GetNumberOfDevices
        lib.GetNumberOfDevices.argtypes = []
        lib.GetNumberOfDevices.restype = c_int

        # GetDeviceName
        lib.GetDeviceName.argtypes = [c_int, c_char_p, c_int]
        lib.GetDeviceName.restype = c_bool

        # GetDeviceType
        lib.GetDeviceType.argtypes = [c_char_p, POINTER(c_int)]
        lib.GetDeviceType.restype = c_bool

        # GetDeviceDescription
        lib.GetDeviceDescription.argtypes = [c_char_p, c_char_p, c_int]
        lib.GetDeviceDescription.restype = c_bool

        return lib

    def check_interface_version(self) -> None:
        """Check the interface version."""
        if (mod_ver := self.module_version) != MODULE_INTERFACE_VERSION:
            raise ValueError(
                f"Incompatible module interface version (required = "
                f"{MODULE_INTERFACE_VERSION}; found = {mod_ver})"
            )

        if (dev_ver := self.device_interface_version) != DEVICE_INTERFACE_VERSION:
            raise ValueError(
                f"Incompatible device interface version (required = "
                f"{DEVICE_INTERFACE_VERSION}; found = {dev_ver})"
            )

    def __repr__(self) -> str:
        """Return a string representation of the object."""
        return f"LoadedDeviceAdapter({self.name!r}, {Path(self.filename).name!r})"

    def initialize_module_data(self) -> None:
        """Initialize the module data."""
        self._module.InitializeModuleData()

    @property
    def module_version(self) -> int:
        """Return the version of the module."""
        return self._module.GetModuleVersion()  # type: ignore

    @property
    def device_interface_version(self) -> int:
        """Return the device interface version."""
        return self._module.GetDeviceInterfaceVersion()  # type: ignore

    # def load_device(self, name: str) -> DeviceInstance:
    # """Load a device."""
    # creates a DeviceInstance subclass based on the device type

    def create_device(self, name: str) -> c_void_p:
        """Create a device by name."""
        return self._module.CreateDevice(name.encode("utf-8"))  # type: ignore

    def delete_device(self, device: c_void_p) -> None:
        """Delete a device."""
        self._module.DeleteDevice(device)

    @property
    def num_devices(self) -> int:
        """Return the number of devices."""
        return self._module.GetNumberOfDevices()  # type: ignore

    def get_device_name(self, i: int) -> str:
        """Return the name of a device."""
        if i >= self.num_devices:
            raise IndexError(f"Index {i} out of range")

        s = ctypes.create_string_buffer(256)
        if not self._module.GetDeviceName(i, s, len(s)):
            raise ValueError(f"Failed to get device name for index {i}")
        return s.value.decode("utf-8")

    def get_device_type(self, name: str) -> int:
        """Return the type of a device."""
        t = c_int(0)
        if not self._module.GetDeviceType(name.encode("utf-8"), byref(t)):
            raise ValueError(f"Failed to get device type for {name}")
        return t.value

    def get_device_description(self, name: str) -> str:
        """Return the description of a device."""
        s = ctypes.create_string_buffer(1024)
        if not self._module.GetDeviceDescription(name.encode("utf-8"), s, len(s)):
            raise ValueError(f"Failed to get device description for {name}")
        return s.value.decode("utf-8")

    def get_available_device_names(self) -> tuple[str, ...]:
        """Return the names of available devices."""
        return tuple(self.get_device_name(i) for i in range(self.num_devices))


DEFAULT_LIB_NAME_PREFIX = "mmgr_dal_" if os.name == "nt" else "libmmgr_dal_"
DEFAULT_LIB_NAME_SUFFIX = ""
if os.name == "nt":
    DEFAULT_LIB_NAME_SUFFIX = ".dll"
elif sys.platform == "linux":
    DEFAULT_LIB_NAME_SUFFIX = ".so.0"


class PluginManager:
    """Manager for Micro-Manager device adapters."""

    def __init__(
        self,
        search_paths: Sequence[Path | str] = (),
        lib_prefix: str = DEFAULT_LIB_NAME_PREFIX,
        lib_suffix: str = DEFAULT_LIB_NAME_SUFFIX,
    ) -> None:
        self.search_paths: list[Path] = [Path(p).expanduser() for p in search_paths]
        self.lib_prefix = lib_prefix
        self.lib_suffix = lib_suffix
        self._module_map: dict[str, LoadedDeviceAdapter] = {}

    def find(self, filename: str) -> str:
        """Find a file in the search path. If it doesn't exist, return the filename."""
        for p in self.search_paths:
            if (path := (p / filename)).exists():
                return str(path)
        return filename

    def get_device_adapter(self, module_name: str) -> LoadedDeviceAdapter:
        """Get a device adapter by name. If it doesn't exist, load it."""
        if not module_name:
            raise ValueError("module_name must be a non-empty string")

        if module_name not in self._module_map:
            fname = self.find(f"{self.lib_prefix}{module_name}{self.lib_suffix}")
            print(fname)
            self._module_map[module_name] = LoadedDeviceAdapter(module_name, fname)

        return self._module_map[module_name]


if __name__ == "__main__":
    pm = PluginManager(
        ["~/Library/Application Support/pymmcore-plus/mm/Micro-Manager-258aa5ac8/"]
    )

    da = pm.get_device_adapter("DemoCamera")
    print(da)
    print(da.get_available_device_names())
