from __future__ import annotations

import typing

import numpy

__all__ = [
    "AutoFocusInstance",
    "Callable",
    "CameraInstance",
    "Device",
    "DeviceInstance",
    "GalvoInstance",
    "GenericInstance",
    "HubInstance",
    "ImageProcessorInstance",
    "LoadedDeviceAdapter",
    "Logger",
    "MagnifierInstance",
    "MockCMMCore",
    "PluginManager",
    "SLMInstance",
    "SerialInstance",
    "ShutterInstance",
    "SignalIOInstance",
    "StageInstance",
    "StateInstance",
    "XYStageInstance",
]

class AutoFocusInstance:
    def __init__(
        self,
        arg0: MockCMMCore,
        arg1: LoadedDeviceAdapter,
        arg2: str,
        arg3: Device,
        arg4: typing.Callable,
        arg5: str,
        arg6: Logger,
        arg7: Logger,
    ) -> None: ...

class Callable:
    pass

class CameraInstance:
    def GetBinning(self) -> int: ...
    def GetBitDepth(self) -> int: ...
    def GetChannelName(self, arg0: int) -> str: ...
    def GetComponentName(self, arg0: int) -> str: ...
    def GetExposure(self) -> float: ...
    def GetImageArray(self, arg: int = 0) -> numpy.ndarray: ...
    @typing.overload
    def GetImageBuffer(self) -> int: ...
    @typing.overload
    def GetImageBuffer(self, arg0: int) -> int: ...
    def GetImageBufferSize(self) -> int: ...
    def GetImageBytesPerPixel(self) -> int: ...
    def GetImageHeight(self) -> int: ...
    def GetImageWidth(self) -> int: ...
    def GetNumberOfChannels(self) -> int: ...
    def GetNumberOfComponents(self) -> int: ...
    def GetPixelSizeUm(self) -> float: ...
    def Initialize(self) -> None: ...
    def SetBinning(self, arg0: int) -> int: ...
    def SetExposure(self, arg0: float) -> None: ...
    def Shutdown(self) -> None: ...
    def SnapImage(self) -> int: ...
    def __enter__(self) -> CameraInstance: ...
    def __exit__(self, *args) -> None: ...
    def __init__(
        self,
        arg0: MockCMMCore,
        arg1: LoadedDeviceAdapter,
        arg2: str,
        arg3: Device,
        arg4: typing.Callable,
        arg5: str,
        arg6: Logger,
        arg7: Logger,
    ) -> None: ...

class Device:
    pass

class DeviceInstance:
    pass

class GalvoInstance:
    def __init__(
        self,
        arg0: MockCMMCore,
        arg1: LoadedDeviceAdapter,
        arg2: str,
        arg3: Device,
        arg4: typing.Callable,
        arg5: str,
        arg6: Logger,
        arg7: Logger,
    ) -> None: ...

class GenericInstance:
    def __init__(
        self,
        arg0: MockCMMCore,
        arg1: LoadedDeviceAdapter,
        arg2: str,
        arg3: Device,
        arg4: typing.Callable,
        arg5: str,
        arg6: Logger,
        arg7: Logger,
    ) -> None: ...

class HubInstance:
    def __init__(
        self,
        arg0: MockCMMCore,
        arg1: LoadedDeviceAdapter,
        arg2: str,
        arg3: Device,
        arg4: typing.Callable,
        arg5: str,
        arg6: Logger,
        arg7: Logger,
    ) -> None: ...

class ImageProcessorInstance:
    def __init__(
        self,
        arg0: MockCMMCore,
        arg1: LoadedDeviceAdapter,
        arg2: str,
        arg3: Device,
        arg4: typing.Callable,
        arg5: str,
        arg6: Logger,
        arg7: Logger,
    ) -> None: ...

class LoadedDeviceAdapter:
    def __init__(self, arg0: str, arg1: str) -> None: ...
    def get_available_device_names(self) -> list[str]: ...
    def get_device_description(self, deviceName: str) -> str: ...
    def get_name(self) -> str: ...
    def load_camera(self, name: str, label: str) -> CameraInstance: ...
    def load_device(self, name: str, label: str) -> DeviceInstance: ...

class Logger:
    pass

class MagnifierInstance:
    def __init__(
        self,
        arg0: MockCMMCore,
        arg1: LoadedDeviceAdapter,
        arg2: str,
        arg3: Device,
        arg4: typing.Callable,
        arg5: str,
        arg6: Logger,
        arg7: Logger,
    ) -> None: ...

class MockCMMCore:
    pass

class PluginManager:
    def GetAvailableDeviceAdapters(self) -> list[str]: ...
    def GetDeviceAdapter(self, moduleName: str) -> LoadedDeviceAdapter: ...
    def GetSearchPaths(self) -> list[str]: ...
    def SetSearchPaths(self, paths: typing.Iterable) -> None: ...
    def UnloadPluginLibrary(self, moduleName: str) -> None: ...
    def __init__(self) -> None: ...

class SLMInstance:
    def __init__(
        self,
        arg0: MockCMMCore,
        arg1: LoadedDeviceAdapter,
        arg2: str,
        arg3: Device,
        arg4: typing.Callable,
        arg5: str,
        arg6: Logger,
        arg7: Logger,
    ) -> None: ...

class SerialInstance:
    def __init__(
        self,
        arg0: MockCMMCore,
        arg1: LoadedDeviceAdapter,
        arg2: str,
        arg3: Device,
        arg4: typing.Callable,
        arg5: str,
        arg6: Logger,
        arg7: Logger,
    ) -> None: ...

class ShutterInstance:
    def __init__(
        self,
        arg0: MockCMMCore,
        arg1: LoadedDeviceAdapter,
        arg2: str,
        arg3: Device,
        arg4: typing.Callable,
        arg5: str,
        arg6: Logger,
        arg7: Logger,
    ) -> None: ...

class SignalIOInstance:
    def __init__(
        self,
        arg0: MockCMMCore,
        arg1: LoadedDeviceAdapter,
        arg2: str,
        arg3: Device,
        arg4: typing.Callable,
        arg5: str,
        arg6: Logger,
        arg7: Logger,
    ) -> None: ...

class StageInstance:
    def __init__(
        self,
        arg0: MockCMMCore,
        arg1: LoadedDeviceAdapter,
        arg2: str,
        arg3: Device,
        arg4: typing.Callable,
        arg5: str,
        arg6: Logger,
        arg7: Logger,
    ) -> None: ...

class StateInstance:
    def __init__(
        self,
        arg0: MockCMMCore,
        arg1: LoadedDeviceAdapter,
        arg2: str,
        arg3: Device,
        arg4: typing.Callable,
        arg5: str,
        arg6: Logger,
        arg7: Logger,
    ) -> None: ...

class XYStageInstance:
    def __init__(
        self,
        arg0: MockCMMCore,
        arg1: LoadedDeviceAdapter,
        arg2: str,
        arg3: Device,
        arg4: typing.Callable,
        arg5: str,
        arg6: Logger,
        arg7: Logger,
    ) -> None: ...
