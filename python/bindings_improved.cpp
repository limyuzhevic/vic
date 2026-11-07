#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/eigen.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <stdexcept>
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"

namespace py = pybind11;

// NLM Python Exceptions
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& msg) : NLMError("Configuration Error: " + msg) {}
};

class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& msg) : NLMError("Brain Error: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation Error: " + msg) {}
};

// Custom property system for Python
class PropertySystem {
public:
    template<typename T>
    static py::property make_property(const std::string& name, T& value) {
        return py::property(
            [=]() { return value; },
            [=](const T& new_value) { value = new_value; }
        );
    }
};

// Context manager for Brain cleanup
class BrainContextManager {
private:
    std::shared_ptr<nlm::Brain> brain_;
public:
    BrainContextManager(std::shared_ptr<nlm::Brain> brain) : brain_(brain) {}
    
    void __enter__() {
        if (!brain_->initialize()) {
            throw BrainError("Failed to initialize brain in context manager");
        }
    }
    
    void __exit__(py::exception const&, py::object const&, py::object const&) {
        brain_->reset();
    }
    
    std::shared_ptr<nlm::Brain> getBrain() { return brain_; }
};