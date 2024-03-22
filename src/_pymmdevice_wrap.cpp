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

class StubDeviceInstance {};

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

py::array bufferToNumpy(const unsigned char *buffer, unsigned int height, unsigned int width,
                        unsigned int bytesPerPixel) {
  py::dtype dtype;
  if (bytesPerPixel == 1) {
    dtype = py::dtype::of<uint8_t>();
  } else if (bytesPerPixel == 2) {
    dtype = py::dtype::of<uint16_t>();
  } else if (bytesPerPixel == 4) {
    dtype = py::dtype::of<uint32_t>();
  } else if (bytesPerPixel == 8) {
    dtype = py::dtype::of<uint64_t>();
  } else {
    throw std::runtime_error("Unsupported bytes per pixel: " + std::to_string(bytesPerPixel));
  }

  // Assuming the buffer size is height * width * bytesPerPixel
  // Ensure shape and strides are explicitly defined as vectors
  std::vector<ssize_t> shape = {static_cast<ssize_t>(height), static_cast<ssize_t>(width)};
  std::vector<ssize_t> strides = {static_cast<ssize_t>(width * bytesPerPixel),
                                  static_cast<ssize_t>(bytesPerPixel)};

  return py::array(dtype, shape, strides, buffer);
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
  // TODO: these are simply here for pybind11-stubgen ... but the don't work
  py::class_<MM::Device>(m, "Device");
  py::class_<MockCMMCore>(m, "MockCMMCore");
  py::class_<StubDeviceInstance>(m, "DeviceInstance");
  py::class_<mm::logging::Logger>(m, "Logger");
  py::class_<DeleteDeviceFunction>(m, "Callable");

  // Various DeviceInstance subclasses

  bindDeviceInstance<CameraInstance>(m, "CameraInstance")
      .def("__enter__",
           [](CameraInstance &self) -> CameraInstance & {
             self.Initialize();
             return self;
           })
      .def("__exit__", [](CameraInstance &self, py::args args) -> void { self.Shutdown(); })
      .def("AddTag", &CameraInstance::AddTag)
      .def("AddToExposureSequence", &CameraInstance::AddToExposureSequence)
      .def("ClearExposureSequence", &CameraInstance::ClearExposureSequence)
      .def("ClearROI", &CameraInstance::ClearROI)
      .def("GetBinning", &CameraInstance::GetBinning)
      .def("GetBitDepth", &CameraInstance::GetBitDepth)
      .def("GetChannelName", &CameraInstance::GetChannelName)
      .def("GetComponentName", &CameraInstance::GetComponentName)
      .def("GetExposure", &CameraInstance::GetExposure)
      .def("GetExposureSequenceMaxLength", &CameraInstance::GetExposureSequenceMaxLength)
      .def("GetImageBuffer", static_cast<const unsigned char *(CameraInstance::*)()>(
                                 &CameraInstance::GetImageBuffer))
      .def("GetImageBuffer", static_cast<const unsigned char *(CameraInstance::*)(unsigned)>(
                                 &CameraInstance::GetImageBuffer))
      .def(
          "GetImageArray",
          [](CameraInstance &self, unsigned arg) {
            return bufferToNumpy(self.GetImageBuffer(arg), self.GetImageHeight(),
                                 self.GetImageWidth(), self.GetImageBytesPerPixel());
          },
          py::arg("arg") = 0)
      .def("GetImageBufferAsRGB32", &CameraInstance::GetImageBufferAsRGB32)
      .def("GetImageBufferSize", &CameraInstance::GetImageBufferSize)
      .def("GetImageBytesPerPixel", &CameraInstance::GetImageBytesPerPixel)
      .def("GetImageHeight", &CameraInstance::GetImageHeight)
      .def("GetImageWidth", &CameraInstance::GetImageWidth)
      .def("GetMultiROI", &CameraInstance::GetMultiROI)
      .def("GetMultiROICount", &CameraInstance::GetMultiROICount)
      .def("GetNumberOfChannels", &CameraInstance::GetNumberOfChannels)
      .def("GetNumberOfComponents", &CameraInstance::GetNumberOfComponents)
      .def("GetPixelSizeUm", &CameraInstance::GetPixelSizeUm)
      .def("GetROI",
           [](CameraInstance &self) {
             unsigned x, y, xSize, ySize;
             self.GetROI(x, y, xSize, ySize);
             return std::make_tuple(x, y, xSize, ySize);
           })
      .def("GetTags", &CameraInstance::GetTags)
      .def("Initialize", &CameraInstance::Initialize)
      .def("IsCapturing", &CameraInstance::IsCapturing)
      .def("IsExposureSequenceable", &CameraInstance::IsExposureSequenceable)
      .def("IsMultiROISet", &CameraInstance::IsMultiROISet)
      .def("RemoveTag", &CameraInstance::RemoveTag)
      .def("SendExposureSequence", &CameraInstance::SendExposureSequence)
      .def("SetBinning", &CameraInstance::SetBinning)
      .def("SetExposure", &CameraInstance::SetExposure)
      .def("SetMultiROI", &CameraInstance::SetMultiROI)
      .def("SetROI", &CameraInstance::SetROI)
      .def("Shutdown", &CameraInstance::Shutdown)
      .def("SnapImage", &CameraInstance::SnapImage)
      .def("StartExposureSequence", &CameraInstance::StartExposureSequence)
      .def("StopExposureSequence", &CameraInstance::StopExposureSequence)
      .def("StopSequenceAcquisition", &CameraInstance::StopSequenceAcquisition)
      .def("SupportsMultiROI", &CameraInstance::SupportsMultiROI)
      .def("StartSequenceAcquisition", static_cast<int (CameraInstance::*)(long, double, bool)>(
                                           &CameraInstance::StartSequenceAcquisition))
      .def("StartSequenceAcquisition", static_cast<int (CameraInstance::*)(double)>(
                                           &CameraInstance::StartSequenceAcquisition));

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

  // PluginManager

  py::class_<CPluginManager>(m, "PluginManager")
      .def(py::init())
      .def("GetSearchPaths", &CPluginManager::GetSearchPaths)
      .def(
          "SetSearchPaths",
          [](CPluginManager &self, py::iterable paths) {
            std::vector<std::string> searchPaths;
            std::string env_path = getenv("PATH");
            for (py::handle path : paths) {
              // expand user and resolve path
              py::object os_path = py::module::import("os.path");
              path = os_path.attr("realpath")(os_path.attr("expanduser")(path));
              std::string path_str = path.cast<std::string>();
              searchPaths.push_back(path_str);

              // add path to PATH environment variable if it's not already there
              if (env_path.find(path_str) == std::string::npos) {
                env_path = path_str + ":" + env_path;
              }
            }
            self.SetSearchPaths(searchPaths.begin(), searchPaths.end());
            // update PATH environment variable to include new paths
            setenv("PATH", env_path.c_str(), 1);
          },
          py::arg("paths"))
      .def("GetAvailableDeviceAdapters", &CPluginManager::GetAvailableDeviceAdapters)
      .def("GetDeviceAdapter",
           static_cast<std::shared_ptr<LoadedDeviceAdapter> (CPluginManager::*)(
               const std::string &)>(&CPluginManager::GetDeviceAdapter),
           py::arg("moduleName"))
      .def("UnloadPluginLibrary", &CPluginManager::UnloadPluginLibrary, py::arg("moduleName"));

  // DeviceAdapter (a.k.a. LoadedDeviceAdapter)

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
