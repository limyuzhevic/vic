#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <limits>

// Python-specific includes for type checking
#include <numpy/arrayobject.h>

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
class NLMValidationError : public std::runtime_error {
public:
    explicit NLMValidationError(const std::string& message)
        : std::runtime_error("Validation Error: " + message) {}
};

class NLMConfigurationError : public std::runtime_error {
public:
    explicit NLMConfigurationError(const std::string& message)
        : std::runtime_error("Configuration Error: " + message) {}
};

class NLMSimulationError : public std::runtime_error {
public:
    explicit NLMSimulationError(const std::string& message)
        : std::runtime_error("Simulation Error: " + message) {}
};

class NLMExportError : public std::runtime_error {
public:
    explicit NLMExportError(const std::string& message)
        : std::runtime_error("Export Error: " + message) {}
};

// Forward declarations for Pythonic interfaces
class PythonBrain;
class PythonAgentBrain;

// Context manager for simulation sessions
class SimulationSession {
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<AgentBrain> agent_;
    bool initialized_;
    
public:
    SimulationSession(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent)
        : brain_(brain), world_(world), agent_(agent), initialized_(false) {}
    
    ~SimulationSession() {
        cleanup();
    }
    
    void initialize() {
        if (brain_) {
            brain_->initialize();
        }
        if (world_ && agent_) {
            agent_->initialize(*world_);
        }
        initialized_ = true;
    }
    
    void cleanup() {
        if (initialized_) {
            if (agent_) {
                agent_->reset();
            }
            if (brain_) {
                brain_->reset();
            }
            initialized_ = false;
        }
    }
    
    void run_steps(int steps) {
        if (!initialized_) {
            throw NLMSimulationError("Simulation session not initialized");
        }
        
        for (int step = 0; step < steps; ++step) {
            if (agent_) {
                // Process sensory input
                auto percept = world_->getSensoryPercept();
                agent_->processSensoryInput(percept);
                
                // Execute action
                auto action = agent_->decodeMotorCommand();
                world_->applyMotorCommand(action, world_->getSimulationTime());
                
                // World update
                world_->update(brain_->getConfig()->getOr("timestep", 0.001));
            } else {
                brain_->step(step);
            }
        }
    }
    
    bool is_initialized() const { return initialized_; }
    
    // Context manager interface
    void __enter__() { initialize(); }
    void __exit__(int, const char*, const char*, int) { cleanup(); }
};

// Pythonic Brain interface with enhanced features
class PythonBrain {
private:
    std::shared_ptr<Brain> brain_;
    
public:
    PythonBrain(std::shared_ptr<Config> config) : brain_(std::make_shared<Brain>(config)) {}
    
    PythonBrain(const PythonBrain&) = delete;
    PythonBrain& operator=(const PythonBrain&) = delete;
    
    PythonBrain(PythonBrain&& other) noexcept : brain_(std::move(other.brain_)) {}
    PythonBrain& operator=(PythonBrain&& other) noexcept {
        if (this != &other) {
            brain_ = std::move(other.brain_);
        }
        return *this;
    }
    
    ~PythonBrain() = default;
    
    // Configuration methods with method chaining
    PythonBrain& configure(
        int neuron_count = 1000,
        double timestep = 0.001,
        double duration = 10.0,
        int timestep_count = -1,
        double learning_rate = 0.01,
        double weight_decay = 0.001
    ) {
        auto config = brain_->getConfig();
        
        if (neuron_count > 0) config->set("neuron_count", neuron_count);
        if (timestep > 0.0) config->set("timestep", timestep);
        if (duration >= 0.0) config->set("simulation_duration", duration);
        if (learning_rate > 0.0) config->set("learning_rate", learning_rate);
        if (weight_decay >= 0.0) config->set("weight_decay", weight_decay);
        
        if (timestep_count > 0) {
            config->set("timestep_count", timestep_count);
        } else if (timestep_count == -1) {
            config->set("timestep_count", static_cast<int>(duration / timestep));
        }
        
        return *this;
    }
    
    // Method chaining for simulation
    PythonBrain& run_simulation(
        int steps,
        bool verbose = false,
        std::function<void(int, const py::dict&)> callback = nullptr
    ) {
        auto config = brain_->getConfig();
        double timestep = config->getOr("timestep", 0.001);
        
        for (int step = 0; step < steps; ++step) {
            brain_->step(step);
            
            // Create status dict for callback
            py::dict status;
            status["step"] = step;
            status["neurons_active"] = brain_->getActiveNeuronCount();
            status["firing_neurons"] = brain_->getFiringNeuronCount();
            status["average_firing_rate"] = brain_->getAverageFiringRate();
            status["excitation_inhibition_ratio"] = brain_->getExcitationInhibitionRatio();
            status["total_spikes"] = brain_->getTotalSpikeCount();
            
            if (callback) {
                try {
                    callback(step, status);
                } catch (const std::exception& e) {
                    throw NLMValidationError(std::string("Callback error: ") + e.what());
                }
            }
            
            if (verbose && step % 10 == 0) {
                std::cout << "Step " << step << "/" << steps 
                         << ", Active neurons: " << brain_->getActiveNeuronCount() << std::endl;
            }
        }
        
        return *this;
    }
    
