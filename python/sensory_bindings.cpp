#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "../src/sensory/SensoryInput.hpp"

namespace py = pybind11;
namespace nlm {

// sensory_bindings.cpp - SensoryInput, Vision, Audio, InternalSignals
// Purpose: Defines Python bindings for sensory input classes and their implementations

// Register sensory input class bindings to the given module
void register_sensory(pybind11::module_& m) {

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for all sensory input types in the NLM system.
        
        SensoryInput represents sensory information received from the environment or
        internal brain states. It serves as the base class for all specific sensory
        modalities including vision (external visual information), audio (sound input),
        and internal signals (brain state information). Sensory inputs are processed
        by the agent's brain to generate appropriate actions.
        
        This is an abstract base class and cannot be instantiated directly.
        Use derived classes like Vision, Audio, or InternalSignals for actual
        sensory data.
        
        Examples:
            >>> # Create a vision sensory input
            >>> vision = nlm.Vision(640, 480, 3)
            >>> vision.setData([0.5, 0.7, 0.9] * 640 * 480 * 3)
            >>> # Get sensory type
            >>> vision.getType()
            <NeuronType.Sensory: 4>
        )pbdoc")
        .def("getType", &SensoryInput::getType,
             R"pbdoc(
             Get the type of this sensory input.
             
             Returns:
                 NeuronType: The neuron type associated with this sensory input
                           (typically NeuronType.Sensory)
             
             Examples:
                 >>> vision = nlm.Vision(100, 100)
                 >>> vision.getType()
                 <NeuronType.Sensory: 4>
             )pbdoc")
        .def("getData", &SensoryInput::getData,
             R"pbdoc(
             Get the raw data of this sensory input.
             
             Returns:
                 list[float]: Raw sensor data as a list of floating-point values.
                           The format and length depend on the specific sensory type.
             
             Examples:
                 >>> audio = nlm.Audio(44100, 1024)
                 >>> audio.setData([0.1, 0.2, 0.3] * 1024)
                 >>> data = audio.getData()
                 >>> len(data) == 1024 * 3
                 True
             )pbdoc")
        .def("getDimensions", &SensoryInput::getDimensions,
             R"pbdoc(
             Get the dimensionality of the sensory input data.
             
             Returns:
                 int: The number of dimensions in the sensory data
             
             Examples:
                 >>> vision = nlm.Vision(640, 480, 3)  # width, height, channels
                 >>> vision.getDimensions()
                 3
                 >>> audio = nlm.Audio(44100, 1024)
                 >>> audio.getDimensions()
                 2  # sample_rate, num_samples
             )pbdoc")
        .def("getTimestamp", &SensoryInput::getTimestamp,
             R"pbdoc(
             Get the timestamp when this sensory input was captured.
             
             Returns:
                 float: The simulation time when this sensory input was generated
             
             Examples:
                 >>> vision = nlm.Vision(100, 100)
                 >>> ts = vision.getTimestamp()
                 >>> ts >= 0.0
                 True
             )pbdoc")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             R"pbdoc(
             Set the timestamp when this sensory input was captured.
             
             Args:
                 timestamp: The simulation time to set for this sensory input
             
             Examples:
                 >>> vision = nlm.Vision(100, 100)
                 >>> vision.setTimestamp(1.5)
                 >>> vision.getTimestamp()
                 1.5
             )pbdoc");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Visual sensory input representing image data from the environment.
        
        Vision captures visual information from the world, typically representing
        a 2D image with color channels. It's used for visual perception tasks like
        object recognition, navigation, and scene analysis in the NLM brain.
        
        Vision data is typically organized as a 2D array of RGB values, where each
        pixel has red, green, and blue channel values between 0.0 and 1.0.
        
        Examples:
            >>> # Create a 640x480 RGB vision system
            >>> vision = nlm.Vision(640, 480, 3)
            >>> vision.setData(list(range(640 * 480 * 3)))
            >>> width = vision.getWidth()
            >>> height = vision.getHeight()
            >>> channels = vision.getChannels()
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3,
             R"pbdoc(
             Initialize Vision sensory input with specified dimensions.
             
             Args:
                 width: Image width in pixels (default: 640)
                 height: Image height in pixels (default: 480)
                 channels: Number of color channels (default: 3 for RGB)
             
