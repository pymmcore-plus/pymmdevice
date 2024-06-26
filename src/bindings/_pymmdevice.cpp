#include <pybind11/pybind11.h>
#include <pybind11/stl.h>  // For automatic conversion between C++ and Python containers

#include <iostream>

#include "AutoFocusInstance.h"
#include "CameraInstance.h"
#include "CoreCallback.h"
#include "CoreUtils.h"
#include "DeviceInstance.h"
#include "DeviceManager.h"
#include "GalvoInstance.h"
#include "GenericInstance.h"
#include "HubInstance.h"
#include "ImageProcessorInstance.h"
#include "LoadableModules/LoadedDeviceAdapter.h"
#include "MMCore.h"
#include "MMDeviceConstants.h"
#include "MagnifierInstance.h"
#include "PluginManager.h"
#include "SLMInstance.h"
#include "SerialInstance.h"
#include "ShutterInstance.h"
#include "SignalIOInstance.h"
#include "StageInstance.h"
#include "StateInstance.h"
#include "XYStageInstance.h"
#include "utils.h"

namespace py = pybind11;
using namespace pybind11::literals;

// Minimal mock class
class MockCMMCore : public CMMCore {
 public:
  // Implement only the necessary methods, no-ops or simple returns
  // You might not need to implement anything if LoadDevice truly does nothing with it
};

class PyDeviceInstance {};

template <typename DType>
py::class_<DType, std::shared_ptr<DType>> bindDeviceInstance(py::module_ &m,
                                                             const std::string &className) {
  auto cls = py::class_<DType, std::shared_ptr<DType>>(m, className.c_str());

  cls.def(py::init([](MockCMMCore *core, std::shared_ptr<LoadedDeviceAdapter> adapter,
                      const std::string &name, MM::Device *pDevice,
                      DeleteDeviceFunction deleteFunction, const std::string &label,
                      mm::logging::Logger deviceLogger, mm::logging::Logger coreLogger) {
    return new DType(core, adapter, name, pDevice, deleteFunction, label, deviceLogger,
                     coreLogger);
  }));
  cls.def("__enter__", [](DType &self) -> DType & {
    self.Initialize();
    return self;
  });
  cls.def("__exit__", [](DType &self, py::args args) -> void { self.Shutdown(); });

  cls.def("AddToPropertySequence", &DeviceInstance::AddToPropertySequence);
  cls.def("Busy", &DeviceInstance::Busy);
  cls.def("ClearPropertySequence", &DeviceInstance::ClearPropertySequence);
  cls.def("DetectDevice", &DeviceInstance::DetectDevice);
  cls.def("GetAdapterModule", &DeviceInstance::GetAdapterModule);
  cls.def("GetDelayMs", &DeviceInstance::GetDelayMs);
  cls.def("GetDescription", &DeviceInstance::GetDescription);
  cls.def("GetErrorText", &DeviceInstance::GetErrorText);
  cls.def("GetLabel", &DeviceInstance::GetLabel);
  cls.def("GetName", &DeviceInstance::GetName);
  cls.def("GetNumberOfPropertyValues", &DeviceInstance::GetNumberOfPropertyValues);
  cls.def("GetParentID", &DeviceInstance::GetParentID);
  cls.def("GetProperty", &DeviceInstance::GetProperty);
  cls.def("GetPropertyInitStatus", &DeviceInstance::GetPropertyInitStatus);
  cls.def("GetPropertyLowerLimit", &DeviceInstance::GetPropertyLowerLimit);
  cls.def("GetPropertyNames", &DeviceInstance::GetPropertyNames);
  cls.def("GetPropertyReadOnly", &DeviceInstance::GetPropertyReadOnly);
  cls.def("GetPropertySequenceMaxLength", &DeviceInstance::GetPropertySequenceMaxLength);
  cls.def("GetPropertyType", &DeviceInstance::GetPropertyType);
  cls.def("GetPropertyUpperLimit", &DeviceInstance::GetPropertyUpperLimit);
  cls.def("GetPropertyValueAt", &DeviceInstance::GetPropertyValueAt);
  cls.def("GetRawPtr", &DeviceInstance::GetRawPtr);
  cls.def("GetType", &DeviceInstance::GetType);
  cls.def("HasInitializationBeenAttempted", &DeviceInstance::HasInitializationBeenAttempted);
  cls.def("HasProperty", &DeviceInstance::HasProperty);
  cls.def("HasPropertyLimits", &DeviceInstance::HasPropertyLimits);
  cls.def("Initialize", &DeviceInstance::Initialize);
  cls.def("IsInitialized", &DeviceInstance::IsInitialized);
  cls.def("IsPropertySequenceable", &DeviceInstance::IsPropertySequenceable);
  cls.def("LogMessage", &DeviceInstance::LogMessage);
  cls.def("SendPropertySequence", &DeviceInstance::SendPropertySequence);
  cls.def("SetCallback", &DeviceInstance::SetCallback);
  cls.def("SetDelayMs", &DeviceInstance::SetDelayMs);
  cls.def("SetDescription", &DeviceInstance::SetDescription);
  cls.def("SetParentID", &DeviceInstance::SetParentID);
  cls.def("SetProperty", &DeviceInstance::SetProperty);
  cls.def("Shutdown", &DeviceInstance::Shutdown);
  cls.def("StartPropertySequence", &DeviceInstance::StartPropertySequence);
  cls.def("StopPropertySequence", &DeviceInstance::StopPropertySequence);
  cls.def("SupportsDeviceDetection", &DeviceInstance::SupportsDeviceDetection);
  cls.def("UsesDelay", &DeviceInstance::UsesDelay);
  cls.def("__repr__", [className](const DType &self) {
    std::string repr = "<" + className;
    repr += " '" + self.GetLabel() + "'";
    repr += " from ";
    repr += self.GetAdapterModule()->GetName() + "[" + self.GetName() + "]";
    repr += ">";
    return repr;
  });

  return cls;
}

// Standalone function to replace the lambda

