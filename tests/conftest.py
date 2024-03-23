import os
from contextlib import suppress
from typing import Iterator, NamedTuple

import pytest

import pymmdevice as pmmd

os.environ["MESONPY_EDITABLE_VERBOSE"] = "1"


class D(NamedTuple):
    name: str
    type: str
    desc: str


DEMO_DEVICES = [
    D("DCam", "CameraDevice", "Demo camera"),
    D("DWheel", "StateDevice", "Demo filter wheel"),
    D("DStateDevice", "StateDevice", "Demo State Device"),
    D("DObjective", "StateDevice", "Demo objective turret"),
    D("DStage", "StageDevice", "Demo stage"),
    D("DXYStage", "XYStageDevice", "Demo XY stage"),
    D("DLightPath", "StateDevice", "Demo light path"),
    D("DAutoFocus", "AutoFocusDevice", "Demo auto focus"),
    D("DShutter", "ShutterDevice", "Demo shutter"),
    D("D-DA", "SignalIODevice", "Demo DA"),
    D("D-DA2", "SignalIODevice", "Demo DA-2"),
    D("DOptovar", "MagnifierDevice", "Demo Optovar"),
    D("DGalvo", "GalvoDevice", "Demo Galvo"),
    D("TransposeProcessor", "ImageProcessorDevice", "TransposeProcessor"),
    D("ImageFlipX", "ImageProcessorDevice", "ImageFlipX"),
    D("ImageFlipY", "ImageProcessorDevice", "ImageFlipY"),
    D("MedianFilter", "ImageProcessorDevice", "MedianFilter"),
    D("DHub", "HubDevice", "DHub"),
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
