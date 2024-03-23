#include <pybind11/pybind11.h>
#include <pybind11/stl.h>  // For automatic conversion between C++ and Python containers

#include <iostream>

#include "AutoFocusInstance.h"
#include "CameraInstance.h"
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

  return cls;
}

// Standalone function to replace the lambda
auto loadDeviceFunction = [](LoadedDeviceAdapter &self, const std::string &name,
                             const std::string &label) -> std::shared_ptr<DeviceInstance> {
  MockCMMCore mockCore;
  mm::logging::internal::GenericLogger<mm::logging::EntryData> deviceLogger(0);
  mm::logging::internal::GenericLogger<mm::logging::EntryData> coreLogger(0);
  return self.LoadDevice(&mockCore, name, label, deviceLogger, coreLogger);
};

std::string getErrorMessage(DeviceInstance *device, int errorCode) {
  return "Error in device " + ToQuotedString(device->GetLabel()) + ": " +
         device->GetErrorText(errorCode) + " (" + ToString(errorCode) + ")";
}

PYBIND11_MODULE(_pymmdevice, m) {
  // TODO: these are simply here for pybind11-stubgen ... but they don't work
  py::class_<MM::Device>(m, "Device");
  py::class_<MockCMMCore>(m, "MockCMMCore");
  py::class_<PyDeviceInstance>(m, "DeviceInstance");
  py::class_<mm::logging::Logger>(m, "Logger");
  py::class_<DeleteDeviceFunction>(m, "Callable");
  py::class_<MM::Core> core(m, "Core");

  // Enums

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
          py::arg("arg") = 0);

  /////////////////////// ShutterInstance ///////////////////////

  bindDeviceInstance<ShutterInstance>(m, "ShutterInstance")
      .def("SetOpen", &ShutterInstance::SetOpen, py::arg("open"))
      .def("GetOpen",
           [](ShutterInstance &self) {
             bool open;
             self.GetOpen(open);
             return open;
           })
      .def("Fire", &ShutterInstance::Fire, py::arg("deltaT"));

  /////////////////////// StageInstance ///////////////////////

  bindDeviceInstance<StageInstance>(m, "StageInstance")
      .def("SetPositionUm", &StageInstance::SetPositionUm, py::arg("pos"))
      .def("SetRelativePositionUm", &StageInstance::SetRelativePositionUm, py::arg("d"))
      .def("Move", &StageInstance::Move, py::arg("velocity"))
      .def("Stop", &StageInstance::Stop)
      .def("Home", &StageInstance::Home)
      .def("SetAdapterOriginUm", &StageInstance::SetAdapterOriginUm, py::arg("d"))
      .def("GetPositionUm",
           [](StageInstance &self) {
             double pos;
             self.GetPositionUm(pos);
             return pos;
           })
      .def("SetPositionSteps", &StageInstance::SetPositionSteps, py::arg("steps"))
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
      .def("SetFocusDirection", &StageInstance::SetFocusDirection, py::arg("direction"))
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
      .def("AddToStageSequence", &StageInstance::AddToStageSequence, py::arg("position"))
      .def("SendStageSequence", &StageInstance::SendStageSequence)
      .def("SetStageLinearSequence", &StageInstance::SetStageLinearSequence, py::arg("dZ_um"),
           py::arg("nSlices"));

  /////////////////////// XYStageInstance ///////////////////////

  bindDeviceInstance<XYStageInstance>(m, "XYStageInstance")
      .def("SetPositionUm", &XYStageInstance::SetPositionUm, py::arg("x"), py::arg("y"))
      .def("SetRelativePositionUm", &XYStageInstance::SetRelativePositionUm, py::arg("dx"),
           py::arg("dy"))
      .def("SetAdapterOriginUm", &XYStageInstance::SetAdapterOriginUm, py::arg("x"), py::arg("y"))
      .def("GetPositionUm",
           [](XYStageInstance &self) {
             double x, y;
             self.GetPositionUm(x, y);
             return std::make_pair(x, y);
           })
      .def("SetPositionSteps", &XYStageInstance::SetPositionSteps, py::arg("x"), py::arg("y"))
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
      .def("AddToXYStageSequence", &XYStageInstance::AddToXYStageSequence, py::arg("positionX"),
           py::arg("positionY"))
      .def("SendXYStageSequence", &XYStageInstance::SendXYStageSequence);

  /////////////////////// StateInstance ///////////////////////

  bindDeviceInstance<StateInstance>(m, "StateInstance")
      .def("SetPosition", py::overload_cast<long>(&StateInstance::SetPosition), py::arg("pos"))
      .def("SetPosition", py::overload_cast<const char *>(&StateInstance::SetPosition),
           py::arg("label"))
      .def("GetPosition",
           [](StateInstance &self) {
             long pos;
             self.GetPosition(pos);
             return pos;
           })
      .def("GetPositionLabel", [](const StateInstance &self) { return self.GetPositionLabel(); })
      .def(
          "GetPositionLabel",
          [](const StateInstance &self, long pos) { return self.GetPositionLabel(pos); },
          py::arg("pos"))
      .def(
          "GetLabelPosition",
          [](StateInstance &self, const char *label) {
            long pos;
            self.GetLabelPosition(label, pos);
            return pos;
          },
          py::arg("label"))
      .def("SetPositionLabel", &StateInstance::SetPositionLabel, py::arg("pos"), py::arg("label"))
      .def("GetNumberOfPositions", &StateInstance::GetNumberOfPositions)
      .def("SetGateOpen", &StateInstance::SetGateOpen, py::arg("open") = true)
      .def("GetGateOpen", [](StateInstance &self) {
        bool open;
        self.GetGateOpen(open);
        return open;
      });

  /////////////////////// SerialInstance ///////////////////////

  bindDeviceInstance<SerialInstance>(m, "SerialInstance")
      .def("GetPortType", &SerialInstance::GetPortType)
      .def("SetCommand", &SerialInstance::SetCommand, py::arg("command"), py::arg("term"))
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
          py::arg("term"))
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
          py::arg("data"))
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
      .def("SetContinuousFocusing", &AutoFocusInstance::SetContinuousFocusing, py::arg("state"))
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
      .def("SetOffset", &AutoFocusInstance::SetOffset, py::arg("offset"));

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
          py::arg("buffer"), py::arg("width"), py::arg("height"), py::arg("byteDepth"));

  /////////////////////// SignalIOInstance ///////////////////////

  bindDeviceInstance<SignalIOInstance>(m, "SignalIOInstance")
      .def("SetGateOpen", &SignalIOInstance::SetGateOpen, py::arg("open") = true)
      .def("GetGateOpen",
           [](SignalIOInstance &self) {
             bool open;
             self.GetGateOpen(open);
             return open;
           })
      .def("SetSignal", &SignalIOInstance::SetSignal, py::arg("volts"))
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
      .def("AddToDASequence", &SignalIOInstance::AddToDASequence, py::arg("voltage"))
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
          py::arg("pixels"))
      .def("DisplayImage", &SLMInstance::DisplayImage)
      .def("SetPixelsTo", py::overload_cast<unsigned char>(&SLMInstance::SetPixelsTo),
           py::arg("intensity"))
      .def("SetPixelsTo",
           py::overload_cast<unsigned char, unsigned char, unsigned char>(
               &SLMInstance::SetPixelsTo),
           py::arg("red"), py::arg("green"), py::arg("blue"))
      .def("SetExposure", &SLMInstance::SetExposure, py::arg("interval_ms"))
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
          py::arg("pixels"))
      .def("SendSLMSequence", &SLMInstance::SendSLMSequence);

  /////////////////////// GalvoInstance ///////////////////////

  bindDeviceInstance<GalvoInstance>(m, "GalvoInstance")
      .def("PointAndFire", &GalvoInstance::PointAndFire, py::arg("x"), py::arg("y"),
           py::arg("time_us"))
      .def("SetSpotInterval", &GalvoInstance::SetSpotInterval, py::arg("pulseInterval_us"))
      .def("SetPosition", &GalvoInstance::SetPosition, py::arg("x"), py::arg("y"))
      .def("GetPosition",
           [](GalvoInstance &self) {
             double x, y;
             self.GetPosition(x, y);
             return std::make_pair(x, y);
           })
      .def("SetIlluminationState", &GalvoInstance::SetIlluminationState, py::arg("on"))
      .def("GetXRange", &GalvoInstance::GetXRange)
      .def("GetXMinimum", &GalvoInstance::GetXMinimum)
      .def("GetYRange", &GalvoInstance::GetYRange)
      .def("GetYMinimum", &GalvoInstance::GetYMinimum)
      .def("AddPolygonVertex", &GalvoInstance::AddPolygonVertex, py::arg("polygonIndex"),
           py::arg("x"), py::arg("y"))
      .def("DeletePolygons", &GalvoInstance::DeletePolygons)
      .def("RunSequence", &GalvoInstance::RunSequence)
      .def("LoadPolygons", &GalvoInstance::LoadPolygons)
      .def("SetPolygonRepetitions", &GalvoInstance::SetPolygonRepetitions, py::arg("repetitions"))
      .def("RunPolygons", &GalvoInstance::RunPolygons)
      .def("StopSequence", &GalvoInstance::StopSequence)
      .def("GetChannel", &GalvoInstance::GetChannel);

  /////////////////////// HubInstance ///////////////////////

  bindDeviceInstance<HubInstance>(m, "HubInstance")
      .def("GetInstalledPeripheralNames", &HubInstance::GetInstalledPeripheralNames);

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
