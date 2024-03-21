#include <pybind11/pybind11.h>
#include <pybind11/stl.h>  // For automatic conversion between C++ and Python containers

#include "AutoFocusInstance.h"
#include "CameraInstance.h"
#include "DeviceInstance.h"
#include "GalvoInstance.h"
#include "GenericInstance.h"
#include "HubInstance.h"
#include "ImageProcessorInstance.h"
#include "LoadedDeviceAdapter.h"
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

PYBIND11_MODULE(_pymmdevice, m) {
  bindDeviceInstance<CameraInstance>(m, "CameraInstance")
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
      .def("Initialize", &CameraInstance::Initialize);

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
      .def(
          "load_device",
          [](LoadedDeviceAdapter &self, const std::string &name, const std::string &label) {
            MockCMMCore mockCore;
            mm::logging::internal::GenericLogger<mm::logging::EntryData> deviceLogger(0);
            mm::logging::internal::GenericLogger<mm::logging::EntryData> coreLogger(0);
            return self.LoadDevice(&mockCore, name, label, deviceLogger, coreLogger);
          },
          py::arg("name"), py::arg("label"), py::return_value_policy::automatic);
}