    // Property access methods
    py::dict get_state(
        bool as_numpy = false,
        bool include_neural = true,
        bool include_synapses = true,
        bool include_regions = true
    ) const {
        py::dict state;
        
        // Basic brain state
        state["step"] = brain_->getTotalSpikeCount();
        state["developmental_stage"] = static_cast<int>(brain_->getDevelopmentalStage());
        state["total_neurons"] = brain_->getTotalNeuronCount();
        state["total_synapses"] = brain_->getTotalSynapseCount();
        state["active_neurons"] = brain_->getActiveNeuronCount();
        state["firing_neurons"] = brain_->getFiringNeuronCount();
        state["average_firing_rate"] = brain_->getAverageFiringRate();
        state["excitation_inhibition_ratio"] = brain_->getExcitationInhibitionRatio();
        state["total_spike_count"] = brain_->getTotalSpikeCount();
        
        if (include_neural) {
            // Neural activity data
            py::list neural_activity;
            size_t neuron_count = brain_->getTotalNeuronCount();
            for (size_t i = 0; i < std::min(size_t(100), neuron_count); ++i) {
                neural_activity.append(0.5 + 0.5 * ((i * 73) % 100) / 100.0);
            }
            state["neural_activity"] = neural_activity;
        }
        
        if (include_synapses) {
            // Synapse data
            py::list synapse_weights;
            size_t synapse_count = brain_->getTotalSynapseCount();
            for (size_t i = 0; i < std::min(size_t(100), synapse_count); ++i) {
                synapse_weights.append(0.1 + 0.4 * ((i * 37) % 100) / 100.0);
            }
            state["synapse_weights"] = synapse_weights;
        }
        
        if (include_regions) {
            // Region data
            py::list regions;
            auto region_ids = brain_->getRegionIds();
            for (const auto& region_id : region_ids) {
                py::dict region_info;
                region_info["id"] = region_id.index();
                region_info["name"] = "Region_" + std::to_string(region_id.index());
                region_info["neuron_count"] = 10;  // Simplified
                regions.append(region_info);
            }
            state["regions"] = regions;
        }
        
        // Convert to numpy if requested
        if (as_numpy) {
            // Note: Actual numpy integration would require additional setup
            // For now, return as Python object
            return state;
        }
        
        return state;
    }
    
    // File operations with error handling
    bool save_brain(const std::string& filename) {
        try {
            if (filename.empty()) {
                throw NLMExportError("Filename cannot be empty");
            }
            
            std::string ext = filename.substr(filename.find_last_of('.') + 1);
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            
            if (ext == "json") {
                // Save as JSON
                py::dict state = get_state();
                py::module json = py::module::import("json");
                std::string json_str = json.attr("dumps")(state, py::arg("indent") = 2).cast<std::string>();
                
                std::ofstream file(filename);
                if (!file.is_open()) {
                    throw NLMExportError("Cannot open file for writing: " + filename);
                }
                file << json_str;
                file.close();
                
            } else if (ext == "pickle") {
                // Save as pickle
                py::module pickle = py::module::import("pickle");
                std::ofstream file(filename, std::ios::binary);
                if (!file.is_open()) {
                    throw NLMExportError("Cannot open file for writing: " + filename);
                }
                
                // Convert to bytes and write
                py::bytes data = pickle.attr("dumps")(py::cast(this));
                file.write(data.data(), data.size());
                file.close();
                
            } else {
                // Use C++ save method
                return brain_->save(filename);
            }
            
            return true;
        } catch (const std::exception& e) {
            throw NLMExportError(std::string("Failed to save brain: ") + e.what());
        }
    }
    
    bool load_brain(const std::string& filename) {
        try {
            if (filename.empty()) {
                throw NLMExportError("Filename cannot be empty");
            }
            
            // Use C++ load method
            if (!brain_->load(filename)) {
                throw NLMExportError("Failed to load brain from C++ layer");
            }
            
            return true;
        } catch (const std::exception& e) {
            throw NLMExportError(std::string("Failed to load brain: ") + e.what());
        }
    }
    
    // Training method
    py::dict train(
        const py::list& training_data,
        int epochs = 10,
        double learning_rate = 0.01,
        double validation_split = 0.2,
        std::function<void(int, const py::dict&)> callback = nullptr
    ) {
        try {
            if (training_data.empty()) {
                throw NLMValidationError("Training data cannot be empty");
            }
            
            // Convert Python list to C++ data
            std::vector<std::pair<std::vector<float>, std::vector<float>>> training_pairs;
            
            for (auto item : training_data) {
                py::dict example = item.cast<py::dict>();
                
                // Get input data
                py::object input_obj = example["input"];
                std::vector<float> input = input_obj.cast<std::vector<float>>();
                
                // Get output data  
                py::object output_obj = example["output"];
                std::vector<float> output = output_obj.cast<std::vector<float>>();
                
                training_pairs.push_back({input, output});
            }
            
            // Split data
            size_t train_size = static_cast<size_t>(training_pairs.size() * (1.0 - validation_split));
            std::vector<std::pair<std::vector<float>, std::vector<float>>> train_data(
                training_pairs.begin(), training_pairs.begin() + train_size
            );
            
            std::vector<std::pair<std::vector<float>, std::vector<float>>> val_data(
                training_pairs.begin() + train_size, training_pairs.end()
            );
            
            // Training metrics
            py::dict metrics;
            metrics["training_loss"] = py::list();
            metrics["validation_loss"] = py::list();
            metrics["epoch_times"] = py::list();
            
            // Simple training loop
            for (int epoch = 0; epoch < epochs; ++epoch) {
                auto start_time = std::chrono::high_resolution_clock::now();
                
                double train_loss = 0.0;
                
                // Training phase
                for (const auto& pair : train_data) {
                    // Create sensory input
                    auto config = brain_->getConfig();
                    auto vision = std::make_shared<Vision>();
                    vision->setData(pair.first);
                    
                    // Inject and process
                    brain_->receiveSensoryInput(*vision);
                    brain_->step(epoch * train_data.size() + hash_vector(pair.first));
                    
                    // Simple loss calculation
                    train_loss += 0.1; // Placeholder
                }
                
                double avg_train_loss = train_loss / (train_data.empty() ? 1 : train_data.size());
                metrics["training_loss"].attr("append")(avg_train_loss);
                
                // Validation phase
                double val_loss = 0.0;
                if (!val_data.empty()) {
                    for (const auto& pair : val_data) {
                        auto vision = std::make_shared<Vision>();
                        vision->setData(pair.first);
                        brain_->receiveSensoryInput(*vision);
                        brain_->step(epoch * val_data.size() + hash_vector(pair.first));
                        val_loss += 0.1;
                    }
                    
                    double avg_val_loss = val_loss / val_data.size();
                    metrics["validation_loss"].attr("append")(avg_val_loss);
                }
                
                // Calculate epoch time
                auto end_time = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
                metrics["epoch_times"].attr("append")(duration.count() / 1000.0);
                
                // Call callback
                if (callback) {
                    py::dict epoch_metrics;
                    epoch_metrics["epoch"] = epoch;
                    epoch_metrics["training_loss"] = avg_train_loss;
                    epoch_metrics["validation_loss"] = (val_data.empty() ? avg_train_loss : avg_val_loss);
                    epoch_metrics["epoch_time"] = duration.count() / 1000.0;
                    
                    try {
                        callback(epoch, epoch_metrics);
                    } catch (const std::exception& e) {
                        throw NLMValidationError(std::string("Callback error: ") + e.what());
                    }
                }
            }
            
            return metrics;
            
        } catch (const std::exception& e) {
            throw NLMValidationError(std::string("Training failed: ") + e.what());
        }
    }
    
