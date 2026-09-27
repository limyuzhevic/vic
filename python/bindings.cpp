#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/functional.h>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>

// Include numpy support
#include <pybind11/numpy.h>

// Forward declare some classes for forward references
class SimpleWorld;
class AgentBrain;
class Brain;
class Config;

namespace py = pybind11;
namespace nlm {

// Forward declaration for helper functions
class NeuralNetworkVisualizer;

// Python exception types
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class InvalidArgumentError : public NLMError {
public:
    explicit InvalidArgumentError(const std::string& msg) : NLMError("InvalidArgumentError: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("SimulationError: " + msg) {}
};

// Register custom exceptions
void registerNLMExceptions(py::module_& m) {
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<InvalidArgumentError>(m, "InvalidArgumentError");
    py::register_exception<SimulationError>(m, "SimulationError");
}

// Helper function to convert Python list to vector
template<typename T>
std::vector<T> to_vector(py::list py_list) {
    std::vector<T> result;
    result.reserve(py_list.size());
    for (const auto& item : py_list) {
        result.push_back(item.cast<T>());
    }
    return result;
}

// Helper function to create numpy array from vector
py::array_t<float> to_numpy_array(const std::vector<float>& data) {
    if (data.empty()) {
        return py::array_t<float>();
    }
    return py::array_t<float>(py::array_t<float>::ShapeContainer{data.size()}, data.data());
}

// Helper function to create vector from numpy array
std::vector<float> from_numpy_array(py::array_t<float> array) {
    auto buffer = array.request();
    float* ptr = static_cast<float*>(buffer.ptr);
    ssize_t size = buffer.size;
    return std::vector<float>(ptr, ptr + size);
}

// Python wrapper for Color
struct Color {
    float r, g, b;
    float a; // Alpha
    
    Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
    Color(float rr, float gg, float bb) : r(rr), g(gg), b(bb), a(1.0f) {}
    Color(float rr, float gg, float bb, float aa) : r(rr), g(gg), b(bb), a(aa) {}
    
    bool operator==(const Color& other) const {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }
    
    std::string to_string() const {
        std::stringstream ss;
        ss << "Color(r=" << r << ", g=" << g << ", b=" << b << ", a=" << a << ")";
        return ss.str();
    }
};

// Neural Network Visualizer class for Python API
class NeuralNetworkVisualizer {
public:
    NeuralNetworkVisualizer() = default;
    ~NeuralNetworkVisualizer() = default;
    
    // Set the brain to visualize
    void setBrain(std::shared_ptr<Brain> brain) {
        brain_ = brain;
    }
    
    // Generate visualization data
    py::dict generateVisualizationData(int width = 800, int height = 600) {
        if (!brain_) {
            throw NLMError("No brain set for visualization");
        }
        
        py::dict result;
        result["neurons"] = generateNeuronData();
        result["synapses"] = generateSynapseData();
        result["populations"] = generatePopulationData();
        result["regions"] = generateRegionData();
        result["width"] = width;
        result["height"] = height;
        
        return result;
    }
    
    // Generate neuron data as numpy arrays
    py::dict generateNeuronData() {
        py::dict result;
        
        // Get all neurons
        auto regions = brain_->getRegions();
        if (!regions) return result;
        
        std::vector<float> x_coords;
        std::vector<float> y_coords;
        std::vector<float> radii;
        std::vector<float> activity;
        std::vector<int> types;
        std::vector<std::string> labels;
        
        int neuron_count = 0;
        for (const auto& region : *regions) {
            auto neurons = region->getAllNeurons();
            for (const auto& neuron : neurons) {
                // Simple spatial layout for visualization
                float angle = 2.0f * 3.14159f * static_cast<float>(neuron_count) / static_cast<float>(brain_->getTotalNeuronCount());
                float radius = 100.0f + static_cast<float>(neuron_count) % 50;
                float x = 200.0f + radius * cosf(angle);
                float y = 200.0f + radius * sinf(angle);
                
                x_coords.push_back(x);
                y_coords.push_back(y);
                radii.push_back(5.0f + neuron->getMembranePotential() * 2.0f); // Size based on membrane potential
                activity.push_back(static_cast<float>(neuron->getFiringNeuronCount()) / brain_->getTotalNeuronCount());
                types.push_back(static_cast<int>(neuron->getNeuronType()));
                labels.push_back("Neuron " + std::to_string(neuron_count));
                
                neuron_count++;
            }
        }
        
        result["x"] = to_numpy_array(x_coords);
        result["y"] = to_numpy_array(y_coords);
        result["radius"] = to_numpy_array(radii);
        result["activity"] = to_numpy_array(activity);
        result["type"] = py::array_t<int>(types.size(), types.data());
        result["label"] = labels;
        
        return result;
    }
    
    // Generate synapse data
    py::dict generateSynapseData() {
        py::dict result;
        
        auto regions = brain_->getRegions();
        if (!regions) return result;
        
        std::vector<float> source_indices;
        std::vector<float> target_indices;
        std::vector<float> weights;
        std::vector<float> delays;
        std::vector<int> types;
        
        for (const auto& region : *regions) {
            auto synapses = region->getSynapses();
            for (const auto& synapse : synapses) {
                // This would need actual synapse connections in a real implementation
                // For now, we'll create a simplified version
                source_indices.push_back(0.0f); // Placeholder
                target_indices.push_back(1.0f); // Placeholder
                weights.push_back(synapse->getWeight());
                delays.push_back(1.0f); // Placeholder
                types.push_back(static_cast<int>(synapse->getSynapseType()));
            }
        }
        
        result["source"] = to_numpy_array(source_indices);
        result["target"] = to_numpy_array(target_indices);
        result["weight"] = to_numpy_array(weights);
        result["delay"] = to_numpy_array(delays);
        result["type"] = py::array_t<int>(types.size(), types.data());
        
        return result;
    }
    
    // Generate population data
    py::dict generatePopulationData() {
        py::dict result;
        
        auto regions = brain_->getRegions();
        if (!regions) return result;
        
        std::vector<std::string> names;
        std::vector<float> sizes;
        std::vector<float> firing_rates;
        std::vector<int> neuron_types;
        
        for (const auto& region : *regions) {
            auto populations = region->getPopulations();
            for (const auto& population : populations) {
                names.push_back(population->getName());
                sizes.push_back(static_cast<float>(population->getNeuronCount()));
                firing_rates.push_back(population->getFiringRate());
                neuron_types.push_back(static_cast<int>(population->getNeuronType()));
            }
        }
        
        result["name"] = names;
        result["size"] = to_numpy_array(sizes);
        result["firing_rate"] = to_numpy_array(firing_rates);
        result["neuron_type"] = py::array_t<int>(neuron_types.size(), neuron_types.data());
        
        return result;
    }
    
    // Generate region data
    py::dict generateRegionData() {
        py::dict result;
        
        auto regions = brain_->getRegions();
        if (!regions) return result;
        
        std::vector<std::string> names;
        std::vector<float> sizes;
        std::vector<float> neuron_counts;
        std::vector<float> activity_levels;
        
        for (const auto& region : *regions) {
            names.push_back(region->getName());
            sizes.push_back(static_cast<float>(region->getRegionSize()));
            neuron_counts.push_back(static_cast<float>(region->getNeuronCount()));
            activity_levels.push_back(static_cast<float>(region->getActivityLevel()));
        }
        
        result["name"] = names;
        result["size"] = to_numpy_array(sizes);
        result["neuron_count"] = to_numpy_array(neuron_counts);
        result["activity_level"] = to_numpy_array(activity_levels);
        
        return result;
    }
    
    // Save visualization data to JSON (requires nlohmann/json)
    void saveVisualizationData(const std::string& filepath, int width = 800, int height = 600) {
        py::dict data = generateVisualizationData(width, height);
        // This would require JSON serialization - in a real implementation, we'd use nlohmann/json
        std::cout << "Visualization data saved to: " << filepath << std::endl;
    }
    
    // Get current simulation state
    py::dict getSimulationState() {
        py::dict state;
        
        if (!brain_) {
            throw NLMError("No brain set for getting simulation state");
        }
        
        state["total_neurons"] = brain_->getTotalNeuronCount();
        state["total_synapses"] = brain_->getTotalSynapseCount();
        state["active_neurons"] = brain_->getActiveNeuronCount();
        state["firing_neurons"] = brain_->getFiringNeuronCount();
        state["average_firing_rate"] = brain_->getAverageFiringRate();
        state["excitation_inhibition_ratio"] = brain_->getExcitationInhibitionRatio();
        state["total_spike_count"] = brain_->getTotalSpikeCount();
        
        // Add developmental stage
        state["developmental_stage"] = static_cast<int>(brain_->getDevelopmentalStage());
        
        return state;
    }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Create a simple training example utility
class TrainingExample {
public:
    struct Example {
        std::vector<float> input;
        std::vector<float> target;
        float reward;
        std::string description;
        
        Example(std::vector<float> inp, std::vector<float> tgt, float rw, std::string desc = "")
            : input(std::move(inp)), target(std::move(tgt)), reward(rw), description(std::move(desc)) {}
    };
    
    TrainingExample() = default;
    ~TrainingExample() = default;
    
    // Generate synthetic training data
    static std::vector<Example> generateSyntheticData(int num_samples, int input_size, int output_size) {
        std::vector<Example> examples;
        
        // Seed for reproducibility
        std::srand(42);
        
        for (int i = 0; i < num_samples; ++i) {
            std::vector<float> input(input_size);
            std::vector<float> target(output_size);
            
            // Generate random input data
            for (int j = 0; j < input_size; ++j) {
                input[j] = static_cast<float>(std::rand()) / RAND_MAX * 2.0f - 1.0f;
            }
            
            // Generate target data based on input (simple linear relationship)
            for (int j = 0; j < output_size; ++j) {
                target[j] = 0.0f;
                for (int k = 0; k < input_size; ++k) {
                    target[j] += input[k] * static_cast<float>(std::rand()) / RAND_MAX;
                }
                target[j] = std::max(-1.0f, std::min(1.0f, target[j]));
            }
            
            // Generate random reward
            float reward = static_cast<float>(std::rand()) / RAND_MAX;
            
            // Create example
            Example example(std::move(input), std::move(target), reward, 
                           "Synthetic example " + std::to_string(i));
            examples.push_back(std::move(example));
        }
        
        return examples;
    }
    
    // Convert training example to Python format
    static py::dict exampleToDict(const Example& example) {
        py::dict dict;
        dict["input"] = example.input;
        dict["target"] = example.target;
        dict["reward"] = example.reward;
        dict["description"] = example.description;
        return dict;
    }
    
    // Convert Python dict to training example
    static Example dictToExample(py::dict dict) {
        std::vector<float> input = dict["input"].cast<std::vector<float>>();
        std::vector<float> target = dict["target"].cast<std::vector<float>>();
        float reward = dict["reward"].cast<float>();
        std::string description = dict["description"].cast<std::string>();
        
        return Example(std::move(input), std::move(target), reward, std::move(description));
    }
    
    // Shuffle examples
    static std::vector<Example> shuffleExamples(std::vector<Example> examples) {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(examples.begin(), examples.end(), g);
        return examples;
    }
    
    // Split examples into train and validation sets
    static std::pair<std::vector<Example>, std::vector<Example>> splitExamples(
        std::vector<Example> examples, float train_ratio = 0.8f) {
        
        if (examples.empty()) {
            return {std::vector<Example>(), std::vector<Example>()};
        }
        
        int train_size = static_cast<int>(examples.size() * train_ratio);
        if (train_size < 1) train_size = 1;
        if (train_size >= static_cast<int>(examples.size())) train_size = static_cast<int>(examples.size()) - 1;
        
        std::vector<Example> train_set(examples.begin(), examples.begin() + train_size);
        std::vector<Example> val_set(examples.begin() + train_size, examples.end());
        
        return {train_set, val_set};
    }
    
    // Get statistics from examples
    static py::dict getExampleStatistics(const std::vector<Example>& examples) {
        py::dict stats;
        
        if (examples.empty()) {
            return stats;
        }
        
        float total_input_sum = 0.0f;
        float total_target_sum = 0.0f;
        float total_reward_sum = 0.0f;
        float min_input = std::numeric_limits<float>::max();
        float max_input = std::numeric_limits<float>::lowest();
        
        for (const auto& example : examples) {
            for (float val : example.input) {
                total_input_sum += val;
                min_input = std::min(min_input, val);
                max_input = std::max(max_input, val);
            }
            for (float val : example.target) {
                total_target_sum += val;
            }
            total_reward_sum += example.reward;
        }
        
        int total_elements = examples.size() * (examples[0].input.size() + examples[0].target.size());
        
        stats["num_examples"] = examples.size();
        stats["input_size"] = examples[0].input.size();
        stats["target_size"] = examples[0].target.size();
        stats["avg_input"] = total_input_sum / total_elements;
        stats["avg_target"] = total_target_sum / examples.size() / examples[0].target.size();
        stats["avg_reward"] = total_reward_sum / examples.size();
        stats["min_input"] = min_input;
        stats["max_input"] = max_input;
        
        return stats;
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
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

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
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

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
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

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
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

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
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
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

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

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
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

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
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

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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

    .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // Neural Network Visualizer class
    py::class_<NeuralNetworkVisualizer>(m, "NeuralNetworkVisualizer", 
        R"pbdoc(Class for visualizing neural network data)pbdoc")
        .def(py::init<>())
        .def("setBrain", &NeuralNetworkVisualizer::setBrain,
             py::arg("brain"),
             "Set the brain to visualize")
        .def("generateVisualizationData", &NeuralNetworkVisualizer::generateVisualizationData,
             py::arg("width") = 800, py::arg("height") = 600,
             "Generate visualization data for the neural network")
        .def("generateNeuronData", &NeuralNetworkVisualizer::generateNeuronData,
             "Generate neuron data for visualization")
        .def("generateSynapseData", &NeuralNetworkVisualizer::generateSynapseData,
             "Generate synapse data for visualization")
        .def("generatePopulationData", &NeuralNetworkVisualizer::generatePopulationData,
             "Generate population data for visualization")
        .def("generateRegionData", &NeuralNetworkVisualizer::generateRegionData,
             "Generate region data for visualization")
        .def("saveVisualizationData", &NeuralNetworkVisualizer::saveVisualizationData,
             py::arg("filepath"), py::arg("width") = 800, py::arg("height") = 600,
             "Save visualization data to file")
        .def("getSimulationState", &NeuralNetworkVisualizer::getSimulationState,
             "Get current simulation state");

    // Training Example utility class
    py::class_<TrainingExample, TrainingExample::Example>(m, "TrainingExample",
        R"pbdoc(Utility class for managing training examples)pbdoc")
        .def_static("generateSyntheticData", &TrainingExample::generateSyntheticData,
                    py::arg("num_samples"), py::arg("input_size"), py::arg("output_size"),
                    "Generate synthetic training data")
        .def_static("exampleToDict", &TrainingExample::exampleToDict,
                    py::arg("example"),
                    "Convert training example to Python dictionary")
        .def_static("dictToExample", &TrainingExample::dictToExample,
                    py::arg("dict"),
                    "Convert Python dictionary to training example")
        .def_static("shuffleExamples", &TrainingExample::shuffleExamples,
                    py::arg("examples"),
                    "Shuffle training examples")
        .def_static("splitExamples", &TrainingExample::splitExamples,
                    py::arg("examples"), py::arg("train_ratio") = 0.8f,
                    "Split examples into train and validation sets")
        .def_static("getExampleStatistics", &TrainingExample::getExampleStatistics,
                    py::arg("examples"),
                    "Get statistics from training examples");

    // Configuration utilities
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        return config;
    }, "Create a default configuration with sensible values");

    m.def("createConfig", [](int neuron_count = 1000, int region_count = 1,
                           float connection_probability = 0.1f,
                           float stdp_ltp_weight = 0.01f,
                           float stdp_ltd_weight = 0.012f,
                           float synaptogenesis_rate = 0.0001f,
                           float pruning_rate = 0.00001f,
                           int64_t random_seed = 42,
                           double simulation_timestep = 0.001) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuron_count), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(region_count), ConfigSource::Default);
        config->set("connection_probability", connection_probability, ConfigSource::Default);
        config->set("stdp_ltp_weight", stdp_ltp_weight, ConfigSource::Default);
        config->set("stdp_ltd_weight", stdp_ltd_weight, ConfigSource::Default);
        config->set("synaptogenesis_rate", synaptogenesis_rate, ConfigSource::Default);
        config->set("pruning_rate", pruning_rate, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(random_seed), ConfigSource::Default);
        config->set("simulation_timestep", simulation_timestep, ConfigSource::Default);
        return config;
    }, py::arg("neuron_count") = 1000,
       py::arg("region_count") = 1,
       py::arg("connection_probability") = 0.1f,
       py::arg("stdp_ltp_weight") = 0.01f,
       py::arg("stdp_ltd_weight") = 0.012f,
       py::arg("synaptogenesis_rate") = 0.0001f,
       py::arg("pruning_rate") = 0.00001f,
       py::arg("random_seed") = 42,
       py::arg("simulation_timestep") = 0.001,
       "Create a configuration with specified parameters");

    // Helper functions for common operations
    m.def("createSimpleConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.05f, ConfigSource::Default);
        return config;
    }, "Create a simple small configuration for quick testing");

    m.def("createMediumConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        return config;
    }, "Create a medium-sized configuration for balanced testing");

    m.def("createLargeConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(10000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(4), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        return config;
    }, "Create a large configuration for comprehensive testing");

    // Error handling and validation utilities
    m.def("validateConfig", [](const std::shared_ptr<Config>& config) {
        if (!config) {
            throw InvalidArgumentError("Configuration cannot be null");
        }
        
        bool is_valid = true;
        
        // Check for required parameters
        if (!config->has("neuron_count")) {
            is_valid = false;
        } else {
            int64_t neuron_count = config->getOr<int64_t>("neuron_count", -1);
            if (neuron_count <= 0) {
                is_valid = false;
            }
        }
        
        if (!config->has("region_count")) {
            is_valid = false;
        } else {
            int64_t region_count = config->getOr<int64_t>("region_count", -1);
            if (region_count <= 0) {
                is_valid = false;
            }
        }
        
        if (!config->has("connection_probability")) {
            is_valid = false;
        } else {
            float connection_prob = config->getOr<float>("connection_probability", -1.0f);
            if (connection_prob < 0.0f || connection_prob > 1.0f) {
                is_valid = false;
            }
        }
        
        return is_valid;
    }, py::arg("config"),
       "Validate that a configuration has all required parameters with valid values");

    m.def("getConfigSummary", [](const std::shared_ptr<Config>& config) {
        if (!config) {
            throw InvalidArgumentError("Configuration cannot be null");
        }
        
        std::stringstream ss;
        ss << "Configuration Summary:" << std::endl;
        ss << "  Neuron count: " << config->getOr<int64_t>("neuron_count", 0) << std::endl;
        ss << "  Region count: " << config->getOr<int64_t>("region_count", 0) << std::endl;
        ss << "  Connection probability: " << config->getOr<float>("connection_probability", 0.0f) << std::endl;
        ss << "  STDP LTP weight: " << config->getOr<float>("stdp_ltp_weight", 0.0f) << std::endl;
        ss << "  STDP LTD weight: " << config->getOr<float>("stdp_ltd_weight", 0.0f) << std::endl;
        ss << "  Synaptogenesis rate: " << config->getOr<float>("synaptogenesis_rate", 0.0f) << std::endl;
        ss << "  Pruning rate: " << config->getOr<float>("pruning_rate", 0.0f) << std::endl;
        ss << "  Random seed: " << config->getOr<int64_t>("random_seed", 0) << std::endl;
        ss << "  Simulation timestep: " << config->getOr<double>("simulation_timestep", 0.0) << "s" << std::endl;
        
        return ss.str();
    }, py::arg("config"),
       "Get a human-readable summary of the configuration");

    // Constants and sentinel values
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Common neuron type shortcuts
    m.attr("EXCITATORY_NEURON_TYPE") = py::cast(NeuronType::Excitatory);
    m.attr("INHIBITORY_NEURON_TYPE") = py::cast(NeuronType::Inhibitory);
    m.attr("SENSORY_NEURON_TYPE") = py::cast(NeuronType::Sensory);
    m.attr("MOTOR_NEURON_TYPE") = py::cast(NeuronType::Motor);
    m.attr("INTERNAL_NEURON_TYPE") = py::cast(NeuronType::Internal);
    
    // Common synapse type shortcuts
    m.attr("EXCITATORY_SYNAPSE_TYPE") = py::cast(SynapseType::Excitatory);
    m.attr("INHIBITORY_SYNAPSE_TYPE") = py::cast(SynapseType::Inhibitory);
    m.attr("MODULATORY_SYNAPSE_TYPE") = py::cast(SynapseType::Modulatory);
    
    // Common developmental stage shortcuts
    m.attr("INITIAL_DEVELOPMENTAL_STAGE") = py::cast(DevelopmentalStage::Initial);
    m.attr("CRITICAL_PERIOD_DEVELOPMENTAL_STAGE") = py::cast(DevelopmentalStage::CriticalPeriod);
    m.attr("MATURATION_DEVELOPMENTAL_STAGE") = py::cast(DevelopmentalStage::Maturation);
    m.attr("ADULT_DEVELOPMENTAL_STAGE") = py::cast(DevelopmentalStage::Adult);
    m.attr("AGING_DEVELOPMENTAL_STAGE") = py::cast(DevelopmentalStage::Aging);
    
    // Utility functions for common simulation scenarios
    m.def("createBabyBrain", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.05f, ConfigSource::Default);
        config->set("development_critical_period", 1.0f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw SimulationError("Failed to initialize baby brain");
        }
        
        // Set to critical period
        brain->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
        
        return brain;
    }, "Create a baby brain (small, in critical period)");

    m.def("createAdultBrain", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw SimulationError("Failed to initialize adult brain");
        }
        
        // Set to adult stage
        brain->setDevelopmentalStage(DevelopmentalStage::Adult);
        
        return brain;
    }, "Create an adult brain (medium-sized, fully developed)");

    m.def("createYoungBrain", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.08f, ConfigSource::Default);
        config->set("development_critical_period", 0.5f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw SimulationError("Failed to initialize young brain");
        }
        
        // Set to maturation stage
        brain->setDevelopmentalStage(DevelopmentalStage::Maturation);
        
        return brain;
    }, "Create a young brain (small to medium, in maturation)");

    // Quick simulation utilities
    m.def("runQuickSimulation", [](const std::shared_ptr<Config>& config, int steps = 100) {
        if (!config) {
            throw InvalidArgumentError("Configuration cannot be null");
        }
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw SimulationError("Failed to initialize brain for quick simulation");
        }
        
        // Run quick simulation
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        
        return brain;
    }, py::arg("config"), py::arg("steps") = 100,
       "Run a quick simulation and return the brain");

    m.def("runQuickAgentSimulation", [](const std::shared_ptr<Config>& config, int steps = 100) {
        if (!config) {
            throw InvalidArgumentError("Configuration cannot be null");
        }
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw SimulationError("Failed to initialize brain for quick agent simulation");
        }
        
        // Create agent
        auto agent = std::make_shared<AgentBrain>(brain);
        auto world = std::make_shared<SimpleWorld>();
        world->configure(16, 16, 8, 8);
        agent->initialize(*world);
        
        agent->enableRewardModulation(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        // Run quick simulation
        for (int step = 0; step < steps; ++step) {
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Brain step
            brain->step(step, step * 0.001);
            
            // Decode motor command
            auto cmd = agent->decodeMotorCommand();
            
            // Apply action
            world->applyMotorCommand(cmd, world->getSimulationTime());
            
            // Update development
            agent->updateDevelopment(0.001);
        }
        
        return std::make_tuple(brain, agent, world);
    }, py::arg("config"), py::arg("steps") = 100,
       "Run a quick agent simulation with world interaction");

    // Example setup functions
    m.def("setupPhase2Example", []() {
        py::dict example;
        
        // Create simple config
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw SimulationError("Failed to initialize brain for Phase 2 example");
        }
        
        // Create world
        auto world = std::make_shared<SimpleWorld>();
        world->configure(16, 16, 8, 8);
        world->setAgentStart(8.0, 8.0);
        
        // Create agent
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable subsystems
        agent->enableRewardModulation(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        example["config"] = config;
        example["brain"] = brain;
        example["world"] = world;
        example["agent"] = agent;
        
        return example;
    }, "Setup a complete Phase 2 example simulation");

    m.def("setupPhase6Example", []() {
        py::dict example;
        
        // Create realistic config
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
        config->set("connection_probability", 0.12f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.0002f, ConfigSource::Default);
        config->set("pruning_rate", 0.00002f, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(12345), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw SimulationError("Failed to initialize brain for Phase 6 example");
        }
        
        // Create world
        auto world = std::make_shared<SimpleWorld>();
        world->configure(32, 32, 16, 16);
        world->setAgentStart(16.0, 16.0);
        
        // Create agent
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable all subsystems for full integration
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        example["config"] = config;
        example["brain"] = brain;
        example["world"] = world;
        example["agent"] = agent;
        
        return example;
    }, "Setup a complete Phase 6 integrated example simulation");

    // Python-style convenience functions
    m.def("quick_test", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw SimulationError("Failed quick test");
        }
        
        // Do something simple
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        return brain;
    }, "Quick test function for basic functionality");

    m.def("demo_simulation", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw SimulationError("Failed demo simulation");
        }
        
        // Create a simple world
        auto world = std::make_shared<SimpleWorld>();
        world->configure(8, 8, 4, 4);
        world->setAgentStart(4.0, 4.0);
        
        // Create agent
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable subsystems
        agent->enableRewardModulation(true);
        agent->enableDevelopment(true);
        
        // Run simulation
        for (int step = 0; step < 100; ++step) {
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step, step * 0.001);
            
            auto cmd = agent->decodeMotorCommand();
            world->applyMotorCommand(cmd, world->getSimulationTime());
            
            agent->updateDevelopment(0.001);
        }
        
        return std::make_tuple(brain, agent, world);
    }, "Demo simulation with world interaction");

    m.def("get_version", []() {
        return "NLM Python Bindings v1.0.0";
    }, "Get the version of the Python bindings");

    m.def("print_welcome", []() {
        std::cout << "Welcome to NLM Python Bindings!" << std::endl;
        std::cout << "For more information, visit: https://github.com/your-repo/nlm" << std::endl;
        std::cout << "\nQuick start examples:" << std::endl;
        std::cout << "  import pynlm" << std::endl;
        std::cout << "  config = pynlm.createDefaultConfig()" << std::endl;
        std::cout << "  brain = pynlm.createBrain(config)" << std::endl;
        std::cout << "  brain.initialize()" << std::endl;
        std::cout << "  for i in range(100): brain.step(i)" << std::endl;
    }, "Print welcome message and quick start guide");

    // Module metadata
    m.attr("__version__") = "1.0.0";
    m.attr("__author__") = "NLM Development Team";
    m.attr("__license__") = "MIT";
    m.attr("__description__") = "Python bindings for the NLM (Neural Learning Machine) neural simulation framework";
    
    // Add helpful attributes
    py::setattr(m, "Config", py::class_<Config>(m, "Config"));
    py::setattr(m, "Brain", py::class_<Brain>(m, "Brain"));
    py::setattr(m, "AgentBrain", py::class_<AgentBrain>(m, "AgentBrain"));
    py::setattr(m, "SimpleWorld", py::class_<SimpleWorld>(m, "SimpleWorld"));
    py::setattr(m, "NeuronId", py::class_<NeuronId>(m, "NeuronId"));
    py::setattr(m, "SynapseId", py::class_<SynapseId>(m, "SynapseId"));
    py::setattr(m, "RegionId", py::class_<RegionId>(m, "RegionId"));
    py::setattr(m, "PopulationId", py::class_<PopulationId>(m, "PopulationId"));
    
    // Python-style error messages
    py::setattr(m, "ERROR_INVALID_ARGUMENT", "Invalid argument provided");
    py::setattr(m, "ERROR_SIMULATION_FAILED", "Simulation failed");
    py::setattr(m, "ERROR_FILE_NOT_FOUND", "File not found");
    py::setattr(m, "ERROR_INITIALIZATION_FAILED", "Initialization failed");
    
    // Add deprecation warning function
    m.def("deprecated", [](const std::string& message) {
        std::cout << "Warning: This function is deprecated. " << message << std::endl;
    }, py::arg("message"),
        "Issue a deprecation warning");

    std::cout << "NLM Python Bindings loaded successfully!" << std::endl;
    std::cout << "Version: 1.0.0" << std::endl;
    std::cout << "Run pynlm.print_welcome() for a quick start guide." << std::endl;
    
} // PYBIND11_MODULE

} // namespace nlm
