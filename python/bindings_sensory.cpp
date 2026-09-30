#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/core/Types/Types.hpp"

namespace py = pybind11;
namespace nlm {

void bindSensory(py::module_ &m) {
    // Base SensoryInput class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("__repr__", [](const SensoryInput& input) {
            return "<SensoryInput type=" + std::to_string(input.getType()) + 
                   " dimensions=" + std::to_string(input.getDimensions()) + ">";
        });

    // Vision class - simplified interface
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data as a vector of floats")
        .def("getWidth", &Vision::getWidth, "Get vision width")
        .def("getHeight", &Vision::getHeight, "Get vision height")
        .def("getChannels", &Vision::getChannels, "Get number of channels")
        .def("shape", [](const Vision& self) {
            return std::vector<size_t>{self.getWidth(), self.getHeight(), self.getChannels()};
        }, "Get vision dimensions as a vector [width, height, channels]")
        .def("as_image", [](const Vision& self) {
            // Return a Python-friendly representation
            std::vector<float> data = self.getData();
            py::array_t<float> array({self.getHeight(), self.getWidth(), self.getChannels()});
            auto buf = array.mutable_unchecked();
            for (size_t h = 0; h < self.getHeight(); ++h) {
                for (size_t w = 0; w < self.getWidth(); ++w) {
                    for (size_t c = 0; c < self.getChannels(); ++c) {
                        size_t idx = ((h * self.getWidth() + w) * self.getChannels()) + c;
                        buf(h, w, c) = data[idx];
                    }
                }
            }
            return array;
        }, "Return vision data as a NumPy-like array (height x width x channels)");

    // Audio class - simplified interface
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data as a vector of floats")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set audio sample rate")
        .def("getSampleRate", &Audio::getSampleRate, "Get audio sample rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of audio samples")
        .def("duration", [](const Audio& self) {
            return self.getNumSamples() / static_cast<double>(self.getSampleRate());
        }, "Get audio duration in seconds")
        .def("to_array", [](const Audio& self) {
            return py::array_t<float>({self.getNumSamples()}, self.getData());
        }, "Return audio data as a NumPy-like array");

    // InternalSignals class - simplified interface
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", 
        R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add an internal signal value")
        .def("getSignals", &InternalSignals::getData, "Get all internal signals as vector")
        .def("getSignalCount", &InternalSignals::getDimensions, "Get number of internal signals")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all internal signals")
        .def("add_multiple", [](InternalSignals& self, const std::vector<float>& values) {
            for (float val : values) {
                self.addSignal(val);
            }
        }, py::arg("values"), "Add multiple signal values at once")
        .def("__len__", &InternalSignals::getDimensions, 
             "Get number of internal signals")
        .def("__getitem__", [](const InternalSignals& self, size_t index) {
            const std::vector<float>& data = self.getData();
            if (index < data.size()) {
                return data[index];
            }
            throw py::index_error("Internal signal index out of range");
        })
        .def("__setitem__", [](InternalSignals& self, size_t index, float value) {
            std::vector<float>& data = self.getData();
            if (index < data.size()) {
                data[index] = value;
            } else {
                throw py::index_error("Internal signal index out of range");
            }
        })
        .def("__repr__", [](const InternalSignals& signals) {
            return "<InternalSignals count=" + std::to_string(signals.getDimensions()) + ">";
        });
}
