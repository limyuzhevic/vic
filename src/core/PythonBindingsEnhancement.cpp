#include "PythonBindingsEnhancement.hpp"

namespace nlm {
namespace python {

void extendPythonModule(pybind11::module_& m) {
    // Add enhanced configuration utilities
    m.def("createDefaultConfig", &PythonConfigUtils::createDefaultConfig,
          "Create a default NLM configuration with Phase 6 settings");
    
    m.def("loadConfigFromJSON", &PythonConfigUtils::loadFromJSON,
          pybind11::arg("json_string"), "Load configuration from JSON string");
    
    m.def("configToJSON", &PythonConfigUtils::configToJSON,
          pybind11::arg("config"), "Convert configuration to JSON string");
    
    m.def("getSystemInfo", &PythonConfigUtils::getSystemInfo,
          "Get system information and capabilities");
    
    // Add utility functions
    m.def("formatNeuronId", &utils::formatNeuronId, pybind11::arg("id"),
          "Format NeuronId for Python display");
    
    m.def("formatSynapseId", &utils::formatSynapseId, pybind11::arg("id"),
          "Format SynapseId for Python display");
    
    m.def("formatRegionId", &utils::formatRegionId, pybind11::arg("id"),
          "Format RegionId for Python display");
    
    m.def("formatDevelopmentStage", &utils::formatDevelopmentStage, pybind11::arg("stage"),
          "Format DevelopmentalStage as string");
    
    m.def("formatNeuronType", &utils::formatNeuronType, pybind11::arg("type"),
          "Format NeuronType as string");
    
    m.def("formatActionType", &utils::formatActionType, pybind11::arg("type"),
          "Format ActionType as string");
    
    m.def("formatWorldObjectType", &utils::formatWorldObjectType, pybind11::arg("type"),
          "Format WorldObjectType as string");
    
    m.def("generateRandomSeed", &utils::generateRandomSeed,
          "Generate a random seed for configuration");
    
    m.def("split", &utils::split, pybind11::arg("str"), pybind11::arg("delimiter") = '.', 
          "Split string by delimiter");
    
    m.def("join", &utils::join, pybind11::arg("strings"), pybind11::arg("delimiter") = " ",
          "Join vector of strings with delimiter");
}

} // namespace python
} // namespace nlm
