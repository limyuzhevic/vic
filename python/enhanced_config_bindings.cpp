#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "core/Config/Config.hpp"

namespace py = pybind11;

void export_config_to_python(py::module& m) {
    // Export Config class
    py::class_<Config, std::shared_ptr<Config>>(m, "Config")
        .def(py::init<>())
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON or YAML file")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a simple file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", &Config::get<py::args>(), py::arg("key"), py::arg("default_value") = nullptr,
             "Get configuration value with default")
        .def("set", &Config::set<py::args>(), py::arg("key"), py::arg("value"), py::arg("source") = 3,
             "Set configuration value")
        .def("get_keys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("validate", &Config::validate,
             "Validate configuration and return True if valid");
    
    // Factory functions
    m.def("create_config", &Config::create,
          "Create a new configuration object");
    
    m.def("create_config_from_preset", &Config::createFromPreset,
          py::arg("preset_name"), "Create configuration from preset");
    
    m.def("validate_config", &Config::validateConfig,
          py::arg("config"), "Validate configuration");
    
    // Preset configurations
    m.def("apply_preset", [](std::shared_ptr<Config> config, const std::string& preset_name) {
        config->applyPreset(preset_name);
    }, py::arg("config"), py::arg("preset_name"), "Apply a preset configuration");
    
    // Configuration presets
    py::enum_<ConfigSource>(m, "ConfigSource", "Configuration source enumeration")
        .value("Default", ConfigSource::Default)
        .value("File", ConfigSource::File)
        .value("CommandLine", ConfigSource::CommandLine)
        .value("Runtime", ConfigSource::Runtime)
        .export_values();
}

PYBIND11_MODULE(pynlm_enhanced_config, m) {
    m.doc() = R"pbdoc(
        Enhanced Configuration Module for NLM
        -------------------------------------
        
        Advanced configuration management with support for:
        - JSON and YAML configuration files
        - Preset configurations (minimal, small, medium, large, learning, development)
        - Configuration validation with detailed error messages
        - Environment variable expansion
        - Environment variable substitution
        - Command line argument parsing
        - Comprehensive error handling
        
        Key features:
        • Multiple file format support (JSON, YAML, simple key=value)
        • Extensive preset configurations for common use cases
        • Robust validation with detailed error reporting
        • Environment variable expansion for dynamic configuration
        • Type-safe configuration access with proper defaults
        • Comprehensive documentation and examples
        • Pythonic API design with intuitive methods
        
        Usage examples:
        >>> import pynlm_enhanced_config as config
        >>> cfg = config.create_config()
        >>> cfg.apply_preset("learning")
        >>> cfg.set("neuron_count", 1000)
        >>> cfg.set("learning_rate", 0.01)
        >>> result = cfg.validate()
        >>> if result:
        ...     print("Configuration is valid!")
    )pbdoc";
    
    // Export enhanced configuration functionality
    export_config_to_python(m);
    
    // Configuration preset enumeration
    py::enum_<ConfigSource>(m, "ConfigSource", "Configuration source enumeration")
        .value("Default", ConfigSource::Default)
        .value("File", ConfigSource::File)
        .value("CommandLine", ConfigSource::CommandLine)
        .value("Runtime", ConfigSource::Runtime)
        .export_values();
    
    // Preset configuration names
    m.attr("PRESET_MINIMAL") = "minimal";
    m.attr("PRESET_SMALL") = "small";
    m.attr("PRESET_MEDIUM") = "medium";
    m.attr("PRESET_LARGE") = "large";
    m.attr("PRESET_LEARNING") = "learning";
    m.attr("PRESET_DEVELOPMENT") = "development";
    
    // Platform information
    m.def("get_platform_info", []() {
        return std::string("NLM Enhanced Configuration v1.0.0");
    }, "Get platform information");
    
    m.attr("__version__") = "1.0.0";
    m.attr("__author__") = "NLM Team";
    m.attr("__license__") = "MIT";
}
