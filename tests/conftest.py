import os
from contextlib import suppress
from typing import Iterator, NamedTuple

import pytest

import pymmdevice as pmmd

os.environ["MESONPY_EDITABLE_VERBOSE"] = "1"


class Di(NamedTuple):
    name: str
    type: str
    desc: str
    library: str = "DemoCamera"


DEMO_DEVICES = [
    Di("DCam", "CameraDevice", "Demo camera"),
    Di("DWheel", "StateDevice", "Demo filter wheel"),
    Di("DStateDevice", "StateDevice", "Demo State Device"),
    Di("DObjective", "StateDevice", "Demo objective turret"),
    Di("DStage", "StageDevice", "Demo stage"),
    Di("DXYStage", "XYStageDevice", "Demo XY stage"),
    Di("DLightPath", "StateDevice", "Demo light path"),
    Di("DAutoFocus", "AutoFocusDevice", "Demo auto focus"),
    Di("DShutter", "ShutterDevice", "Demo shutter"),
    Di("D-DA", "SignalIODevice", "Demo DA"),
    Di("D-DA2", "SignalIODevice", "Demo DA-2"),
    Di("DOptovar", "MagnifierDevice", "Demo Optovar"),
    Di("DGalvo", "GalvoDevice", "Demo Galvo"),
    Di("TransposeProcessor", "ImageProcessorDevice", "TransposeProcessor"),
    Di("ImageFlipX", "ImageProcessorDevice", "ImageFlipX"),
    Di("ImageFlipY", "ImageProcessorDevice", "ImageFlipY"),
    Di("MedianFilter", "ImageProcessorDevice", "MedianFilter"),
    Di("DHub", "HubDevice", "DHub"),
]


@pytest.fixture
def mm_lib_dir() -> str:
    if mm_path := os.getenv("MM_LIB_DIR"):
        return mm_path

    with suppress(ImportError):
        from pymmcore_plus import find_micromanager

        if mm_path := find_micromanager():
            return mm_path

    raise RuntimeError(
        "MM_LIB_DIR not set and Micro-Manager not found.\n"
        "Please either set the MM_LIB_DIR environment variable to the\ndirectory "
        "containing the Micro-Manager libraries, or\n`pip install pymmcore-plus` and "
        "run `mmcore build-dev`."
    )


@pytest.fixture
def pm(mm_lib_dir: str) -> Iterator[pmmd.PluginManager]:
    pm = pmmd.PluginManager()
    pm.SetSearchPaths([mm_lib_dir])
    yield pm


@pytest.fixture(params=DEMO_DEVICES, ids=lambda d: d.name)
def device_info(request: pytest.FixtureRequest) -> Iterator[Di]:
    yield request.param
