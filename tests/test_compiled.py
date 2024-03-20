from __future__ import annotations

import pymmdevice._core as m


def test_loaded_dev() -> None:
    a = m.LoadedDeviceAdapter(
        "DemoCamera",
        "/Users/talley/Library/Application Support/pymmcore-plus/"
        "mm/Micro-Manager-80d5ac1/libmmgr_dal_DemoCamera",
    )
    print(a.get_available_device_names())
    print(a.get_device_description("DCam"))

    print(a.load_device("DCam", "MyCamera"))
