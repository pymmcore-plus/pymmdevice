from __future__ import annotations

from typing import cast

import numpy as np

import pymmdevice as pmmd


def test_loaded_dev(pm: pmmd.CPluginManager) -> None:
    module = pm.GetDeviceAdapter("DemoCamera")

    assert "DCam" in module.get_available_device_names()
    assert module.get_device_description("DCam") == "Demo camera"

    cam = cast(pmmd.CameraInstance, module.load_device("DCam", "MyCamera"))
    cam.Initialize()
    assert cam.GetBinning() == 1
    cam.SetBinning(2)
    assert cam.GetBinning() == 2

    assert (cam.GetImageWidth(), cam.GetImageHeight()) == (256, 256)
    assert not cam.GetImageBuffer()
    cam.SnapImage()
    ary = cam.GetImageArray()

    assert isinstance(ary, np.ndarray)
    assert ary.shape == (256, 256)
    assert ary.dtype == np.uint8

    cam.Shutdown()