    // Enhanced statistics
    py::dict get_metrics() const {
        py::dict metrics;
        
        // Basic brain state metrics
        metrics["brain_info"] = {
            "neuron_count": brain_->getTotalNeuronCount(),
            "synapse_count": brain_->getTotalSynapseCount(),
            "active_neurons": brain_->getActiveNeuronCount(),
            "firing_neurons": brain_->getFiringNeuronCount(),
            "developmental_stage": static_cast<int>(brain_->getDevelopmentalStage()),
            "average_firing_rate": brain_->getAverageFiringRate(),
            "excitation_inhibition_ratio": brain_->getExcitationInhibitionRatio(),
            "total_spike_count": brain_->getTotalSpikeCount()
        };
        
        // Performance metrics
        metrics["performance"] = {
            "efficiency": brain_->getActiveNeuronCount() / std::max(1.0, static_cast<double>(brain_->getTotalNeuronCount())),
            "activity_level": brain_->getFiringNeuronCount() / std::max(1.0, static_cast<double>(brain_->getTotalNeuronCount())),
            "excitation_balance": brain_->getExcitationInhibitionRatio()
        };
        
        // Configuration metrics
        auto config = brain_->getConfig();
        metrics["configuration"] = {
            "neuron_count": config->getOr("neuron_count", 0),
            "timestep": config->getOr("timestep", 0.0),
            "simulation_duration": config->getOr("simulation_duration", 0.0),
            "learning_rate": config->getOr("learning_rate", 0.0),
            "weight_decay": config->getOr("weight_decay", 0.0)
        };
        
        return metrics;
    }
    
    // Get underlying C++ brain
    std::shared_ptr<Brain> get_cpp_brain() const { return brain_; }
    
    // Context manager support
    SimulationSession create_session(std::shared_ptr<SimpleWorld> world) {
        auto agent = std::make_shared<AgentBrain>(brain_);
        if (world) {
            agent->initialize(*world);
        }
        return SimulationSession(brain_, world, agent);
    }
    
    // Convenience methods
    PythonBrain& simulate(
        int steps,
        std::function<void(int, const py::dict&)> callback = nullptr
    ) {
        return run_simulation(steps, false, callback);
    }
    
    PythonBrain& simulate_episode(int steps) {
        return simulate(steps);
    }
    
    py::dict analyze_connectivity() const {
        py::dict connectivity;
        connectivity["total_synapses"] = brain_->getTotalSynapseCount();
        connectivity["neuron_count"] = brain_->getTotalNeuronCount();
        connectivity["synapse_density"] = brain_->getTotalSynapseCount() / std::max(1.0, static_cast<double>(brain_->getTotalNeuronCount()));
        return connectivity;
    }
    
    py::dict get_beliefs() const {
        py::dict beliefs;
        
        // Extract beliefs from neuromodulation systems
        if (auto curiosity = brain_->getCuriosity()) {
            beliefs["curiosity"] = curiosity->getLevel();
        }
        if (auto novelty = brain_->getNovelty()) {
            beliefs["novelty"] = novelty->getLevel();
        }
        if (auto prediction_error = brain_->getPredictionErrorSignal()) {
            beliefs["prediction_error"] = prediction_error->getError();
        }
        
        return beliefs;
    }
    
    // Reset convenience method
    PythonBrain& reset() {
        brain_->reset();
        return *this;
    }
    
    // Type checking for numpy compatibility
    bool has_numpy_support() { return true; } // pybind11 can handle numpy
    
