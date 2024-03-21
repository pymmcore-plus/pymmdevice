#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>  // For automatic conversion between C++ and Python containers

#include <iostream>

#include "AutoFocusInstance.h"
#include "CameraInstance.h"
#include "DeviceInstance.h"
#include "GalvoInstance.h"
#include "GenericInstance.h"
#include "HubInstance.h"
#include "ImageProcessorInstance.h"
#include "LoadableModules/LoadedDeviceAdapter.h"
#include "MMCore.h"
#include "MagnifierInstance.h"
#include "PluginManager.h"
#include "SLMInstance.h"
#include "SerialInstance.h"
#include "ShutterInstance.h"
#include "SignalIOInstance.h"
#include "StageInstance.h"
#include "StateInstance.h"
#include "XYStageInstance.h"

namespace py = pybind11;

// Minimal mock class
class MockCMMCore : public CMMCore {
 public:
  // Implement only the necessary methods, no-ops or simple returns
  // You might not need to implement anything if LoadDevice truly does nothing with it
};

// Define a holder type for DeviceInstance
using DeviceInstanceHolder = std::shared_ptr<DeviceInstance>;

template <typename DeviceInstanceType>
py::class_<DeviceInstanceType, std::shared_ptr<DeviceInstanceType>> bindDeviceInstance(
    py::module_ &m, const std::string &pythonClassName) {
  return py::class_<DeviceInstanceType, std::shared_ptr<DeviceInstanceType>>(
             m, pythonClassName.c_str())
      .def(py::init([](MockCMMCore *core, std::shared_ptr<LoadedDeviceAdapter> adapter,
                       const std::string &name, MM::Device *pDevice,
                       DeleteDeviceFunction deleteFunction, const std::string &label,
                       mm::logging::Logger deviceLogger, mm::logging::Logger coreLogger) {
        return new DeviceInstanceType(core, adapter, name, pDevice, deleteFunction, label,
                                      deviceLogger, coreLogger);
      }));
}

// Standalone function to replace the lambda
auto loadDeviceFunction = [](LoadedDeviceAdapter &self, const std::string &name,
                             const std::string &label) -> std::shared_ptr<DeviceInstance> {
  MockCMMCore mockCore;
  mm::logging::internal::GenericLogger<mm::logging::EntryData> deviceLogger(0);
  mm::logging::internal::GenericLogger<mm::logging::EntryData> coreLogger(0);
  return self.LoadDevice(&mockCore, name, label, deviceLogger, coreLogger);
};

