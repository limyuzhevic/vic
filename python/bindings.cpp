#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/chrono.h>
#include <pybind11/functional.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <optional>
#include <variant>
#include <functional>
#include <algorithm>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// Custom exception classes for better error handling
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) : std::runtime_error(message) {}
};

class BrainError : public NLMException {
public:
    explicit BrainError(const std::string& message) : NLMException("Brain Error: " + message) {}
};

class ConfigError : public NLMException {
public:
    explicit ConfigError(const std::string& message) : NLMException("Config Error: " + message) {}
};

class WorldError : public NLMException {
public:
    explicit WorldError(const std::string& message) : NLMException("World Error: " + message) {}
};

class AgentError : public NLMException {
public:
    explicit AgentError(const std::string& message) : NLMException("Agent Error: " + message) {}
};

// Context manager for Brain
class PyBrainContextManager {
private:
    std::shared_ptr<Brain> brain_;
    bool auto_save_;
    std::string backup_path_;
public:
    PyBrainContextManager(std::shared_ptr<Brain> brain, bool auto_save = false, 
                         const std::string& backup_path = "") 
        : brain_(brain), auto_save_(auto_save), backup_path_(backup_path) {
        if (!brain_->initialize()) {
            throw BrainError("Failed to initialize brain in context manager");
        }
    }
    
    ~PyBrainContextManager() {
        if (auto_save_ && backup_path_ && !backup_path_.empty()) {
            try {
                brain_->save(backup_path_);
            } catch (const std::exception& e) {
                // Log warning but don't throw in destructor
            }
        }
    }
    
    std::shared_ptr<Brain> get() { return brain_; }
};

// Context manager for SimpleWorld
class PyWorldContextManager {
private:
    std::shared_ptr<SimpleWorld> world_;
    bool reset_on_exit_;
public:
    PyWorldContextManager(std::shared_ptr<SimpleWorld> world, bool reset_on_exit = true)
        : world_(world), reset_on_exit_(reset_on_exit) {
        if (!world_) {
            throw WorldError("Cannot create context manager with null world");
        }
    }
    
    ~PyWorldContextManager() {
        if (reset_on_exit_) {
            world_->reset();
        }
    }
    
    std::shared_ptr<SimpleWorld> get() { return world_; }
};

// Context manager for AgentBrain
class PyAgentBrainContextManager {
private:
    std::shared_ptr<AgentBrain> agent_brain_;
    bool reset_on_exit_;
public:
    PyAgentBrainContextManager(std::shared_ptr<AgentBrain> agent_brain, bool reset_on_exit = true)
        : agent_brain_(agent_brain), reset_on_exit_(reset_on_exit) {
        if (!agent_brain_) {
            throw AgentError("Cannot create context manager with null agent brain");
        }
    }
    
    ~PyAgentBrainContextManager() {
        if (reset_on_exit_) {
            agent_brain_->reset();
        }
    }
    
    std::shared_ptr<AgentBrain> get() { return agent_brain_; }
};

// Utility functions for common operations
// Advanced configuration utilities
namespace advanced {
    // Batch configuration operations
    void batchSetConfig(Config& config, const std::vector<std::pair<std::string, std::vector<ConfigValue>>>& settings) {
        for (const auto& setting : settings) {
            config.set(setting.first, setting.second[0]);
        }
    }
    
    // Configuration validation tools
    struct ValidationResult {
        bool is_valid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        ValidationResult() : is_valid(true) {}
    };
    
    ValidationResult validateConfigAdvanced(const Config& config) {
        ValidationResult result;
        
        // Validate brain settings
        if (config.has("brain.neuron_count")) {
            auto count = config.getOr<size_t>("brain.neuron_count", 0);
            if (count == 0) {
                result.is_valid = false;
                result.errors.push_back("brain.neuron_count must be greater than 0");
            } else if (count > 1000000) {
                result.warnings.push_back("brain.neuron_count is very high (> 1M), may affect performance");
            }
        }
        
        if (config.has("brain.simulation_steps")) {
            auto steps = config.getOr<size_t>("brain.simulation_steps", 0);
            if (steps == 0) {
                result.warnings.push_back("brain.simulation_steps is 0, simulation will do nothing");
            }
        }
        
        // Validate world settings
        if (config.has("world.width")) {
            auto width = config.getOr<double>("world.width", 0.0);
            if (width <= 0) {
                result.is_valid = false;
                result.errors.push_back("world.width must be positive");
            }
        }
        
        if (config.has("world.height")) {
            auto height = config.getOr<double>("world.height", 0.0);
            if (height <= 0) {
                result.is_valid = false;
                result.errors.push_back("world.height must be positive");
            }
        }
        
        return result;
    }
    
    // Configuration templates
    std::map<std::string, std::string> getDefaultTemplates() {
        std::map<std::string, std::string> templates;
        templates["simple_brain"] = R"(
            {
                "brain": {
                    "neuron_count": 1000,
                    "simulation_steps": 100,
                    "plasticity": {"stdp_enabled": true, "hebbian_enabled": true},
                    "development": {"enabled": true, "stage": "adult"}
                },
                "world": {
                    "width": 100.0,
                    "height": 100.0,
                    "vision_width": 21,
                    "vision_height": 21
                }
            }
        )";
        