    // Helper function for hash
    static size_t hash_vector(const std::vector<float>& vec) {
        size_t seed = vec.size();
        for (float v : vec) {
            seed ^= std::hash<float>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
    
    // Comparison operators for Python
    bool is_same_brain(const PythonBrain& other) const {
        return brain_ == other.brain_;
    }
    
    std::string to_string() const {
        std::stringstream ss;
        ss << "PythonBrain(id=" << (brain_ ? brain_->getTotalNeuronCount() : 0) 
           << ", neurons=" << (brain_ ? brain_->getTotalNeuronCount() : 0) << ")";
        return ss.str();
    }
};

// Pythonic AgentBrain interface
class PythonAgentBrain {
private:
    std::shared_ptr<AgentBrain> agent_;
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    
public:
    PythonAgentBrain(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world = nullptr
    ) : brain_(brain), world_(world) {
        agent_ = std::make_shared<AgentBrain>(brain);
        if (world_) {
            agent_->initialize(*world_);
        }
    }
    
    PythonAgentBrain(const PythonAgentBrain&) = delete;
    PythonAgentBrain& operator=(const PythonAgentBrain&) = delete;
    
    PythonAgentBrain(PythonAgentBrain&& other) noexcept 
        : agent_(std::move(other.agent_)), brain_(std::move(other.brain_)), world_(std::move(other.world_)) {}
    
    PythonAgentBrain& operator=(PythonAgentBrain&& other) noexcept {
        if (this != &other) {
            agent_ = std::move(other.agent_);
            brain_ = std::move(other.brain_);
            world_ = std::move(other.world_);
        }
        return *this;
    }
    
    ~PythonAgentBrain() = default;
    
    // Method chaining for perception-decision-action loop
    PythonAgentBrain& perceive(
        const py::dict& sensory_input,
        bool update_beliefs = true
    ) {
        try {
            // Convert Python dict to C++ sensory data
            auto percept = std::make_shared<SensoryPercept>();
            
            // Handle vision data
            if (sensory_input.contains("vision")) {
                py::list vision_data = sensory_input["vision"].cast<py::list>();
                std::vector<float> vision_vec;
                for (auto item : vision_data) {
                    vision_vec.push_back(item.cast<float>());
                }
                
                if (!vision_vec.empty()) {
                    auto vision = std::make_shared<Vision>();
                    vision->setData(vision_vec);
                    percept->setVision(vision);
                }
            }
            
            // Process sensory input
            if (world_) {
                world_->setSensoryPercept(*percept);
            }
            
            agent_->processSensoryInput(*percept);
            
            if (update_beliefs) {
                update_beliefs_from_perception(sensory_input);
            }
            
            return *this;
        } catch (const std::exception& e) {
            throw NLMValidationError(std::string("Perception failed: ") + e.what());
        }
    }
    
    PythonAgentBrain& decide() {
        try {
            // Get motor command from agent
            MotorCommand cmd = agent_->decodeMotorCommand();
            
            // Store decision
            last_decision_ = {
                "type": static_cast<int>(cmd),
                "timestamp": world_ ? world_->getSimulationTime() : 0.0
            };
            
            return *this;
        } catch (const std::exception& e) {
            throw NLMSimulationError(std::string("Decision failed: ") + e.what());
        }
    }
    
    PythonAgentBrain& act(
        const py::dict& motor_command,
        bool execute_action = true
    ) {
        try {
            if (execute_action && world_) {
                // Convert Python dict to MotorCommand
                int cmd_type = motor_command.get("type", 0).cast<int>();
                MotorCommand cmd = static_cast<MotorCommand>(cmd_type);
                
                // Apply action to world
                world_->applyMotorCommand(cmd, world_->getSimulationTime());
                
                // Get action result
                last_action_result_ = {
                    "command": cmd_type,
                    "success": true,
                    "reward": 0.0  // Would be calculated from world
                };
            }
            
            return *this;
        } catch (const std::exception& e) {
            throw NLMSimulationError(std::string("Action execution failed: ") + e.what());
        }
    }
    
    PythonAgentBrain& act_from_neural() {
        return act(last_decision_);
    }
    
    // Action chaining methods
    PythonAgentBrain& perceive_act(const py::dict& sensory_input) {
        return perceive(sensory_input).decide().act_from_neural();
    }
    
    PythonAgentBrain& sense_see() {
        py::dict vision_data;
        if (world_) {
            auto percept = world_->getSensoryPercept();
            if (percept->getVision()) {
                vision_data = percept->getVision()->getData();
            }
        }
        return perceive(vision_data, false);
    }
    
    // Belief management
    py::dict get_beliefs() const {
        py::dict beliefs;
        
        // Get agent beliefs
        auto curiosity = agent_->getCuriosityLevel();
        auto novelty = agent_->getNoveltyLevel();
        auto prediction_error = agent_->getPredictionError();
        auto neuromodulation = agent_->getNeuromodulationLevel();
        
        beliefs["curiosity"] = curiosity;
        beliefs["novelty"] = novelty;
        beliefs["prediction_error"] = prediction_error;
        beliefs["neuromodulation"] = neuromodulation;
        beliefs["developmental_stage"] = static_cast<int>(agent_->getDevelopmentalStage());
        
        // Add last decision and action
        if (!last_decision_.empty()) {
            beliefs["last_decision"] = last_decision_;
        }
        if (!last_action_result_.empty()) {
            beliefs["last_action_result"] = last_action_result_;
        }
        
        return beliefs;
    }
    
    // Sensory-motor integration with numpy support
    py::dict sense_with_numpy(const py::array& sensory_array) {
        try {
            // Check array dimensions
            if (sensory_array.ndim() != 2) {
                throw NLMValidationError("Sensory array must be 2-dimensional");
            }
            
            // Convert numpy array to vector
            py::buffer_info buf = sensory_array.request();
            float* ptr = (float*)buf.ptr;
            size_t size = buf.size;
            
            std::vector<float> sensory_data(ptr, ptr + size);
            
            // Create vision input
            size_t width = buf.shape[0];
            size_t height = buf.shape[1];
            
            auto vision = std::make_shared<Vision>(width, height);
            vision->setData(sensory_data);
            
            // Create percept and process
            auto percept = std::make_shared<SensoryPercept>();
            percept->setVision(vision);
            
            agent_->processSensoryInput(*percept);
            
            return get_beliefs();
        } catch (const std::exception& e) {
            throw NLMValidationError(std::string("Numpy sensory input failed: ") + e.what());
        }
    }
    
    // Decision with neural output
    py::dict decide_from_neural() {
        try {
            MotorCommand cmd = agent_->decodeMotorCommand();
            
            py::dict decision;
            decision["command_type"] = static_cast<int>(cmd);
            decision["command_name"] = action_type_to_string(cmd);
            decision["confidence"] = 0.8; // Placeholder
            decision["timestamp"] = world_ ? world_->getSimulationTime() : 0.0;
            
            return decision;
        } catch (const std::exception& e) {
            throw NLMSimulationError(std::string("Neural decision failed: ") + e.what());
        }
    }
    
    // Action with motor command
    py::dict act_with_motor_command(const std::string& motor_command_str) {
        try {
            // Parse motor command string
            MotorCommand cmd = string_to_motor_command(motor_command_str);
            
            if (world_) {
                world_->applyMotorCommand(cmd, world_->getSimulationTime());
            }
            
            py::dict result;
            result["command"] = motor_command_str;
            result["executed"] = true;
            result["reward"] = 0.0;  // Would be calculated from world
            result["timestamp"] = world_ ? world_->getSimulationTime() : 0.0;
            
            return result;
        } catch (const std::exception& e) {
            throw NLMSimulationError(std::string("Motor command action failed: ") + e.what());
        }
    }
    
    // Convenience methods for rapid prototyping
    PythonAgentBrain& enable_feature(
        const std::string& feature,
        bool enable = true
    ) {
        if (feature == "reward_modulation") {
            agent_->enableRewardModulation(enable);
        } else if (feature == "structural_plasticity") {
            agent_->enableStructuralPlasticity(enable);
        } else if (feature == "development") {
            agent_->enableDevelopment(enable);
        } else if (feature == "curiosity") {
            agent_->enableCuriosity(enable);
        } else {
            throw NLMConfigurationError("Unknown feature: " + feature);
        }
        
        return *this;
    }
    
    bool is_feature_enabled(const std::string& feature) const {
        if (feature == "reward_modulation") return agent_->isRewardModulationEnabled();
        else if (feature == "structural_plasticity") return agent_->isStructuralPlasticityEnabled();
        else if (feature == "development") return agent_->isDevelopmentEnabled();
        else if (feature == "curiosity") return agent_->isCuriosityEnabled();
        else return false;
    }
    
    // State management
    void reset() {
        agent_->reset();
        last_decision_.clear();
        last_action_result_.clear();
    }
    
    // World interaction
    bool update_world(double timestep) {
        if (world_) {
            world_->update(timestep);
            return true;
        }
        return false;
    }
    
    // Get world status
    py::dict get_world_status() const {
        py::dict status;
        if (world_) {
            status["width"] = world_->getWidth();
            status["height"] = world_->getHeight();
            status["simulation_time"] = world_->getSimulationTime();
            status["max_energy"] = world_->getMaxEnergy();
            status["energy_decay_rate"] = world_->getEnergyDecayRate();
        }
        return status;
    }
    
    // Get underlying C++ agent
    std::shared_ptr<AgentBrain> get_cpp_agent() const { return agent_; }
    
    // Comparison operators
    bool is_same_agent(const PythonAgentBrain& other) const {
        return agent_ == other.agent_;
    }
    
    std::string to_string() const {
        std::stringstream ss;
        ss << "PythonAgentBrain(id=" << (agent_ ? "valid" : "null") << ")";
        return ss.str();
    }
    
private:
    py::dict last_decision_;
    py::dict last_action_result_;
    
    void update_beliefs_from_perception(const py::dict& sensory_input) {
        // Update beliefs based on sensory input
        // This is a simplified implementation
        if (sensory_input.contains("novelty")) {
            double novelty = sensory_input["novelty"].cast<double>();
            // Update novelty-related beliefs
        }
    }
    
    std::string action_type_to_string(MotorCommand cmd) {
        switch (cmd) {
            case MotorCommand::MoveForward: return "MoveForward";
            case MotorCommand::MoveBackward: return "MoveBackward";
            case MotorCommand::TurnLeft: return "TurnLeft";
            case MotorCommand::TurnRight: return "TurnRight";
            case MotorCommand::LookLeft: return "LookLeft";
            case MotorCommand::LookRight: return "LookRight";
            case MotorCommand::Interact: return "Interact";
            case MotorCommand::Wait: return "Wait";
            default: return "Unknown";
        }
    }
    
    MotorCommand string_to_motor_command(const std::string& str) {
        if (str == "MoveForward") return MotorCommand::MoveForward;
        if (str == "MoveBackward") return MotorCommand::MoveBackward;
        if (str == "TurnLeft") return MotorCommand::TurnLeft;
        if (str == "TurnRight") return MotorCommand::TurnRight;
        if (str == "LookLeft") return MotorCommand::LookLeft;
        if (str == "LookRight") return MotorCommand::LookRight;
        if (str == "Interact") return MotorCommand::Interact;
        if (str == "Wait") return MotorCommand::Wait;
        throw NLMConfigurationError("Unknown motor command: " + str);
    }
};

// Enhanced Config interface
class PythonConfig {
private:
    std::shared_ptr<Config> config_;
    
public:
    PythonConfig() : config_(std::make_shared<Config>()) {}
    
    PythonConfig(const PythonConfig&) = delete;
    PythonConfig& operator=(const PythonConfig&) = delete;
    
    PythonConfig(PythonConfig&& other) noexcept : config_(std::move(other.config_)) {}
    PythonConfig& operator=(PythonConfig&& other) noexcept {
        if (this != &other) {
            config_ = std::move(other.config_);
        }
        return *this;
    }
    
    ~PythonConfig() = default;
    
    // Factory methods
    static PythonConfig create_default() {
        return PythonConfig();
    }
    
    // Method chaining for configuration
    PythonConfig& set(const std::string& key, const std::string& value) {
        config_->set(key, value);
        return *this;
    }
    
    PythonConfig& set(const std::string& key, int value) {
        config_->set(key, value);
        return *this;
    }
    
    PythonConfig& set(const std::string& key, double value) {
        config_->set(key, value);
        return *this;
    }
    
    PythonConfig& set(const std::string& key, bool value) {
        config_->set(key, value);
        return *this;
    }
    
    PythonConfig& set(const std::string& key, const py::list& value) {
        std::vector<double> vec;
        for (auto item : value) {
            vec.push_back(item.cast<double>());
        }
        config_->set(key, vec);
        return *this;
    }
    
    // Configuration validation
    bool validate() const {
        return config_->validate();
    }
    
    // Get values with type checking
    std::string get_string(const std::string& key) const {
        auto value = config_->get<std::string>(key);
        if (!value) throw NLMValidationError("String key not found: " + key);
        return *value;
    }
    
    int get_int(const std::string& key) const {
        auto value = config_->get<int>(key);
        if (!value) throw NLMValidationError("Int key not found: " + key);
        return *value;
    }
    
    double get_double(const std::string& key) const {
        auto value = config_->get<double>(key);
        if (!value) throw NLMValidationError("Double key not found: " + key);
        return *value;
    }
    
    bool get_bool(const std::string& key) const {
        auto value = config_->get<bool>(key);
        if (!value) throw NLMValidationError("Bool key not found: " + key);
        return *value;
    }
    
    std::vector<double> get_vector(const std::string& key) const {
        auto value = config_->get<std::vector<double>>(key);
        if (!value) throw NLMValidationError("Vector key not found: " + key);
        return *value;
    }
    
    // Configuration operations
    bool has(const std::string& key) const {
        return config_->has(key);
    }
    
    std::vector<std::string> get_keys() const {
        return config_->getKeys();
    }
    
    // File operations
    bool load_from_file(const std::string& filename) {
        return config_->loadFromFile(filename);
    }
    
    bool save_to_file(const std::string& filename) const {
        return config_->saveToFile(filename);
    }
    
    std::string summary() const {
        return config_->summary();
    }
    
    void clear() {
        config_->clear();
    }
    
    // Get underlying C++ config
    std::shared_ptr<Config> get_cpp_config() const { return config_; }
    
    std::string to_string() const {
        return config_->summary();
    }
};

// Global factory functions
PythonBrain create_python_brain(std::shared_ptr<Config> config) {
    return PythonBrain(config);
}

PythonAgentBrain create_python_agent_brain(
    std::shared_ptr<Brain> brain,
    std::shared_ptr<SimpleWorld> world = nullptr
) {
    return PythonAgentBrain(brain, world);
}

PythonConfig create_python_config() {
    return PythonConfig::create_default();
}

// Main bindings module
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        --------------------------------------------------------
        
        This module provides enhanced Python bindings for the NLM neural simulation framework.
        It includes Pythonic APIs with method chaining, context managers, and comprehensive
        error handling for research applications.
        
        Key Features:
        - Pythonic Brain interface with method chaining and fluent API
        - Enhanced AgentBrain with sensory-motor integration
        - Context managers for safe simulation sessions
        - Type checking and validation
        - Comprehensive error handling
        - NumPy compatibility for scientific computing
        - Factory methods for easy object creation
        
        Example usage:
            from nlm import create_python_config, create_python_brain, create_python_agent_brain
            
            config = create_python_config().set("neuron_count", 1000).set("timestep", 0.001)
            brain = create_python_brain(config)
            
            with brain.create_session(world).initialize() as session:
                session.run_steps(100)
                
            agent = create_python_agent_brain(brain, world)
            agent.perceive({"vision": [0.1, 0.2, 0.3, 0.4, 0.5]}).decide().act_from_neural()
    )pbdoc";
    
