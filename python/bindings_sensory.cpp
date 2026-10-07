// Sensorimotor system bindings

#include "bindings.h"

namespace nlm {

void bind_sensory_system(py::module_& m) {
    // SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for all sensory input.
        
        This is the abstract base class for all sensory modalities:
        - Vision: Visual input from the environment
        - Audio: Auditory input from the environment
        - InternalSignals: Internal brain state signals
        
        Sensory inputs provide:
        - Type information for modality identification
        - Raw data arrays for processing
        - Dimensionality information
        - Timestamp tracking
        - Cloning for backup/restore operations
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("clone", &SensoryInput::clone, "Create a copy of this sensory input");

    // Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input representing visual information.
        
        Vision input provides:
        - 2D image data with configurable dimensions
        - Color channels (typically RGB)
        - Width and height properties
        - Data setting methods for image initialization
        
        Visual data is stored as:
        - Vector of float values (flattened image)
        - Organized as [R,G,B,R,G,B,...] for RGB images
        - Normalized to [0.0, 1.0] range
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3, "Create a vision input with specified dimensions")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set the image data (flattened RGB array)")
        .def("getWidth", &Vision::getWidth, "Get image width in pixels")
        .def("getHeight", &Vision::getHeight, "Get image height in pixels")
        .def("getChannels", &Vision::getChannels, "Get number of color channels");

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input representing sound information.
        
        Audio input provides:
        - Time-series audio samples
        - Sample rate (frequency) specification
        - Number of samples for analysis
        - Data setting methods for audio initialization
        
        Audio data is stored as:
        - Vector of float values (audio waveform)
        - Normalized to [-1.0, 1.0] range for typical audio
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Create an audio input with specified sample rate and length")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set the audio waveform data")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set the audio sample rate (Hz)")
        .def("getSampleRate", &Audio::getSampleRate, "Get the audio sample rate (Hz)")
        .def("getNumSamples", &Audio::getNumSamples, "Get the number of audio samples");

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal signals sensory input representing brain internal state.
        
        Internal signals provide:
        - Neural activity patterns from different brain regions
        - Working memory content
        - Attention focus areas
        - Prediction error signals
        - Motivation and neuromodulation levels
        
        Internal data is stored as:
        - Vector of float values representing neural activations
        - Can store multiple signal types simultaneously
        - Useful for self-monitoring and feedback
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value to the internal signals")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all internal signals");
}

} // namespace nlm