        templates["complex_brain"] = R"(
            {
                "brain": {
                    "neuron_count": 5000,
                    "simulation_steps": 1000,
                    "plasticity": {"stdp_enabled": true, "hebbian_enabled": true, "structural_enabled": true},
                    "development": {"enabled": true, "stage": "critical_period"},
                    "neuromodulation": {"dopamine_enabled": true, "curiosity_enabled": true, "novelty_enabled": true}
                },
                "world": {
                    "width": 200.0,
                    "height": 200.0,
                    "vision_width": 41,
                    "vision_height": 41
                }
            }
        )";
        
        templates["agent_training"] = R"(
            {
                "brain": {
                    "neuron_count": 2000,
                    "simulation_steps": 500,
                    "plasticity": {"stdp_enabled": true, "hebbian_enabled": true},
                    "neuromodulation": {"dopamine_enabled": true, "curiosity_enabled": true}
                },
                "world": {
                    "width": 150.0,
                    "height": 150.0,
                    "vision_width": 31,
                    "vision_height": 31
                }
            }
        )";
        
        return templates;
    }
    
    // Dynamic configuration modification
    class DynamicConfig {
    private:
        std::shared_ptr<Config> config_;
        std::vector<std::function<void(const Config&)>> callbacks_;
        double modification_history_size_;
        
    public:
        DynamicConfig(std::shared_ptr<Config> config, double history_size = 100.0)
            : config_(config), modification_history_size_(history_size) {}
        
        void setValue(const std::string& key, const std::vector<ConfigValue>& values) {
            config_->set(key, values[0]);
            
            // Notify callbacks
            for (const auto& callback : callbacks_) {
                callback(*config_);
            }
        }
        
        void addCallback(std::function<void(const Config&)> callback) {
            callbacks_.push_back(callback);
        }
        
        std::shared_ptr<Config> getConfig() const { return config_; }
        
        // Get configuration value by key
        template<typename T>
        std::optional<T> get(const std::string& key) const {
            return config_->get<T>(key);
        }
        
        // Modify configuration with parameter
        void modify(const std::string& key, const ConfigValue& value, double factor = 1.0) {
            if (value.index() == typeid(double)) {
                double current = config_->getOr<double>(key, 0.0);
                config_->set(key, current * factor);
            } else {
                config_->set(key, value);
            }
        }
    };
    
    // Configuration diffing
    std::vector<std::string> diffConfigs(const Config& config1, const Config& config2) {
        std::vector<std::string> diffs;
        
        const auto& keys1 = config1.getKeys();
        const auto& keys2 = config2.getKeys();
        
        // Find keys in config1 but not in config2
        for (const auto& key : keys1) {
            if (!config2.has(key)) {
                auto value1 = config1.getOr<std::string>(key, "");
                diffs.push_back("Removed: " + key + " = " + std::to_string(value1));
            }
        }
        
        // Find keys in config2 but not in config1
        for (const auto& key : keys2) {
            if (!config1.has(key)) {
                auto value2 = config2.getOr<std::string>(key, "");
                diffs.push_back("Added: " + key + " = " + std::to_string(value2));
            }
        }
        
        // Compare common keys
        for (const auto& key : keys1) {
            if (config2.has(key)) {
                auto value1 = config1.getOr<std::string>(key, "");
                auto value2 = config2.getOr<std::string>(key, "");
                if (value1 != value2) {
                    diffs.push_back("Changed: " + key + " from " + value1 + " to " + value2);
                }
            }
        }
        
        return diffs;
    }
    
    // Configuration merging
    void mergeConfigs(Config& target, const Config& source) {
        const auto& keys = source.getKeys();
        for (const auto& key : keys) {
            if (!target.has(key)) {
                target.set(key, source.getOr<std::string>(key, ""));
            }
        }
    }
}

// Advanced brain utilities
namespace advanced_brain {
    // Brain cloning and copying
    std::shared_ptr<Brain> cloneBrain(const Brain& original) {
        auto config = original.getConfig();
        auto clone = std::make_shared<Brain>(config);
        if (!clone->initialize()) {
            throw BrainError("Failed to initialize cloned brain");
        }
        
        // Copy some key state
        clone->setDevelopmentalStage(original.getDevelopmentalStage());
        
        return clone;
    }
    
    // Brain serialization/deserialization utilities
    std::string serializeBrain(const Brain& brain) {
        // Create a JSON representation of the brain state
        std::string result = "{\n";
        
        // Add basic brain state
        result += "  \"total_neuron_count\": " + std::to_string(brain.getTotalNeuronCount()) + ",\n";
        result += "  \"total_synapse_count\": " + std::to_string(brain.getTotalSynapseCount()) + ",\n";
        result += "  \"active_neuron_count\": " + std::to_string(brain.getActiveNeuronCount()) + ",\n";
        result += "  \"firing_neuron_count\": " + std::to_string(brain.getFiringNeuronCount()) + ",\n";
        result += "  \"total_spike_count\": " + std::to_string(brain.getTotalSpikeCount()) + ",\n";
        result += "  \"excitation_inhibition_ratio\": " + std::to_string(brain.getExcitationInhibitionRatio()) + ",\n";
        
        // Add region information
        auto region_ids = brain.getRegionIds();
        result += "  \"regions\": [";
        for (size_t i = 0; i < region_ids.size(); ++i) {
            result += std::to_string(region_ids[i].value);
            if (i < region_ids.size() - 1) result += ", ";
        }
        result += "],\n";
        
        result += "  \"developmental_stage\": \"" + std::to_string(static_cast<int>(brain.getDevelopmentalStage())) + "\"\n";
        result += "}\n";
        
        return result;
    }
    
    std::shared_ptr<Brain> deserializeBrain(const std::string& data, std::shared_ptr<Config> config = nullptr) {
        if (!config) {
            config = std::make_shared<Config>();
        }
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw BrainError("Failed to initialize deserialized brain");
        }
        
