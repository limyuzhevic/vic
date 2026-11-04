// Sensory bindings
void bind_sensory(py::module_& m) {
    m.doc() = "Sensory bindings for NLM"; 
    
    py::class_<SensoryInput>(m, "SensoryInput", "Base class for sensory input")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", "Vision sensory input")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        // Pythonic convenience methods
        .def("as_array", [](Vision& self) {
            return self.getData();
        }, "Get vision data as numpy array (if available)")
        .def("resize", [](Vision& self, size_t width, size_t height, size_t channels = 3) {
            // This would need implementation in Vision class
            // For now, placeholder
            return "Resize not implemented in this binding";
        }, py::arg("width"), py::arg("height"), py::arg("channels") = 3,
           "Resize vision sensor (placeholder)");

    py::class_<Audio, SensoryInput>(m, "Audio", "Audio sensory input")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        // Pythonic convenience methods
        .def("as_array", [](Audio& self) {
            return self.getData();
        }, "Get audio data as numpy array (if available)")
        .def("normalize", [](Audio& self, float target_level = 1.0f) {
            // This would need implementation in Audio class
            // For now, placeholder
            return "Normalize not implemented in this binding";
        }, py::arg("target_level") = 1.0f,
           "Normalize audio level (placeholder)");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", "Internal signals sensory input")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        // Pythonic convenience methods
        .def("add_signals", [](InternalSignals& self, const std::vector<float>& values) {
            for (float v : values) self.addSignal(v);
        }, py::arg("values"), "Add multiple signals")
        .def("signals", [](InternalSignals& self) {
            // This would need a getter method in InternalSignals
            // For now, placeholder
            return std::vector<float>();
        }, "Get all signals");
}

define_bind_sensory() {
    // This function exists for backward compatibility but is no longer needed
}