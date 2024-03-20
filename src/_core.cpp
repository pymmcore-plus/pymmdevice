#include <pybind11/pybind11.h>
#include <pybind11/stl.h>  // For automatic conversion between C++ and Python containers

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
  //   py::class_<DeviceInstance, std::shared_ptr<DeviceInstance>>(m, "DeviceInstance",
  //                                                               py::module_local())
  //       .def("get_adapter_module", &DeviceInstance::GetAdapterModule)
  //       .def("get_label", &DeviceInstance::GetLabel)
  //       .def("get_description", &DeviceInstance::GetDescription)
  //       .def("set_description", &DeviceInstance::SetDescription)
  //       .def("get_raw_ptr", &DeviceInstance::GetRawPtr, py::return_value_policy::reference)
  //       .def("log_message", &DeviceInstance::LogMessage)
  //       .def("is_initialized", &DeviceInstance::IsInitialized)
  //       .def("has_initialization_been_attempted",
  //       &DeviceInstance::HasInitializationBeenAttempted) .def_property_readonly("label",
  //       &DeviceInstance::GetLabel) .def_property("description", &DeviceInstance::GetDescription,
  //                     &DeviceInstance::SetDescription);

  py::class_<StateInstance, std::shared_ptr<StateInstance>>(m, "StateInstance");
  //  .def("set_position", &StateInstance::SetPosition)
  //  .def("get_position", &StateInstance::GetPosition)
  //  .def("get_position_label", &StateInstance::GetPositionLabel)
  //  .def("set_position_label", &StateInstance::SetPositionLabel)
  //  .def("get_number_of_positions", &StateInstance::GetNumberOfPositions)
  //  .def("set_gate_open", &StateInstance::SetGateOpen)
  //  .def("get_gate_open", &StateInstance::GetGateOpen);

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