PYBIND11_MODULE(_pymmdevice, m) {
  bindDeviceInstance<CameraInstance>(m, "CameraInstance")
      .def("__enter__",
           [](CameraInstance &self) -> CameraInstance & {
             self.Initialize();
             return self;
           })
      .def("__exit__", [](CameraInstance &self, py::args args) -> void { self.Shutdown(); })
      .def("SnapImage", &CameraInstance::SnapImage)
      .def("GetNumberOfComponents", &CameraInstance::GetNumberOfComponents)
      .def("GetComponentName", &CameraInstance::GetComponentName)
      .def("GetNumberOfChannels", &CameraInstance::GetNumberOfChannels)
      .def("GetChannelName", &CameraInstance::GetChannelName)
      .def("GetImageBufferSize", &CameraInstance::GetImageBufferSize)
      .def("GetImageWidth", &CameraInstance::GetImageWidth)
      .def("GetImageHeight", &CameraInstance::GetImageHeight)
      .def("GetImageBytesPerPixel", &CameraInstance::GetImageBytesPerPixel)
      .def("GetBitDepth", &CameraInstance::GetBitDepth)
      .def("GetPixelSizeUm", &CameraInstance::GetPixelSizeUm)
      .def("GetBinning", &CameraInstance::GetBinning)
      .def("SetBinning", &CameraInstance::SetBinning)
      .def("SetExposure", &CameraInstance::SetExposure)
      .def("GetExposure", &CameraInstance::GetExposure)
      .def("Initialize", &CameraInstance::Initialize)
      .def("Shutdown", &CameraInstance::Shutdown)
      .def("GetImageBuffer", static_cast<const unsigned char *(CameraInstance::*)()>(
                                 &CameraInstance::GetImageBuffer))
      .def("GetImageBuffer", static_cast<const unsigned char *(CameraInstance::*)(unsigned)>(
                                 &CameraInstance::GetImageBuffer))
      .def("GetImageArray",
           [](CameraInstance &self, py::args args) {
             // Infer the shape and type of the numpy array from the camera instance
             unsigned height = self.GetImageHeight();
             unsigned width = self.GetImageWidth();
             unsigned bytesPerPixel = self.GetImageBytesPerPixel();
             py::dtype dtype;
             switch (bytesPerPixel) {
               case 1:
                 dtype = py::dtype::of<uint8_t>();
                 break;
               case 2:
                 dtype = py::dtype::of<uint16_t>();
                 break;
               case 4:
                 dtype = py::dtype::of<uint32_t>();
                 break;
               case 8:
                 dtype = py::dtype::of<uint64_t>();
                 break;
               default:
                 throw std::runtime_error("Unsupported bytes per pixel");
             }

             // Assuming the buffer size is height * width * bytesPerPixel
             size_t size = height * width * bytesPerPixel;
             const unsigned char *buffer = args.size() == 0
                                               ? self.GetImageBuffer()
                                               : self.GetImageBuffer(args[0].cast<unsigned>());

             // Ensure shape and strides are explicitly defined as vectors
             std::vector<ssize_t> shape = {static_cast<ssize_t>(height),
                                           static_cast<ssize_t>(width)};
             std::vector<ssize_t> strides = {static_cast<ssize_t>(width * bytesPerPixel),
                                             static_cast<ssize_t>(bytesPerPixel)};

             return py::array(dtype, shape, strides, buffer);

             // Create a NumPy array that shares the buffer without copying
             // return py::array(dtype, {height, width}, {width * bytesPerPixel, bytesPerPixel},
             // buffer);
           })
      .def("SnapImage", &CameraInstance::SnapImage);

  bindDeviceInstance<ShutterInstance>(m, "ShutterInstance");
  bindDeviceInstance<StageInstance>(m, "StageInstance");
  bindDeviceInstance<XYStageInstance>(m, "XYStageInstance");
  bindDeviceInstance<StateInstance>(m, "StateInstance");
  bindDeviceInstance<SerialInstance>(m, "SerialInstance");
  bindDeviceInstance<GenericInstance>(m, "GenericInstance");
  bindDeviceInstance<AutoFocusInstance>(m, "AutoFocusInstance");
  bindDeviceInstance<ImageProcessorInstance>(m, "ImageProcessorInstance");
  bindDeviceInstance<SignalIOInstance>(m, "SignalIOInstance");
  bindDeviceInstance<MagnifierInstance>(m, "MagnifierInstance");
  bindDeviceInstance<SLMInstance>(m, "SLMInstance");
  bindDeviceInstance<GalvoInstance>(m, "GalvoInstance");
  bindDeviceInstance<HubInstance>(m, "HubInstance");

  py::class_<CPluginManager>(m, "CPluginManager")
      .def(py::init())
      .def(
          "SetSearchPaths",
          [](CPluginManager &self, py::iterable paths) {
            std::vector<std::string> searchPaths;
            for (py::handle path : paths) {
              searchPaths.push_back(path.cast<std::string>());
            }
            self.SetSearchPaths(searchPaths.begin(), searchPaths.end());
          },
          py::arg("paths"))
      .def("GetDeviceAdapter",
           static_cast<std::shared_ptr<LoadedDeviceAdapter> (CPluginManager::*)(
               const std::string &)>(&CPluginManager::GetDeviceAdapter),
           py::arg("moduleName"));

  py::class_<LoadedDeviceAdapter, std::shared_ptr<LoadedDeviceAdapter>>(m, "LoadedDeviceAdapter")
      .def(py::init<const std::string &, const std::string &>())
      .def("get_name", &LoadedDeviceAdapter::GetName)
      .def("get_available_device_names", &LoadedDeviceAdapter::GetAvailableDeviceNames)
      .def("get_device_description",
           static_cast<std::string (LoadedDeviceAdapter::*)(const std::string &) const>(
               &LoadedDeviceAdapter::GetDeviceDescription),
           py::arg("deviceName"))
      .def("load_device", loadDeviceFunction, py::arg("name"), py::arg("label"),
           py::return_value_policy::automatic)
      // the following methods simply call load_device internally, but ensure
      // that the return type is the correct DeviceInstance subclass
      .def(
          "load_camera",
          [](LoadedDeviceAdapter &self, const std::string &name,
             const std::string &label) -> std::shared_ptr<CameraInstance> {
            auto device = loadDeviceFunction(self, name, label);
            std::shared_ptr<DeviceInstance> deviceHolder(device);
            std::shared_ptr<CameraInstance> camera =
                std::dynamic_pointer_cast<CameraInstance>(deviceHolder);
            if (!camera) {
              std::ostringstream msg;
              msg << "'" << name << "' is not a CameraInstance";
              PyErr_SetString(PyExc_TypeError, msg.str().c_str());
              throw py::error_already_set();
            }
            return camera;
          },
          py::arg("name"), py::arg("label"), py::return_value_policy::automatic);
}
