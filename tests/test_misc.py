import pymmdevice as pmmd


def test_version() -> None:
    assert isinstance(pmmd.DEVICE_INTERFACE_VERSION, int)
    assert pmmd.DEVICE_INTERFACE_VERSION >= 70
