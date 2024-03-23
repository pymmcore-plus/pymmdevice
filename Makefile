BUILDDIR := $(shell ls -d build/cp3* | head -n 1)

.PHONY: build clean install test stubs check

build:
	meson compile -C $(BUILDDIR)

clean:
	rm -rf build dist
	rm -rf coverage.info coverage.xml

install:
	make clean
	uv pip install -e . --no-build-isolation --force-reinstall

test:
	meson test -C $(BUILDDIR) --verbose
	lcov --directory $(BUILDDIR) --capture --output-file coverage.info

stubs:
	pybind11-stubgen pymmdevice._pymmdevice -o src
	ruff format src/pymmdevice/_pymmdevice.pyi -v
	ruff check src/pymmdevice/_pymmdevice.pyi --fix

check:
	pre-commit run --all-files --hook-stage manual