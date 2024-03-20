project(
    'pymmdevice',
    'cpp',
    version: '0.1.0',
    license: 'BSD',
    meson_version: '>= 1.1.0',
    default_options: [
        'cpp_std=c++14',
    ],
)

py = import('python').find_installation(pure: false)
pybind11_dep = dependency('pybind11')

# Execute the Python script to get the list of include directories
print_include_dirs = '''
import pathlib
m = pathlib.Path('extern/mmCoreAndDevices')
headers = (*(m/'MMDevice').rglob('*.h'), *(m/'MMCore').rglob('*.h'))
print(';'.join({str(x.parent) for x in headers}))
'''
include_dirs_list = run_command(py.full_path(), '-c', print_include_dirs, check: true).stdout().strip().split(';')
include_dirs = include_directories(include_dirs_list, is_system: true)
foreach dir : include_dirs_list
    message('Including: ' + dir)
endforeach

# all necessary CPP files
cpp_sources = ['src/_core.cpp']
print_sources = '''
import pathlib
m = pathlib.Path('extern/mmCoreAndDevices')
sources = (*(m/'MMDevice').rglob('*.cpp'), *(m/'MMCore').rglob('*.cpp'))
print(';'.join({str(x) for x in sources if 'unittest' not in str(x)}))
'''
cpp_sources += run_command(py.full_path(), '-c', print_sources, check: true).stdout().strip().split(';')



py.extension_module('_core',
    files(cpp_sources),
    subdir: 'pymmdevice',
    install: true,
    dependencies : [pybind11_dep],
    include_directories: include_dirs,
    cpp_args: ['-DMMDEVICE_CLIENT_BUILD'],
)

# install the Python package into the site-packages directory
install_subdir('src/pymmdevice', install_dir: py.get_install_dir() / 'pymmdevice', strip_directory: true)