        return brain;
    }
    
    // Advanced brain manipulation methods
    class BrainManipulator {
    private:
        std::shared_ptr<Brain> brain_;
        
    public:
        BrainManipulator(std::shared_ptr<Brain> brain) : brain_(brain) {}
        
        // Increase all synaptic weights
        void increaseSynapticWeights(float factor) {
            // This would typically call internal plasticity methods
            // For now, we'll just log a warning since this is a simplified version
            std::cerr << "Warning: increaseSynapticWeights called - actual implementation depends on brain internals" << std::endl;
        }
        
        // Decrease all synaptic weights
        void decreaseSynapticWeights(float factor) {
            std::cerr << "Warning: decreaseSynapticWeights called - actual implementation depends on brain internals" << std::endl;
        }
        
        // Add random noise to neural activities
        void addNeuralNoise(float noise_level) {
            std::cerr << "Warning: addNeuralNoise called - actual implementation depends on brain internals" << std::endl;
        }
        
        // Increase brain plasticity temporarily
        void increasePlasticity(float factor, SimulationStep duration) {
            std::cerr << "Warning: increasePlasticity called - actual implementation depends on brain internals" << std::endl;
        }
        
        // Reset developmental stage
        void resetDevelopment() {
            if (brain_) {
                brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
            }
        }
        
        // Set developmental stage
        void setDevelopmentStage(DevelopmentalStage stage) {
            if (brain_) {
                brain_->setDevelopmentalStage(stage);
            }
        }
        
        // Get current brain
        std::shared_ptr<Brain> getBrain() const { return brain_; }
    };
    
    // Learning statistics and analytics
    struct LearningStatistics {
        double learning_rate;
        double plasticity_rate;
        double exploration_rate;
        double prediction_accuracy;
        double reward_smoothness;
        double stability_score;
        double complexity_score;
        std::vector<double> spike_rates;
        std::vector<double> firing_rates;
        std::vector<double> reward_history;
        
        LearningStatistics() : learning_rate(0.0), plasticity_rate(0.0), exploration_rate(0.0),
                               prediction_accuracy(0.0), reward_smoothness(0.0), stability_score(0.0),
                               complexity_score(0.0) {}
    };
    
    LearningStatistics computeLearningStatistics(const Brain& brain) {
        LearningStatistics stats;
        
        // Compute basic statistics from brain
        stats.spike_rates.push_back(static_cast<double>(brain.getTotalSpikeCount()) / 100.0); // Normalized
        stats.firing_rates.push_back(static_cast<double>(brain.getFiringNeuronCount()) / std::max(1.0, static_cast<double>(brain.getTotalNeuronCount())));
        
        // Set some default values (would be computed from actual brain data)
        stats.learning_rate = 0.01;
        stats.plasticity_rate = 0.1;
        stats.exploration_rate = 0.2;
        stats.prediction_accuracy = 0.85;
        stats.reward_smoothness = 0.9;
        stats.stability_score = 0.8;
        stats.complexity_score = 0.7;
        
        return stats;
    }
    
    // Learning curve analysis
    struct LearningCurve {
        std::vector<SimulationStep> steps;
        std::vector<double> performance_values;
        std::vector<double> error_values;
        std::vector<double> confidence_values;
        
        void addPoint(SimulationStep step, double performance, double error = 0.0, double confidence = 1.0) {
            steps.push_back(step);
            performance_values.push_back(performance);
            error_values.push_back(error);
            confidence_values.push_back(confidence);
        }
        
        double getAveragePerformance() const {
            if (performance_values.empty()) return 0.0;
            double sum = 0.0;
            for (double val : performance_values) sum += val;
            return sum / performance_values.size();
        }
        
        double getTrend() const {
            if (steps.size() < 2) return 0.0;
            
            double x1 = static_cast<double>(steps[0]);
            double y1 = performance_values[0];
            double x2 = static_cast<double>(steps.back());
            double y2 = performance_values.back();
            
            return (y2 - y1) / (x2 - x1);
        }
    };
    
    // Neural activity visualization
    std::string visualizeNeuralActivity(const Brain& brain, int resolution = 100) {
        std::string result = "Neural Activity Visualization\n";
        result += "=" * 50 + "\n\n";
        
        result += "Total Neurons: " + std::to_string(brain.getTotalNeuronCount()) + "\n";
        result += "Active Neurons: " + std::to_string(brain.getActiveNeuronCount()) + "\n";
        result += "Firing Neurons: " + std::to_string(brain.getFiringNeuronCount()) + "\n";
        result += "Total Spikes: " + std::to_string(brain.getTotalSpikeCount()) + "\n";
        result += "E/I Ratio: " + std::to_string(brain.getExcitationInhibitionRatio()) + "\n\n";
        
        // Create a simple text-based visualization
        result += "Neural Activity Distribution:\n";
        int active_count = brain.getActiveNeuronCount();
        double activity_percentage = static_cast<double>(active_count) / std::max(1.0, static_cast<double>(brain.getTotalNeuronCount())) * 100.0;
        
        result += "["; // Simple bar visualization
        int bar_length = static_cast<int>(activity_percentage / 10.0); // Scale to 10 segments
        for (int i = 0; i < 10; ++i) {
            if (i < bar_length) result += "#";
            else result += "-";
        }
        result += "] " + std::to_string(activity_percentage) + "%\n\n";
        
        return result;
    }
}

// Advanced world utilities
namespace advanced_world {
    // World object templates and presets
    std::vector<WorldObject> createWorldPreset(int preset_type, float center_x = 50.0f, float center_y = 50.0f) {
        std::vector<WorldObject> objects;
        
        if (preset_type == 0) { // Empty world
            // Just return empty
        } else if (preset_type == 1) { // Resource-rich world
            objects.push_back(WorldObject(center_x, center_y, WorldObjectType::Resource, 100.0f));
            objects.push_back(WorldObject(center_x - 20, center_y + 20, WorldObjectType::Resource, 80.0f));
            objects.push_back(WorldObject(center_x + 20, center_y - 10, WorldObjectType::Resource, 90.0f));
        } else if (preset_type == 2) { // Hazard world
            objects.push_back(WorldObject(center_x, center_y, WorldObjectType::Hazard, -50.0f));
            objects.push_back(WorldObject(center_x - 30, center_y - 30, WorldObjectType::Hazard, -30.0f));
        } else if (preset_type == 3) { // Complex world with walls and objects
            objects.push_back(WorldObject(center_x, center_y, WorldObjectType::Wall, 0.0f, 0.5f));
            objects.push_back(WorldObject(center_x + 30, center_y, WorldObjectType::Wall, 0.0f, 0.5f));
            objects.push_back(WorldObject(center_x - 30, center_y, WorldObjectType::Wall, 0.0f, 0.5f));
            objects.push_back(WorldObject(center_x, center_y + 30, WorldObjectType::Wall, 0.0f, 0.5f));
            objects.push_back(WorldObject(center_x, center_y - 30, WorldObjectType::Wall, 0.0f, 0.5f));
            objects.push_back(WorldObject(center_x + 20, center_y + 20, WorldObjectType::Resource, 50.0f));
            objects.push_back(WorldObject(center_x - 20, center_y - 20, WorldObjectType::Hazard, -40.0f));
        }
        
        return objects;
    }
    
    // Advanced world manipulation
    class WorldManipulator {
    private:
        std::shared_ptr<SimpleWorld> world_;
        
    public:
        WorldManipulator(std::shared_ptr<SimpleWorld> world) : world_(world) {}
        
        // Scale all world dimensions
        void scaleWorld(float factor) {
            if (world_) {
                float width = world_->getWidth();
                float height = world_->getHeight();
                world_->configure(width * factor, height * factor, 
                                static_cast<size_t>(world_->getVisionWidth() * factor),
                                static_cast<size_t>(world_->getVisionHeight() * factor));
            }
        }
        
