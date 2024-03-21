#include <pybind11/functional.h>  // For std::function bindings
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>  // For automatic conversion between C++ and Python containers

#include "CameraInstance.h"
#include "DeviceInstance.h"
#include "LoadedDeviceAdapter.h"
#include "MMCore.h"
#include "StateInstance.h"
namespace py = pybind11;

// Minimal mock class
class MockCMMCore : public CMMCore {
 public:
  // Implement only the necessary methods, no-ops or simple returns
  // You might not need to implement anything if LoadDevice truly does nothing with it
};

PYBIND11_MODULE(_core, m) {
  // py::class_<DeviceInstance, std::shared_ptr<DeviceInstance>>(m, "DeviceInstance");

  // Bind the CameraInstance class
  py::class_<CameraInstance, std::shared_ptr<CameraInstance>>(m, "CameraInstance")
      .def(py::init([](MockCMMCore *core, std::shared_ptr<LoadedDeviceAdapter> adapter,
                       const std::string &name, MM::Device *pDevice,
                       DeleteDeviceFunction deleteFunction, const std::string &label,
                       mm::logging::Logger deviceLogger, mm::logging::Logger coreLogger) {
             // This lambda is just a direct pass-through to the constructor.
             return new CameraInstance(core, adapter, name, pDevice, deleteFunction, label,
                                       deviceLogger, coreLogger);
           }),
           py::arg("core"), py::arg("adapter"), py::arg("name"), py::arg("pDevice"),
           py::arg("deleteFunction"), py::arg("label"), py::arg("deviceLogger"),
           py::arg("coreLogger"))
      .def("SnapImage", &CameraInstance::SnapImage)
      // .def("GetImageBuffer",
      //      [](const CameraInstance &self) {
      //        return py::bytes(reinterpret_cast<const char *>(self.GetImageBuffer()),
      //                         self.GetImageBufferSize());
      //      })
      // .def("GetImageBuffer",
      //      [](const CameraInstance &self, unsigned channelNr) {
      //        return py::bytes(reinterpret_cast<const char *>(self.GetImageBuffer(channelNr)),
      //                         self.GetImageBufferSize());
      //      })
      // .def("GetImageBufferAsRGB32",
      //      [](const CameraInstance &self) {
      //        return py::bytes(reinterpret_cast<const char *>(self.GetImageBufferAsRGB32()),
      //                         self.GetImageBufferSize());
      //      })
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