std::string getErrorMessage(DeviceInstance *device, int errorCode) {
  return "Error in device " + ToQuotedString(device->GetLabel()) + ": " +
         device->GetErrorText(errorCode) + " (" + ToString(errorCode) + ")";
}

// Just a hack for now...
// It's the CoreCallback that makes me recognize that this may never work.
// Much of the Core API is exposed to the Devices through the CoreCallback,
// So, it might not ever make sense to directly control device libraries without
// a core (even if you don't technically need it).
class PyCoreCallback : public CoreCallback {
 public:
  using CoreCallback::CoreCallback;  // Inherit constructors

  // .def("GetLoadedDeviceOfType", &PyCoreCallback::GetLoadedDeviceOfType, "caller"_a,
  //      "devType"_a, "deviceName"_a, "deviceIterator"_a);

  MM::ImageProcessor *GetImageProcessor(const MM::Device *caller) { return nullptr; }
  MM::State *GetStateDevice(const MM::Device *caller, const char *label) { return nullptr; }
  MM::SignalIO *GetSignalIODevice(const MM::Device *caller, const char *label) { return nullptr; }
  MM::AutoFocus *GetAutoFocus(const MM::Device *caller) { return nullptr; }
  MM::Hub *GetParentHub(const MM::Device *caller) const { return nullptr; }
  int LogMessage(const MM::Device *caller, const char *msg, bool debugOnly) const {
    return DEVICE_OK;
  }
};

