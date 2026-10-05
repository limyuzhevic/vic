#pragma once

#include "sensory_bindings.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <typeindex>
#include <unordered_map>

namespace py = pybind11;

namespace nlm {
    
    class SensoryModule : public BindingModule {
    public:
        std::string getName() const override { return "sensory"; }
        std::string getVersion() const override { return "1.0.0"; }
        std::string getDescription() const override { 
            return "Sensory system bindings for NLM (Vision, Audio, InternalSignals)"; 
        }
        
        void registerBindings(pybind11::module_& m) override {
            // Sensory class bindings using ModuleBuilder pattern
            auto builder = ModuleBuilder(m);
            
            // SensoryInput base class
            builder.class_<SensoryInput>("SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
                .def("getType", &SensoryInput::getType, "Get the type of sensory input")
                .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
                .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
                .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
                .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
                     "Set the timestamp");
            
            // Vision class
            builder.class_<Vision, SensoryInput>("Vision", R"pbdoc(Vision sensory input)pbdoc")
                .def(py::init<>())
                .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
                     py::arg("channels") = 3)
                .def("setData", [](Vision& self, const std::vector<float>& data) {
                    self.setData(data);
                }, py::arg("data"))
                .def("getWidth", &Vision::getWidth)
                .def("getHeight", &Vision::getHeight)
                .def("getChannels", &Vision::getChannels);
            
            // Audio class
            builder.class_<Audio, SensoryInput>("Audio", R"pbdoc(Audio sensory input)pbdoc")
                .def(py::init<>())
                .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
                .def("setData", [](Audio& self, const std::vector<float>& data) {
                    self.setData(data);
                }, py::arg("data"))
                .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
                .def("getSampleRate", &Audio::getSampleRate)
                .def("getNumSamples", &Audio::getNumSamples);
            
            // InternalSignals class
            builder.class_<InternalSignals, SensoryInput>("InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
                .def(py::init<>())
                .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
                .def("clearSignals", &InternalSignals::clearSignals);
            
            // Context manager support for sensory inputs
            builder.class_<Vision>("VisionContext", R"pbdoc(Context manager for Vision operations)pbdoc")
                .def(py::init<Vision>())
                .def("enter", [](Vision& self) { return self; })
                .def("exit", [](Vision&, const py::args& args) { return true; });
            
            builder.class_<Audio>("AudioContext", R"pbdoc(Context manager for Audio operations)pbdoc")
                .def(py::init<Audio>())
                .def("enter", [](Audio& self) { return self; })
                .def("exit", [](Audio&, const py::args& args) { return true; });
            
            // Convenience functions
            m.def("createVision", [](size_t width, size_t height, size_t channels) {
                return std::make_shared<Vision>(width, height, channels);
            }, py::arg("width"), py::arg("height"), py::arg("channels") = 3,
                "Create a new Vision sensory input object");
            
            m.def("createAudio", [](size_t sampleRate, size_t numSamples) {
                return std::make_shared<Audio>(sampleRate, numSamples);
            }, py::arg("sampleRate"), py::arg("numSamples"),
                "Create a new Audio sensory input object");
            
            m.def("createInternalSignals", []() {
                return std::make_shared<InternalSignals>();
            }, "Create a new InternalSignals sensory input object");
            
            // Sensory input factory
            m.def("createSensoryInput", [](const std::string& type) {
                if (type == "vision") {
                    return std::make_shared<Vision>();
                } else if (type == "audio") {
                    return std::make_shared<Audio>(44100, 0);
                } else if (type == "internal") {
                    return std::make_shared<InternalSignals>();
                }
                throw std::invalid_argument("Unknown sensory input type: " + type);
            }, py::arg("type"), "Create sensory input based on type");
            
            // Data manipulation utilities
            m.def("normalize_vision_data", [](Vision& vision, float min = 0.0f, float max = 1.0f) {
                auto data = vision.getData();
                for (auto& value : data) {
                    value = (value - min) / (max - min);
                }
                vision.setData(data);
            }, py::arg("vision"), py::arg("min") = 0.0f, py::arg("max") = 1.0f,
                "Normalize vision data to range [0,1]");
            
            m.def("resample_audio", [](Audio& audio, double newSampleRate) {
                auto data = audio.getData();
                size_t newNumSamples = static_cast<size_t>(data.size() * newSampleRate / audio.getSampleRate());
                audio.setSampleRate(static_cast<size_t>(newSampleRate));
                audio.setData(std::vector<float>(newNumSamples, 0.0f));
            }, py::arg("audio"), py::arg("newSampleRate"),
                "Resample audio to new sample rate");
        }
        
        void initialize() override {
            // Initialize sensory module
        }
        
        void shutdown() override {
            // Cleanup sensory module resources
        }
        
        bool hasFeature(const std::string& feature) const override {
            return feature == "data_processing" || feature == "conversion" || 
                   feature == "formatting" || feature == "validation" ||
                   feature == "pythonic_api" || feature == "context_managers";
        }
        
        py::object getFeature(const std::string& feature) override {
            if (feature == "data_processing") {
                return py::module_::import("nlm.sensory").attr("data_processing");
            } else if (feature == "conversion") {
                return py::module_::import("nlm.sensory").attr("conversion");
            }
            throw std::runtime_error("Feature not available: " + feature);
        }
    };
    
    // Register the sensory module
    static SensoryModule register_sensory_module() {
        auto module = std::make_shared<SensoryModule>();
        BindingRegistry::getInstance().registerModule(module);
        return *module;
    }
    
    // Module registration function
    PYBIND11_MODULE(sensory_bindings, m) {
        static SensoryModule module;
        module.registerBindings(m);
        
        m.doc() = R"pbdoc(
            NLM Sensory Python Bindings
            ----------------------------
            
            This module provides Python bindings for the NLM sensory system,
            including Vision, Audio, and InternalSignals classes.
            
            It provides sensory input interfaces for visual data, audio signals,
            and internal neural signals, with context managers and data processing
            utilities.
            
            .. currentmodule:: sensory_bindings
            
            Sensory API
            ============""";
        
        // Register with the central registry
        BindingRegistry::getInstance().registerModule(std::make_shared<SensoryModule>(module));
    }
}