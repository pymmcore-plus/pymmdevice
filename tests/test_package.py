from __future__ import annotations

import importlib.metadata

import pkg as m


def test_version():
    assert importlib.metadata.version("pkg") == m.__version__
