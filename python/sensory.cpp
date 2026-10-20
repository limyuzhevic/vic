#include "sensory.h"
#include "error_handling.h"
#include <pybind11/stl.h>

namespace py = pybind11;
namespace nlm {

void include_sensory(py::module_& m) {
    // SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)
    
    The SensoryInput class is the base class for all sensory modalities in the NLM system.
    It provides the interface for receiving and processing sensory information from the world.
    
    Subclasses include:
        Vision: Visual sensory input
        Audio: Audio sensory input  
        InternalSignals: Internal neural signals
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input as string")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector of floats")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality of the sensory data")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp of this sensory input")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp for this sensory input")
        .def("getTypeEnum", &SensoryInput::getType,
             "Get the sensory input type as enum (SensoryInputType)");

    // Type conversion utility for SensoryInput types
    m.def("getSensoryInputTypeName", [](SensoryInputType type) {
        switch (type) {
            case SensoryInputType::Vision: return "Vision";
            case SensoryInputType::Audio: return "Audio";
            case SensoryInputType::InternalSignals: return "InternalSignals";
            default: return "Unknown";
        }
    }, "Get the string name for a sensory input type", py::arg("type"));

    // Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)
    
    Visual sensory input representing camera or vision data.
    
    Vision data is typically stored as a flattened array of RGB pixel values.
    The data is organized as [width * height * channels] where channels=3 for RGB.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3,
             "Create a Vision sensory input with specified dimensions."
             "Channels defaults to 3 for RGB. Raises ValueError if dimensions invalid.")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw std::runtime_error("Vision data cannot be empty");
            }
            self.setData(data);
        }, py::arg("data"), "Set vision data. Must be non-empty vector. Raises RuntimeError if invalid.")
        .def("getWidth", &Vision::getWidth, "Get the width of the vision image in pixels")
        .def("getHeight", &Vision::getHeight, "Get the height of the vision image in pixels")
        .def("getChannels", &Vision::getChannels, "Get the number of color channels (typically 3 for RGB)")
        .def("getPixelCount", &Vision::getWidth, "Get total number of pixels (width * height)")
        .def("createFromImageData", [](size_t width, size_t height, size_t channels, 
                                     const std::vector<float>& data) {
            Vision vision(width, height, channels);
            vision.setData(data);
            return vision;
        }, py::arg("width"), py::arg("height"), py::arg("channels") = 3, py::arg("data"),
           "Factory method to create a Vision object from raw image data.
           Total data size must equal width * height * channels.")
        .def("clone", [](const Vision& self) {
            return Vision(self);
        }, "Create a copy of this Vision object");

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)
    
    Audio sensory input representing sound data.
    
    Audio data is stored as a 1D array of audio samples at a specific sample rate.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Create an Audio sensory input with specified sample rate and sample count.
             Raises ValueError if sampleRate or numSamples is zero.")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw std::runtime_error("Audio data cannot be empty");
            }
            self.setData(data);
        }, py::arg("data"), "Set audio data. Must be non-empty vector. Raises RuntimeError if invalid.")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set the audio sample rate in Hz. Must be positive.")
        .def("getSampleRate", &Audio::getSampleRate, "Get the audio sample rate in Hz")
        .def("getNumSamples", &Audio::getNumSamples, "Get the number of audio samples")
        .def("getDuration", [](const Audio& self) {
            return static_cast<double>(self.getNumSamples()) / self.getSampleRate();
        }, "Get the audio duration in seconds")
        .def("createFromSamples", [](size_t sampleRate, const std::vector<float>& samples) {
            Audio audio(sampleRate, samples.size());
            audio.setData(samples);
            return audio;
        }, py::arg("sampleRate"), py::arg("samples"),
           "Factory method to create an Audio object from sample data.")
        .def("clone", [](const Audio& self) {
            return Audio(self);
        }, "Create a copy of this Audio object");

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)
    
    Internal signals sensory input representing neural or cognitive states.
    
    InternalSignals stores a collection of signal values representing various
    internal states like activation levels, predictions, or neuromodulatory signals.
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value to the internal signals collection. Value should be between -1.0 and 1.0.")
        .def("setSignal", [](InternalSignals& self, size_t index, double value) {
            if (index >= self.getData().size()) {
                throw std::runtime_error("Signal index out of range");
            }
            self.getData()[index] = value;
        }, py::arg("index"), py::arg("value"),
             "Set a specific signal value by index. Raises RuntimeError if index out of range.")
        .def("getSignal", [](const InternalSignals& self, size_t index) {
            const auto& data = self.getData();
            if (index >= data.size()) {
                throw std::runtime_error("Signal index out of range");
            }
            return data[index];
        }, py::arg("index"), "Get a specific signal value by index. Raises RuntimeError if index out of range.")
        .def("getSignalCount", [](const InternalSignals& self) {
            return self.getData().size();
        }, "Get the number of signals in the collection")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all signals")
        .def("normalizeSignals", [](InternalSignals& self) {
            self.normalize();
        }, "Normalize all signal values to range [-1, 1]")
        .def("getMinMax", [](const InternalSignals& self) {
            const auto& data = self.getData();
            if (data.empty()) {
                return std::make_pair(0.0, 1.0);
            }
            double min = *std::min_element(data.begin(), data.end());
            double max = *std::max_element(data.begin(), data.end());
            return std::make_pair(min, max);
        }, "Get the minimum and maximum signal values")
        .def("clone", [](const InternalSignals& self) {
            return InternalSignals(self);
        }, "Create a copy of this InternalSignals object");

} // namespace nlm