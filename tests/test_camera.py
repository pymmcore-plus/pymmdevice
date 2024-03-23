from __future__ import annotations

from typing import cast

import numpy as np
import pytest

import pymmdevice as pmmd


def test_demo_camera(pm: pmmd.PluginManager) -> None:
    module = pm.GetDeviceAdapter("DemoCamera")

    available = module.GetAvailableDeviceNames()
    assert "DCam" in available
    assert module.GetDeviceDescription("DCam") == "Demo camera"

    cam = cast(pmmd.CameraInstance, module.LoadDevice("DCam", "MyCamera"))
    cam.Initialize()
    try:
        assert (cam.GetImageWidth(), cam.GetImageHeight()) == (512, 512)
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
    finally:
        cam.Shutdown()


def test_demo_camera_context(pm: pmmd.PluginManager) -> None:
    module = pm.GetDeviceAdapter("DemoCamera")

    with pytest.raises(TypeError, match="'DStateDevice' is not a CameraInstance"):
        module.load_camera("DStateDevice", "MyCamera")

    with module.load_camera("DCam", "MyCamera") as cam:
        assert cam.GetBinning() == 1
        cam.SetBinning(2)
        assert cam.GetBinning() == 2

        assert cam.GetROI() == (0, 0, 256, 256)
        cam.SetROI(64, 64, 128, 128)
        assert cam.GetROI() == (64, 64, 128, 128)
