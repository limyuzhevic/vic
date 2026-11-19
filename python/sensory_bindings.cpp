// Sensory bindings implementation
#include "sensory_bindings.h"

namespace nlm {

void bindSensoryInput(py::module_& m) {
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        // Add error handling for advanced usage
        .def("getData", [](const SensoryInput& self) {
            try {
                return self.getData();
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, e.what());
                throw py::error_already_set();
            }
        }, "Get the raw data as a vector with error handling");
}

void bindVision(py::module_& m) {
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        // Add high-level convenience methods for advanced users
        .def("createFromImage", [](Vision& self, const std::string& imagePath, size_t channels = 3) {
            // This would typically load an image file
            throw std::runtime_error("Image loading requires additional dependencies");
        }, py::arg("imagePath"), py::arg("channels") = 3,
             "Create Vision from image file (requires image loading library)")
        .def("normalize", &Vision::normalize, "Normalize pixel values to [0, 1]")
        .def("resize", &Vision::resize, py::arg("width"), py::arg("height"),
             "Resize image to specified dimensions")
        .def("grayscale", &Vision::grayscale, "Convert to grayscale")
        .def("to_array", &Vision::to_array, "Convert to numpy array (if available)")
        .def("getFeatures", &Vision::getFeatures, "Extract features from image");
}

void bindAudio(py::module_& m) {
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        // Add audio processing utilities
        .def("normalize", &Audio::normalize, "Normalize audio levels")
        .def("filter", &Audio::filter, py::arg("freq"), py::arg("q") = 1.0,
             "Apply frequency filter")
        .def("to_wav", &Audio::to_wav, py::arg("filepath"), "Export to WAV file")
        .def("getFrequencySpectrum", &Audio::getFrequencySpectrum, "Get frequency spectrum");
}

void bindInternalSignals(py::module_& m) {
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        // Add signal processing utilities
        .def("addNoise", &InternalSignals::addNoise, py::arg("mean") = 0.0f, py::arg("stddev") = 1.0f,
             "Add Gaussian noise to signals")
        .def("smooth", &InternalSignals::smooth, py::arg("windowSize") = 5,
             "Apply moving average filter")
        .def("getStatistics", &InternalSignals::getStatistics, "Get signal statistics")
        .def("findPeaks", &InternalSignals::findPeaks, py::arg("threshold"), "Find signal peaks");
}

} // namespace nlm