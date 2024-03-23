BUILDDIR := $(shell ls -d build/cp3* | head -n 1)

.PHONY: build clean install test coverage stubs check

build:
	meson compile -C $(BUILDDIR)

clean:
	rm -rf build dist
	rm -rf coverage.info coverage.xml coverage_cpp.xml

install:
	make clean
	uv pip install -e . \
		--no-build-isolation \
		--force-reinstall \
		--config-settings=editable-verbose=true \
		--config-settings=setup-args="-Db_coverage=true" \
		--config-settings=compile-args="-j8"


test:
	meson test -C $(BUILDDIR) --verbose

coverage:
	rm -rf coverage coverage.xml coverage_cpp.xml
	mkdir coverage
	make test
	gcovr --xml coverage_cpp.xml
	gcovr --html-details -o coverage/index.html --exclude .venv
	open coverage/index.html


stubs:
	pybind11-stubgen pymmdevice._pymmdevice -o src
	ruff format src/pymmdevice/_pymmdevice.pyi -v
	ruff check src/pymmdevice/_pymmdevice.pyi --fix

check:
	pre-commit run --all-files --hook-stage manual