from __future__ import annotations

from typing import TYPE_CHECKING

if TYPE_CHECKING:
    import pymmdevice as pmmd

    from .conftest import Di


def test_device_manager(
    pm: pmmd.PluginManager, dm: pmmd.DeviceManager, device_info: Di
) -> None:
    module = pm.GetDeviceAdapter("DemoCamera")
    dev = dm.LoadDevice(module, device_info.name, f"My{device_info.name}")
    assert dev is not None

    assert module.GetDeviceDescription(device_info.name) == device_info.desc
    assert dev.GetDescription() == device_info.desc