             Examples:
                 >>> vision = nlm.Vision(640, 480, 3)
                 >>> vision.getWidth()
                 640
                 >>> vision.getHeight()
                 480
                 >>> vision.getChannels()
                 3
             )pbdoc")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             R"pbdoc(
             Set the visual data for this Vision input.
             
             Args:
                 data: List of float values representing pixel data. Should contain
                       width * height * channels values in row-major order.
             
             Examples:
                 >>> vision = nlm.Vision(10, 10, 3)
                 >>> vision.setData([0.1, 0.5, 0.9] * 10 * 10 * 3)
                 >>> # Set 10x10 RGB image
             )pbdoc")
        .def("getWidth", &Vision::getWidth,
             R"pbdoc(Get the image width in pixels)pbdoc")
        .def("getHeight", &Vision::getHeight,
             R"pbdoc(Get the image height in pixels)pbdoc")
        .def("getChannels", &Vision::getChannels,
             R"pbdoc(Get the number of color channels)pbdoc");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input representing sound data from the environment.
        
        Audio captures auditory information from the world, typically representing
        sound waveforms as time-series data. It's used for sound localization,
        speech recognition, and environmental awareness in the NLM brain.
        
        Audio data is represented as discrete samples at a specific sampling rate,
        with each sample representing the amplitude of the sound wave at that time.
        
        Examples:
            >>> # Create audio input with 44.1kHz sampling rate
            >>> audio = nlm.Audio(44100, 1024)
            >>> audio.setData(list(range(44100)))
            >>> audio.setSampleRate(48000)
            >>> audio.getSampleRate()
            48000
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             R"pbdoc(
             Initialize Audio sensory input with specified audio parameters.
             
             Args:
                 sampleRate: Audio sampling rate in Hertz (default: 44100)
                 numSamples: Number of audio samples (default: 1024)
             
             Examples:
                 >>> audio = nlm.Audio(44100, 1024)
                 >>> audio.getSampleRate()
                 44100
                 >>> audio.getNumSamples()
                 1024
             )pbdoc")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             R"pbdoc(
             Set the audio data for this Audio input.
             
             Args:
                 data: List of float values representing audio samples
             
             Examples:
                 >>> audio = nlm.Audio(44100, 100)
                 >>> audio.setData([0.1, -0.2, 0.3] * 100)
                 >>> # Set 100-sample audio chunk
             )pbdoc")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             R"pbdoc(Set the audio sampling rate)pbdoc")
        .def("getSampleRate", &Audio::getSampleRate,
             R"pbdoc(Get the audio sampling rate)pbdoc")
        .def("getNumSamples", &Audio::getNumSamples,
             R"pbdoc(Get the number of audio samples)pbdoc");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal sensory input representing brain state information.
        
        InternalSignals captures the current state of the brain's internal systems,
        including neural activity, memory states, and neuromodulation levels. This
        type of sensory input represents what the brain senses about its own current
        state, enabling self-monitoring and self-regulation.
        
        Internal signals include working memory contents, attention states, and
        various brain system indicators. This allows the brain to be aware of its
        own processing state.
        
        Examples:
            >>> internal = nlm.InternalSignals()
            >>> internal.addSignal(0.5)  # Add a brain state signal
            >>> internal.addSignal(0.8)  # Add another signal
            >>> internal.clearSignals()  # Clear all signals
        )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             R"pbdoc(
             Add a signal to the internal signals buffer.
             
             Args:
                 value: Float value representing an internal brain state signal
             
             Examples:
                 >>> internal = nlm.InternalSignals()
                 >>> internal.addSignal(0.5)  # Add attention level signal
                 >>> internal.addSignal(0.8)  # Add arousal signal
             )pbdoc")
        .def("clearSignals", &InternalSignals::clearSignals,
             R"pbdoc(
             Clear all internal signals.
             
             This removes all internal state information from the buffer,
             allowing for a fresh internal state.
             
             Examples:
                 >>> internal = nlm.InternalSignals()
                 >>> internal.addSignal(0.5)
                 >>> internal.addSignal(0.8)
                 >>> internal.clearSignals()
             )pbdoc");
}

} // namespace nlm