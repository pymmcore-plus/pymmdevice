import os
from contextlib import suppress

import pytest

os.environ["MESONPY_EDITABLE_VERBOSE"] = "1"


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
