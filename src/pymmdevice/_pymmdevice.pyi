from __future__ import annotations
import typing
__all__ = ['AutoFocusInstance', 'CPluginManager', 'CameraInstance', 'GalvoInstance', 'GenericInstance', 'HubInstance', 'ImageProcessorInstance', 'LoadedDeviceAdapter', 'MagnifierInstance', 'SLMInstance', 'SerialInstance', 'ShutterInstance', 'SignalIOInstance', 'StageInstance', 'StateInstance', 'XYStageInstance']
class AutoFocusInstance:
    def __init__(self, arg0: MockCMMCore, arg1: LoadedDeviceAdapter, arg2: str, arg3: ..., arg4: ..., arg5: str, arg6: ..., arg7: ...) -> None:
        ...
class CPluginManager:
    def GetDeviceAdapter(self, moduleName: str) -> LoadedDeviceAdapter:
        ...
    def SetSearchPaths(self, paths: typing.Iterable) -> None:
        ...
    def __init__(self) -> None:
        ...
class CameraInstance:
    def GetBinning(self) -> int:
        ...
    def GetBitDepth(self) -> int:
        ...
    def GetChannelName(self, arg0: int) -> str:
        ...
    def GetComponentName(self, arg0: int) -> str:
        ...
    def GetExposure(self) -> float:
        ...
    @typing.overload
    def GetImageBuffer(self) -> int:
        ...
    @typing.overload
    def GetImageBuffer(self, arg0: int) -> int:
        ...
    def GetImageBufferSize(self) -> int:
        ...
    def GetImageBytesPerPixel(self) -> int:
        ...
    def GetImageHeight(self) -> int:
        ...
    def GetImageWidth(self) -> int:
        ...
    def GetNumberOfChannels(self) -> int:
        ...
    def GetNumberOfComponents(self) -> int:
        ...
    def GetPixelSizeUm(self) -> float:
        ...
    def Initialize(self) -> None:
        ...
    def SetBinning(self, arg0: int) -> int:
        ...
    def SetExposure(self, arg0: float) -> None:
        ...
    @typing.overload
    def SnapImage(self) -> int:
        ...
    @typing.overload
    def SnapImage(self) -> int:
        ...
    def __init__(self, arg0: MockCMMCore, arg1: LoadedDeviceAdapter, arg2: str, arg3: ..., arg4: ..., arg5: str, arg6: ..., arg7: ...) -> None:
        ...
class GalvoInstance:
    def __init__(self, arg0: MockCMMCore, arg1: LoadedDeviceAdapter, arg2: str, arg3: ..., arg4: ..., arg5: str, arg6: ..., arg7: ...) -> None:
        ...
class GenericInstance:
    def __init__(self, arg0: MockCMMCore, arg1: LoadedDeviceAdapter, arg2: str, arg3: ..., arg4: ..., arg5: str, arg6: ..., arg7: ...) -> None:
        ...
class HubInstance:
    def __init__(self, arg0: MockCMMCore, arg1: LoadedDeviceAdapter, arg2: str, arg3: ..., arg4: ..., arg5: str, arg6: ..., arg7: ...) -> None:
        ...
class ImageProcessorInstance:
    def __init__(self, arg0: MockCMMCore, arg1: LoadedDeviceAdapter, arg2: str, arg3: ..., arg4: ..., arg5: str, arg6: ..., arg7: ...) -> None:
        ...
class LoadedDeviceAdapter:
    def __init__(self, arg0: str, arg1: str) -> None:
        ...
    def get_available_device_names(self) -> list[str]:
        ...
    def get_device_description(self, deviceName: str) -> str:
        ...
    def get_name(self) -> str:
        ...
    def load_device(self, name: str, label: str) -> DeviceInstance:
        ...
class MagnifierInstance:
    def __init__(self, arg0: MockCMMCore, arg1: LoadedDeviceAdapter, arg2: str, arg3: ..., arg4: ..., arg5: str, arg6: ..., arg7: ...) -> None:
        ...
class SLMInstance:
    def __init__(self, arg0: MockCMMCore, arg1: LoadedDeviceAdapter, arg2: str, arg3: ..., arg4: ..., arg5: str, arg6: ..., arg7: ...) -> None:
        ...
class SerialInstance:
    def __init__(self, arg0: MockCMMCore, arg1: LoadedDeviceAdapter, arg2: str, arg3: ..., arg4: ..., arg5: str, arg6: ..., arg7: ...) -> None:
        ...
class ShutterInstance:
    def __init__(self, arg0: MockCMMCore, arg1: LoadedDeviceAdapter, arg2: str, arg3: ..., arg4: ..., arg5: str, arg6: ..., arg7: ...) -> None:
        ...
class SignalIOInstance:
    def __init__(self, arg0: MockCMMCore, arg1: LoadedDeviceAdapter, arg2: str, arg3: ..., arg4: ..., arg5: str, arg6: ..., arg7: ...) -> None:
        ...
class StageInstance:
    def __init__(self, arg0: MockCMMCore, arg1: LoadedDeviceAdapter, arg2: str, arg3: ..., arg4: ..., arg5: str, arg6: ..., arg7: ...) -> None:
        ...
class StateInstance:
    def __init__(self, arg0: MockCMMCore, arg1: LoadedDeviceAdapter, arg2: str, arg3: ..., arg4: ..., arg5: str, arg6: ..., arg7: ...) -> None:
        ...
class XYStageInstance:
    def __init__(self, arg0: MockCMMCore, arg1: LoadedDeviceAdapter, arg2: str, arg3: ..., arg4: ..., arg5: str, arg6: ..., arg7: ...) -> None:
        ...