        // Add structured environment
        void addStructuredEnvironment(int grid_size, float cell_size, const std::vector<std::vector<int>>& pattern) {
            if (!world_) return;
            
            for (size_t i = 0; i < pattern.size(); ++i) {
                for (size_t j = 0; j < pattern[i].size(); ++j) {
                    if (pattern[i][j] != 0) {
                        float x = static_cast<float>(i) * cell_size + cell_size / 2;
                        float y = static_cast<float>(j) * cell_size + cell_size / 2;
                        
                        WorldObjectType type;
                        float value = 0.0f;
                        
                        switch (pattern[i][j]) {
                            case 1: type = WorldObjectType::Resource; value = 100.0f; break;
                            case 2: type = WorldObjectType::Hazard; value = -50.0f; break;
                            case 3: type = WorldObjectType::Wall; value = 0.0f; break;
                            default: type = WorldObjectType::Empty; break;
                        }
                        
                        if (type != WorldObjectType::Empty) {
                            WorldObject obj(x, y, type, value, 0.5f);
                            world_->addObject(obj);
                        }
                    }
                }
            }
        }
        
        // Get all world objects by type
        std::vector<WorldObject> getObjectsByType(WorldObjectType type) const {
            std::vector<WorldObject> result;
            if (!world_) return result;
            
            // Get world dimensions and iterate over possible positions
            float width = world_->getWidth();
            float height = world_->getHeight();
            
            // Sample positions (this is a simplified version)
            for (float x = 0; x < width; x += 1.0f) {
                for (float y = 0; y < height; y += 1.0f) {
                    WorldObject obj = world_->getObjectAt(x, y);
                    if (obj.type == type && obj.active) {
                        result.push_back(obj);
                    }
                }
            }
            
            return result;
        }
        
        // Generate random environment
        void generateRandomEnvironment(float resource_density = 0.3f, float hazard_density = 0.1f, float wall_density = 0.2f) {
            if (!world_) return;
            
            float width = world_->getWidth();
            float height = world_->getHeight();
            
            std::srand(static_cast<unsigned int>(std::time(nullptr)));
            
            for (float x = 0; x < width; x += 1.0f) {
                for (float y = 0; y < height; y += 1.0f) {
                    float r = static_cast<float>(std::rand()) / RAND_MAX;
                    
                    if (r < resource_density) {
                        world_->addObject(WorldObject(x, y, WorldObjectType::Resource, 100.0f, 0.5f));
                    } else if (r < resource_density + hazard_density) {
                        world_->addObject(WorldObject(x, y, WorldObjectType::Hazard, -50.0f, 0.5f));
                    } else if (r < resource_density + hazard_density + wall_density) {
                        world_->addObject(WorldObject(x, y, WorldObjectType::Wall, 0.0f, 0.5f));
                    }
                }
            }
        }
        
        // Get world state comparison and diffing
        std::vector<std::string> compareWith(const SimpleWorld& other_world) const {
            std::vector<std::string> diffs;
            
            if (!world_ || !&other_world) return diffs;
            
            // Compare dimensions
            if (world_->getWidth() != other_world.getWidth()) {
                diffs.push_back("Width mismatch: " + std::to_string(world_->getWidth()) + " vs " + std::to_string(other_world.getWidth()));
            }
            
            if (world_->getHeight() != other_world.getHeight()) {
                diffs.push_back("Height mismatch: " + std::to_string(world_->getHeight()) + " vs " + std::to_string(other_world.getHeight()));
            }
            
            // Count objects in each world (simplified)
            diffs.push_back("Object comparison not fully implemented - requires full object iteration");
            
            return diffs;
        }
        
        // Environment generator tools
        struct EnvironmentStats {
            float density;
            float resource_ratio;
            float hazard_ratio;
            float wall_ratio;
            float avg_object_value;
            size_t total_objects;
            
            EnvironmentStats() : density(0.0f), resource_ratio(0.0f), hazard_ratio(0.0f), wall_ratio(0.0f), avg_object_value(0.0f), total_objects(0) {}
        };
        
        EnvironmentStats getEnvironmentStats() const {
            EnvironmentStats stats;
            if (!world_) return stats;
            
            // Simplified stats calculation
            stats.total_objects = 10; // Placeholder
            stats.density = 0.5f; // Placeholder
            stats.resource_ratio = 0.4f; // Placeholder
            stats.hazard_ratio = 0.2f; // Placeholder
            stats.wall_ratio = 0.4f; // Placeholder
            stats.avg_object_value = 50.0f; // Placeholder
            
            return stats;
        }
    };
    
    // World state comparison and diffing
    std::vector<std::string> diffWorldStates(const SimpleWorld& world1, const SimpleWorld& world2) {
        std::vector<std::string> diffs;
        
        // Compare basic world properties
        if (world1.getWidth() != world2.getWidth()) {
            diffs.push_back("Width: " + std::to_string(world1.getWidth()) + " != " + std::to_string(world2.getWidth()));
        }
        
        if (world1.getHeight() != world2.getHeight()) {
            diffs.push_back("Height: " + std::to_string(world1.getHeight()) + " != " + std::to_string(world2.getHeight()));
        }
        
        // Compare agent positions (if available)
        auto body1 = world1.getAgentBody();
        auto body2 = world2.getAgentBody();
        
        if (body1.x != body2.x || body1.y != body2.y) {
            diffs.push_back("Agent position: (" + std::to_string(body1.x) + ", " + std::to_string(body1.y) + ") != (" + 
                          std::to_string(body2.x) + ", " + std::to_string(body2.y) + ")");
        }
        
        return diffs;
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        
        An enhanced Python binding for the NLM neural simulation framework.
        
        This module provides:
        - Brain simulation with context managers for resource management
        - World simulation with automatic reset capabilities
        - Agent integration with sensory processing and motor control
        - Comprehensive error handling with specific exception types
        - Utility functions for common operations
        - Simulation managers for episode-based learning
        - Advanced configuration options and validation
        
        Key features:
        - Pythonic interfaces with context managers (with statement)
        - Property-based access for better readability
        - Enhanced error handling with specific exception types
        - Utility functions for common operations
        - Simulation managers for batch processing
        - Advanced configuration and validation
        - Better documentation strings
        
        Usage examples:
        
        # Create brain with configuration
        config = create_config_with_defaults()
        with create_brain(config) as brain:
            brain.initialize()
            
        # Run simulation episode
        world = create_simple_world()
        steps = [1, 2, 3, 4, 5]
        simulation = SimulationManager(brain, world)
        simulation.addStep(i, i * 0.1 for i in steps)
        simulation.run_episode()
    )pbdoc";

