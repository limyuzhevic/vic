// Motor bindings
void bind_motor(py::module_& m) {
    m.doc() = "Motor bindings for NLM"; 
    
    py::class_<SensoryPercept>(m, "SensoryPercept", "Sensory percept data")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        // Pythonic convenience methods
        .def("update_from_vision", [](SensoryPercept& self, const Vision& vision) {
            self.setVision(vision);
        }, py::arg("vision"), "Update percept from vision sensor")
        .def("update_from_audio", [](SensoryPercept& self, const Audio& audio) {
            self.setAudio(audio);
        }, py::arg("audio"), "Update percept from audio sensor")
        .def("get_data_summary", [](SensoryPercept& self) {
            py::dict summary;
            summary["has_vision"] = self.getVision() != nullptr;
            summary["has_touch"] = !self.getTouch().empty();
            summary["has_internal"] = !self.getInternal().empty();
            summary["has_proprioception"] = !self.getProprioception().empty();
            summary["has_audio"] = self.getAudio() != nullptr;
            summary["timestamp"] = self.getTimestamp();
            return summary;
        }, "Get summary of sensory data");
}

define_bind_motor() {
    // This function exists for backward compatibility but is no longer needed
}