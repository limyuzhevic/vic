#include "core_bindings.h"

namespace nlm {
    
    class CoreModule : public BindingModule {
    public:
        std::string getName() const override { return "core"; }
        std::string getVersion() const override { return "1.0.0"; }
        std::string getDescription() const override { 
            return "Core NLM types and utilities for NeuronId, SynapseId, RegionId, PopulationId"; 
        }
        
        void registerBindings(pybind11::module_& m) override {
            // Basic type bindings using ModuleBuilder pattern
            auto builder = ModuleBuilder(m);
            
            // NeuronId bindings
            builder.class_<NeuronId>("NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
                .def(py::init<>())
                .def(py::init<uint64_t>(), py::arg("value"))
                .def_readwrite("value", &NeuronId::value)
                .def("index", &NeuronId::index)
                .def("__eq__", &NeuronId::operator==)
                .def("__ne__", &NeuronId::operator!=)
                .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
                .def("__repr__", [](const NeuronId& id) {
                    return "<NeuronId: " + std::to_string(id.value) + ">";
                });
            
            // SynapseId bindings
            builder.class_<SynapseId>("SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
                .def(py::init<>())
                .def(py::init<uint64_t>(), py::arg("value"))
                .def_readwrite("value", &SynapseId::value)
                .def("index", &SynapseId::index)
                .def("__eq__", &SynapseId::operator==)
                .def("__ne__", &SynapseId::operator!=)
                .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
                .def("__repr__", [](const SynapseId& id) {
                    return "<SynapseId: " + std::to_string(id.value) + ">";
                });
            
            // RegionId bindings
            builder.class_<RegionId>("RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
                .def(py::init<>())
                .def(py::init<uint64_t>(), py::arg("value"))
                .def_readwrite("value", &RegionId::value)
                .def("index", &RegionId::index)
                .def("__eq__", &RegionId::operator==)
                .def("__ne__", &RegionId::operator!=)
                .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
                .def("__repr__", [](const RegionId& id) {
                    return "<RegionId: " + std::to_string(id.value) + ">";
                });
            
            // PopulationId bindings
            builder.class_<PopulationId>("PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
                .def(py::init<>())
                .def(py::init<uint64_t>(), py::arg("value"))
                .def_readwrite("value", &PopulationId::value)
                .def("index", &PopulationId::index)
                .def("__eq__", &PopulationId::operator==)
                .def("__ne__", &PopulationId::operator!=)
                .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); });
            
            // Context manager support for ID types
            builder.class_<NeuronId>("NeuronIdContext", R"pbdoc(Context manager for NeuronId operations)pbdoc")
                .def(py::init<NeuronId>())
                .def("enter", [](NeuronId& self) { return self; })
                .def("exit", [](NeuronId&, const py::args& args) { return true; });
            
            // Property accessors
            builder.class_<NeuronId>("NeuronIdProperties")
                .def_property_readonly("is_valid", [](const NeuronId& id) { return id.value != UINT64_MAX; })
                .def_property_readonly("index", &NeuronId::index);
            
            // Module-level constants
            m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
            m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
            m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
            m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
            
            // Type conversion utilities
            m.def("to_int", &type_conversion::stringToInt, "Convert string to integer",
                  py::arg("str"));
            m.def("to_float", &type_conversion::stringToDouble, "Convert string to float",
                  py::arg("str"));
            m.def("to_bool", &type_conversion::stringToBool, "Convert string to boolean",
                  py::arg("str"));
        }
        
        void initialize() override {
            // Initialize core module
        }
        
        void shutdown() override {
            // Cleanup core module resources
        }
        
        bool hasFeature(const std::string& feature) const override {
            return feature == "type_conversion" || feature == "context_managers" || 
                   feature == "properties" || feature == "basic_types";
        }
        
        py::object getFeature(const std::string& feature) override {
            if (feature == "type_conversion") {
                return py::module_::import("nlm.core").attr("type_conversion");
            } else if (feature == "context_managers") {
                return py::module_::import("nlm.core").attr("context_managers");
            }
            throw std::runtime_error("Feature not available: " + feature);
        }
    };
    
    // Register the core module
    static CoreModule register_core_module() {
        auto module = std::make_shared<CoreModule>();
        BindingRegistry::getInstance().registerModule(module);
        return *module;
    }
    
    // Module registration function
    PYBIND11_MODULE(core_bindings, m) {
        static CoreModule module;
        module.registerBindings(m);
        
        m.doc() = R"pbdoc(
            Core NLM Python Bindings
            ---------------------------
            
            This module provides basic type bindings for NLM (Neural Learning Machine),
            including NeuronId, SynapseId, RegionId, and PopulationId identifiers.
            
            It also provides utility functions for type conversion and context managers
            for safe resource management.
            
            .. currentmodule:: core_bindings
            
            Core module API
            =================""";
        
        // Register with the central registry
        BindingRegistry::getInstance().registerModule(std::make_shared<CoreModule>(module));
    }
}