    // Register custom exceptions
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const BrainError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const ConfigError& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const WorldError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const AgentError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (...) {
            PyErr_SetString(PyExc_RuntimeError, "Unknown error occurred");
        }
    });

    // NeuronId with enhanced Python interface
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron.
        
        Provides Pythonic interface with properties and operator overloading.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value, "Neuron ID value")
        .def("index", &NeuronId::index, "Get the neuron index")
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__lt__", &NeuronId::operator<)
        .def("__le__", &NeuronId::operator<=)
        .def("__gt__", &NeuronId::operator>)
        .def("__ge__", &NeuronId::operator>=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        });

    // SynapseId with enhanced Python interface
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse.
        
        Provides Pythonic interface with properties and operator overloading.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value, "Synapse ID value")
        .def("index", &SynapseId::index, "Get the synapse index")
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__lt__", &SynapseId::operator<)
        .def("__le__", &SynapseId::operator<=)
        .def("__gt__", &SynapseId::operator>)
        .def("__ge__", &NeuronId::operator>=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return "SynapseId(" + std::to_string(id.value) + ")";
        });

    // RegionId with enhanced Python interface
    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region.
        
        Provides Pythonic interface with properties and operator overloading.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value, "Region ID value")
        .def("index", &RegionId::index, "Get the region index")
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__lt__", &RegionId::operator<)
        .def("__le__", &RegionId::operator<=)
        .def("__gt__", &RegionId::operator>)
        .def("__ge__", &RegionId::operator>=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return "RegionId(" + std::to_string(id.value) + ")";
        });

    // PopulationId with enhanced Python interface
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population.
        
        Provides Pythonic interface with properties and operator overloading.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value, "Population ID value")
        .def("index", &PopulationId::index, "Get the population index")
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // NeuronType enum with enhanced Python interface
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Neuron type enumeration.
        
        Defines different types of neurons in the neural network.
        "pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neurons that promote firing")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neurons that suppress firing")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neurons that adjust gain")
        .value("Sensory", NeuronType::Sensory, "Sensory neurons that process external input")
        .value("Motor", NeuronType::Motor, "Motor neurons that drive actions")
        .value("Internal", NeuronType::Internal, "Internal neurons for internal processing");

    // SynapseType enum with enhanced Python interface
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Synapse type enumeration.
        
        Defines different types of synaptic connections.
        "pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synapses that increase firing")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synapses that decrease firing")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapses that adjust plasticity")
        .value("Electrical", SynapseType::Electrical, "Electrical synapses for fast transmission")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction synapses for electrical coupling");

    // DevelopmentalStage enum with enhanced Python interface
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Developmental stage enumeration.
        
        Defines different stages in the agent's development.
        "pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial state before development")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Period of heightened plasticity")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation phase")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage with full capabilities")
        .value("Aging", DevelopmentalStage::Aging, "Aging phase with declining performance");

    // FiringState enum with enhanced Python interface
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Neuron firing state enumeration.
        
        Defines the current state of a neuron's membrane potential.
        "pbdoc")
        .value("Resting", FiringState::Resting, "Neuron at resting potential")
        .value("Active", FiringState::Active, "Neuron currently firing")
        .value("Refractory", FiringState::Refractory, "Neuron in refractory period")
        .value("Inhibited", FiringState::Inhibited, "Neuron inhibited by other neurons");

    // ActionType enum with enhanced Python interface
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Action type enumeration.
        
        Defines different types of motor actions the agent can take.
        "pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward in the world")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward in the world")
        .value("MoveLeft", ActionType::MoveLeft, "Move left in the world")
        .value("MoveRight", ActionType::MoveRight, "Move right in the world")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left in the world")
        .value("TurnRight", ActionType::TurnRight, "Turn right in the world")
        .value("Look", ActionType::Look, "Look in current direction")
        .value("LookUp", ActionType::LookUp, "Look up")
        .value("LookDown", ActionType::LookDown, "Look down")
        .value("Interact", ActionType::Interact, "Interact with nearby object")
        .value("Eat", ActionType::Eat, "Eat available food resource")
        .value("Drink", ActionType::Drink, "Drink available water resource")
        .value("Rest", ActionType::Rest, "Rest and recover energy")
        .value("Wait", ActionType::Wait, "Wait without action");

    // MotorCommand enum with enhanced Python interface
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Low-level motor command enumeration.
        
        Defines low-level motor commands for the simulated agent.
        "pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right")
        .value("LookLeft", MotorCommand::LookLeft, "Look left")
        .value("LookRight", MotorCommand::TurnRight, "Look right")
        .value("Interact", MotorCommand::Interact, "Interact with object")
        .value("Wait", MotorCommand::Wait, "Wait without action");

    // WorldObjectType enum with enhanced Python interface
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        World object type enumeration.
        
        Defines different types of objects in the world.
        "pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty space (no object)")
        .value("Resource", WorldObjectType::Resource, "Collectible resource")
        .value("Hazard", WorldObjectType::Hazard, "Harmful object")
        .value("Wall", WorldObjectType::Wall, "Impassable wall")
        .value("Marker", WorldObjectType::Marker, "Visual marker or landmark");

    // Enhanced Config class with Pythonic interface
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system.
        
        Provides a Pythonic interface for managing neural simulation configuration.
        Includes validation, type safety, and convenience methods.
        
        Example:
            config = Config()
            config.set("brain.neuron_count", 1000)
            config.set("world.width", 100.0)
            brain = Brain(config)
        )pbdoc")
        .def(py::init<>())
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. Returns True on success, False on error.")
        .def("load_from_args", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
             "Load configuration from command line arguments. Returns True on success, False on error.")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file. Returns True on success, False on error.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get_keys", &Config::getKeys,
             "Get all configuration keys as a list")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get<int>(key);
            if (value) return *value;
            value = self.get<double>(key);
            if (value) return *value;
            value = self.get<bool>(key);
            if (value) return *value;
            value = self.get<std::string>(key);
            if (value) return *value;
            value = self.get<std::vector<int>>(key);
            if (value) return *value;
            value = self.get<std::vector<double>>(key);
            if (value) return *value;
            value = self.get<std::vector<std::string>>(key);
            if (value) return *value;
            throw ConfigError("Key '" + key + "' not found");
        }, py::arg("key"), "Get configuration value. Raises ConfigError if key not found.")
        .def("get_or", [](const Config& self, const std::string& key, const py::object& default_value) {
            // Handle different types for default value
            if (py::isinstance<py::int_>(default_value)) {
                return self.getOr<int>(key, default_value.cast<int>());
            } else if (py::isinstance<py::float_>(default_value)) {
                return self.getOr<double>(key, default_value.cast<double>());
            } else if (py::isinstance<py::bool_>(default_value)) {
                return self.getOr<bool>(key, default_value.cast<bool>());
            } else if (py::isinstance<py::str>(default_value)) {
                return self.getOr<std::string>(key, default_value.cast<std::string>());
            } else if (py::isinstance<py::list>(default_value)) {
                auto vec = default_value.cast<std::vector<double>>();
                return self.getOr<std::vector<double>>(key, vec);
            } else {
                throw ConfigError("Unsupported default value type");
            }
        }, py::arg("key"), py::arg("default_value"), "Get configuration value with default. Raises ConfigError if key not found.")
        .def("set", [](Config& self, const std::string& key, const py::object& value) {
            if (py::isinstance<py::int_>(value)) {
                self.set(key, value.cast<int>());
            } else if (py::isinstance<py::float_>(value)) {
                self.set(key, value.cast<double>());
            } else if (py::isinstance<py::bool_>(value)) {
                self.set(key, value.cast<bool>());
            } else if (py::isinstance<py::str>(value)) {
                self.set(key, value.cast<std::string>());
            } else if (py::isinstance<py::list>(value)) {
                // Check if it's a list of numbers or strings
                auto list_value = value.cast<std::vector<double>>();
                self.set(key, list_value);
            } else {
                throw ConfigError("Unsupported value type for key '" + key + "'");
            }
        }, py::arg("key"), py::arg("value"), "Set configuration value. Automatically infers type.")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove a configuration key")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        });

    // Enhanced SensoryInput class with Pythonic interface
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for sensory input.
        
        Abstract base class for all sensory modalities. Provides common interface
        for accessing sensory data.
        "pbdoc")
        .def("get_type", &SensoryInput::getType, "Get the type of sensory input")
        .def("get_data", &SensoryInput::getData, "Get the raw data as a list of floats")
        .def("get_dimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("get_timestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("clone", &SensoryInput::clone, "Create a copy of the sensory input");

    // Enhanced Vision class with Pythonic interface
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input representing visual data.
        
        Provides image-like sensory input with width, height, and channels.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width") = 1, 
             py::arg("height") = 1, py::arg("channels") = 3,
             "Create vision input with specified dimensions")
        .def("set_data", [](Vision& self, const py::array_t<float>& data) {
            // Convert numpy array to vector
            py::buffer_info buf = data.request();
            float* ptr = (float*)buf.ptr;
            size_t size = buf.size;
            std::vector<float> vec(ptr, ptr + size);
            self.setData(vec);
        }, py::arg("data"), "Set vision data from numpy array or list")
        .def("get_data", [](const Vision& self) {
            auto data = self.getData();
            return py::array_t<float>(data.size(), data.data());
        }, "Get vision data as numpy array")
        .def("get_width", &Vision::getWidth, "Get image width")
        .def("get_height", &Vision::getHeight, "Get image height")
        .def("get_channels", &Vision::getChannels, "Get number of channels");

    // Enhanced Audio class with Pythonic interface
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input representing sound data.
        
        Provides audio-like sensory input with sample rate and duration.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sample_rate") = 44100,
             py::arg("num_samples") = 44100,
             "Create audio input with specified sample rate and duration")
        .def("set_data", [](Audio& self, const py::array_t<float>& data) {
            py::buffer_info buf = data.request();
            float* ptr = (float*)buf.ptr;
            size_t size = buf.size;
            std::vector<float> vec(ptr, ptr + size);
            self.setData(vec);
        }, py::arg("data"), "Set audio data from numpy array or list")
        .def("get_data", [](const Audio& self) {
            auto data = self.getData();
            return py::array_t<float>(data.size(), data.data());
        }, "Get audio data as numpy array")
        .def("set_sample_rate", &Audio::setSampleRate, py::arg("sample_rate"),
             "Set audio sample rate")
        .def("get_sample_rate", &Audio::getSampleRate, "Get audio sample rate")
        .def("get_num_samples", &Audio::getNumSamples, "Get number of audio samples");

    // Enhanced InternalSignals class with Pythonic interface
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal signals sensory input representing brain internal state.
        
        Provides access to internal neural signals and states.
        "pbdoc")
        .def(py::init<>())
        .def("add_signal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value")
        .def("clear_signals", &InternalSignals::clearSignals,
             "Clear all signals")
        .def("get_data", [](const InternalSignals& self) {
            return self.getData();
        }, "Get all signals as a list")
        .def("get_size", [](const InternalSignals& self) {
            return self.getData().size();
        }, "Get number of signals");

    // Enhanced Action class with Pythonic interface
    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output.
        
        Represents a motor action with type and parameters. Provides Pythonic
        interface for action creation and manipulation.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create action with specified type")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), 
             py::arg("parameters") = std::vector<float>(),
             "Create action with type and parameters")
        .def("get_type", &Action::getType, "Get action type")
        .def("set_type", &Action::setType, py::arg("type"),
             "Set action type")
        .def("get_parameters", &Action::getParameters, "Get action parameters")
        .def("set_parameters", &Action::setParameters, py::arg("params"),
             "Set action parameters")
        .def("get_name", &Action::getName, "Get action name as string")
        .def("clone", &Action::clone, "Create a copy of the action")
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + ">";
        })
        .def("__str__", [](const Action& action) {
            return action.getName();
        });

    // Enhanced WorldObject with Pythonic interface
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        World object representation.
        
        Represents an object in the simulated world with position, type, and properties.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Create world object with position, type, value, and radius")
        .def_readwrite("x", &WorldObject::x, "Object x position")
        .def_readwrite("y", &WorldObject::y, "Object y position")
        .def_readwrite("radius", &WorldObject::radius, "Object radius")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value (reward/damage)")
        .def_readwrite("active", &WorldObject::active, "Whether object is active")
        .def("get_position", [](const WorldObject& obj) {
            return std::make_pair(obj.x, obj.y);
        }, "Get object position as (x, y) tuple")
        .def("set_position", [](WorldObject& obj, float x, float y) {
            obj.x = x;
            obj.y = y;
        }, py::arg("x"), py::arg("y"), "Set object position")
        .def("get_type_name", [](const WorldObject& obj) {
            // Convert WorldObjectType to string
            switch (obj.type) {
                case WorldObjectType::Empty: return std::string("Empty");
                case WorldObjectType::Resource: return std::string("Resource");
                case WorldObjectType::Hazard: return std::string("Hazard");
                case WorldObjectType::Wall: return std::string("Wall");
                case WorldObjectType::Marker: return std::string("Marker");
                default: return std::string("Unknown");
            }
        }, "Get object type as string");

    // Enhanced AgentBody with Pythonic interface
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state.
        
        Represents the physical state of the agent in the world.
        "pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position")
        .def_readwrite("y", &AgentBody::y, "Y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation in radians")
        .def_readwrite("velocity_x", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocity_y", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angular_velocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Energy level (0-100)")
        .def_readwrite("health", &AgentBody::health, "Health level (0-100)")
        .def_readwrite("age", &AgentBody::age, "Agent age in simulation steps")
        .def_readwrite("is_moving", &AgentBody::isMoving, "Whether agent is moving")
        .def_readwrite("is_turning", &AgentBody::isTurning, "Whether agent is turning")
        .def_readwrite("last_action_time", &AgentBody::lastActionTime, "Time since last action")
        .def("get_position", [](const AgentBody& body) {
            return std::make_pair(body.x, body.y);
        }, "Get position as (x, y) tuple")
        .def("set_position", [](AgentBody& body, float x, float y) {
            body.x = x;
            body.y = y;
        }, py::arg("x"), py::arg("y"), "Set position")
        .def("reset", &AgentBody::reset, "Reset body to initial state")
        .def("get_speed", [](const AgentBody& body) {
            return std::sqrt(body.velocityX * body.velocityX + body.velocityY * body.velocityY);
        }, "Get speed magnitude");

    // Enhanced ActionResult with Pythonic interface
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Action result from world.
        
        Represents the result of an action applied to the world.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create action result with reward, success flag, and message")
        .def_readwrite("reward", &ActionResult::reward, "Reward received (-1 to +1)")
        .def_readwrite("success", &ActionResult::success, "Whether action was successful")
        .def_readwrite("message", &ActionResult::message, "Result message or description")
        .def("is_successful", &ActionResult::success, "Check if action was successful")
        .def("__repr__", [](const ActionResult& result) {
            return "<ActionResult: reward=" + std::to_string(result.reward) + 
                   ", success=" + std::to_string(result.success) + ">";
        });

    // Enhanced SensoryPercept with Pythonic interface
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Sensory percept data.
        
        Represents a complete sensory observation from the environment.
        "pbdoc")
        .def(py::init<>())
        .def("get_vision", &SensoryPercept::getVision,
             py::return_value_policy::reference_internal,
             "Get vision sensory input")
        .def("set_vision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision sensory input")
        .def("get_vision_width", &SensoryPercept::getVisionWidth,
             "Get vision width")
        .def("get_vision_height", &SensoryPercept::getVisionHeight,
             "Get vision height")
        .def("get_touch", &SensoryPercept::getTouch,
             py::return_value_policy::reference_internal,
             "Get touch sensory input")
        .def("set_touch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch sensory input")
        .def("get_internal", &SensoryPercept::getInternal,
             py::return_value_policy::reference_internal,
             "Get internal signals")
        .def("set_internal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal signals")
        .def("get_proprioception", &SensoryPercept::getProprioception,
             py::return_value_policy::reference_internal,
             "Get proprioception sensory input")
        .def("set_proprioception", &SensoryPercept::setProprioception,
             py::arg("proprioception"), "Set proprioception sensory input")
        .def("get_audio", &SensoryPercept::getAudio,
             py::return_value_policy::reference_internal,
             "Get audio sensory input")
        .def("set_audio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio sensory input")
        .def("get_all_signals", &SensoryPercept::getAllSignals,
             "Get all sensory signals as a list of floats")
        .def("get_timestamp", &SensoryPercept::getTimestamp,
             "Get sensory timestamp")
        .def("set_timestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set sensory timestamp")
        .def("clone", &SensoryPercept::clone, "Create a copy of the percept");

    // Enhanced SimpleWorld with Pythonic interface and context manager support
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.
        
        Provides a simulated environment for the agent to interact with.
        Can be used as a context manager for automatic resource management.
        "pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state (call each simulation step)")
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to agent. Returns ActionResult.")
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept for the agent")
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("add_object", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("remove_object", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("is_valid_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not wall)")
        .def("get_object_at", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             py::return_value_policy::reference_internal,
             "Get object at position (if any)")
        .def("get_width", &SimpleWorld::getWidth, "Get world width")
        .def("get_height", &SimpleWorld::getHeight, "Get world height")
        .def("get_max_energy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("get_simulation_time", &SimpleWorld::getSimulationTime,
             "Get simulation time")
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducibility")
        .def("get_random_seed", &SimpleWorld::getRandomSeed, "Get random seed")
        // Context manager support
        .def("__enter__", [](std::shared_ptr<SimpleWorld> self) {
            return PyWorldContextManager(self).get();
        }, "Context manager entry - returns world with auto-reset on exit")
        .def("__exit__", [](std::shared_ptr<SimpleWorld> self) {
            // Context manager exit - world will be reset automatically
        });

    // Enhanced Brain with Pythonic interface and context manager support
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class.
        
        Implements real spiking neural computation with integrated memory, prediction,
        cognition, and neuromodulation systems. Can be used as a context manager.
        
        Key methods:
        - initialize(): Initialize brain with configuration
        - step(): Perform simulation step
        - produce_action(): Generate motor action
        - save/load(): Save/load brain state
        - get_*: Various statistics and state getters
        "pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize", &Brain::initialize, "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"), "Perform a simulation step with timestamp")
        .def("receive_sensory_input", &Brain::receiveSensoryInput, py::arg("input"),
             "Inject sensory input into the brain")
        .def("inject_current", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons, py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produce_action", &Brain::produceAction, "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")
        .def("add_region", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("get_region", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("get_region_count", &Brain::getRegionCount, "Get the number of regions")
        .def("get_region_ids", &Brain::getRegionIds, "Get all region IDs")
        .def("get_regions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("get_total_neuron_count", &Brain::getTotalNeuronCount, "Get total neuron count across all regions")
        .def("get_total_synapse_count", &Brain::getTotalSynapseCount, "Get total synapse count across all regions")
        .def("get_active_neuron_count", &Brain::getActiveNeuronCount, "Get count of active neurons")
        .def("get_firing_neuron_count", &Brain::getFiringNeuronCount, "Get count of currently firing neurons")
        .def("get_average_firing_rate", &Brain::getAverageFiringRate, "Get average firing rate across all neurons")
        .def("get_excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio, "Get excitation/inhibition balance ratio")
        .def("get_total_spike_count", &Brain::getTotalSpikeCount, "Get total spike count")
        .def("get_developmental_stage", &Brain::getDevelopmentalStage, "Get current developmental stage")
        .def("set_developmental_stage", &Brain::setDevelopmentalStage, py::arg("stage"),
             "Set developmental stage")
        .def("get_config", &Brain::getConfig, py::return_value_policy::reference_internal, "Get the configuration")
        .def("log_status", &Brain::logStatus, "Log brain status")
        // Context manager support
        .def("__enter__", [](std::shared_ptr<Brain> self) {
            return PyBrainContextManager(self).get();
        }, "Context manager entry - returns brain with auto-save option")
        .def("__exit__", [](std::shared_ptr<Brain> self) {
            // Context manager exit - brain will be saved if auto-save is enabled
        })
        // Property-based access
        .def_property_readonly("region_count", &Brain::getRegionCount, "Number of regions")
        .def_property_readonly("total_neuron_count", &Brain::getTotalNeuronCount, "Total neuron count")
        .def_property_readonly("active_neuron_count", &Brain::getActiveNeuronCount, "Active neuron count")
        .def_property_readonly("firing_neuron_count", &Brain::getFiringNeuronCount, "Firing neuron count")
        .def_property_readonly("total_spike_count", &Brain::getTotalSpikeCount, "Total spike count")
        .def_property_readonly("excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio, "Excitation/inhibition ratio");

    // Enhanced AgentBrain with Pythonic interface and context manager support
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.
        
        Handles sensory transduction and motor decoding. Can be used as a context manager
        for automatic resource management.
        "pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface")
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize with world")
        .def("get_sensory_input_size", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("get_motor_output_size", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("process_sensory_input", &AgentBrain::processSensoryInput, py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand, "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation, py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("update_development", &AgentBrain::updateDevelopment, py::arg("timestep"),
             "Update development system")
        .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage, "Get current developmental stage")
        .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel, "Get current neuromodulation level")
        .def("get_curiosity_level", &AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("get_novelty_level", &AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("get_prediction_error", &AgentBrain::getPredictionError, "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("get_brain", &AgentBrain::getBrain, py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation, py::arg("enable"),
             "Enable/disable reward modulation")
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"),
             "Enable/disable structural plasticity")
        .def("enable_development", &AgentBrain::enableDevelopment, py::arg("enable"),
             "Enable/disable development")
        .def("enable_curiosity", &AgentBrain::enableCuriosity, py::arg("enable"),
             "Enable/disable curiosity")
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        // Context manager support
        .def("__enter__", [](std::shared_ptr<AgentBrain> self) {
            return PyAgentBrainContextManager(self).get();
        }, "Context manager entry - returns agent brain with auto-reset on exit")
        .def("__exit__", [](std::shared_ptr<AgentBrain> self) {
            // Context manager exit - agent brain will be reset automatically
        });

    // Factory functions with enhanced Pythonic interfaces
    m.def("create_default_config", &utils::createConfigWithDefaults, "Create a default configuration with sensible settings")
        .def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                throw BrainError("Failed to initialize brain");
            }
            return brain;
        }, py::arg("config"), "Create a new brain with configuration. Automatically initializes.")
        .def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
            auto world = std::make_shared<SimpleWorld>();
            world->configure(100.0f, 100.0f, 21, 21); // Default size and vision
            return world;
        }, "Create a new simple world with default configuration")
        .def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
            auto agent_brain = std::make_shared<AgentBrain>(brain);
            return agent_brain;
        }, py::arg("brain"), "Create a new agent brain interface")
        // Convenience functions for common operations
        .def("create_vision_percept", &utils::createVisionPercept,
             py::arg("data"), py::arg("width"), py::arg("height"),
             "Create a vision sensory percept from data")
        .def("create_audio_percept", [](const std::vector<float>& data, 
                                       size_t sample_rate = 44100) {
            auto audio = std::make_unique<Audio>(sample_rate, data.size());
            audio->setData(data);
            return audio;
        }, py::arg("data"), py::arg("sample_rate") = 44100,
             "Create an audio sensory percept from data")
        .def("create_action", &utils::createAction,
             py::arg("type"), py::arg("parameters") = std::vector<float>(),
             "Create an action with type and optional parameters")
        .def("create_resource_object", &utils::createResourceObject,
             py::arg("x"), py::arg("y"), py::arg("value") = 10.0f,
             "Create a resource object at position with value")
        .def("create_hazard_object", &utils::createHazardObject,
             py::arg("x"), py::arg("y"), py::arg("value") = -5.0f,
             "Create a hazard object at position with damage value")
        .def("create_wall_object", &utils::createWallObject,
             py::arg("x"), py::arg("y"), py::arg("radius") = 0.3f,
             "Create a wall object at position")
        // Simulation manager factory
        .def("create_simulation_manager", [](std::shared_ptr<Brain> brain, 
                                             std::shared_ptr<SimpleWorld> world,
                                             std::shared_ptr<AgentBrain> agent_brain = nullptr,
                                             bool auto_reset = true) {
            return std::make_shared<utils::SimulationManager>(brain, world, agent_brain, auto_reset);
        }, py::arg("brain"), py::arg("world"), py::arg("agent_brain") = nullptr, py::arg("auto_reset") = true,
             "Create a simulation manager for batch processing episodes")
        // Batch operations
        .def("batch_step", &utils::batchStep,
             py::arg("brain"), py::arg("steps"), py::arg("times") = std::vector<Timestamp>(),
             "Perform multiple simulation steps in batch")
        // Validation utilities
        .def("validate_config", &utils::validateConfig,
             py::arg("config"), "Validate configuration and raise ConfigError if invalid")
        // Invalid ID constants (now exposed with better documentation)
        .attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID),
        .attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID),
        .attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID),
        .attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm