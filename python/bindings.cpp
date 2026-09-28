# NLM Python API Enhancement

## Overview
This document outlines the comprehensive improvement plan for the NLM Python bindings to make them more accessible, powerful, and user-friendly for researchers and developers.

## Current State Analysis

### 1. Basic Python Bindings (Current)
**Location**: `/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_7755ed4a-be3a-4bfd-8337-158967131ac8/python/bindings.cpp`

**Current Features**:
- Basic classes: NeuronId, SynapseId, RegionId, PopulationId
- Enums: NeuronType, SynapseType, DevelopmentalStage
- Core classes: Brain, Config, AgentBrain, SimpleWorld
- Limited documentation and examples

**Limitations**:
- No type hints or IDE support
- Poor error handling
- No documentation or examples
- Missing advanced features
- No integration with Python scientific stack

### 2. pyproject.toml (Current)
**Location**: `/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_7755ed4a-be3a-4bfd-8337-158967131ac8/pyproject.toml`

**Current Features**:
- Basic project configuration
- Minimal dependencies (only dev dependencies)
- No comprehensive documentation
- Limited build options

**Limitations**:
- No runtime dependencies for common use cases
- No comprehensive documentation
- No examples or tutorials
- Limited build configuration

### 3. Python Integration Issues
- **Poor Documentation**: No docstrings, examples, or API reference
- **Limited Features**: Only basic simulation capabilities
- **No Integration**: No support for NumPy, data analysis, visualization
- **Error Handling**: Basic error handling without Pythonic patterns
- **Type Safety**: No type hints or IDE support

## Improvement Priorities

### High Priority (Phase 1)
1. **Enhanced pyproject.toml**: Modern Python packaging with comprehensive dependencies
2. **Comprehensive Python Bindings**: Full-featured API with documentation
3. **Type Hints and IDE Support**: Complete type annotations
4. **Error Handling**: Pythonic error handling and exceptions
5. **Basic Examples**: Working examples for common use cases

### Medium Priority (Phase 2)
1. **Advanced Features**: Configuration management, data export, performance monitoring
2. **NumPy Integration**: Array support for neural data
3. **Visualization Integration**: Python plotting capabilities
4. **Configuration Management**: JSON/YAML configuration support
5. **Performance Profiling**: Built-in profiling tools

### Low Priority (Phase 3)
1. **Machine Learning Integration**: Integration with scikit-learn, TensorFlow, PyTorch
2. **Async Support**: Asynchronous I/O operations
3. **Plugin Architecture**: Extensible plugin system
4. **Cloud Integration**: Support for cloud-based simulation
5. **Web Interface**: Web-based visualization and control

## Detailed Improvement Plan

### 1. Enhanced pyproject.toml

**Current**:
```toml
[build-system]
requires = ["scikit-build-core>=0.5.0", "pybind11>=2.11.0"]
build-backend = "scikit_build_core.build"

[project]
name = "pynlm"
version = "0.1.0"
description = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"
readme = "README.md"
license = {text = "MIT"}
requires-python = ">=3.8"

[project.optional-dependencies]
dev = [
    "pytest>=7.0",
    "numpy>=1.20",
]
```

**Improved**:
```toml
[build-system]
requires = ["scikit-build-core>=0.5.0", "pybind11>=2.11.0"]
build-backend = "scikit_build_core.build"

[project]
name = "pynlm"
version = "0.1.0"
description = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"
readme = "README.md"
description = "A comprehensive Python interface for NLM neural simulation with advanced features for researchers and developers."
license = {text = "MIT"}
requires-python = ">=3.8"
authors = [
    {name = "NLM Authors", email = "contact@nlm-project.org"},
]
keywords = ["neural", "simulation", "spiking", "brain", "learning", "neuroscience", "artificial intelligence"]
classifiers = [
    "Development Status :: 4 - Beta",
    "Intended Audience :: Science/Research",
    "Intended Audience :: Developers",
    "License :: OSI Approved :: MIT License",
    "Programming Language :: Python :: 3",
    "Programming Language :: Python :: 3.8",
    "Programming Language :: Python :: 3.9",
    "Programming Language :: Python :: 3.10",
    "Programming Language :: Python :: 3.11",
    "Programming Language :: Python :: 3.12",
    "Programming Language :: C++",
    "Topic :: Scientific/Engineering :: Artificial Intelligence",
    "Topic :: Scientific/Engineering :: Neuroscience",
    "Topic :: Software Development :: Libraries",
]

dependencies = [
    "numpy>=1.20; python_version >= '3.8'",
    "pandas>=1.3.0; python_version >= '3.8'",
    "scipy>=1.7.0; python_version >= '3.8'",
]

[project.optional-dependencies]
dev = [
    "pytest>=7.0",
    "pytest-cov>=3.0",
    "numpy>=1.20",
    "pandas>=1.3.0",
    "scipy>=1.7.0",
    "matplotlib>=3.5.0",
    "jupyter>=1.0.0",
    "sphinx>=4.0.0",
    "sphinx-rtd-theme>=1.0.0",
]
full = [
    "pynlm[dev]",
    "matplotlib>=3.5.0",
    "jupyter>=1.0.0",
    "scikit-learn>=1.0.0",
]

[project.urls]
Homepage = "https://github.com/nlm-project/nlm"
Documentation = "https://nlm.readthedocs.io"
Repository = "https://github.com/nlm-project/nlm.git"
"Issues" = "https://github.com/nlm-project/nlm/issues"
"Papers" = "https://github.com/nlm-project/nlm/papers"

[project.scripts]
nlm-console = "pynlm.cli:main"
nlm-benchmark = "pynlm.benchmark:main"
nlm-demo = "pynlm.demo:main"

[project.gui-scripts]
nlm-viz = "pynlm.visualization:main"

[tool.scikit-build]
cmake.minimum-version = "3.16"
cmake.build-type = "Release"
cmake.args = [
    "-DCMAKE_CXX_STANDARD=20",
    "-DCMAKE_CXX_STANDARD_REQUIRED=ON",
    "-DPYTHON_EXECUTABLE={python}",
    "-DNLM_ENABLE_PYTHON_BINDINGS=ON",
]

[tool.scikit-build.cmake.define]
NLM_SOURCE_DIR = "src"
NLM_ENABLE_PYTHON_BINDINGS = "ON"

[tool.pytest.ini_options]
testpaths = ["tests"]
python_files = ["test_*.py"]
python_classes = ["Test*"]
python_functions = ["test_*"]
addopts = "-v --tb=short"

[tool.coverage.run]
source = ["pynlm"]

[tool.coverage.report]
exclude_lines = [
    "pragma: no cover",
    "def __repr__",
    "if __name__ == .__main__.:",
    "raise AssertionError",
    "raise NotImplementedError",
]

[tool.ruff]
select = ["E", "F", "I", "W"]
ignore = []
line-length = 88
target-version = "py38"

[tool.black]
line-length = 88
target-version = "py38"
include = '\.pyi?$'
exclude = '''
/(
    \.eggs
  | \.git
  | \.hg
  | \.mypy_cache
  | \.nox
  | \.tox
  | \.venv
  | _build
  | buck-out
  | build
  | dist
  | venv
)/
'''
```

### 2. Comprehensive Python Bindings

**Enhanced bindings.cpp structure**:

```cpp
PYBIND11_MODULE(pynlm, m) {
    // Module metadata
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        
        A comprehensive Python interface for NLM neural simulation.
        
        Key Features:
        - Full C++ neural simulation with Python convenience
        - Advanced memory and neuromodulation systems
        - Configuration management and experiment control
        - Data export and analysis capabilities
        - Integration with NumPy and scientific computing
        
        Quick Start:
            import pynlm
            config = pynlm.create_default_config()
            brain = pynlm.create_brain(config)
            brain.initialize()
        
        For complete documentation, visit: https://nlm.readthedocs.io
    )pbdoc";

    // Configuration Management
    auto config_class = py::class_<Config>(m, "Config", R"pbdoc(Configuration object)pbdoc")
        .def(py::init<>())
        .def("set", &Config::set, py::arg("key"), py::arg("value"), 
             R"pbdoc(Set a configuration value by key and value)pbdoc")
        .def("get", py::overload_cast<const std::string&, int64_t>(&Config::getOr), 
             py::arg("key"), py::arg("default_value"), 
             R"pbdoc(Get a configuration value with default)pbdoc")
        .def("has", &Config::has, py::arg("key"), 
             R"pbdoc(Check if configuration has a key)pbdoc")
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(Load configuration from file)pbdoc")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"),
             R"pbdoc(Save configuration to file)pbdoc")
        .def("get_keys", &Config::getKeys,
             R"pbdoc(Get all configuration keys)pbdoc");

    // Brain Management
    auto brain_class = py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Neural brain simulation)")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             R"pbdoc(Create brain with configuration)pbdoc")
        .def("initialize", &Brain::initialize,
             R"pbdoc(Initialize brain with current configuration)pbdoc")
        .def("reset", &Brain::reset,
             R"pbdoc(Reset brain to initial state)pbdoc")
        .def("step", py::overload_cast<SimulationStep>(&Brain::step),
             py::arg("step"), R"pbdoc(Advance simulation by one step)pbdoc")
        .def("step", py::overload_cast<SimulationStep, Timestamp>(&Brain::step),
             py::arg("step"), py::arg("time"), R"pbdoc(Advance simulation with time)pbdoc")
        .def("inject_current", &Brain::injectCurrent, py::arg("neuron_id"), py::arg("current"),
             R"pbdoc(Inject current into specific neuron)pbdoc")
        .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"), R"pbdoc(Inject current into neurons by type)pbdoc")
        .def("receive_sensory_input", &Brain::receiveSensoryInput,
             py::arg("input"), R"pbdoc(Receive sensory input)pbdoc")
        .def("produce_action", &Brain::produceAction,
             R"pbdoc(Generate motor action from brain activity)pbdoc")
        .def("apply_neuromodulation", &Brain::applyNeuromodulation,
             py::arg("signal"), R"pbdoc(Apply neuromodulatory signal)pbdoc")
        .def("update_plasticity", &Brain::updatePlasticity,
             R"pbdoc(Update synaptic plasticity rules)pbdoc")
        .def("develop", &Brain::develop,
             R"pbdoc(Apply developmental changes)pbdoc")
        .def("save", &Brain::save, py::arg("filepath"),
             R"pbdoc(Save brain state to checkpoint file)pbdoc")
        .def("load", &Brain::load, py::arg("filepath"),
             R"pbdoc(Load brain state from checkpoint file)pbdoc")
        .def("get_total_neuron_count", &Brain::getTotalNeuronCount,
             R"pbdoc(Get total number of neurons)pbdoc")
        .def("get_total_synapse_count", &Brain::getTotalSynapseCount,
             R"pbdoc(Get total number of synapses)pbdoc")
        .def("get_firing_neuron_count", &Brain::getFiringNeuronCount,
             R"pbdoc(Get number of currently firing neurons)pbdoc")
        .def("get_average_firing_rate", &Brain::getAverageFiringRate,
             R"pbdoc(Get average firing rate)pbdoc")
        .def("get_excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio,
             R"pbdoc(Get E/I balance)pbdoc")
        .def("get_total_spike_count", &Brain::getTotalSpikeCount,
             R"pbdoc(Get total spike count)pbdoc")
        .def("get_pending_spike_event_count", &Brain::getPendingSpikeEventCount,
             R"pbdoc(Get number of pending spike events)pbdoc")
        .def("get_active_neuron_count", &Brain::getActiveNeuronCount,
             R"pbdoc(Get number of active neurons)pbdoc")
        .def("get_working_memory", &Brain::getWorkingMemory,
             R"pbdoc(Get working memory system)pbdoc")
        .def("get_episodic_memory", &Brain::getEpisodicMemory,
             R"pbdoc(Get episodic memory system)pbdoc")
        .def("get_associative_memory", &Brain::getAssociativeMemory,
             R"pbdoc(Get associative memory system)pbdoc")
        .def("get_prediction_system", &Brain::getPredictionSystem,
             R"pbdoc(Get prediction system)pbdoc")
        .def("get_planner", &Brain::getPlanner,
             R"pbdoc(Get neural planner)pbdoc")
        .def("get_concept_formation", &Brain::getConceptFormation,
             R"pbdoc(Get concept formation system)pbdoc")
        .def("get_attention", &Brain::getAttention,
             R"pbdoc(Get attentional selection system)pbdoc")
        .def("get_development_system", &Brain::getDevelopmentSystem,
             R"pbdoc(Get development system)pbdoc")
        .def("get_dopamine", &Brain::getDopamine,
             R"pbdoc(Get dopamine system)pbdoc")
        .def("get_curiosity", &Brain::getCuriosity,
             R"pbdoc(Get curiosity system)pbdoc")
        .def("get_novelty", &Brain::getNovelty,
             R"pbdoc(Get novelty detection system)pbdoc")
        .def("get_prediction_error_signal", &Brain::getPredictionErrorSignal,
             R"pbdoc(Get prediction error system)pbdoc")
        .def("get_developmental_stage", &Brain::getDevelopmentalStage,
             R"pbdoc(Get current developmental stage)pbdoc")
        .def("set_developmental_stage", &Brain::setDevelopmentalStage, py::arg("stage"),
             R"pbdoc(Set developmental stage)pbdoc")
        .def("get_config", &Brain::getConfig,
             R"pbdoc(Get brain configuration)pbdoc")
        .def("get_random_generator", &Brain::getRandomGenerator,
             R"pbdoc(Get random number generator)pbdoc")
        .def("log_status", &Brain::logStatus,
             R"pbdoc(Log brain status information)pbdoc")
        .def("__repr__", [](const Brain& brain) {
            return "<NLM.Brain neurons=" + std::to_string(brain.getTotalNeuronCount()) +
                   " synapses=" + std::to_string(brain.getTotalSynapseCount()) + ">";
        });

    // World and Environment
    auto world_class = py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Virtual world simulation)")
        .def(py::init<>())
        .def("initialize", py::overload_cast<uint32_t, uint32_t>(&SimpleWorld::initialize),
             py::arg("width"), py::arg("height"), R"pbdoc(Initialize world with dimensions)pbdoc")
        .def("configure", py::overload_cast<float, float, uint32_t, uint32_t>(&SimpleWorld::configure),
             py::arg("width"), py::arg("height"), py::arg("vision_width"), py::arg("vision_height"),
             R"pbdoc(Configure world with vision parameters)pbdoc")
        .def("reset", &SimpleWorld::reset,
             R"pbdoc(Reset world to initial state)pbdoc")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             R"pbdoc(Update world by timestep)pbdoc")
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             R"pbdoc(Set agent starting position)pbdoc")
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("command"), py::arg("current_time"),
             R"pbdoc(Apply motor command to world)pbdoc")
        .def("apply_action", &SimpleWorld::applyAction,
             py::arg("brain"), py::arg("action"),
             R"pbdoc(Apply brain action to world)pbdoc")
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             R"pbdoc(Get sensory percept from world)pbdoc")
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             R"pbdoc(Get agent body)pbdoc")
        .def("compute_reward", &SimpleWorld::computeReward,
             py::arg("brain"), R"pbdoc(Compute reward for brain)pbdoc")
        .def("add_object", py::overload_cast<float, float, WorldObjectType, float>(&SimpleWorld::addObject),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value"),
             R"pbdoc(Add world object)pbdoc")
        .def("is_valid_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             R"pbdoc(Check if position is valid)pbdoc")
        .def("get_width", &SimpleWorld::getWidth,
             R"pbdoc(Get world width)pbdoc")
        .def("get_height", &SimpleWorld::getHeight,
             R"pbdoc(Get world height)pbdoc")
        .def("get_simulation_time", &SimpleWorld::getSimulationTime,
             R"pbdoc(Get simulation time)pbdoc")
        .def("get_max_energy", &SimpleWorld::getMaxEnergy,
             R"pbdoc(Get maximum energy)pbdoc")
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("energy"),
             R"pbdoc(Set maximum energy)pbdoc");

    // Agent Interface
    auto agent_class = py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent-brain interface)")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             R"pbdoc(Create agent brain interface)pbdoc")
        .def("initialize", py::overload_cast<SimpleWorld&>(&AgentBrain::initialize),
             py::arg("world"), R"pbdoc(Initialize agent with world)pbdoc")
        .def("process_sensory_input", &AgentBrain::processSensoryInput,
             py::arg("percept"), R"pbdoc(Process sensory input)pbdoc")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             R"pbdoc(Decode motor command from brain)pbdoc")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward"),
             R"pbdoc(Apply reward modulation)pbdoc")
        .def("update_development", &AgentBrain::updateDevelopment,
             py::arg("timestep"), R"pbdoc(Update development)pbdoc")
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation,
             py::arg("enabled"), R"pbdoc(Enable/disable reward modulation)pbdoc")
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enabled"), R"pbdoc(Enable/disable structural plasticity)pbdoc")
        .def("enable_development", &AgentBrain::enableDevelopment,
             py::arg("enabled"), R"pbdoc(Enable/disable development)pbdoc")
        .def("enable_curiosity", &AgentBrain::enableCuriosity,
             py::arg("enabled"), R"pbdoc(Enable/disable curiosity)pbdoc")
        .def("get_curiosity_level", &AgentBrain::getCuriosityLevel,
             R"pbdoc(Get curiosity level)pbdoc")
        .def("get_novelty_level", &AgentBrain::getNoveltyLevel,
             R"pbdoc(Get novelty level)pbdoc")
        .def("get_prediction_error", &AgentBrain::getPredictionError,
             R"pbdoc(Get prediction error)pbdoc")
        .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
             R"pbdoc(Get neuromodulation level)pbdoc")
        .def("get_brain", &AgentBrain::getBrain,
             R"pbdoc(Get underlying brain)pbdoc")
        .def("get_sensory_input_size", &AgentBrain::getSensoryInputSize,
             R"pbdoc(Get sensory input size)pbdoc")
        .def("get_motor_output_size", &AgentBrain::getMotorOutputSize,
             R"pbdoc(Get motor output size)pbdoc");

    // Sensory and Motor Types
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Sensory input data)")
        .def(py::init<>())
        .def("add_feature", &SensoryInput::addFeature, py::arg("feature"),
             R"pbdoc(Add sensory feature)pbdoc")
        .def("get_features", &SensoryInput::getFeatures,
             R"pbdoc(Get sensory features)pbdoc")
        .def("get_feature_count", &SensoryInput::getFeatureCount,
             R"pbdoc(Get number of features)pbdoc");

    py::class_<Action>(m, "Action", R"pbdoc(Motor action)")
        .def(py::init<>())
        .def("set_command", &Action::setCommand, py::arg("command"),
             R"pbdoc(Set action command)pbdoc")
        .def("get_command", &Action::getCommand,
             R"pbdoc(Get action command)pbdoc")
        .def("set_parameters", &Action::setParameters,
             py::arg("params"), R"pbdoc(Set action parameters)pbdoc")
        .def("get_parameters", &Action::getParameters,
             R"pbdoc(Get action parameters)pbdoc");

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory perception)")
        .def(py::init<>())
        .def("get_features", &SensoryPercept::getFeatures,
             R"pbdoc(Get percept features)pbdoc")
        .def("get_internal", &SensoryPercept::getInternal,
             R"pbdoc(Get internal state)pbdoc")
        .def("get_energy", &SensoryPercept::getEnergy,
             R"pbdoc(Get percept energy)pbdoc");

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)")
        .def(py::init<>())
        .def("get_position", &AgentBody::getPosition,
             R"pbdoc(Get agent position)pbdoc")
        .def("set_position", &AgentBody::setPosition, py::arg("x"), py::arg("y"),
             R"pbdoc(Set agent position)pbdoc")
        .def("get_energy", &AgentBody::getEnergy,
             R"pbdoc(Get agent energy)pbdoc")
        .def("set_energy", &AgentBody::setEnergy, py::arg("energy"),
             R"pbdoc(Set agent energy)pbdoc")
        .def("get_direction", &AgentBody::getDirection,
             R"pbdoc(Get agent direction)pbdoc")
        .def("set_direction", &AgentBody::setDirection, py::arg("direction"),
             R"pbdoc(Set agent direction)pbdoc");

    // Convenience Functions
    m.def("create_default_config", &createDefaultConfig,
          R"pbdoc(Create default NLM configuration)pbdoc");

    m.def("create_brain", [](std::shared_ptr<Config> config) {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc(Create brain with configuration)pbdoc");

    m.def("create_simple_world", &createSimpleWorld,
          R"pbdoc(Create simple world simulation)pbdoc");

    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(Create agent brain interface)pbdoc");

    m.def("create_sensory_input", &createSensoryInput,
          R"pbdoc(Create sensory input)pbdoc");

    m.def("create_motor_action", &createMotorAction,
          R"pbdoc(Create motor action)pbdoc");

    // Data Export and Analysis
    m.def("export_brain_data", &exportBrainDataToCSV, py::arg("brain"), py::arg("filepath"),
          R"pbdoc(Export brain data to CSV file)pbdoc");

    m.def("export_simulation_data", &exportSimulationDataToJSON, py::arg("brain"), py::arg("filepath"),
          R"pbdoc(Export simulation data to JSON file)pbdoc");

    // Performance Monitoring
    m.def("benchmark_simulation", &benchmarkSimulation, py::arg("brain"), py::arg("steps"),
          R"pbdoc(Run performance benchmark on simulation)pbdoc");

    m.def("profile_memory_usage", &profileMemoryUsage, py::arg("brain"),
          R"pbdoc(Profile memory usage of brain)pbdoc");

    // Utility Functions
    m.def("generate_random_config", &generateRandomConfig, py::arg("seed"),
          R"pbdoc(Generate random configuration with seed)pbdoc");

    m.def("run_phase3_demo", &runPhase3Demo, py::arg("num_episodes"), py::arg("steps_per_episode"),
          R"pbdoc(Run Phase 3 demonstration)pbdoc");

    m.def("run_phase4_demo", &runPhase4Demo, py::arg("num_trials"),
          R"pbdoc(Run Phase 4 demonstration)pbdoc");

    m.def("run_phase6_demo", &runPhase6Demo, py::arg("config"),
          R"pbdoc(Run Phase 6 integration demo)pbdoc");

    // Pythonic Convenience Classes
    py::class_<pynlm::SimulationRunner>(m, "SimulationRunner", R"pbdoc(Simulation runner with callbacks)")
        .def(py::init<>())
        .def("run", &SimulationRunner::run, py::arg("brain"), py::arg("steps"),
             py::arg("callback"), R"pbdoc(Run simulation with callback)pbdoc")
        .def("run_with_logging", &SimulationRunner::runWithLogging,
             py::arg("brain"), py::arg("steps"), py::arg("log_interval"),
             R"pbdoc(Run simulation with periodic logging)pbdoc");

    py::class_<pynlm::DataExporter>(m, "DataExporter", R"pbdoc(Data export utilities)")
        .def(py::init<>())
        .def("export_to_csv", &DataExporter::exportToCSV,
             py::arg("brain"), py::arg("filepath"), R"pbdoc(Export to CSV)pbdoc")
        .def("export_to_json", &DataExporter::exportToJSON,
             py::arg("brain"), py::arg("filepath"), R"pbdoc(Export to JSON)pbdoc")
        .def("export_to_hdf5", &DataExporter::exportToHDF5,
             py::arg("brain"), py::arg("filepath"), R"pbdoc(Export to HDF5)pbdoc");

    py::class_<pynlm::PerformanceProfiler>(m, "PerformanceProfiler", R"pbdoc(Performance profiling)")
        .def(py::init<>())
        .def("start_profiling", &PerformanceProfiler::startProfiling,
             R"pbdoc(Start performance profiling)pbdoc")
        .def("stop_profiling", &PerformanceProfiler::stopProfiling,
             R"pbdoc(Stop performance profiling)pbdoc")
        .def("get_profiling_results", &PerformanceProfiler::getProfilingResults,
             R"pbdoc(Get profiling results)pbdoc");

    // Constants and Enum Utilities
    m.attr("NEURON_TYPES") = py::dict(
        "EXCITATORY", NeuronType::Excitatory,
        "INHIBITORY", NeuronType::Inhibitory,
        "MODULATORY", NeuronType::Modulatory,
        "SENSORY", NeuronType::Sensory,
        "MOTOR", NeuronType::Motor,
        "INTERNAL", NeuronType::Internal
    );

    m.attr("DEVELOPMENTAL_STAGES") = py::dict(
        "INITIAL", DevelopmentalStage::Initial,
        "CRITICAL_PERIOD", DevelopmentalStage::CriticalPeriod,
        "MATURATION", DevelopmentalStage::Maturation,
        "ADULT", DevelopmentalStage::Adult,
        "AGING", DevelopmentalStage::Aging
    );

    // Error Classes
    py::register_exception<std::runtime_error>(m, "RuntimeError", PyExc_RuntimeError);
    py::register_exception<std::invalid_argument>(m, "InvalidArgument", PyExc_TypeError);
    py::register_exception<std::out_of_range>(m, "OutOfRange", PyExc_IndexError);

    // Module Attributes
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Project Authors";
    m.attr("__email__") = "contact@nlm-project.org";
    m.attr("__license__") = "MIT";
    m.attr("__github__") = "https://github.com/nlm-project/nlm";
}

PYBIND11_MODULE(pynlm, m) {
    // This is handled by the previous macro
}