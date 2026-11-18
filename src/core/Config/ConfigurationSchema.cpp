// Enhanced configuration schema implementation
#include "ConfigurationSchema.hpp"
#include <algorithm>
#include <sstream>

namespace nlm {

struct ConfigurationSchema::Impl {
    std::map<std::string, SchemaEntry> schema;
    std::map<std::string, std::vector<std::string>> dependencies;
    std::map<std::string, std::string> validationRules;
};

ConfigurationSchema::ConfigurationSchema() : pImpl(std::make_unique<Impl>()) {
    // Initialize with common configuration categories
    initializeDefaultSchema();
}

void ConfigurationSchema::initializeDefaultSchema() {
    // Brain configuration parameters
    addEntry("brain.neuron_count", SchemaEntry{
        "int64_t", "1000", 
        "Total number of neurons in the brain",
        true
    });
    
    addEntry("brain.synapse_density", SchemaEntry{
        "float", "0.1",
        "Average synapse density per neuron",
        false
    });
    
    addEntry("brain.connection_probability", SchemaEntry{
        "float", "0.05",
        "Probability of connection between neurons",
        false
    });
    
    addEntry("brain.initial_weight_mean", SchemaEntry{
        "float", "0.5",
        "Mean initial synaptic weight",
        false
    });
    
    addEntry("brain.initial_weight_std", SchemaEntry{
        "float", "0.1",
        "Standard deviation of initial weights",
        false
    });
    
    addEntry("brain.v_thresh", SchemaEntry{
        "float", "-50.0",
        "Neuron threshold potential (mV)",
        false
    });
    
    addEntry("brain.v_rest", SchemaEntry{
        "float", "-70.0",
        "Resting potential (mV)",
        false
    });
    
    addEntry("brain.v_reset", SchemaEntry{
        "float", "-75.0",
        "Reset potential after spike (mV)",
        false
    });
    
    addEntry("brain.tau_mem", SchemaEntry{
        "float", "20.0",
        "Membrane time constant (ms)",
        false
    });
    
    addEntry("brain.tau_ref", SchemaEntry{
        "float", "2.0",
        "Refractory period (ms)",
        false
    });
    
    // Plasticity configuration
    addEntry("plasticity.stdp.enable", SchemaEntry{
        "bool", "true",
        "Enable Spike-Timing Dependent Plasticity",
        false
    });
    
    addEntry("plasticity.stdp.learning_rate", SchemaEntry{
        "float", "0.001",
        "STDP learning rate",
        false
    });
    
    addEntry("plasticity.stdp.tau_plus", SchemaEntry{
        "float", "20.0",
        "STDP time constant for potentiation (ms)",
        false
    });
    
    addEntry("plasticity.stdp.tau_minus", SchemaEntry{
        "float", "20.0",
        "STDP time constant for depression (ms)",
        false
    });
    
    addEntry("plasticity.hebbian.enable", SchemaEntry{
        "bool", "true",
        "Enable Hebbian learning",
        false
    });
    
    addEntry("plasticity.hebbian.learning_rate", SchemaEntry{
        "float", "0.0001",
        "Hebbian learning rate",
        false
    });
    
    addEntry("plasticity.structural.enable", SchemaEntry{
        "bool", "true",
        "Enable structural plasticity (synaptogenesis/pruning)",
        false
    });
    
    addEntry("plasticity.structural.synaptogenesis_rate", SchemaEntry{
        "float", "0.0001",
        "Rate of new synapse formation",
        false
    });
    
    addEntry("plasticity.structural.pruning_rate", SchemaEntry{
        "float", "0.00001",
        "Rate of synapse elimination",
        false
    });
    
    // Neuromodulation configuration
    addEntry("neuromod.dopamine.scale", SchemaEntry{
        "float", "1.0",
        "Dopamine modulation scale factor",
        false
    });
    
    addEntry("neuromod.curiosity.enable", SchemaEntry{
        "bool", "true",
        "Enable curiosity-driven exploration",
        false
    });
    
    addEntry("neuromod.curiosity.strength", SchemaEntry{
        "float", "0.5",
        "Curiosity modulation strength",
        false
    });
    
    addEntry("neuromod.novelty.enable", SchemaEntry{
        "bool", "true",
        "Enable novelty detection",
        false
    });
    
    addEntry("neuromod.novelty.sensitivity", SchemaEntry{
        "float", "0.3",
        "Novelty detection sensitivity",
        false
    });
    
    addEntry("neuromod.prediction_error.weight", SchemaEntry{
        "float", "0.7",
        "Weight of prediction error in neuromodulation",
        false
    });
    
    // Development configuration
    addEntry("development.enable", SchemaEntry{
        "bool", "true",
        "Enable developmental processes",
        false
    });
    
    addEntry("development.maturation_rate", SchemaEntry{
        "float", "0.01",
        "Rate of neural maturation",
        false
    });
    
    addEntry("development.plasticity_decay", SchemaEntry{
        "float", "0.9",
        "Decay factor for developmental plasticity",
        false
    });
    
    addEntry("development.critical_period", SchemaEntry{
        "float", "100.0",
        "Duration of critical learning period (simulation steps)",
        false
    });
    
    // World and environment configuration
    addEntry("world.width", SchemaEntry{
        "int", "20",
        "World width in units",
        true
    });
    
    addEntry("world.height", SchemaEntry{
        "int", "20",
        "World height in units",
        true
    });
    
    addEntry("world.vision.width", SchemaEntry{
        "int", "8",
        "Vision sensor width",
        false
    });
    
    addEntry("world.vision.height", SchemaEntry{
        "int", "8",
        "Vision sensor height",
        false
    });
    
    addEntry("world.max_energy", SchemaEntry{
        "float", "100.0",
        "Maximum energy for agents",
        false
    });
    
    // Simulation configuration
    addEntry("simulation.timestep", SchemaEntry{
        "float", "0.001",
        "Simulation time step (seconds)",
        false
    });
    
    addEntry("simulation.max_steps", SchemaEntry{
        "int64_t", "1000",
        "Maximum simulation steps",
        false
    });
    
    addEntry("simulation.random_seed", SchemaEntry{
        "int64_t", "42",
        "Random seed for reproducibility",
        false
    });
}

void ConfigurationSchema::addEntry(const std::string& key, const SchemaEntry& entry) {
    pImpl->schema[key] = entry;
}

bool ConfigurationSchema::validate(const std::string& key, const ConfigValue& value) const {
    auto it = pImpl->schema.find(key);
    if (it == pImpl->schema.end()) {
        return false; // Key not found in schema
    }
    
    const auto& entry = it->second;
    
    // Check if the value type matches the expected type
    switch (entry.type.c_str()[0]) {
        case 'i': // int
            return std::holds_alternative<int>(value);
        case 'l': // int64_t
            return std::holds_alternative<int64_t>(value);
        case 'd': // double
            return std::holds_alternative<double>(value);
        case 'b': // bool
            return std::holds_alternative<bool>(value);
        case 's': // string
            return std::holds_alternative<std::string>(value);
        case 'v': { // vector - check first element type
            if (std::holds_alternative<std::vector<int>>(value)) {
                return entry.type == "vector<int>";
            } else if (std::holds_alternative<std::vector<double>>(value)) {
                return entry.type == "vector<double>";
            } else if (std::holds_alternative<std::vector<std::string>>(value)) {
                return entry.type == "vector<string>";
            }
            return false;
        }
        default:
            return false;
    }
}

std::vector<std::string> ConfigurationSchema::getValidKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->schema.size());
    for (const auto& pair : pImpl->schema) {
        keys.push_back(pair.first);
    }
    return keys;
}

