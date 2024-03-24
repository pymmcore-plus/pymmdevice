from __future__ import annotations

from typing import TYPE_CHECKING

import pymmdevice as pmmd

if TYPE_CHECKING:
    from .conftest import Di


def test_device_manager(
    pm: pmmd.PluginManager, dm: pmmd.DeviceManager, device_info: Di
) -> None:
    module = pm.GetDeviceAdapter("DemoCamera")
    lbl = f"My{device_info.name}"
    dev = dm.LoadDevice(module, device_info.name, lbl)
    assert dev is not None
    assert dm.GetDevice(lbl) is dev
    assert dm.GetDeviceOfType(lbl, getattr(pmmd.DeviceType, device_info.type)) is dev

    assert lbl in dm.GetDeviceList()
    assert lbl not in dm.GetDeviceList(pmmd.DeviceType.GenericDevice)

    assert module.GetDeviceDescription(device_info.name) == device_info.desc
    assert dev.GetDescription() == device_info.desc

    assert isinstance(dm.GetLoadedPeripherals(lbl), list)
    if device_info.type == "HubDevice":
        assert isinstance(dm.GetParentDevice(dev), pmmd.HubInstance)
        assert dm.GetParentDevice(dev) is dev  # hub is its own parent
    else:
        assert dm.GetParentDevice(dev) is None


def test_loaded_module(pm: pmmd.PluginManager, dm: pmmd.DeviceManager) -> None:
    module = pm.GetDeviceAdapter("DemoCamera")
    assert module.LoadDevice("DCam", "MyDemoCamera")
    assert module.GetLock()
    assert module.GetDeviceDescription("DCam") == "Demo camera"
    assert module.GetAdvertisedDeviceType("DCam") == pmmd.DeviceType.CameraDevice
    assert "DCam" in module.GetAvailableDeviceNames()
    assert module.GetName() == "DemoCamera"