auto loadDevice_ = [](LoadedDeviceAdapter &self, const std::string &name,
                      const std::string &label) -> std::shared_ptr<DeviceInstance> {
  MockCMMCore mockCore;
  mm::logging::internal::GenericLogger<mm::logging::EntryData> deviceLogger(0);
  mm::logging::internal::GenericLogger<mm::logging::EntryData> coreLogger(0);
  // NOTE:
  // in DeviceManager.LoadDevice, the description is taken from the module
  // and assigned to the device.  It's not immediately obvious why that shouldn't
  // also be done here...
  std::shared_ptr<DeviceInstance> dev =
      self.LoadDevice(&mockCore, name, label, deviceLogger, coreLogger);
  // PyCoreCallback *cb = new PyCoreCallback(&mockCore);
  // dev->SetCallback(cb);
  return dev;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

PYBIND11_MODULE(_pymmdevice, m) {
  // define module level attribute for DEVICE_INTERFACE_VERSION
  m.attr("DEVICE_INTERFACE_VERSION") = DEVICE_INTERFACE_VERSION;

  // TODO: these are simply here for pybind11-stubgen ... but they don't work
  py::class_<MMThreadLock, std::shared_ptr<MMThreadLock>>(m, "MMThreadLock");
  py::class_<MM::Device>(m, "Device");
  py::class_<MockCMMCore>(m, "MockCMMCore");
  py::class_<PyDeviceInstance>(m, "DeviceInstance");
  py::class_<mm::logging::Logger>(m, "Logger");
  py::class_<DeleteDeviceFunction>(m, "Callable");
  py::class_<MM::Core> core(m, "Core");

  // PyCoreCallback

  py::class_<PyCoreCallback, MM::Core>(m, "PyCoreCallback", py::dynamic_attr(),
                                       py::multiple_inheritance())
      .def(py::init<>([]() {
        CMMCore core;
        printf("CoreCallback constructor\n");
        return new PyCoreCallback(&core);
      }))
      .def("OnExposureChanged", &PyCoreCallback::OnExposureChanged)

      .def("GetDeviceProperty", &PyCoreCallback::GetDeviceProperty, "deviceName"_a, "propName"_a,
           "value"_a)
      .def("SetDeviceProperty", &PyCoreCallback::SetDeviceProperty, "deviceName"_a, "propName"_a,
           "value"_a)
      .def("LogMessage", &PyCoreCallback::LogMessage, "caller"_a, "msg"_a, "debugOnly"_a)
      .def("GetDevice", &PyCoreCallback::GetDevice, "caller"_a, "label"_a)
      // .def("GetSerialPortType", &PyCoreCallback::GetSerialPortType, "portName"_a)
      .def("SetSerialProperties", &PyCoreCallback::SetSerialProperties, "portName"_a,
           "answerTimeout"_a, "baudRate"_a, "delayBetweenCharsMs"_a, "handshaking"_a, "parity"_a,
           "stopBits"_a)
      .def("WriteToSerial", &PyCoreCallback::WriteToSerial, "caller"_a, "portName"_a, "buf"_a,
           "length"_a)
      .def("ReadFromSerial", &PyCoreCallback::ReadFromSerial, "caller"_a, "portName"_a, "buf"_a,
           "bufLength"_a, "bytesRead"_a)
      .def("PurgeSerial", &PyCoreCallback::PurgeSerial, "caller"_a, "portName"_a)
      .def("SetSerialCommand", &PyCoreCallback::SetSerialCommand, "device"_a, "portName"_a,
           "command"_a, "term"_a)
      .def("GetSerialAnswer", &PyCoreCallback::GetSerialAnswer, "device"_a, "portName"_a,
           "ansLength"_a, "answerTxt"_a, "term"_a)
      .def("GetClockTicksUs", &PyCoreCallback::GetClockTicksUs, "caller"_a)
      // .def("GetCurrentMMTime", &PyCoreCallback::GetCurrentMMTime)
      .def("Sleep", &PyCoreCallback::Sleep, "caller"_a, "intervalMs"_a)
      // .def("InsertImage"...
      // .def("InsertImage"...
      // .def("InsertImage"...
      .def("ClearImageBuffer", &PyCoreCallback::ClearImageBuffer, "caller"_a)
      .def("InitializeImageBuffer", &PyCoreCallback::InitializeImageBuffer, "channels"_a,
           "slices"_a, "w"_a, "h"_a, "pixDepth"_a)
      .def("AcqFinished", &PyCoreCallback::AcqFinished, "caller"_a, "statusCode"_a)
      .def("PrepareForAcq", &PyCoreCallback::PrepareForAcq, "caller"_a)
      .def("GetImage", &PyCoreCallback::GetImage)
      .def("GetImageDimensions", &PyCoreCallback::GetImageDimensions, "width"_a, "height"_a,
           "depth"_a)
      .def("GetFocusPosition", &PyCoreCallback::GetFocusPosition, "pos"_a)
      .def("SetFocusPosition", &PyCoreCallback::SetFocusPosition, "pos"_a)
      .def("MoveFocus", &PyCoreCallback::MoveFocus, "v"_a)
      .def("SetXYPosition", &PyCoreCallback::SetXYPosition, "x"_a, "y"_a)
      .def("GetXYPosition", &PyCoreCallback::GetXYPosition, "x"_a, "y"_a)
      .def("MoveXYStage", &PyCoreCallback::MoveXYStage, "vX"_a, "vY"_a)
      .def("SetExposure", &PyCoreCallback::SetExposure, "expMs"_a)
      .def("GetExposure", &PyCoreCallback::GetExposure, "expMs"_a)
      .def("SetConfig", &PyCoreCallback::SetConfig, "group"_a, "name"_a)
      .def("GetCurrentConfig", &PyCoreCallback::GetCurrentConfig, "group"_a, "bufLen"_a, "name"_a)
      .def("GetChannelConfig", &PyCoreCallback::GetChannelConfig, "channelConfigName"_a,
           "channelConfigIterator"_a)
      .def("OnPropertiesChanged", &PyCoreCallback::OnPropertiesChanged, "caller"_a)
      .def("OnPropertyChanged", &PyCoreCallback::OnPropertyChanged, "device"_a, "propName"_a,
           "value"_a)
      .def("OnStagePositionChanged", &PyCoreCallback::OnStagePositionChanged, "device"_a, "pos"_a)
      .def("OnXYStagePositionChanged", &PyCoreCallback::OnXYStagePositionChanged, "device"_a,
           "xpos"_a, "ypos"_a)
      .def("OnExposureChanged", &PyCoreCallback::OnExposureChanged, "device"_a, "newExposure"_a)
      .def("OnSLMExposureChanged", &PyCoreCallback::OnSLMExposureChanged, "device"_a,
           "newExposure"_a)
      .def("OnMagnifierChanged", &PyCoreCallback::OnMagnifierChanged, "device"_a)
      .def("NextPostedError", &PyCoreCallback::NextPostedError, "errorCode"_a, "pMessage"_a,
           "maxlen"_a, "messageLength"_a)
      .def("PostError", &PyCoreCallback::PostError, "errorCode"_a, "pMessage"_a)
      // .def("GetImageProcessor", &PyCoreCallback::GetImageProcessor, "caller"_a)
      // .def("GetStateDevice", &PyCoreCallback::GetStateDevice, "caller"_a, "label"_a)
      // .def("GetSignalIODevice", &PyCoreCallback::GetSignalIODevice, "caller"_a, "label"_a)
      // .def("GetAutoFocus", &PyCoreCallback::GetAutoFocus, "caller"_a)
      // .def("GetParentHub", &PyCoreCallback::GetParentHub, "caller"_a);
      // .def("GetLoadedDeviceOfType", &PyCoreCallback::GetLoadedDeviceOfType, "caller"_a,
      //      "devType"_a, "deviceName"_a, "deviceIterator"_a);
      .def("ClearPostedErrors", &PyCoreCallback::ClearPostedErrors);

  /////////////////// Enums ///////////////////

  py::enum_<MM::DeviceType>(m, "DeviceType")
      .value("UnknownType", MM::DeviceType::UnknownType)
      .value("AnyType", MM::DeviceType::AnyType)
      .value("CameraDevice", MM::DeviceType::CameraDevice)
      .value("ShutterDevice", MM::DeviceType::ShutterDevice)
      .value("StateDevice", MM::DeviceType::StateDevice)
      .value("StageDevice", MM::DeviceType::StageDevice)
      .value("XYStageDevice", MM::DeviceType::XYStageDevice)
      .value("SerialDevice", MM::DeviceType::SerialDevice)
      .value("GenericDevice", MM::DeviceType::GenericDevice)
      .value("AutoFocusDevice", MM::DeviceType::AutoFocusDevice)
      .value("CoreDevice", MM::DeviceType::CoreDevice)
      .value("ImageProcessorDevice", MM::DeviceType::ImageProcessorDevice)
      .value("SignalIODevice", MM::DeviceType::SignalIODevice)
      .value("MagnifierDevice", MM::DeviceType::MagnifierDevice)
      .value("SLMDevice", MM::DeviceType::SLMDevice)
      .value("HubDevice", MM::DeviceType::HubDevice)
      .value("GalvoDevice", MM::DeviceType::GalvoDevice);

  py::enum_<MM::DeviceDetectionStatus>(m, "DeviceDetectionStatus")
      .value("Misconfigured", MM::DeviceDetectionStatus::Misconfigured)
      .value("Unimplemented", MM::DeviceDetectionStatus::Unimplemented)
      .value("CanNotCommunicate", MM::DeviceDetectionStatus::CanNotCommunicate)
      .value("CanCommunicate", MM::DeviceDetectionStatus::CanCommunicate);

  py::enum_<MM::PropertyType>(m, "PropertyType")
      .value("Undef", MM::PropertyType::Undef)
      .value("String", MM::PropertyType::String)
      .value("Float", MM::PropertyType::Float)
      .value("Integer", MM::PropertyType::Integer);

  py::enum_<MM::FocusDirection>(m, "FocusDirection")
      .value("FocusDirectionUnknown", MM::FocusDirection::FocusDirectionUnknown)
      .value("FocusDirectionTowardSample", MM::FocusDirection::FocusDirectionTowardSample)
      .value("FocusDirectionAwayFromSample", MM::FocusDirection::FocusDirectionAwayFromSample);

  py::enum_<MM::PortType>(m, "PortType")
      .value("InvalidPort", MM::PortType::InvalidPort)
      .value("SerialPort", MM::PortType::SerialPort)
      .value("USBPort", MM::PortType::USBPort)
      .value("HIDPort", MM::PortType::HIDPort);

  //////////////////////// PluginManager ////////////////////////

  py::class_<CPluginManager>(m, "PluginManager")
      .def(py::init())
      .def("GetSearchPaths", &CPluginManager::GetSearchPaths)
      .def(
          "SetSearchPaths",
          [](CPluginManager &self, py::iterable paths) {
            std::vector<std::string> searchPaths;
            std::string env_path = getenv("PATH");
            for (py::handle path : paths) {
              std::string path_str = util::resolvePath(path);
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
          "paths"_a)
      .def("GetAvailableDeviceAdapters", &CPluginManager::GetAvailableDeviceAdapters)
      .def("GetDeviceAdapter",
           static_cast<std::shared_ptr<LoadedDeviceAdapter> (CPluginManager::*)(
               const std::string &)>(&CPluginManager::GetDeviceAdapter),
           "moduleName"_a)
      .def("UnloadPluginLibrary", &CPluginManager::UnloadPluginLibrary, "moduleName"_a);

  ////////////////////// DeviceManager //////////////////////

  py::class_<mm::DeviceManager, std::shared_ptr<mm::DeviceManager>>(m, "DeviceManager")
      .def(py::init<>())
      .def("__enter__", [](mm::DeviceManager &self) -> mm::DeviceManager & { return self; })
      .def("__exit__",
           [](mm::DeviceManager &self, py::args args) -> void { self.UnloadAllDevices(); })
      .def(
          "LoadDevice",
          [](mm::DeviceManager &self, std::shared_ptr<LoadedDeviceAdapter> module,
             const std::string &deviceName,
             const std::string &label) -> std::shared_ptr<DeviceInstance> {
            MockCMMCore mockCore;
            mm::logging::internal::GenericLogger<mm::logging::EntryData> deviceLogger(0);
            mm::logging::internal::GenericLogger<mm::logging::EntryData> coreLogger(0);
            return self.LoadDevice(module, deviceName, label, &mockCore, deviceLogger, coreLogger);
          },
          "module"_a, "deviceName"_a, "label"_a, py::return_value_policy::automatic,
          "Load the specified device and assign a device label.")
      .def("UnloadDevice", &mm::DeviceManager::UnloadDevice, "device"_a, "Unload a device.")
      .def("UnloadAllDevices", &mm::DeviceManager::UnloadAllDevices, "Unload all devices.")
      .def("GetDevice",
           (std::shared_ptr<DeviceInstance>(mm::DeviceManager::*)(const char *) const) &
               mm::DeviceManager::GetDevice,
           "label"_a, "Get a device by label.")
      // .def("GetDevice",
      //      (std::shared_ptr<DeviceInstance>(mm::DeviceManager::*)(const MM::Device *) const) &
      //          mm::DeviceManager::GetDevice,
      //      "rawPtr"_a, "Get a device from a raw pointer to its MMDevice object.")
      .def("GetCameraDevice",
           (std::shared_ptr<CameraInstance>(mm::DeviceManager::*)(std::shared_ptr<DeviceInstance>)
                const) &
               mm::DeviceManager::GetDeviceOfType<CameraInstance>,
           "device"_a, "Get a device by label, requiring a specific type.")
      .def("GetStageDevice",
           (std::shared_ptr<StageInstance>(mm::DeviceManager::*)(std::shared_ptr<DeviceInstance>)
                const) &
               mm::DeviceManager::GetDeviceOfType<StageInstance>,
           "device"_a, "Get a device by label, requiring a specific type.")
      .def(
          "GetDeviceOfType",
          [](const mm::DeviceManager &self, const std::string &label,
             MM::DeviceType device_type) -> std::shared_ptr<DeviceInstance> {
            switch (device_type) {
              case MM::DeviceType::CameraDevice:
                return self.GetDeviceOfType<CameraInstance>(label);
              case MM::DeviceType::StageDevice:
                return self.GetDeviceOfType<StageInstance>(label);
              case MM::DeviceType::XYStageDevice:
                return self.GetDeviceOfType<XYStageInstance>(label);
              case MM::DeviceType::ShutterDevice:
                return self.GetDeviceOfType<ShutterInstance>(label);
              case MM::DeviceType::StateDevice:
                return self.GetDeviceOfType<StateInstance>(label);
              case MM::DeviceType::SerialDevice:
                return self.GetDeviceOfType<SerialInstance>(label);
              case MM::DeviceType::GenericDevice:
                return self.GetDeviceOfType<GenericInstance>(label);
              case MM::DeviceType::AutoFocusDevice:
                return self.GetDeviceOfType<AutoFocusInstance>(label);
              case MM::DeviceType::ImageProcessorDevice:
                return self.GetDeviceOfType<ImageProcessorInstance>(label);
              case MM::DeviceType::SignalIODevice:
                return self.GetDeviceOfType<SignalIOInstance>(label);
              case MM::DeviceType::MagnifierDevice:
                return self.GetDeviceOfType<MagnifierInstance>(label);
              case MM::DeviceType::SLMDevice:
                return self.GetDeviceOfType<SLMInstance>(label);
              case MM::DeviceType::HubDevice:
                return self.GetDeviceOfType<HubInstance>(label);
              case MM::DeviceType::GalvoDevice:
                return self.GetDeviceOfType<GalvoInstance>(label);

              default:
                throw std::runtime_error("Unsupported device type");
            }
          },
          "label"_a, "device_type"_a, "Get a device by label, requiring a specific type.")

      .def("GetDeviceList", &mm::DeviceManager::GetDeviceList, "t"_a = MM::DeviceType::AnyType,
           "Get the labels of all loaded devices of a given type.")
      .def("GetLoadedPeripherals", &mm::DeviceManager::GetLoadedPeripherals, "hubLabel"_a,
           "Get the labels of all loaded peripherals of a hub device.")
      // FIXME: stupid dumb dumb workaround for fact that we don't have real subclasses yet.
      .def("GetParentDevice",
           [](mm::DeviceManager &self, std::shared_ptr<StageInstance> device) {
             return self.GetParentDevice(device);
           })
      .def("GetParentDevice",
           [](mm::DeviceManager &self, std::shared_ptr<HubInstance> device) {
             return self.GetParentDevice(device);
           })
      .def("GetParentDevice",
           [](mm::DeviceManager &self, std::shared_ptr<CameraInstance> device) {
             return self.GetParentDevice(device);
           })
      .def("GetParentDevice",
           [](mm::DeviceManager &self, std::shared_ptr<XYStageInstance> device) {
             return self.GetParentDevice(device);
           })
      .def("GetParentDevice",
           [](mm::DeviceManager &self, std::shared_ptr<ImageProcessorInstance> device) {
             return self.GetParentDevice(device);
           })
      .def("GetParentDevice",
           [](mm::DeviceManager &self, std::shared_ptr<MagnifierInstance> device) {
             return self.GetParentDevice(device);
           })
      .def("GetParentDevice",
           [](mm::DeviceManager &self, std::shared_ptr<SignalIOInstance> device) {
             return self.GetParentDevice(device);
           })
      .def("GetParentDevice",
           [](mm::DeviceManager &self, std::shared_ptr<ShutterInstance> device) {
             return self.GetParentDevice(device);
           })
      .def("GetParentDevice",
           [](mm::DeviceManager &self, std::shared_ptr<AutoFocusInstance> device) {
             return self.GetParentDevice(device);
           })
      .def("GetParentDevice",
           [](mm::DeviceManager &self, std::shared_ptr<StateInstance> device) {
             return self.GetParentDevice(device);
           })
      .def("GetParentDevice", [](mm::DeviceManager &self, std::shared_ptr<GalvoInstance> device) {
        return self.GetParentDevice(device);
      });

  ////////////////////// DeviceAdapter (a.k.a. LoadedDeviceAdapter) //////////////////////

  py::class_<LoadedDeviceAdapter, std::shared_ptr<LoadedDeviceAdapter>>(m, "LoadedDeviceAdapter")
      .def(py::init<const std::string &, const std::string &>())
      .def("__repr__",
           [](const LoadedDeviceAdapter &self) {
             return "<LoadedDeviceAdapter '" + self.GetName() + "'>";
           })
      // @classmethod from_file
      .def_static(
          "from_file",
          [](const std::string &filename,
             const std::string &name) -> std::shared_ptr<LoadedDeviceAdapter> {
            // if name is empty, use the filename without the libmmgr_dal_ prefix and without a
            // suffix
            std::string processedName;
            if (name.empty()) {
              std::string prefix = "libmmgr_dal_";
              processedName = filename;

              // Split on the last file path separator and take the last name
              size_t pos1 = processedName.find_last_of("/\\");
              if (pos1 != std::string::npos) {
                processedName = processedName.substr(pos1 + 1);
              }

              // Remove prefix if it exists
              if (processedName.find(prefix) == 0) {
                processedName = processedName.substr(prefix.size());
              }

              // Split on the first period and take everything to the left
              size_t pos = processedName.find(".");
              if (pos != std::string::npos) {
                processedName = processedName.substr(0, pos);
              }
            } else {
              processedName = name;
            }
            std::cout << "processedName: " << processedName << std::endl;
            return std::make_shared<LoadedDeviceAdapter>(processedName, filename);
          },
          "filename"_a, "moduleName"_a = std::string())
      .def("Unload", &LoadedDeviceAdapter::Unload)
      .def("GetName", &LoadedDeviceAdapter::GetName)
      .def("GetLock", &LoadedDeviceAdapter::GetLock, py::return_value_policy::reference)
      .def("GetAvailableDeviceNames", &LoadedDeviceAdapter::GetAvailableDeviceNames)
      .def("GetAdvertisedDeviceType", &LoadedDeviceAdapter::GetAdvertisedDeviceType,
           "deviceName"_a)
      .def("GetDeviceDescription",
           static_cast<std::string (LoadedDeviceAdapter::*)(const std::string &) const>(
               &LoadedDeviceAdapter::GetDeviceDescription),
           "deviceName"_a)
      .def("LoadDevice", loadDevice_, "name"_a, "label"_a, py::return_value_policy::automatic)
      // the following methods simply call LoadDevice internally, but ensure
      // that the return type is the correct DeviceInstance subclass
      .def(
          "load_camera",
          [](LoadedDeviceAdapter &self, const std::string &name,
             const std::string &label) -> std::shared_ptr<CameraInstance> {
            auto device = loadDevice_(self, name, label);
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
          "name"_a, "label"_a, py::return_value_policy::automatic);

  // Various DeviceInstance subclasses

  /////////////////////// CameraInstance ///////////////////////

  bindDeviceInstance<CameraInstance>(m, "CameraInstance")
      .def("SnapImage", &CameraInstance::SnapImage)
      .def("GetImageBufferAsRGB32", &CameraInstance::GetImageBufferAsRGB32)
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
      .def("SetROI", &CameraInstance::SetROI)
      .def("GetROI",
           [](CameraInstance &self) {
             unsigned x, y, xSize, ySize;
             self.GetROI(x, y, xSize, ySize);
             return std::make_tuple(x, y, xSize, ySize);
           })
      .def("ClearROI", &CameraInstance::ClearROI)
      .def("SupportsMultiROI", &CameraInstance::SupportsMultiROI)
      .def("IsMultiROISet", &CameraInstance::IsMultiROISet)
      .def("GetMultiROICount", &CameraInstance::GetMultiROICount)
      .def("SetMultiROI", &CameraInstance::SetMultiROI)
      .def("GetMultiROI", &CameraInstance::GetMultiROI)
      .def("StartSequenceAcquisition", static_cast<int (CameraInstance::*)(long, double, bool)>(
                                           &CameraInstance::StartSequenceAcquisition))
      .def("StartSequenceAcquisition",
           static_cast<int (CameraInstance::*)(double)>(&CameraInstance::StartSequenceAcquisition))
      .def("StopSequenceAcquisition", &CameraInstance::StopSequenceAcquisition)
      .def("PrepareSequenceAcquisition", &CameraInstance::PrepareSequenceAcqusition)
      .def("IsCapturing", &CameraInstance::IsCapturing)
      .def("GetTags", &CameraInstance::GetTags)
      .def("AddTag", &CameraInstance::AddTag)
      .def("RemoveTag", &CameraInstance::RemoveTag)
      .def("IsExposureSequenceable", &CameraInstance::IsExposureSequenceable)
      .def("GetExposureSequenceMaxLength", &CameraInstance::GetExposureSequenceMaxLength)
      .def("StartExposureSequence", &CameraInstance::StartExposureSequence)
      .def("StopExposureSequence", &CameraInstance::StopExposureSequence)
      .def("ClearExposureSequence", &CameraInstance::ClearExposureSequence)
      .def("AddToExposureSequence", &CameraInstance::AddToExposureSequence)
      .def("SendExposureSequence", &CameraInstance::SendExposureSequence)

      .def("GetImageBuffer", static_cast<const unsigned char *(CameraInstance::*)()>(
                                 &CameraInstance::GetImageBuffer))
      .def("GetImageBuffer", static_cast<const unsigned char *(CameraInstance::*)(unsigned)>(
                                 &CameraInstance::GetImageBuffer))
      .def(
          "GetImageArray",
          [](CameraInstance &self, unsigned arg) {
            return util::bufferToNumpy(self.GetImageBuffer(arg), self.GetImageHeight(),
                                       self.GetImageWidth(), self.GetImageBytesPerPixel());
          },
          "arg"_a = 0);

  /////////////////////// ShutterInstance ///////////////////////

  bindDeviceInstance<ShutterInstance>(m, "ShutterInstance")
      .def("SetOpen", &ShutterInstance::SetOpen, "open"_a)
      .def("GetOpen",
           [](ShutterInstance &self) {
             bool open;
             self.GetOpen(open);
             return open;
           })
      .def("Fire", &ShutterInstance::Fire, "deltaT"_a);

  /////////////////////// StageInstance ///////////////////////

  bindDeviceInstance<StageInstance>(m, "StageInstance")
      .def("SetPositionUm", &StageInstance::SetPositionUm, "pos"_a)
      .def("SetRelativePositionUm", &StageInstance::SetRelativePositionUm, "d"_a)
      .def("Move", &StageInstance::Move, "velocity"_a)
      .def("Stop", &StageInstance::Stop)
      .def("Home", &StageInstance::Home)
      .def("SetAdapterOriginUm", &StageInstance::SetAdapterOriginUm, "d"_a)
      .def("GetPositionUm",
           [](StageInstance &self) {
             double pos;
             self.GetPositionUm(pos);
             return pos;
           })
      .def("SetPositionSteps", &StageInstance::SetPositionSteps, "steps"_a)
      .def("GetPositionSteps",
           [](StageInstance &self) {
             long steps;
             self.GetPositionSteps(steps);
             return steps;
           })
      .def("SetOrigin", &StageInstance::SetOrigin)
      .def("GetLimits",
           [](StageInstance &self) {
             double lower, upper;
             self.GetLimits(lower, upper);
             return std::make_pair(lower, upper);
           })
      .def("GetFocusDirection", &StageInstance::GetFocusDirection)
      .def("SetFocusDirection", &StageInstance::SetFocusDirection, "direction"_a)
      .def("IsStageSequenceable",
           [](StageInstance &self) {
             bool isSequenceable;
             self.IsStageSequenceable(isSequenceable);
             return isSequenceable;
           })
      .def("IsStageLinearSequenceable",
           [](StageInstance &self) {
             bool isSequenceable;
             self.IsStageLinearSequenceable(isSequenceable);
             return isSequenceable;
           })
      .def("IsContinuousFocusDrive", &StageInstance::IsContinuousFocusDrive)
      .def("GetStageSequenceMaxLength",
           [](StageInstance &self) {
             long nrEvents;
             self.GetStageSequenceMaxLength(nrEvents);
             return nrEvents;
           })
      .def("StartStageSequence", &StageInstance::StartStageSequence)
      .def("StopStageSequence", &StageInstance::StopStageSequence)
      .def("ClearStageSequence", &StageInstance::ClearStageSequence)
      .def("AddToStageSequence", &StageInstance::AddToStageSequence, "position"_a)
      .def("SendStageSequence", &StageInstance::SendStageSequence)
      .def("SetStageLinearSequence", &StageInstance::SetStageLinearSequence, "dZ_um"_a,
           "nSlices"_a);

  /////////////////////// XYStageInstance ///////////////////////

  bindDeviceInstance<XYStageInstance>(m, "XYStageInstance")
      .def("SetPositionUm", &XYStageInstance::SetPositionUm, "x"_a, "y"_a)
      .def("SetRelativePositionUm", &XYStageInstance::SetRelativePositionUm, "dx"_a, "dy"_a)
      .def("SetAdapterOriginUm", &XYStageInstance::SetAdapterOriginUm, "x"_a, "y"_a)
      .def("GetPositionUm",
           [](XYStageInstance &self) {
             double x, y;
             self.GetPositionUm(x, y);
             return std::make_pair(x, y);
           })
      .def("SetPositionSteps", &XYStageInstance::SetPositionSteps, "x"_a, "y"_a)
      .def("GetPositionSteps",
           [](XYStageInstance &self) {
             long x, y;
             self.GetPositionSteps(x, y);
             return std::make_pair(x, y);
           })
      .def("SetOrigin", &XYStageInstance::SetOrigin)
      .def("GetStepSizeXUm", &XYStageInstance::GetStepSizeXUm)
      .def("GetStepSizeYUm", &XYStageInstance::GetStepSizeYUm)
      .def("GetStepSize",  // NOT in the original class
           [](XYStageInstance &self) {
             return std::make_pair(self.GetStepSizeXUm(), self.GetStepSizeYUm());
           })
      .def(
          "GetLimitsUm",
          [](XYStageInstance &self) {
            double xMin, xMax, yMin, yMax;
            self.GetLimitsUm(xMin, xMax, yMin, yMax);
            return std::make_tuple(xMin, xMax, yMin, yMax);
          },
          "Return limits of the XY stage in um (xMin, xMax, yMin, yMax)")
      .def("IsXYStageSequenceable",
           [](XYStageInstance &self) {
             bool isSequenceable;
             self.IsXYStageSequenceable(isSequenceable);
             return isSequenceable;
           })
      .def("GetXYStageSequenceMaxLength",
           [](XYStageInstance &self) {
             long nrEvents;
             self.GetXYStageSequenceMaxLength(nrEvents);
             return nrEvents;
           })
      .def("StartXYStageSequence", &XYStageInstance::StartXYStageSequence)
      .def("StopXYStageSequence", &XYStageInstance::StopXYStageSequence)
      .def("ClearXYStageSequence", &XYStageInstance::ClearXYStageSequence)
      .def("AddToXYStageSequence", &XYStageInstance::AddToXYStageSequence, "positionX"_a,
           "positionY"_a)
      .def("SendXYStageSequence", &XYStageInstance::SendXYStageSequence);

  /////////////////////// StateInstance ///////////////////////

  bindDeviceInstance<StateInstance>(m, "StateInstance")
      .def("SetPosition", py::overload_cast<long>(&StateInstance::SetPosition), "pos"_a)
      .def("SetPosition", py::overload_cast<const char *>(&StateInstance::SetPosition), "label"_a)
      .def("GetPosition",
           [](StateInstance &self) {
             long pos;
             self.GetPosition(pos);
             return pos;
           })
      .def("GetPositionLabel", [](const StateInstance &self) { return self.GetPositionLabel(); })
      .def(
          "GetPositionLabel",
          [](const StateInstance &self, long pos) { return self.GetPositionLabel(pos); }, "pos"_a)
      .def(
          "GetLabelPosition",
          [](StateInstance &self, const char *label) {
            long pos;
            self.GetLabelPosition(label, pos);
            return pos;
          },
          "label"_a)
      .def("SetPositionLabel", &StateInstance::SetPositionLabel, "pos"_a, "label"_a)
      .def("GetNumberOfPositions", &StateInstance::GetNumberOfPositions)
      .def("SetGateOpen", &StateInstance::SetGateOpen, "open"_a = true)
      .def("GetGateOpen", [](StateInstance &self) {
        bool open;
        self.GetGateOpen(open);
        return open;
      });

  /////////////////////// SerialInstance ///////////////////////

  bindDeviceInstance<SerialInstance>(m, "SerialInstance")
      .def("GetPortType", &SerialInstance::GetPortType)
      .def("SetCommand", &SerialInstance::SetCommand, "command"_a, "term"_a)
      // logic borrowed from MMCore.cpp
      .def(
          "GetAnswer",
          [](SerialInstance &self, const std::string &term) {
            if (term.empty())
              throw py::value_error("Null or empty terminator; cannot delimit received message");

            const int bufLen = 1024;
            char answerBuf[bufLen];
            int ret = self.GetAnswer(answerBuf, bufLen, term.c_str());
            if (ret != DEVICE_OK) {
              std::string errMsg = getErrorMessage(&self, ret);
              throw std::runtime_error(errMsg);
            }
            return std::string(answerBuf);
          },
          "term"_a)
      .def(
          "Write",
          [](SerialInstance &self, const std::string &data) {
            int ret =
                self.Write(reinterpret_cast<const unsigned char *>(data.data()), data.size());
            if (ret != DEVICE_OK) {
              std::string errMsg = getErrorMessage(&self, ret);
              throw std::runtime_error(errMsg);
            }
            return ret;
          },
          "data"_a)
      .def("Read",
           [](SerialInstance &self) {
             const int bufLen = 1024;
             unsigned char answerBuf[bufLen];
             unsigned long charsRead;
             int ret = self.Read(answerBuf, bufLen, charsRead);
             if (ret != DEVICE_OK) {
               std::string errMsg = getErrorMessage(&self, ret);
               throw std::runtime_error(errMsg);
             }
             std::vector<char> data;
             data.resize(charsRead, 0);
             if (charsRead > 0) std::memcpy(&(data[0]), answerBuf, charsRead);
             return data;
           })
      .def("Purge", &SerialInstance::Purge);

  /////////////////////// GenericInstance ///////////////////////

  bindDeviceInstance<GenericInstance>(m, "GenericInstance");
  // has no additional methods

  /////////////////////// AutoFocusInstance ///////////////////////

  bindDeviceInstance<AutoFocusInstance>(m, "AutoFocusInstance")
      .def("SetContinuousFocusing", &AutoFocusInstance::SetContinuousFocusing, "state"_a)
      .def("GetContinuousFocusing",
           [](AutoFocusInstance &self) {
             bool state;
             self.GetContinuousFocusing(state);
             return state;
           })
      .def("IsContinuousFocusLocked", &AutoFocusInstance::IsContinuousFocusLocked)
      .def("FullFocus", &AutoFocusInstance::FullFocus)
      .def("IncrementalFocus", &AutoFocusInstance::IncrementalFocus)
      .def("GetLastFocusScore",
           [](AutoFocusInstance &self) {
             double score;
             self.GetLastFocusScore(score);
             return score;
           })
      .def("GetCurrentFocusScore",
           [](AutoFocusInstance &self) {
             double score;
             self.GetCurrentFocusScore(score);
             return score;
           })
      .def("AutoSetParameters", &AutoFocusInstance::AutoSetParameters)
      .def("GetOffset",
           [](AutoFocusInstance &self) {
             double offset;
             self.GetOffset(offset);
             return offset;
           })
      .def("SetOffset", &AutoFocusInstance::SetOffset, "offset"_a);

  /////////////////////// ImageProcessorInstance ///////////////////////

  bindDeviceInstance<ImageProcessorInstance>(m, "ImageProcessorInstance")
      .def(
          "Process",
          [](ImageProcessorInstance &self, unsigned char *buffer, int width, int height,
             int byteDepth) {
            PyErr_SetString(PyExc_NotImplementedError,
                            "ImageProcessorInstance.Process is not implemented yet.");
            throw py::error_already_set();
          },
          "buffer"_a, "width"_a, "height"_a, "byteDepth"_a);

  /////////////////////// SignalIOInstance ///////////////////////

  bindDeviceInstance<SignalIOInstance>(m, "SignalIOInstance")
      .def("SetGateOpen", &SignalIOInstance::SetGateOpen, "open"_a = true)
      .def("GetGateOpen",
           [](SignalIOInstance &self) {
             bool open;
             self.GetGateOpen(open);
             return open;
           })
      .def("SetSignal", &SignalIOInstance::SetSignal, "volts"_a)
      .def("GetSignal",
           [](SignalIOInstance &self) {
             double volts;
             self.GetSignal(volts);
             return volts;
           })
      .def("GetLimits",
           [](SignalIOInstance &self) {
             double minVolts, maxVolts;
             self.GetLimits(minVolts, maxVolts);
             return std::make_pair(minVolts, maxVolts);
           })
      .def("IsDASequenceable",
           [](SignalIOInstance &self) {
             bool isSequenceable;
             self.IsDASequenceable(isSequenceable);
             return isSequenceable;
           })
      .def("GetDASequenceMaxLength",
           [](SignalIOInstance &self) {
             long nrEvents;
             self.GetDASequenceMaxLength(nrEvents);
             return nrEvents;
           })
      .def("StartDASequence", &SignalIOInstance::StartDASequence)
      .def("StopDASequence", &SignalIOInstance::StopDASequence)
      .def("ClearDASequence", &SignalIOInstance::ClearDASequence)
      .def("AddToDASequence", &SignalIOInstance::AddToDASequence, "voltage"_a)
      .def("SendDASequence", &SignalIOInstance::SendDASequence);

  /////////////////////// MagnifierInstance ///////////////////////

  bindDeviceInstance<MagnifierInstance>(m, "MagnifierInstance")
      .def("GetMagnification", &MagnifierInstance::GetMagnification);

  /////////////////////// SLMInstance ///////////////////////

  bindDeviceInstance<SLMInstance>(m, "SLMInstance")
      .def(
          "SetImage",
          [](SLMInstance &self, py::buffer b) {
            PyErr_SetString(PyExc_NotImplementedError,
                            "SLMInstance.SetImage is not implemented yet.");
            throw py::error_already_set();
          },
          "pixels"_a)
      .def("DisplayImage", &SLMInstance::DisplayImage)
      .def("SetPixelsTo", py::overload_cast<unsigned char>(&SLMInstance::SetPixelsTo),
           "intensity"_a)
      .def("SetPixelsTo",
           py::overload_cast<unsigned char, unsigned char, unsigned char>(
               &SLMInstance::SetPixelsTo),
           "red"_a, "green"_a, "blue"_a)
      .def("SetExposure", &SLMInstance::SetExposure, "interval_ms"_a)
      .def("GetExposure", &SLMInstance::GetExposure)
      .def("GetWidth", &SLMInstance::GetWidth)
      .def("GetHeight", &SLMInstance::GetHeight)
      .def("GetNumberOfComponents", &SLMInstance::GetNumberOfComponents)
      .def("GetBytesPerPixel", &SLMInstance::GetBytesPerPixel)
      .def("IsSLMSequenceable",
           [](SLMInstance &self) {
             bool isSequenceable;
             self.IsSLMSequenceable(isSequenceable);
             return isSequenceable;
           })
      .def("GetSLMSequenceMaxLength",
           [](SLMInstance &self) {
             long nrEvents;
             self.GetSLMSequenceMaxLength(nrEvents);
             return nrEvents;
           })
      .def("StartSLMSequence", &SLMInstance::StartSLMSequence)
      .def("StopSLMSequence", &SLMInstance::StopSLMSequence)
      .def("ClearSLMSequence", &SLMInstance::ClearSLMSequence)
      .def(
          "AddToSLMSequence",
          [](SLMInstance &self, py::buffer b) {
            PyErr_SetString(PyExc_NotImplementedError,
                            "SLMInstance.AddToSLMSequence is not implemented yet.");
            throw py::error_already_set();
          },
          "pixels"_a)
      .def("SendSLMSequence", &SLMInstance::SendSLMSequence);

  /////////////////////// GalvoInstance ///////////////////////

  bindDeviceInstance<GalvoInstance>(m, "GalvoInstance")
      .def("PointAndFire", &GalvoInstance::PointAndFire, "x"_a, "y"_a, "time_us"_a)
      .def("SetSpotInterval", &GalvoInstance::SetSpotInterval, "pulseInterval_us"_a)
      .def("SetPosition", &GalvoInstance::SetPosition, "x"_a, "y"_a)
      .def("GetPosition",
           [](GalvoInstance &self) {
             double x, y;
             self.GetPosition(x, y);
             return std::make_pair(x, y);
           })
      .def("SetIlluminationState", &GalvoInstance::SetIlluminationState, "on"_a)
      .def("GetXRange", &GalvoInstance::GetXRange)
      .def("GetXMinimum", &GalvoInstance::GetXMinimum)
      .def("GetYRange", &GalvoInstance::GetYRange)
      .def("GetYMinimum", &GalvoInstance::GetYMinimum)
      .def("AddPolygonVertex", &GalvoInstance::AddPolygonVertex, "polygonIndex"_a, "x"_a, "y"_a)
      .def("DeletePolygons", &GalvoInstance::DeletePolygons)
      .def("RunSequence", &GalvoInstance::RunSequence)
      .def("LoadPolygons", &GalvoInstance::LoadPolygons)
      .def("SetPolygonRepetitions", &GalvoInstance::SetPolygonRepetitions, "repetitions"_a)
      .def("RunPolygons", &GalvoInstance::RunPolygons)
      .def("StopSequence", &GalvoInstance::StopSequence)
      .def("GetChannel", &GalvoInstance::GetChannel);

  /////////////////////// HubInstance ///////////////////////

  bindDeviceInstance<HubInstance>(m, "HubInstance")
      .def("GetInstalledPeripheralNames", &HubInstance::GetInstalledPeripheralNames);
}