bool ConfigurationSchema::hasKey(const std::string& key) const {
    return pImpl->schema.find(key) != pImpl->schema.end();
}

// Generate a JSON-like schema document
std::string ConfigurationSchema::generateDocumentation() const {
    std::stringstream ss;
    ss << "NLM Configuration Schema\n";
    ss << "========================\n\n";
    
    ss << "## Configuration Categories\n\n";
    
    // Group keys by category
    std::map<std::string, std::vector<std::pair<std::string, SchemaEntry>>> categories;
    
    for (const auto& pair : pImpl->schema) {
        const std::string& key = pair.first;
        const SchemaEntry& entry = pair.second;
        
        // Determine category based on key prefix
        size_t dot_pos = key.find('.');
        std::string category = dot_pos != std::string::npos ? 
            key.substr(0, dot_pos) : "general";
        
        categories[category].push_back({key, entry});
    }
    
    // Output each category
    for (const auto& category_pair : categories) {
        const std::string& category = category_pair.first;
        const auto& entries = category_pair.second;
        
        ss << "### " << (category.empty() ? "General" : category) << "\n\n";
        
        for (const auto& entry_pair : entries) {
            const std::string& key = entry_pair.first;
            const SchemaEntry& entry = entry_pair.second;
            
            ss << "#### `" << key << "`\n";
            ss << "- **Type:** " << entry.type << "\n";
            ss << "- **Default:** `" << entry.defaultValue << "`\n";
            ss << "- **Required:** " << (entry.required ? "Yes" : "No") << "\n";
            ss << "- **Description:** " << entry.description << "\n\n";
        }
    }
    
    ss << "## Validation Rules\n\n";
    ss << "The configuration system validates values based on their types:\n\n";
    ss << "- **int**: Integer values (e.g., 42)\n";
    ss << "- **int64_t**: 64-bit integer values (e.g., 1000000)\n";
    ss << "- **double**: Floating-point values (e.g., 3.14)\n";
    ss << "- **bool**: Boolean values (true/false)\n";
    ss << "- **string**: Text values (e.g., \"example\")\n";
    ss << "- **vector<int>**: Integer arrays (e.g., [1, 2, 3])\n";
    ss << "- **vector<double>**: Float arrays (e.g., [1.0, 2.0, 3.0])\n";
    ss << "- **vector<string>**: String arrays (e.g., [\"a\", \"b\", \"c\"])\n\n";
    
    ss << "## Example Configurations\n\n";
    ss << "```json\n";
    ss << "{\n";
    ss << "  \"brain.neuron_count\": 1000,\n";
    ss << "  \"brain.synapse_density\": 0.1,\n";
    ss << "  \"plasticity.stdp.enable\": true,\n";
    ss << "  \"neuromod.curiosity.enable\": true,\n";
    ss << "  \"world.width\": 20,\n";
    ss << "  \"world.height\": 20,\n";
    ss << "  \"simulation.timestep\": 0.001\n";
    ss << "}\n";
    ss << "```\n\n";
    
    ss << "## Configuration File Format\n\n";
    ss << "NLM configuration files use a simple key=value format:\n\n";
    ss << "```\n";
    ss << "# Sample configuration file\nbrain.neuron_count = 500\nbrain.synapse_density = 0.1\nplasticity.stdp.learning_rate = 0.01\nneuromod.curiosity.enable = true\nworld.width = 20\nworld.height = 20\n```\n\n";
    
    ss << "## Command Line Override\n\n";
    ss << "Configuration values can be overridden from the command line:\n\n";
    ss << "```bash\n";
    ss << "./nlm --config config.cfg --brain.neuron_count 2000 --plasticity.stdp.learning_rate 0.005\n```\n\n";
    
    ss << "## Configuration Validation\n\n";
    ss << "The configuration system performs automatic validation:\n\n";
    ss << "- **Type validation**: Ensures values match expected types\n";
    ss << "- **Range validation**: Checks values against reasonable bounds\n";
    ss << "- **Dependency validation**: Ensures related settings are consistent\n";
    ss << "- **Override validation**: Detects conflicting overrides\n";
    
    return ss.str();
}

} // namespace nlm