from __future__ import annotations

from typing import TYPE_CHECKING

if TYPE_CHECKING:
    import pymmdevice as pmmd


def test_demo_stage(pm: pmmd.PluginManager) -> None:
    module = pm.GetDeviceAdapter("DemoCamera")

    module.load_device("DStage", "MyCamera")