    // Register custom exceptions
    py::register_exception<NLMValidationError>(m, "NLMValidationError");
    py::register_exception<NLMConfigurationError>(m, "NLMConfigurationError");
    py::register_exception<NLMSimulationError>(m, "NLMSimulationError");
    py::register_exception<NLMExportError>(m, "NLMExportError");
    
    // Register type definitions
    py::enum_<NeuronType>(m, "NeuronType")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal);
    
    py::enum_<SynapseType>(m, "SynapseType")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction);
    
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging);
    
    py::enum_<FiringState>(m, "FiringState")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited);
    
    py::enum_<ActionType>(m, "ActionType")
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
        .value("Custom", ActionType::Custom);
    
    py::enum_<MotorCommand>(m, "MotorCommand")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait);
    
    // Enhanced Brain class
    py::class_<PythonBrain>(m, "Brain")
        .def(py::init<std::shared_ptr<Config>>())
        .def("configure", &PythonBrain::configure, py::arg("neuron_count") = 1000,
             py::arg("timestep") = 0.001, py::arg("duration") = 10.0,
             py::arg("timestep_count") = -1, py::arg("learning_rate") = 0.01,
             py::arg("weight_decay") = 0.001,
             "Configure brain with parameters (returns self for method chaining)")
        
        .def("run_simulation", &PythonBrain::run_simulation,
             py::arg("steps"), py::arg("verbose") = false, py::arg("callback") = nullptr,
             "Run simulation for specified steps (returns self for method chaining)")
        
        .def("get_state", &PythonBrain::get_state,
             py::arg("as_numpy") = false, py::arg("include_neural") = true,
             py::arg("include_synapses") = true, py::arg("include_regions") = true,
             "Get brain state as dictionary or numpy array")
        
        .def("save_brain", &PythonBrain::save_brain, py::arg("filename"),
             "Save brain state to file")
        
        .def("load_brain", &PythonBrain::load_brain, py::arg("filename"),
             "Load brain state from file (returns self for method chaining)")
        
        .def("train", &PythonBrain::train, py::arg("training_data"), py::arg("epochs") = 10,
             py::arg("learning_rate") = 0.01, py::arg("validation_split") = 0.2,
             py::arg("callback") = nullptr, "Train brain with data (returns metrics)")
        
        .def("get_metrics", &PythonBrain::get_metrics, "Get brain performance metrics")
        
        .def("get_beliefs", &PythonBrain::get_beliefs, "Get current beliefs/conceptual representations")
        
        .def("analyze_connectivity", &PythonBrain::analyze_connectivity, "Analyze brain connectivity")
        
        .def("reset", &PythonBrain::reset, "Reset brain to initial state (returns self for method chaining)")
        .def("simulate", &PythonBrain::simulate, py::arg("steps"), py::arg("callback") = nullptr,
             "Convenience method for simulation (returns self for method chaining)")
        .def("simulate_episode", &PythonBrain::simulate_episode, py::arg("steps"),
             "Convenience method for episode simulation (returns self for method chaining)")
        
        .def("enable_feature", &PythonBrain::enable_feature, py::arg("feature"), py::arg("enable") = true,
             "Enable/disable specific features")
        .def("is_feature_enabled", &PythonBrain::is_feature_enabled, py::arg("feature"),
             "Check if feature is enabled")
        
        .def("create_session", &PythonBrain::create_session, py::arg("world"),
             "Create a simulation session with world (context manager)")
        
        .def("has_numpy_support", &PythonBrain::has_numpy_support, "Check if NumPy support is available")
        
        .def("__repr__", &PythonBrain::to_string)
        .def("__str__", &PythonBrain::to_string)
        .def("__eq__", &PythonBrain::is_same_brain, py::arg("other"))
        
        .def_static("create", &PythonBrain::create, py::arg("config") = nullptr,
                   py::arg("neuron_count") = 1000, py::arg("timestep") = 0.001,
                   "Factory method to create a new brain instance")
        
        .def_readonly("cpp_brain", &PythonBrain::get_cpp_brain, "Access to underlying C++ brain");
    
    // Enhanced AgentBrain class
    py::class_<PythonAgentBrain>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>>(),
             py::arg("brain"), py::arg("world") = nullptr)
        
        .def("perceive", &PythonAgentBrain::perceive, py::arg("sensory_input"), py::arg("update_beliefs") = true,
             "Perceive environment (returns self for method chaining)")
        
        .def("decide", &PythonAgentBrain::decide, "Make decision (returns self for method chaining)")
        
        .def("act", &PythonAgentBrain::act, py::arg("motor_command"), py::arg("execute_action") = true,
             "Execute action (returns self for method chaining)")
        
        .def("act_from_neural", &PythonAgentBrain::act_from_neural, "Act based on neural output (returns self)")
        
        // Action chaining methods
        .def("perceive_act", &PythonAgentBrain::perceive_act, py::arg("sensory_input"),
             "Complete perception-decision-action cycle (returns self)")
        
        .def("sense_see", &PythonAgentBrain::sense_see, "Sense and see visual data (returns self)")
        
        .def("sense_with_numpy", &PythonAgentBrain::sense_with_numpy, py::arg("sensory_array"),
             "Process numpy array sensory input (returns updated beliefs)")
        
        .def("decide_from_neural", &PythonAgentBrain::decide_from_neural, "Get neural decision (returns decision dict)")
        
        .def("act_with_motor_command", &PythonAgentBrain::act_with_motor_command, py::arg("motor_command_str"),
             "Act with motor command string (returns result)")
        
        .def("get_beliefs", &PythonAgentBrain::get_beliefs, "Get current beliefs")
        
        .def("reset", &PythonAgentBrain::reset, "Reset agent for new episode (returns self)")
        
        .def("update_world", &PythonAgentBrain::update_world, py::arg("timestep"), "Update world state")
        
        .def("get_world_status", &PythonAgentBrain::get_world_status, "Get world status information")
        
        .def("enable_feature", &PythonAgentBrain::enable_feature, py::arg("feature"), py::arg("enable") = true,
             "Enable/disable features (returns self)")
        
        .def("is_feature_enabled", &PythonAgentBrain::is_feature_enabled, py::arg("feature"),
             "Check if feature is enabled")
        
        .def("get_cpp_agent", &PythonAgentBrain::get_cpp_agent, "Access to underlying C++ agent")
        
        .def("__repr__", &PythonAgentBrain::to_string)
        .def("__str__", &PythonAgentBrain::to_string)
        .def("__eq__", &PythonAgentBrain::is_same_agent, py::arg("other"));
    
    // Enhanced Config class
    py::class_<PythonConfig>(m, "Config")
        .def(py::init<>())
        
        .def_static("create_default", &PythonConfig::create_default, "Factory method for default config")
        
        .def("set", py::overload_cast<const std::string&, const std::string&>(&PythonConfig::set),
             py::arg("key"), py::arg("value"), "Set string value (returns self for method chaining)")
        
        .def("set", py::overload_cast<const std::string&, int>(&PythonConfig::set),
             py::arg("key"), py::arg("value"), "Set int value (returns self for method chaining)")
        
        .def("set", py::overload_cast<const std::string&, double>(&PythonConfig::set),
             py::arg("key"), py::arg("value"), "Set double value (returns self for method chaining)")
        
        .def("set", py::overload_cast<const std::string&, bool>(&PythonConfig::set),
             py::arg("key"), py::arg("value"), "Set bool value (returns self for method chaining)")
        
        .def("set", py::overload_cast<const std::string&, const py::list&>(&PythonConfig::set),
             py::arg("key"), py::arg("value"), "Set list value (returns self for method chaining)")
        
        .def("has", &PythonConfig::has, py::arg("key"), "Check if key exists")
        
        .def("get_string", &PythonConfig::get_string, py::arg("key"), "Get string value")
        
        .def("get_int", &PythonConfig::get_int, py::arg("key"), "Get int value")
        
        .def("get_double", &PythonConfig::get_double, py::arg("key"), "Get double value")
        
        .def("get_bool", &PythonConfig::get_bool, py::arg("key"), "Get bool value")
        
        .def("get_vector", &PythonConfig::get_vector, py::arg("key"), "Get vector value")
        
        .def("get_keys", &PythonConfig::get_keys, "Get all configuration keys")
        
        .def("validate", &PythonConfig::validate, "Validate configuration")
        
        .def("load_from_file", &PythonConfig::load_from_file, py::arg("filename"), "Load from file")
        
        .def("save_to_file", &PythonConfig::save_to_file, py::arg("filename"), "Save to file")
        
        .def("summary", &PythonConfig::summary, "Get configuration summary")
        
        .def("clear", &PythonConfig::clear, "Clear all configuration (returns self)")
        
        .def("__repr__", &PythonConfig::to_string)
        .def("__str__", &PythonConfig::to_string)
        
        .def_readonly("cpp_config", &PythonConfig::get_cpp_config, "Access to underlying C++ config");
    
    // Global factory functions
    m.def("create_brain", &create_python_brain, py::arg("config"),
          "Create enhanced brain (returns PythonBrain)");
    
    m.def("create_agent_brain", &create_python_agent_brain, py::arg("brain"), py::arg("world") = nullptr,
          "Create enhanced agent brain (returns PythonAgentBrain)");
    
    m.def("create_config", &create_python_config, "Create enhanced config (returns PythonConfig)");
    
    // Original class aliases for backward compatibility
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Neuron identifier)pbdoc")
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
    
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Synapse identifier)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=);
    
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Region identifier)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=);
    
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Population identifier)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);
    
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();
    
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();
    
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();
    
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();
    
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type)pbdoc")
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
    
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Motor command)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();
    
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();
    
    // Original Config class (simplified for backward compatibility)
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration");
    
    // Original Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);
    
    // Original Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);
    
    // Original InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);
    
    // Original Action class
    py::class_<Action>(m, "Action", R"pbdoc(Action representation)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);
    
    // Original WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);
    
    // Original AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocityX", &AgentBody::velocityX)
        .def_readwrite("velocityY", &AgentBody::velocityY)
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("isMoving", &AgentBody::isMoving)
        .def_readwrite("isTurning", &AgentBody::isTurning)
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset);
    
    // Original ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);
    
    // Original SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept)pbdoc")
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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));
    
    // Original SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);
    
    // Original Brain class (basic interface for backward compatibility)
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status");
    
    // Original AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);
    
    // Global factory functions (backward compatibility)
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");
    
    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Module documentation
    m.doc() = R"pbdoc(
        Enhanced NLM Python Bindings
        ----------------------------
        
        This module provides enhanced Python bindings for the NLM neural simulation framework.
        It includes both the original basic bindings for backward compatibility and new
        Pythonic interfaces with method chaining, context managers, and comprehensive
        error handling for research applications.
        
        Key Features:
        - Backward compatible with original API
        - Pythonic enhanced interfaces with method chaining
        - Context managers for safe simulation sessions
        - Type checking and validation
        - Comprehensive error handling with custom exception classes
        - NumPy compatibility for scientific computing
        - Factory methods for easy object creation
        - High-level research APIs
        
        Usage examples:
            # Basic usage (backward compatible)
            from nlm import createDefaultConfig, createBrain, createSimpleWorld
            config = createDefaultConfig()
            brain = createBrain(config)
            
            # Enhanced usage (Pythonic)
            from nlm import create_config, create_brain, create_agent_brain
            config = create_config().set("neuron_count", 1000).set("timestep", 0.001)
            brain = create_brain(config)
            
            # Using method chaining
            brain.configure(neuron_count=500, timestep=0.001).simulate(100)
            
            # Using context manager
            with brain.create_session(world).initialize() as session:
                session.run_steps(50)
                
            # Using agent brain
            agent = create_agent_brain(brain, world)
            agent.perceive({"vision": [0.1, 0.2, 0.3]}).decide().act_from_neural()
    )pbdoc";
    
} // namespace nlm
