from __future__ import annotations

from typing import cast

import pymmdevice._pymmdevice as m


def test_loaded_dev(mm_lib_dir: str) -> None:
    pm = m.CPluginManager()
    pm.SetSearchPaths([mm_lib_dir])
    a = pm.GetDeviceAdapter("DemoCamera")

    assert "DCam" in a.get_available_device_names()
    assert a.get_device_description("DCam") == "Demo camera"

    cam = cast(m.CameraInstance, a.load_device("DCam", "MyCamera"))
    cam.Initialize()
    assert cam.GetBinning() == 1
    cam.SetBinning(2)
    assert cam.GetBinning() == 2
