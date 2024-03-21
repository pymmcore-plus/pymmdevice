from __future__ import annotations

import pymmdevice._core as m


def test_loaded_dev() -> None:
    a = m.LoadedDeviceAdapter(
        "DemoCamera",
        "/Users/talley/Library/Application Support/pymmcore-plus/"
        "mm/Micro-Manager-80d5ac1/libmmgr_dal_DemoCamera",
    )
    assert "DCam" in a.get_available_device_names()
    assert a.get_device_description("DCam") == "Demo camera"

    cam = a.load_device("DCam", "MyCamera")
    cam.Initialize()
    assert cam.GetBinning() == 1
    cam.SetBinning(2)
    assert cam.GetBinning() == 2
