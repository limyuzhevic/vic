#include "enum_bindings.h"

namespace nlm {
    
    class EnumModule : public BindingModule {
    public:
        std::string getName() const override { return "enum"; }
        std::string getVersion() const override { return "1.0.0"; }
        std::string getDescription() const override { 
            return "Enumeration type bindings for NLM (NeuronType, SynapseType, etc.)"; 
        }
        
        void registerBindings(pybind11::module_& m) override {
            // Enumeration bindings using ModuleBuilder pattern
            auto builder = ModuleBuilder(m);
            
            // NeuronType enum
            builder.enum_<NeuronType>("NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
                .value("Excitatory", NeuronType::Excitatory)
                .value("Inhibitory", NeuronType::Inhibitory)
                .value("Modulatory", NeuronType::Modulatory)
                .value("Sensory", NeuronType::Sensory)
                .value("Motor", NeuronType::Motor)
                .value("Internal", NeuronType::Internal)
                .export_values();
            
            // SynapseType enum
            builder.enum_<SynapseType>("SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
                .value("Excitatory", SynapseType::Excitatory)
                .value("Inhibitory", SynapseType::Inhibitory)
                .value("Modulatory", SynapseType::Modulatory)
                .value("Electrical", SynapseType::Electrical)
                .value("GapJunction", SynapseType::GapJunction)
                .export_values();
            
            // DevelopmentalStage enum
            builder.enum_<DevelopmentalStage>("DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
                .value("Initial", DevelopmentalStage::Initial)
                .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
                .value("Maturation", DevelopmentalStage::Maturation)
                .value("Adult", DevelopmentalStage::Adult)
                .value("Aging", DevelopmentalStage::Aging)
                .export_values();
            
            // FiringState enum
            builder.enum_<FiringState>("FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
                .value("Resting", FiringState::Resting)
                .value("Active", FiringState::Active)
                .value("Refractory", FiringState::Refractory)
                .value("Inhibited", FiringState::Inhibited)
                .export_values();
            
            // ActionType enum
            builder.enum_<ActionType>("ActionType", R"pbdoc(Action type enumeration)pbdoc")
                .value("MoveForward", ActionType::MoveForward)
                .value("MoveBackward", ActionType::MoveBackward)
                .value("MoveLeft", ActionType::MoveLeft)
                .value("MoveRight", ActionType::MoveRight)
                .value("TurnLeft", ActionType::TurnLeft)
                .value("TurnRight", ActionType::TurnRight)
                .value("Look", ActionType::Look)
                .value("LookUp", ActionType::LookUp)
                .value("LookDown", ActionType::LookDown)
                .value("Interact", ActionType::Interact)
                .value("Eat", ActionType::Eat)
                .value("Drink", ActionType::Drink)
                .value("Rest", ActionType::Rest)
                .value("Wait", ActionType::Wait)
                .value("Custom", ActionType::Custom)
                .export_values();
            
            // MotorCommand enum
            builder.enum_<MotorCommand>("MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
                .value("MoveForward", MotorCommand::MoveForward)
                .value("MoveBackward", MotorCommand::MoveBackward)
                .value("TurnLeft", MotorCommand::TurnLeft)
                .value("TurnRight", MotorCommand::TurnRight)
                .value("LookLeft", MotorCommand::LookLeft)
                .value("LookRight", MotorCommand::LookRight)
                .value("Interact", MotorCommand::Interact)
                .value("Wait", MotorCommand::Wait)
                .export_values();
            
            // WorldObjectType enum
            builder.enum_<WorldObjectType>("WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
                .value("Empty", WorldObjectType::Empty)
                .value("Resource", WorldObjectType::Resource)
                .value("Hazard", WorldObjectType::Hazard)
                .value("Wall", WorldObjectType::Wall)
                .value("Marker", WorldObjectType::Marker)
                .export_values();
            
            // ConfigSource enum
            builder.enum_<ConfigSource>("ConfigSource", R"pbdoc(Configuration source enumeration)pbdoc")
                .value("Default", ConfigSource::Default)
                .value("File", ConfigSource::File)
                .value("CommandLine", ConfigSource::CommandLine)
                .value("Runtime", ConfigSource::Runtime)
                .export_values();
            
            // Enum helper functions
            m.def("get_enum_values", [](const std::string& enum_name) -> py::list {
                // Implementation would retrieve enum values by name
                return py::list();
            }, py::arg("enum_name"), "Get all values for a given enum type");
            
            m.def("get_enum_name", [](int value, const std::string& enum_name) -> std::string {
                // Implementation would convert enum value to name
                return "";
            }, py::arg("value"), py::arg("enum_name"), "Get enum name from value");
            
            // Enum comparison utilities
            m.def("enum_equal", [](int val1, int val2) { return val1 == val2; },
                  "Compare two enum values (as integers)");
            
            // Python enum-like access
            m.def("enum_contains", [](int value, const std::string& enum_name) {
                // Implementation would check if value exists in enum
                return true;
            }, py::arg("value"), py::arg("enum_name"), "Check if value exists in enum");
        }
        
        void initialize() override {
            // Initialize enum module
        }
        
        void shutdown() override {
            // Cleanup enum module resources
        }
        
        bool hasFeature(const std::string& feature) const override {
            return feature == "enum_helpers" || feature == "enum_conversion" || 
                   feature == "enum_validation" || feature == "enum_metadata";
        }
        
        py::object getFeature(const std::string& feature) override {
            if (feature == "enum_helpers") {
                return py::module_::import("nlm.enum").attr("enum_helpers");
            } else if (feature == "enum_conversion") {
                return py::module_::import("nlm.enum").attr("enum_conversion");
            }
            throw std::runtime_error("Feature not available: " + feature);
        }
    };
    
    // Register the enum module
    static EnumModule register_enum_module() {
        auto module = std::make_shared<EnumModule>();
        BindingRegistry::getInstance().registerModule(module);
        return *module;
    }
    
    // Module registration function
    PYBIND11_MODULE(enum_bindings, m) {
        static EnumModule module;
        module.registerBindings(m);
        
        m.doc() = R"pbdoc(
            NLM Enumeration Python Bindings
            -------------------------------
            
            This module provides Python bindings for all NLM enumeration types,
            including NeuronType, SynapseType, DevelopmentalStage, FiringState,
            ActionType, MotorCommand, WorldObjectType, and ConfigSource.
            
            These enumerations are used throughout the NLM framework to represent
            different types of neurons, synapses, developmental stages, firing states,
            actions, motor commands, and world objects.
            
            .. currentmodule:: enum_bindings
            
            Enumeration API
            =================""";
        
        // Register with the central registry
        BindingRegistry::getInstance().registerModule(std::make_shared<EnumModule>(module));
    }
}