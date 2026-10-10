// NLM Python Bindings - Sensory Input
// Vision, Audio, and InternalSignals class bindings

#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "exceptions.hpp"
#include "validation.hpp"

namespace py = pybind11;
namespace nlm {

void bind_sensory(py::module_& m) {
    // SensoryInput base class
    py::class_<SensoryInput, std::shared_ptr<SensoryInput>>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, R"pbdoc(Get the type of sensory input)
        
        Returns:
            NeuronType indicating the type of sensory input
        )pbdoc")
        .def("getData", &SensoryInput::getData, R"pbdoc(Get the raw data as a vector)
        
        Returns:
            Vector of float values representing sensory data
        )pbdoc")
        .def("getDimensions", &SensoryInput::getDimensions, R"pbdoc(Get the dimensionality)
        
        Returns:
            Number of dimensions in the sensory data
        )pbdoc")
        .def("getTimestamp", &SensoryInput::getTimestamp, R"pbdoc(Get the timestamp)
        
        Returns:
            Simulation timestamp when this input was generated
        )pbdoc")
        .def("setTimestamp", [](SensoryInput& self, Timestamp timestamp) {
            try {
                validation::check_bounds("timestamp", timestamp, 0.0, 1e9);
                self.setTimestamp(timestamp);
            } catch (const ValidationError& e) {
                throw;
            } catch (const std::exception& e) {
                throw ValidationError(std::string("Failed to set timestamp: ") + e.what());
            }
        }, py::arg("timestamp"), R"pbdoc(Set the timestamp)
        
        Args:
            timestamp: Simulation timestamp to set
        
        Raises:
            ValidationError: If timestamp is invalid
        )pbdoc")
        .def("__repr__", [](const SensoryInput& input) {
            return "<SensoryInput: type=" + std::to_string(static_cast<int>(input.getType())) +
                   ", dims=" + std::to_string(input.getDimensions()) + ">";
        });

    // Vision class binding
    py::class_<Vision, SensoryInput, std::shared_ptr<Vision>>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3, R"pbdoc(Create a Vision sensory input)
        
        Args:
            width: Image width in pixels
            height: Image height in pixels
            channels: Number of color channels (default: 3 for RGB)
        
        Raises:
            ValidationError: If dimensions are invalid
        )pbdoc")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            try {
                validation::check_vector_bounds("vision data", data, 1, 1000000);
                self.setData(data);
            } catch (const ValidationError& e) {
                throw;
            } catch (const std::exception& e) {
                throw ValidationError(std::string("Failed to set vision data: ") + e.what());
            }
        }, py::arg("data"), R"pbdoc(Set vision data)
        
        Args:
            data: Vector of float values representing image data (width * height * channels)
        
        Raises:
            ValidationError: If data is invalid
        )pbdoc")
        .def("getWidth", &Vision::getWidth, R"pbdoc(Get image width)
        
        Returns:
            Image width in pixels
        )pbdoc")
        .def("getHeight", &Vision::getHeight, R"pbdoc(Get image height)
        
        Returns:
            Image height in pixels
        )pbdoc")
        .def("getChannels", &Vision::getChannels, R"pbdoc(Get number of channels)
        
        Returns:
            Number of color channels
        )pbdoc")
        .def("getPixelData", [](const Vision& self) {
            // Additional helper to get flattened pixel data
            return self.getData();
        }, R"pbdoc(Get flattened pixel data)
        
        Returns:
            Vector of float values representing pixel data
        )pbdoc")
        .def("__repr__", [](const Vision& vision) {
            return "<Vision: " + std::to_string(vision.getWidth()) + "x" +
                   std::to_string(vision.getHeight()) + "x" +
                   std::to_string(vision.getChannels()) + ">";
        });

    // Audio class binding
    py::class_<Audio, SensoryInput, std::shared_ptr<Audio>>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"), R"pbdoc(Create an Audio sensory input)
        
        Args:
            sampleRate: Sampling rate in Hz
            numSamples: Number of audio samples
        
        Raises:
            ValidationError: If sample rate or sample count is invalid
        )pbdoc")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            try {
                validation::check_vector_bounds("audio data", data, 1, 1000000);
                self.setData(data);
            } catch (const ValidationError& e) {
                throw;
            } catch (const std::exception& e) {
                throw ValidationError(std::string("Failed to set audio data: ") + e.what());
            }
        }, py::arg("data"), R"pbdoc(Set audio data)
        
        Args:
            data: Vector of float values representing audio samples
        
        Raises:
            ValidationError: If data is invalid
        )pbdoc")
        .def("setSampleRate", [](Audio& self, size_t sampleRate) {
            try {
                validation::check_bounds("sample rate", sampleRate, 8000, 192000);
                self.setSampleRate(sampleRate);
            } catch (const ValidationError& e) {
                throw;
            } catch (const std::exception& e) {
                throw ValidationError(std::string("Failed to set sample rate: ") + e.what());
            }
        }, py::arg("sampleRate"), R"pbdoc(Set audio sample rate)
        
        Args:
            sampleRate: Sample rate in Hz
        
        Raises:
            ValidationError: If sample rate is invalid
        )pbdoc")
        .def("getSampleRate", &Audio::getSampleRate, R"pbdoc(Get audio sample rate)
        
        Returns:
            Sample rate in Hz
        )pbdoc")
        .def("getNumSamples", &Audio::getNumSamples, R"pbdoc(Get number of samples)
        
        Returns:
            Number of audio samples
        )pbdoc")
        .def("getDuration", [](const Audio& self) {
            // Calculate duration from sample rate and sample count
            if (self.getSampleRate() > 0) {
                return static_cast<double>(self.getNumSamples()) / self.getSampleRate();
            }
            return 0.0;
        }, R"pbdoc(Get audio duration in seconds)
        
        Returns:
            Duration in seconds
        )pbdoc")
        .def("__repr__", [](const Audio& audio) {
            return "<Audio: " + std::to_string(audio.getSampleRate()) + "Hz, " +
                   std::to_string(audio.getNumSamples()) + " samples>";
        });

    // InternalSignals class binding
    py::class_<InternalSignals, SensoryInput, std::shared_ptr<InternalSignals>>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", [](InternalSignals& self, float value) {
            try {
                validation::validate_synaptic_weight(value);
                self.addSignal(value);
            } catch (const ValidationError& e) {
                throw;
            } catch (const std::exception& e) {
                throw ValidationError(std::string("Failed to add signal: ") + e.what());
            }
        }, py::arg("value"), R"pbdoc(Add a signal value)
        
        Args:
            value: Signal value to add
        
        Raises:
            ValidationError: If value is invalid
        )pbdoc")
        .def("clearSignals", &InternalSignals::clearSignals, R"pbdoc(Clear all signals)
        
        Clears all stored signals.
        )pbdoc")
        .def("getSignalCount", [](const InternalSignals& self) {
            return self.getData().size();
        }, R"pbdoc(Get number of signals)
        
        Returns:
            Number of signals stored
        )pbdoc")
        .def("getSignal", [](const InternalSignals& self, size_t index) {
            try {
                validation::check_bounds("signal index", index, 0, self.getData().size() - 1);
                return self.getData()[index];
            } catch (const ValidationError& e) {
                throw;
            }
        }, py::arg("index"), R"pbdoc(Get signal value at index)
        
        Args:
            index: Signal index
        
        Returns:
            Signal value at the specified index
        
        Raises:
            ValidationError: If index is out of bounds
        )pbdoc")
        .def("__repr__", [](const InternalSignals& signals) {
            return "<InternalSignals: " + std::to_string(signals.getData().size()) + " signals>";
        });

} // namespace nlm
