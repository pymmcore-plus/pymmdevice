project(
    'pymmdevice',
    'cpp',
    version: '0.1.0',
    license: 'BSD',
    meson_version: '>= 1.1.0',
    default_options: [
        'cpp_std=c++11',
    ],
)

py = import('python').find_installation(pure: false)
pybind11_dep = dependency('pybind11')

py.extension_module('_core',
    'src/main.cpp',
    subdir: 'pymmdevice',
    install: true,
    dependencies : [pybind11_dep],
)

install_subdir('src/pymmdevice', install_dir: py.get_install_dir() / 'pymmdevice', strip_directory: true)
