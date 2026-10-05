#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <queue>
#include <functional>
#include <map>
#include <set>
#include <random>
#include <chrono>
#include <fstream>
#include <sstream>
#include <exception>
#include <stdexcept>
#include <cstddef>
#include <cstdint>
#include <atomic>
#include <thread>
#include <mutex>
#include <future>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/motor/Action.hpp"
#include "../src/performance/MemoryPool.hpp"
#include "../src/performance/SIMDVectorization.hpp"
#include "src/performance/ParallelProcessing.hpp"
#include "../src/performance/EventQueue.hpp"
#include "../src/performance/Performance.hpp"
#include "../src/core/SimulationClock/SimulationClock.hpp"
#include "../src/brain/NeuralPopulation.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/brain/ConfigurableNeuron.hpp"
#include "../src/brain/ConfigurableSynapse.hpp"
#include "../src/memory/Memory.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/world/Observation.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/visualization/VisualizationInterface.hpp"
#include "../src/experiments/Experiment.hpp"
#include "../src/experiments/ExperimentRunner.hpp"
#include "../src/experiments/Metrics.hpp"

// ===== WAVE 4: PERFORMANCE OPTIMIZATIONS =====

// Memory Management optimizations
namespace nlm {
    // ReferenceCounter class for smart reference counting
    class ReferenceCounter {
    public:
        ReferenceCounter() : count_(1) {}
        void addReference() { ++count_; }
        void removeReference() { if (--count_ == 0) delete this; }
        size_t references() const { return count_; }
    private:
        std::atomic<size_t> count_;
    };
}

// SIMDProcessor class for SIMD (SSE/AVX) optimizations
class SIMDProcessor {
public:
    SIMDProcessor() : isAvailable_(false) {
#if defined(__AVX512F__) && defined(__AVX512DQ__)
        isAvailable_ = true;
        simdType_ = "AVX-512";
#elif defined(__AVX2__)
        isAvailable_ = true;
        simdType_ = "AVX2";
#elif defined(__SSE4_1__) || defined(__SSE3__)
        isAvailable_ = true;
        simdType_ = "SSE";
#else
        isAvailable_ = false;
        simdType_ = "Scalar";
#endif
    }
    
    bool isAvailable() const { return isAvailable_; }
    const char* getSIMDType() const { return simdType_.c_str(); }
    size_t getVectorWidth() const {
        if (!isAvailable_) return 1;
#if defined(__AVX512F__) && defined(__AVX512DQ__)
        return 16;
#elif defined(__AVX2__)
        return 8;
#elif defined(__SSE4_1__) || defined(__SSE3__)
        return 4;
#endif
        return 1;
    }
    
    void optimizeMemoryAccess(const std::vector<float>& data) {
        // Implementation for cache-aware memory access
        cacheAlignedData_ = data;
    }
    
    const std::vector<float>& getCacheAlignedData() const { return cacheAlignedData_; }
    
private:
    bool isAvailable_;
    std::string simdType_;
    std::vector<float> cacheAlignedData_;
};

// VectorizedOperations class for vectorized mathematical operations
class VectorizedOperations {
public:
    VectorizedOperations() = default;
    
    static float sum(const std::vector<float>& data) {
        if (data.empty()) return 0.0f;
        return std::accumulate(data.begin(), data.end(), 0.0f);
    }
    
    static float mean(const std::vector<float>& data) {
        if (data.empty()) return 0.0f;
        return sum(data) / data.size();
    }
    
    static float variance(const std::vector<float>& data) {
        if (data.size() < 2) return 0.0f;
        float m = mean(data);
        float sumSq = 0.0f;
        for (float val : data) {
            float diff = val - m;
            sumSq += diff * diff;
        }
        return sumSq / (data.size() - 1);
    }
    
    static float max(const std::vector<float>& data) {
        if (data.empty()) return 0.0f;
        return *std::max_element(data.begin(), data.end());
    }
    
    static void clamp(std::vector<float>& data, float minVal, float maxVal) {
        for (float& val : data) {
            if (val < minVal) val = minVal;
            else if (val > maxVal) val = maxVal;
        }
    }
    
    static std::vector<float> normalize(const std::vector<float>& data) {
        if (data.empty()) return data;
        float minVal = *std::min_element(data.begin(), data.end());
        float maxVal = *std::max_element(data.begin(), data.end());
        float range = maxVal - minVal;
        if (range < 0.0001f) return data;
        
        std::vector<float> normalized;
        for (float val : data) {
            normalized.push_back((val - minVal) / range);
        }
        return normalized;
    }
};

// CompressedData class for data compression
class CompressedData {
public:
    CompressedData() = default;
    explicit CompressedData(const std::vector<float>& original) {
        compress(original);
    }
    
    void compress(const std::vector<float>& data) {
        if (data.empty()) return;
        
        // Simple run-length compression for similar values
        std::vector<float> compressed;
        size_t i = 0;
        while (i < data.size()) {
            size_t runLength = 1;
            while (i + runLength < data.size() && 
                   std::abs(data[i + runLength] - data[i]) < 0.0001f) {
                ++runLength;
            }
            
            if (runLength > 2) {
                // Compress as run
                compressed.push_back(data[i]);
                compressed.push_back(static_cast<float>(runLength));
            } else {
                // Store individual values
                for (size_t j = 0; j < runLength; ++j) {
                    compressed.push_back(data[i + j]);
                }
            }
            
            i += runLength;
        }
        
        original_ = data;
        compressed_ = compressed;
    }
    
    void decompress(std::vector<float>& result) const {
        result.clear();
        result.reserve(original_.size());
        
        for (size_t i = 0; i < compressed_.size(); ++i) {
            float value = compressed_[i];
            ++i; // Skip run length or next value
            if (i < compressed_.size() && value < 1000.0f) { // Run length
                size_t runLength = static_cast<size_t>(value);
                for (size_t j = 0; j < runLength; ++j) {
                    result.push_back(original_[result.size()]);
                }
            } else {
                result.push_back(value);
            }
        }
    }
    
    size_t compressedSize() const { return compressed_.size(); }
    size_t originalSize() const { return original_.size(); }
    float compressionRatio() const {
        if (original_.empty()) return 1.0f;
        return static_cast<float>(compressed_.size()) / original_.size();
    }
    
private:
    std::vector<float> original_;
    std::vector<float> compressed_;
};

// SparseMatrix class for sparse matrix operations
class SparseMatrix {
public:
    SparseMatrix() = default;
    SparseMatrix(size_t rows, size_t cols) : rows_(rows), cols_(cols) {}
    
    void set(size_t row, size_t col, float value) {
        if (value != 0.0f) {
            entries_[std::make_pair(row, col)] = value;
        }
    }
    
    float get(size_t row, size_t col) const {
        auto it = entries_.find(std::make_pair(row, col));
        return (it != entries_.end()) ? it->second : 0.0f;
    }
    
    size_t nonZeroCount() const { return entries_.size(); }
    
    void multiply(const std::vector<float>& vector, std::vector<float>& result) const {
        result.assign(rows_, 0.0f);
        for (const auto& entry : entries_) {
            size_t row = entry.first.first;
            size_t col = entry.first.second;
            float value = entry.second;
            result[row] += value * vector[col];
        }
    }
    
    void transpose(SparseMatrix& result) const {
        result = SparseMatrix(cols_, rows_);
        for (const auto& entry : entries_) {
            result.entries_[std::make_pair(entry.first.second, entry.first.first)] = entry.second;
        }
    }
    
private:
    size_t rows_;
    size_t cols_;
    std::unordered_map<std::pair<size_t, size_t>, float> entries_;
};

// TensorView class for efficient tensor operations
class TensorView {
public:
    TensorView() = default;
    TensorView(const std::vector<float>& data, const std::vector<size_t>& shape) 
        : data_(data), shape_(shape) {}
    
    const std::vector<size_t>& shape() const { return shape_; }
    size_t size() const { return data_.size(); }
    
    void reshape(const std::vector<size_t>& newShape) {
        size_t total = 1;
        for (size_t s : newShape) total *= s;
        if (total != data_.size()) {
            throw std::invalid_argument("Cannot reshape tensor with different total elements");
        }
        shape_ = newShape;
    }
    
    void squeeze() {
        // Remove dimensions of size 1
        std::vector<size_t> newShape;
        for (size_t s : shape_) {
            if (s != 1) newShape.push_back(s);
        }
        if (newShape.empty()) newShape = {1};
        reshape(newShape);
    }
    
    void expandDims(size_t axis) {
        // Insert dimension of size 1 at given axis
        std::vector<size_t> newShape = shape_;
        newShape.insert(newShape.begin() + axis, 1);
        reshape(newShape);
    }
    
    float sum() const {
        return std::accumulate(data_.begin(), data_.end(), 0.0f);
    }
    
    float mean() const {
        if (data_.empty()) return 0.0f;
        return sum() / data_.size();
    }
    
    std::vector<float> flatten() const { return data_; }
    
    // Strided access for efficiency
    float get(size_t index, const std::vector<size_t>& strides, size_t offset = 0) const {
        return data_[offset + index];
    }
    
private:
    std::vector<float> data_;
    std::vector<size_t> shape_;
};

// DataBuffer class for optimized data buffering
class DataBuffer {
public:
    DataBuffer() = default;
    explicit DataBuffer(size_t capacity) : capacity_(capacity), size_(0), head_(0) {
        buffer_.resize(capacity);
    }
    
    void push(const std::vector<float>& data) {
        if (data.size() > capacity_ - size_) {
            throw std::runtime_error("Buffer full");
        }
        for (float val : data) {
            buffer_[head_++] = val;
        }
        size_ += data.size();
        if (head_ >= capacity_) head_ = 0;
    }
    
    bool pop(std::vector<float>& data, size_t count) {
        if (count > size_) return false;
        
        data.clear();
        data.reserve(count);
        
        for (size_t i = 0; i < count; ++i) {
            size_t index = (head_ + i) % capacity_;
            data.push_back(buffer_[index]);
        }
        
        head_ = (head_ + count) % capacity_;
        size_ -= count;
        return true;
    }
    
    void clear() {
        size_ = 0;
        head_ = 0;
    }
    
    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }
    
    const std::vector<float>& getBuffer() const { return buffer_; }
    
private:
    size_t capacity_;
    size_t size_;
    size_t head_;
    std::vector<float> buffer_;
};

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ===== CUSTOM EXCEPTION CLASSES =====
    py::register_exception<NLMRuntimeError>(m, "NLMRuntimeError");
    py::register_exception<NLMFileError>(m, "NLMFileError");
    py::register_exception<NLMConfigError>(m, "NLMConfigError");
    py::register_exception<NLMNeuralError>(m, "NLMNeuralError");
    py::register_exception<LMSSensoryError>(m, "LMSSensoryError");
    py::register_exception<NLMFeatureError>(m, "NLMFeatureError");
    py::register_exception<NLMMemoryError>(m, "NLMMemoryError");
    py::register_exception<NLMAnalysisError>(m, "NLMAnalysisError");

    // ===== IDENTIFIER TYPES =====
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0)
        .def(py::init<const NeuronId&>())
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__lt__", &NeuronId::operator<)
        .def("__gt__", &NeuronId::operator>)
        .def("__le__", &NeuronId::operator<=)
        .def("__ge__", &NeuronId::operator>=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        })
        .def_property("value", [](const NeuronId& self) { return self.value; }, 
                     [](NeuronId& self, uint64_t v) { self.value = v; })
        .def_property("index", [](const NeuronId& self) { return self.index(); },
                     nullptr)
        .def("__enter__", [](py::object& self) { return self; })
        .def("__exit__", [](py::object& self, py::args) {});

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0)
        .def(py::init<const SynapseId&>())
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__lt__", &SynapseId::operator<)
        .def("__gt__", &SynapseId::operator>)
        .def("__le__", &SynapseId::operator<=)
        .def("__ge__", &SynapseId::operator>=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        })
        .def_property("value", [](const SynapseId& self) { return self.value; }, 
                     [](SynapseId& self, uint64_t v) { self.value = v; })
        .def_property("index", [](const SynapseId& self) { return self.index(); },
                     nullptr)
        .def("__enter__", [](py::object& self) { return self; })
        .def("__exit__", [](py::object& self, py::args) {});

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0)
        .def(py::init<const RegionId&>())
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__lt__", &RegionId::operator<)
        .def("__gt__", &RegionId::operator>)
        .def("__le__", &RegionId::operator<=)
        .def("__ge__", &RegionId::operator>=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        })
        .def_property("value", [](const RegionId& self) { return self.value; }, 
                     [](RegionId& self, uint64_t v) { self.value = v; })
        .def_property("index", [](const RegionId& self) { return self.index(); },
                     nullptr)
        .def("__enter__", [](py::object& self) { return self; })
        .def("__exit__", [](py::object& self, py::args) {});

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0)
        .def(py::init<const PopulationId&>())
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__lt__", &PopulationId::operator<)
        .def("__gt__", &PopulationId::operator>)
        .def("__le__", &PopulationId::operator<=)
        .def("__ge__", &PopulationId::operator>=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const PopulationId& id) {
            return std::to_string(id.value);
        })
        .def_property("value", [](const PopulationId& self) { return self.value; }, 
                     [](PopulationId& self, uint64_t v) { self.value = v; })
        .def_property("index", [](const PopulationId& self) { return self.index(); },
                     nullptr);

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

    // ===== ACTION TYPE ENUMERATION =====
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

    // ===== CONFIG CLASS =====
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. Raises RuntimeError if file not found or invalid.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file. Raises RuntimeError on write failure.")
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
        })
        .def("__enter__", [](py::object& self) { return self; })
        .def("__exit__", [](py::object& self, py::args) {});

    // ===== SENSORY INPUT CLASSES =====
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def(py::init<>())
        .def_property("type", &SensoryInput::getType, nullptr)
        .def_property("data", &SensoryInput::getData, nullptr)
        .def_property("dimensions", &SensoryInput::getDimensions, nullptr)
        .def_property("timestamp", &SensoryInput::getTimestamp, &SensoryInput::setTimestamp)
        .def("__enter__", [](py::object& self) { return self; })
        .def("__exit__", [](py::object& self, py::args) {});

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            if (data.size() < 3 * self.getWidth() * self.getHeight()) {
                throw std::runtime_error("Vision data size mismatch: expected at least " + 
                                      std::to_string(3 * self.getWidth() * self.getHeight()) + 
                                      " values, got " + std::to_string(data.size()));
            }
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("__enter__", [](py::object& self) { return self; })
        .def("__exit__", [](py::object& self, py::args) {});

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            if (data.size() < self.getNumSamples()) {
                throw std::runtime_error("Audio data size mismatch: expected at least " + 
                                      std::to_string(self.getNumSamples()) + 
                                      " values, got " + std::to_string(data.size()));
            }
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("__enter__", [](py::object& self) { return self; })
        .def("__exit__", [](py::object& self, py::args) {});

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("__enter__", [](py::object& self) { return self; })
        .def("__exit__", [](py::object& self, py::args) {});

namespace py = pybind11;
namespace nlm {

// ===== FEATURE EXTRACTION CLASSES =====

// Feature extractor for automatic feature extraction from sensory data
class FeatureExtractor {
public:
    FeatureExtractor() = default;
    
    // Extract features from vision data
    std::vector<float> extractVisionFeatures(const std::vector<float>& visionData, 
                                            size_t width, size_t height) const {
        if (visionData.empty()) return {};
        
        // Basic feature extraction: compute statistical features
        std::vector<float> features;
        
        // 1. Mean intensity
        float sum = std::accumulate(visionData.begin(), visionData.end(), 0.0f);
        features.push_back(sum / visionData.size());
        
        // 2. Standard deviation (texture/contrast)
        float variance = 0.0f;
        for (float val : visionData) {
            float diff = val - features[0];
            variance += diff * diff;
        }
        features.push_back(std::sqrt(variance / visionData.size()));
        
        // 3. Edge detection using Sobel-like filter
        float edgeSum = 0.0f;
        size_t edgeCount = 0;
        for (size_t y = 1; y < height - 1; ++y) {
            for (size_t x = 1; x < width - 1; ++x) {
                float center = visionData[y * width + x];
                float right = visionData[y * width + (x + 1)];
                float left = visionData[y * width + (x - 1)];
                float down = visionData[(y + 1) * width + x];
                float up = visionData[(y - 1) * width + x];
                
                float gradientX = (right - left) * 0.5f;
                float gradientY = (down - up) * 0.5f;
                edgeSum += std::sqrt(gradientX * gradientX + gradientY * gradientY);
                edgeCount++;
            }
        }
        features.push_back(edgeCount > 0 ? edgeSum / edgeCount : 0.0f);
        
        // 4. Histogram-based features (8 bins)
        float hist[8] = {0.0f};
        float maxVal = *std::max_element(visionData.begin(), visionData.end());
        float minVal = *std::min_element(visionData.begin(), visionData.end());
        float range = maxVal - minVal;
        if (range > 0.0f) {
            for (float val : visionData) {
                int bin = static_cast<int>((val - minVal) / range * 8.0f);
                if (bin >= 0 && bin < 8) hist[bin]++;
            }
            for (int i = 0; i < 8; ++i) {
                features.push_back(hist[i] / visionData.size());
            }
        } else {
            for (int i = 0; i < 8; ++i) features.push_back(0.0f);
        }
        
        return features;
    }
    
    // Extract features from touch data
    std::vector<float> extractTouchFeatures(const std::vector<float>& touchData) const {
        std::vector<float> features;
        
        if (touchData.empty()) return features;
        
        // Binary features: active/inactive
        size_t activeCount = std::count_if(touchData.begin(), touchData.end(), 
                                           [](float v) { return v > 0.5f; });
        features.push_back(static_cast<float>(activeCount) / touchData.size());
        
        // Proximity features (average distance)
        std::vector<float> activeDistances;
        for (float val : touchData) {
            if (val > 0.5f) {
                activeDistances.push_back(1.0f - val); // Convert activation to distance
            }
        }
        if (!activeDistances.empty()) {
            float sum = std::accumulate(activeDistances.begin(), activeDistances.end(), 0.0f);
            features.push_back(sum / activeDistances.size());
        } else {
            features.push_back(0.0f);
        }
        
        return features;
    }
    
    // Extract features from internal signals
    std::vector<float> extractInternalFeatures(const std::vector<float>& internalData) const {
        std::vector<float> features;
        
        if (internalData.empty()) return features;
        
        // Statistical features
        float sum = std::accumulate(internalData.begin(), internalData.end(), 0.0f);
        features.push_back(sum / internalData.size()); // Mean
        
        float variance = 0.0f;
        for (float val : internalData) {
            float diff = val - features[0];
            variance += diff * diff;
        }
        features.push_back(std::sqrt(variance / internalData.size())); // Std dev
        
        // Range
        float maxVal = *std::max_element(internalData.begin(), internalData.end());
        float minVal = *std::min_element(internalData.begin(), internalData.end());
        features.push_back(maxVal - minVal);
        
        return features;
    }
    
    // Extract features from proprioception data
    std::vector<float> extractProprioceptionFeatures(const std::vector<float>& proprioData) const {
        std::vector<float> features;
        
        if (proprioData.empty()) return features;
        
        // Position features
        float sum = std::accumulate(proprioData.begin(), proprioData.end(), 0.0f);
        features.push_back(sum / proprioData.size()); // Mean position
        
        // Velocity features (approximated from differences)
        float velocityVar = 0.0f;
        for (size_t i = 1; i < proprioData.size(); ++i) {
            float diff = proprioData[i] - proprioData[i-1];
            velocityVar += diff * diff;
        }
        features.push_back(std::sqrt(velocityVar / (proprioData.size() - 1))); // Velocity variation
        
        return features;
    }
    
    // Automatic dimensionality reduction using PCA (simplified)
    std::vector<float> reduceDimensions(const std::vector<std::vector<float>>& features, 
                                        size_t targetDim) const {
        if (features.empty() || targetDim >= features[0].size()) {
            // If already smaller or target not specified, return first feature set
            return features.empty() ? std::vector<float>() : features[0];
        }
        
        // Simple dimensionality reduction: keep first N components
        std::vector<float> reduced;
        for (const auto& featureSet : features) {
            size_t minSize = std::min(targetDim, featureSet.size());
            for (size_t i = 0; i < minSize; ++i) {
                reduced.push_back(featureSet[i]);
            }
        }
        
        return reduced;
    }
    
    // Feature selection based on variance threshold
    std::vector<float> selectFeatures(const std::vector<std::vector<float>>& featureSets, 
                                     float varianceThreshold = 0.01f) const {
        if (featureSets.empty()) return {};
        
        size_t numFeatures = featureSets[0].size();
        std::vector<bool> selected(numFeatures, false);
        
        // Calculate variance for each feature across all samples
        for (size_t f = 0; f < numFeatures; ++f) {
            float mean = 0.0f, variance = 0.0f;
            size_t count = 0;
            
            for (const auto& featureSet : featureSets) {
                if (f < featureSet.size()) {
                    mean += featureSet[f];
                    count++;
                }
            }
            
            if (count > 0) {
                mean /= count;
                for (const auto& featureSet : featureSets) {
                    if (f < featureSet.size()) {
                        float diff = featureSet[f] - mean;
                        variance += diff * diff;
                    }
                }
                variance /= count;
                
                if (variance >= varianceThreshold) {
                    selected[f] = true;
                }
            }
        }
        
        // Return selected features
        std::vector<float> selectedFeatures;
        for (size_t f = 0; f < numFeatures; ++f) {
            if (selected[f]) {
                for (const auto& featureSet : featureSets) {
                    if (f < featureSet.size()) {
                        selectedFeatures.push_back(featureSet[f]);
                    }
                }
            }
        }
        
        return selectedFeatures;
    }
    
    // Feature normalization (min-max scaling)
    std::vector<float> normalizeFeatures(const std::vector<float>& features) const {
        if (features.empty()) return features;
        
        float minVal = *std::min_element(features.begin(), features.end());
        float maxVal = *std::max_element(features.begin(), features.end());
        
        if (maxVal - minVal < 0.0001f) return features; // Avoid division by zero
        
        std::vector<float> normalized;
        for (float val : features) {
            normalized.push_back((val - minVal) / (maxVal - minVal));
        }
        
        return normalized;
    }
    
    // Feature scaling (z-score normalization)
    std::vector<float> scaleFeatures(const std::vector<float>& features) const {
        if (features.empty()) return features;
        
        float mean = std::accumulate(features.begin(), features.end(), 0.0f) / features.size();
        float variance = 0.0f;
        for (float val : features) {
            float diff = val - mean;
            variance += diff * diff;
        }
        variance /= features.size();
        float stddev = std::sqrt(variance);
        
        if (stddev < 0.0001f) return features; // Avoid division by zero
        
        std::vector<float> scaled;
        for (float val : features) {
            scaled.push_back((val - mean) / stddev);
        }
        
        return scaled;
    }
};

// Multi-modal sensory fusion class
class SensoryFusion {
public:
    SensoryFusion() = default;
    
    // Combine vision and touch modalities
    std::vector<float> fuseVisionTouch(const std::vector<float>& visionFeatures,
                                        const std::vector<float>& touchFeatures,
                                        const std::vector<float>& weights = {}) const {
        // Use provided weights or default equal weighting
        std::vector<float> w = weights.empty() ? std::vector<float>(visionFeatures.size() + touchFeatures.size(), 1.0f) : weights;
        
        // Normalize weights
        float weightSum = std::accumulate(w.begin(), w.end(), 0.0f);
        if (weightSum > 0.0f) {
            for (float& weight : w) weight /= weightSum;
        }
        
        // Fuse features
        std::vector<float> fused;
        size_t minSize = std::min(visionFeatures.size(), touchFeatures.size());
        
        for (size_t i = 0; i < minSize; ++i) {
            float visionWeight = w[i];
            float touchWeight = w[i + minSize];
            float fusedFeature = visionFeatures[i] * visionWeight + touchFeatures[i] * touchWeight;
            fused.push_back(fusedFeature);
        }
        
        return fused;
    }
    
    // Cross-modal attention and weighting
    std::vector<float> crossModalAttention(const std::vector<std::vector<float>>& modalityFeatures,
                                           const std::vector<float>& attentionWeights) const {
        std::vector<float> attended;
        
        if (modalityFeatures.empty()) return attended;
        size_t numFeatures = modalityFeatures[0].size();
        
        // Apply attention weights to each modality
        for (size_t i = 0; i < numFeatures; ++i) {
            float attendedFeature = 0.0f;
            float weightSum = 0.0f;
            
            for (size_t modality = 0; modality < modalityFeatures.size(); ++modality) {
                if (i < modalityFeatures[modality].size() && modality < attentionWeights.size()) {
                    attendedFeature += modalityFeatures[modality][i] * attentionWeights[modality];
                    weightSum += attentionWeights[modality];
                }
            }
            
            if (weightSum > 0.0f) {
                attended.push_back(attendedFeature / weightSum);
            }
        }
        
        return attended;
    }
    
    // Sensory consistency checking
    float checkSensoryConsistency(const std::vector<std::vector<float>>& sensoryData) const {
        if (sensoryData.size() < 2) return 1.0f;
        
        float consistencyScore = 0.0f;
        
        // Compare all pairs of modalities
        for (size_t i = 0; i < sensoryData.size(); ++i) {
            for (size_t j = i + 1; j < sensoryData.size(); ++j) {
                float similarity = calculateSimilarity(sensoryData[i], sensoryData[j]);
                consistencyScore += similarity;
            }
        }
        
        return consistencyScore / (sensoryData.size() * (sensoryData.size() - 1) * 0.5f);
    }
    
    // Sensor fusion using weighted averaging
    std::vector<float> weightedFusion(const std::vector<std::vector<float>>& modalities,
                                       const std::vector<float>& confidenceWeights) const {
        std::vector<float> fused;
        
        if (modalities.empty()) return fused;
        
        size_t numFeatures = modalities[0].size();
        
        for (size_t i = 0; i < numFeatures; ++i) {
            float sum = 0.0f;
            float weightSum = 0.0f;
            
            for (size_t m = 0; m < modalities.size(); ++m) {
                if (i < modalities[m].size() && m < confidenceWeights.size()) {
                    sum += modalities[m][i] * confidenceWeights[m];
                    weightSum += confidenceWeights[m];
                }
            }
            
            if (weightSum > 0.0f) {
                fused.push_back(sum / weightSum);
            }
        }
        
        return fused;
    }
    
    // Complementary fusion (sum of different scales)
    std::vector<float> complementaryFusion(const std::vector<std::vector<float>>& modalities) const {
        std::vector<float> fused;
        
        if (modalities.empty()) return fused;
        
        size_t numFeatures = modalities[0].size();
        
        for (size_t i = 0; i < numFeatures; ++i) {
            float sum = 0.0f;
            size_t validCount = 0;
            
            for (const auto& modality : modalities) {
                if (i < modality.size()) {
                    sum += modality[i];
                    validCount++;
                }
            }
            
            fused.push_back(validCount > 0 ? sum / validCount : 0.0f);
        }
        
        return fused;
    }
    
private:
    float calculateSimilarity(const std::vector<float>& a, const std::vector<float>& b) const {
        if (a.empty() || b.empty()) return 0.0f;
        
        size_t minSize = std::min(a.size(), b.size());
        float dotProduct = 0.0f, normA = 0.0f, normB = 0.0f;
        
        for (size_t i = 0; i < minSize; ++i) {
            dotProduct += a[i] * b[i];
            normA += a[i] * a[i];
            normB += b[i] * b[i];
        }
        
        normA = std::sqrt(normA);
        normB = std::sqrt(normB);
        
        if (normA * normB < 0.0001f) return 0.0f;
        
        return dotProduct / (normA * normB);
    }
};

// Sensory preprocessing pipeline class
class SensoryPreprocessor {
public:
    SensoryPreprocessor() = default;
    
    // Simple moving average filter
    std::vector<float> movingAverageFilter(const std::vector<float>& data, size_t windowSize) const {
        if (data.empty() || windowSize == 0) return data;
        
        std::vector<float> filtered;
        windowSize = std::min(windowSize, data.size());
        
        for (size_t i = 0; i < data.size(); ++i) {
            size_t start = std::max<size_t>(0, i - static_cast<int>(windowSize) / 2);
            size_t end = std::min(data.size(), i + static_cast<int>(windowSize) / 2 + 1);
            
            float sum = std::accumulate(data.begin() + start, data.begin() + end, 0.0f);
            filtered.push_back(sum / (end - start));
        }
        
        return filtered;
    }
    
    // Low-pass Butterworth filter (simplified)
    std::vector<float> lowPassFilter(const std::vector<float>& data, float cutoffFreq, float sampleRate) const {
        std::vector<float> filtered = data;
        float alpha = 2.0f * M_PI * cutoffFreq / sampleRate;
        
        // Apply simple exponential moving average
        for (size_t i = 1; i < filtered.size(); ++i) {
            filtered[i] = filtered[i-1] * (1.0f - alpha) + filtered[i] * alpha;
        }
        
        return filtered;
    }
    
    // Noise reduction using median filter
    std::vector<float> medianFilter(const std::vector<float>& data, size_t windowSize) const {
        if (data.empty() || windowSize == 0) return data;
        
        std::vector<float> filtered;
        windowSize = std::min(windowSize, data.size());
        
        // Use sorted window for median calculation
        for (size_t i = 0; i < data.size(); ++i) {
            std::vector<float> window;
            size_t start = std::max<size_t>(0, i - static_cast<int>(windowSize) / 2);
            size_t end = std::min(data.size(), i + static_cast<int>(windowSize) / 2 + 1);
            
            window.reserve(end - start);
            for (size_t j = start; j < end; ++j) {
                window.push_back(data[j]);
            }
            
            std::sort(window.begin(), window.end());
            filtered.push_back(window[window.size() / 2]);
        }
        
        return filtered;
    }
    
    // High-pass filter to remove DC offset
    std::vector<float> highPassFilter(const std::vector<float>& data) const {
        std::vector<float> filtered = data;
        
        if (filtered.empty()) return filtered;
        
        // Subtract mean
        float mean = std::accumulate(filtered.begin(), filtered.end(), 0.0f) / filtered.size();
        for (float& val : filtered) {
            val -= mean;
        }
        
        // Apply simple high-pass filter
        float alpha = 0.1f; // Filter coefficient
        for (size_t i = 1; i < filtered.size(); ++i) {
            filtered[i] = filtered[i] - filtered[i-1] * (1.0f - alpha);
        }
        
        return filtered;
    }
    
    // Adaptive preprocessing based on context
    std::vector<float> adaptiveFilter(const std::vector<float>& data, 
                                      float sensitivity = 0.5f) const {
        std::vector<float> filtered = data;
        
        if (filtered.empty()) return filtered;
        
        // Calculate rolling statistics
        size_t windowSize = std::min<size_t>(100, filtered.size());
        std::vector<float> smoothed(windowSize);
        
        for (size_t i = 0; i < windowSize; ++i) {
            size_t start = std::max<size_t>(0, i - windowSize / 2);
            size_t end = std::min(windowSize, i + windowSize / 2 + 1);
            
            float sum = std::accumulate(filtered.begin() + start, filtered.begin() + end, 0.0f);
            smoothed[i] = sum / (end - start);
        }
        
        // Adaptively filter based on sensitivity
        for (size_t i = 0; i < filtered.size(); ++i) {
            if (i < windowSize) {
                float deviation = std::abs(filtered[i] - smoothed[i]);
                float threshold = sensitivity * (filtered[i] - smoothed[i]);
                
                if (deviation > threshold) {
                    filtered[i] = smoothed[i]; // Smooth if deviation is high
                }
            }
        }
        
        return filtered;
    }
    
    // Automated pipeline creation based on data characteristics
    std::vector<std::vector<float>> createPipeline(const std::vector<float>& data) const {
        std::vector<std::vector<float>> pipeline;
        
        if (data.empty()) return pipeline;
        
        // Determine data characteristics
        float range = *std::max_element(data.begin(), data.end()) - 
                      *std::min_element(data.begin(), data.end());
        float mean = std::accumulate(data.begin(), data.end(), 0.0f) / data.size();
        
        // Add appropriate preprocessing steps
        if (range > 10.0f) { // Large range, apply scaling
            std::vector<float> scaled = data;
            float minVal = *std::min_element(data.begin(), data.end());
            for (float& val : scaled) {
                val = (val - minVal) / range;
            }
            pipeline.push_back(scaled);
        }
        
        // Always apply smoothing for time-series data
        std::vector<float> smoothed = movingAverageFilter(data, 5);
        pipeline.push_back(smoothed);
        
        // Apply noise reduction if data is noisy
        float variance = 0.0f;
        for (float val : data) {
            float diff = val - mean;
            variance += diff * diff;
        }
        variance /= data.size();
        
        if (variance > 1.0f) { // High variance likely indicates noise
            std::vector<float> denoised = medianFilter(data, 7);
            pipeline.push_back(denoised);
        }
        
        return pipeline;
    }
};

    // ===== SIMPLE WORLD CLASS =====
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("__enter__", [](py::object& self) { return self; })
        .def("__exit__", [](py::object& self, py::args) {
            self.attr("reset")();
        });

    // ===== BRAIN CLASS =====
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
             "Save brain state to file. Raises RuntimeError on write failure.")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file. Raises RuntimeError if file not found or invalid.")
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
             "Log brain status")
        .def("__enter__", [](py::object& self) { return self; })
        .def("__exit__", [](py::object& self, py::args) {
            self.attr("reset")();
        });

    // ===== AGENT FACTORY CLASS =====
    
    // Agent configuration management
    py::class_<Config>(m, "AgentConfig", R"pbdoc(Agent configuration for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. Raises RuntimeError if file not found or invalid.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file. Raises RuntimeError on write failure.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<AgentConfig: " + cfg.summary() + ">";
        })
        .def("__enter__", [](py::object& self) { return self; })
        .def("__exit__", [](py::object& self, py::args) {});
    
    // Template-based agent generation
    m.def("createAgentTemplate", [](const std::string& typeName, 
                                   const std::unordered_map<std::string, float>& params) {
        auto config = std::make_shared<Config>();
        
        // Standard configuration for different agent types
        if (typeName == "standard") {
            config->set("agent.type", "standard");
            config->set("agent.learning_rate", params.count("learning_rate") ? params.at("learning_rate") : 0.01f);
            config->set("agent.memory_capacity", params.count("memory_capacity") ? params.at("memory_capacity") : 1000);
            config->set("agent.exploration_rate", params.count("exploration_rate") ? params.at("exploration_rate") : 0.1f);
            config->set("agent.reward_decay", params.count("reward_decay") ? params.at("reward_decay") : 0.99f);
            config->set("agent.epsilon_decay", params.count("epsilon_decay") ? params.at("epsilon_decay") : 0.995f);
        } else if (typeName == "exploration") {
            config->set("agent.type", "exploration");
            config->set("agent.curiosity_weight", params.count("curiosity_weight") ? params.at("curiosity_weight") : 1.0f);
            config->set("agent.novelty_threshold", params.count("novelty_threshold") ? params.at("novelty_threshold") : 0.5f);
            config->set("agent.uncertainty_weight", params.count("uncertainty_weight") ? params.at("uncertainty_weight") : 0.8f);
        } else if (typeName == "prediction") {
            config->set("agent.type", "prediction");
            config->set("agent.prediction_horizon", params.count("prediction_horizon") ? params.at("prediction_horizon") : 10);
            config->set("agent.prediction_error_weight", params.count("prediction_error_weight") ? params.at("prediction_error_weight") : 0.5f);
            config->set("agent.model_update_rate", params.count("model_update_rate") ? params.at("model_update_rate") : 0.1f);
        } else if (typeName == "social") {
            config->set("agent.type", "social");
            config->set("agent.social_weight", params.count("social_weight") ? params.at("social_weight") : 0.7f);
            config->set("agent.imitation_rate", params.count("imitation_rate") ? params.at("imitation_rate") : 0.3f);
            config->set("agent.coordination_threshold", params.count("coordination_threshold") ? params.at("coordination_threshold") : 0.6f);
        } else if (typeName == "developmental") {
            config->set("agent.type", "developmental");
            config->set("agent.growth_rate", params.count("growth_rate") ? params.at("growth_rate") : 0.05f);
            config->set("agent.plasticity_window", params.count("plasticity_window") ? params.at("plasticity_window") : 100);
            config->set("agent.maturation_threshold", params.count("maturation_threshold") ? params.at("maturation_threshold") : 0.8f);
        }
        
        return config;
    }, py::arg("typeName"), py::arg("params") = std::unordered_map<std::string, float>(),
       "Create agent configuration template with specified type and parameters");
    
    // Agent parameter validation
    m.def("validateAgentParams", [](const std::unordered_map<std::string, float>& params) {
        std::unordered_map<std::string, std::string> errors;
        
        // Validate learning rate
        if (params.count("learning_rate")) {
            float lr = params.at("learning_rate");
            if (lr < 0.0f || lr > 1.0f) {
                errors["learning_rate"] = "Must be between 0.0 and 1.0";
            }
        }
        
        // Validate exploration rate
        if (params.count("exploration_rate")) {
            float er = params.at("exploration_rate");
            if (er < 0.0f || er > 1.0f) {
                errors["exploration_rate"] = "Must be between 0.0 and 1.0";
            }
        }
        
        // Validate memory capacity
        if (params.count("memory_capacity")) {
            int mc = static_cast<int>(params.at("memory_capacity"));
            if (mc < 10 || mc > 1000000) {
                errors["memory_capacity"] = "Must be between 10 and 1,000,000";
            }
        }
        
        // Validate novelty threshold
        if (params.count("novelty_threshold")) {
            float nt = params.at("novelty_threshold");
            if (nt < 0.0f || nt > 1.0f) {
                errors["novelty_threshold"] = "Must be between 0.0 and 1.0";
            }
        }
        
        return errors;
    }, py::arg("params"),
       "Validate agent parameters and return any errors");
    
    // ===== SPECIALIZED AGENT FACTORIES =====
    
    // Exploration agent factory
    m.def("createExplorationAgent", [](std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        
        // Configure for exploration behavior
        config->set("agent.type", "exploration");
        config->set("agent.exploration_mode", true);
        config->set("agent.curiosity_threshold", 0.5f);
        
        return agentBrain;
    }, py::arg("brain"), py::arg("config"),
       "Create an exploration-focused agent with curiosity and novelty detection");
    
    // Prediction agent factory
    m.def("createPredictionAgent", [](std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        
        // Configure for prediction behavior
        config->set("agent.type", "prediction");
        config->set("agent.prediction_mode", true);
        config->set("agent.prediction_horizon", 10);
        config->set("agent.error_weight", 0.5f);
        
        return agentBrain;
    }, py::arg("brain"), py::arg("config"),
       "Create a prediction-focused agent with forward models and error minimization");
    
    // Social agent factory
    m.def("createSocialAgent", [](std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        
        // Configure for social behavior
        config->set("agent.type", "social");
        config->set("agent.social_mode", true);
        config->set("agent.imitation_rate", 0.3f);
        config->set("agent.coordination_weight", 0.7f);
        
        return agentBrain;
    }, py::arg("brain"), py::arg("config"),
       "Create a social-focused agent with imitation and coordination capabilities");
    
    // Developmental agent factory
    m.def("createDevelopmentalAgent", [](std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        
        // Configure for developmental behavior
        config->set("agent.type", "developmental");
        config->set("agent.development_mode", true);
        config->set("agent.growth_rate", 0.05f);
        config->set("agent.maturation_threshold", 0.8f);
        
        return agentBrain;
    }, py::arg("brain"), py::arg("config"),
       "Create a developmental-focused agent with growth and maturation capabilities");
    
    // ===== ADVANCED API CLASSES =====
    
    // Agent state inspection class
    py::class_<Brain>("AgentInspector", R"pbdoc(Utility class for inspecting agent state)pbdoc")
        .def_static("inspectBrain", [](std::shared_ptr<Brain> brain) {
            return brain->logStatus();
        }, py::arg("brain"),
           "Log brain status and return diagnostic information")
        .def_static("inspectPercept", [](const SensoryPercept& percept) {
            std::unordered_map<std::string, float> features;
            
            // Extract features from each sensory modality
            auto visionFeatures = FeatureExtractor().extractVisionFeatures(
                percept.getVision(), percept.getVisionWidth(), percept.getVisionHeight());
            auto touchFeatures = FeatureExtractor().extractTouchFeatures(percept.getTouch());
            auto internalFeatures = FeatureExtractor().extractInternalFeatures(percept.getInternal());
            auto proprioFeatures = FeatureExtractor().extractProprioceptionFeatures(percept.getProprioception());
            
            // Store features
            features["vision_mean"] = visionFeatures.empty() ? 0.0f : visionFeatures[0];
            features["vision_std"] = visionFeatures.size() > 1 ? visionFeatures[1] : 0.0f;
            features["touch_activity"] = touchFeatures.empty() ? 0.0f : touchFeatures[0];
            features["internal_mean"] = internalFeatures.empty() ? 0.0f : internalFeatures[0];
            features["proprio_mean"] = proprioFeatures.empty() ? 0.0f : proprioFeatures[0];
            
            return features;
        }, py::arg("percept"),
           "Extract and return diagnostic features from sensory percept");
    
    // Performance monitoring class
    py::class_<Brain>("PerformanceMonitor", R"pbdoc(Utility class for monitoring agent performance)pbdoc")
        .def_static("getPerformanceMetrics", [](std::shared_ptr<Brain> brain) {
            std::unordered_map<std::string, float> metrics;
            
            if (!brain) {
                metrics["error"] = 1.0f;
                return metrics;
            }
            
            // Basic brain metrics
            metrics["total_neurons"] = static_cast<float>(brain->getTotalNeuronCount());
            metrics["total_synapses"] = static_cast<float>(brain->getTotalSynapseCount());
            metrics["active_neurons"] = static_cast<float>(brain->getActiveNeuronCount());
            metrics["firing_neurons"] = static_cast<float>(brain->getFiringNeuronCount());
            metrics["average_firing_rate"] = brain->getAverageFiringRate();
            metrics["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
            metrics["total_spikes"] = static_cast<float>(brain->getTotalSpikeCount());
            
            return metrics;
        }, py::arg("brain"),
           "Get performance metrics from brain simulation")
        .def_static("getDevelopmentalStats", [](std::shared_ptr<Brain> brain) {
            std::unordered_map<std::string, float> stats;
            
            if (!brain) {
                stats["error"] = 1.0f;
                return stats;
            }
            
            // Developmental stage
            stats["developmental_stage"] = static_cast<float>(brain->getDevelopmentalStage());
            stats["plasticity_modifier"] = 1.0f; // Would need access to internal state
            
            return stats;
        }, py::arg("brain"),
           "Get developmental statistics");
    
    // Convenience methods for batch processing
    m.def("batchProcessPercepts", [](const std::vector<SensoryPercept>& percepts) {
        std::vector<std::unordered_map<std::string, float>> results;
        FeatureExtractor extractor;
        
        for (const auto& percept : percepts) {
            std::unordered_map<std::string, float> result;
            
            // Extract features from all modalities
            auto visionFeatures = extractor.extractVisionFeatures(
                percept.getVision(), percept.getVisionWidth(), percept.getVisionHeight());
            auto touchFeatures = extractor.extractTouchFeatures(percept.getTouch());
            auto internalFeatures = extractor.extractInternalFeatures(percept.getInternal());
            auto proprioFeatures = extractor.extractProprioceptionFeatures(percept.getProprioception());
            
            // Store key features
            result["vision_mean"] = visionFeatures.empty() ? 0.0f : visionFeatures[0];
            result["touch_activity"] = touchFeatures.empty() ? 0.0f : touchFeatures[0];
            result["internal_mean"] = internalFeatures.empty() ? 0.0f : internalFeatures[0];
            result["proprio_mean"] = proprioFeatures.empty() ? 0.0f : proprioFeatures[0];
            result["timestamp"] = percept.getTimestamp();
            
            results.push_back(result);
        }
        
        return results;
    }, py::arg("percepts"),
       "Process multiple percepts in batch and extract features from each");
    
    // Agent training helper functions
    m.def("createTrainingData", [](const std::vector<SensoryPercept>& percepts,
                                 const std::vector<Action>& actions,
                                 const std::vector<ActionResult>& rewards) {
        std::vector<std::unordered_map<std::string, float>> trainingSamples;
        
        if (percepts.size() != actions.size() || percepts.size() != rewards.size()) {
            throw std::runtime_error("Input vectors must have the same size");
        }
        
        for (size_t i = 0; i < percepts.size(); ++i) {
            std::unordered_map<std::string, float> sample;
            
            // Extract features from percept
            FeatureExtractor extractor;
            auto visionFeatures = extractor.extractVisionFeatures(
                percepts[i].getVision(), percepts[i].getVisionWidth(), percepts[i].getVisionHeight());
            
            // Store percept features
            if (!visionFeatures.empty()) {
                sample["vision_0"] = visionFeatures[0];
                if (visionFeatures.size() > 1) sample["vision_1"] = visionFeatures[1];
            }
            
            // Store action and reward
            sample["action_type"] = static_cast<float>(actions[i].getType());
            sample["reward"] = rewards[i].reward;
            sample["success"] = rewards[i].success ? 1.0f : 0.0f;
            sample["timestamp"] = percepts[i].getTimestamp();
            
            trainingSamples.push_back(sample);
        }
        
return trainingSamples;
    }, py::arg("percepts"), py::arg("actions"), py::arg("rewards"),
        "Create training samples from percept-action-reward tuples");

    // ===== WAVE 3 ADVANCED LEARNING MECHANISMS =====

    // ===== Reinforcement Learning =====

    // Exploration Strategy base class
    py::class_<ExplorationStrategy>(m, "ExplorationStrategy", R"pbdoc(Base class for exploration strategies)pbdoc")
        .def("selectAction", &ExplorationStrategy::selectAction,
              py::arg("qValues"), "Select action using exploration strategy")
        .def("update", &ExplorationStrategy::update,
              py::arg("state"), py::arg("action"), py::arg("reward"), py::arg("nextState"),
              "Update strategy with new experience");

    // Epsilon-greedy strategy
    py::class_<EpsilonGreedy, ExplorationStrategy>(m, "EpsilonGreedy", R"pbdoc(Epsilon-greedy exploration strategy)pbdoc")
        .def(py::init<>())
        .def(py::init<double>(), py::arg("epsilon"))
        .def("setEpsilon", &EpsilonGreedy::setEpsilon, py::arg("epsilon"), "Set exploration rate");

    // UCB (Upper Confidence Bound) strategy
    py::class_<UCB, ExplorationStrategy>(m, "UCB", R"pbdoc(Upper Confidence Bound exploration strategy)pbdoc")
        .def(py::init<>())
        .def(py::init<double>(), py::arg("c"))
        .def("setExplorationConstant", &UCB::setExplorationConstant, py::arg("c"), "Set exploration constant");

    // Thompson Sampling strategy
    py::class_<ThompsonSampling, ExplorationStrategy>(m, "ThompsonSampling", R"pbdoc(Thompson Sampling exploration strategy)pbdoc")
        .def(py::init<>());

    // Policy Gradient class
    py::class_<PolicyGradient>(m, "PolicyGradient", R"pbdoc(Policy gradient learning algorithm)pbdoc")
        .def(py::init<>())
        .def("update", &PolicyGradient::update,
              py::arg("states"), py::arg("actions"), py::arg("rewards"),
              "Update policy using policy gradient");

    // Actor-Critic class
    py::class_<ActorCritic>(m, "ActorCritic", R"pbdoc(Actor-critic learning algorithm)pbdoc")
        .def(py::init<>())
        .def("update", &ActorCritic::update,
              py::arg("state"), py::arg("action"), py::arg("reward"), py::arg("nextState"),
              "Update actor-critic policy");

    // Experience Replay class
    py::class_<ExperienceReplay>(m, "ExperienceReplay", R"pbdoc(Experience replay buffer for RL)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t>(), py::arg("capacity"))
        .def("addExperience", &ExperienceReplay::addExperience,
              py::arg("state"), py::arg("action"), py::arg("reward"), py::arg("nextState"),
              "Add experience to replay buffer")
        .def("sampleBatch", &ExperienceReplay::sampleBatch,
              py::arg("batchSize"), "Sample random batch from buffer");

    // Reward Shaper class
    py::class_<RewardShaper>(m, "RewardShaper", R"pbdoc(Reward shaping utilities)pbdoc")
        .def(py::init<>())
        .def("shapeReward", &RewardShaper::shapeReward,
              py::arg("reward"), py::arg("state"), py::arg("action"), py::arg("nextState"),
              "Shape reward for learning");

    // ===== Neural Network Learning =====

    // Neural Layer class
    py::class_<NeuralLayer>(m, "NeuralLayer", R"pbdoc(Custom neural network layer)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("inputSize"), py::arg("outputSize"))
        .def("forward", &NeuralLayer::forward,
              py::arg("input"), "Forward pass through layer")
        .def("backward", &NeuralLayer::backward,
              py::arg("input"), py::arg("gradient"), py::arg("learningRate"),
              "Backward pass through layer");

    // Connection Optimizer class
    py::class_<ConnectionOptimizer>(m, "ConnectionOptimizer", R"pbdoc(Connection weight optimization)pbdoc")
        .def(py::init<>())
        .def("optimizeConnections", &ConnectionOptimizer::optimizeConnections,
              py::arg("weights"), py::arg("gradients"),
              "Optimize connection weights");

    // Pattern Extractor class
    py::class_<PatternExtractor>(m, "PatternExtractor", R"pbdoc(Pattern extraction and learning)pbdoc")
        .def(py::init<>())
        .def("extractPattern", &PatternExtractor::extractPattern,
              py::arg("data"), "Extract pattern from data");

    // Learning Rule base class
    py::class_<LearningRule>(m, "LearningRule", R"pbdoc(Base class for learning rules)pbdoc")
        .def("update", &LearningRule::update,
              py::arg("preSynaptic"), py::arg("postSynaptic"),
              "Update synaptic weights using learning rule");

    // Hebbian learning rule
    py::class_<Hebbian, LearningRule>(m, "Hebbian", R"pbdoc(Hebbian learning rule)pbdoc")
        .def(py::init<>());

    // Anti-Hebbian learning rule
    py::class_<AntiHebbian, LearningRule>(m, "AntiHebbian", R"pbdoc(Anti-Hebbian learning rule)pbdoc")
        .def(py::init<>());

    // Sparse coding learning rule
    py::class_<SparseCoding, LearningRule>(m, "SparseCoding", R"pbdoc(Sparse coding learning rule)pbdoc")
        .def(py::init<>());

    // ===== Transfer Learning =====

    // Model Checkpoint class
    py::class_<ModelCheckpoint>(m, "ModelCheckpoint", R"pbdoc(Model checkpoint management)pbdoc")
        .def(py::init<>())
        .def("save", &ModelCheckpoint::save,
              py::arg("modelPath"), py::arg("epoch"),
              "Save model checkpoint");

    // Transfer Learner class
    py::class_<TransferLearner>(m, "TransferLearner", R"pbdoc(Domain adaptation and fine-tuning)pbdoc")
        .def(py::init<>())
        .def("fineTune", &TransferLearner::fineTune,
              py::arg("sourceModel"), py::arg("targetData"), py::arg("targetDomain"),
              "Fine-tune model for target domain");

    // Pretrained Network Loader class
    py::class_<PretrainedNetworkLoader>(m, "PretrainedNetworkLoader", R"pbdoc(Load pre-trained networks)pbdoc")
        .def(py::init<>())
        .def("load", &PretrainedNetworkLoader::load,
              py::arg("modelPath"), py::arg("networkType"),
              "Load pre-trained network");

// Visualization and Debugging tools

// NeuralVisualizer class for neural network visualization
class NeuralVisualizer {
public:
    NeuralVisualizer() = default;
    explicit NeuralVisualizer(size_t width, size_t height) 
        : width_(width), height_(height) {
        canvas_.resize(width * height, 0.0f);
    }
    
    void drawNeuron(size_t x, size_t y, float intensity, int radius = 3) {
        if (x >= width_ || y >= height_) return;
        
        for (int dy = -radius; dy <= radius; ++dy) {
            for (int dx = -radius; dx <= radius; ++dx) {
                size_t nx = x + dx;
                size_t ny = y + dy;
                if (nx < width_ && ny < height_) {
                    size_t index = ny * width_ + nx;
                    float dist = std::sqrt(dx * dx + dy * dy);
                    float influence = intensity * (1.0f - dist / radius);
                    canvas_[index] = std::max(canvas_[index], influence);
                }
            }
        }
    }
    
    void drawConnection(size_t x1, size_t y1, size_t x2, size_t y2, float intensity) {
        // Bresenham line drawing algorithm
        int dx = std::abs(x2 - x1);
        int dy = std::abs(y2 - y1);
        int sx = (x1 < x2) ? 1 : -1;
        int sy = (y1 < y2) ? 1 : -1;
        int err = dx - dy;
        
        int x = x1;
        int y = y1;
        while (x != x2 || y != y2) {
            if (x >= 0 && x < static_cast<int>(width_) && y >= 0 && y < static_cast<int>(height_)) {
                canvas_[y * width_ + x] = std::max(canvas_[y * width_ + x], intensity);
            }
            int e2 = 2 * err;
            if (e2 > -dy) { err -= dy; x += sx; }
            if (e2 < dx) { err += dx; y += sy; }
        }
    }
    
    std::vector<float> getCanvas() const { return canvas_; }
    
    void clear() {
        std::fill(canvas_.begin(), canvas_.end(), 0.0f);
    }
    
    void saveToFile(const std::string& filename) const {
        std::ofstream file(filename, std::ios::binary);
        if (file.is_open()) {
            // Simple PPM format
            file << "P6\n" << width_ << " " << height_ << "\n255\n";
            for (float val : canvas_) {
                unsigned char pixel = static_cast<unsigned char>(val * 255);
                file.write(reinterpret_cast<const char*>(&pixel), 1);
            }
            file.close();
        }
    }
    
private:
    size_t width_;
    size_t height_;
    std::vector<float> canvas_;
};

// SimulationVisualizer class for simulation visualization
class SimulationVisualizer {
public:
    SimulationVisualizer() = default;
    
    void visualizeState(const std::vector<float>& data, const std::string& type) {
        if (type == "heatmap") {
            visualizeHeatmap(data);
        } else if (type == "trace") {
            visualizeTrace(data);
        } else if (type == "3d") {
            visualize3D(data);
        }
    }
    
    std::vector<float> getVisualizationData() const { return visualizationData_; }
    
    void saveVisualization(const std::string& filename) const {
        // Implementation for saving visualization
    }
    
private:
    void visualizeHeatmap(const std::vector<float>& data) {
        size_t size = data.size();
        visualizationData_.resize(size);
        for (size_t i = 0; i < size; ++i) {
            visualizationData_[i] = data[i];  // Simple identity for now
        }
    }
    
    void visualizeTrace(const std::vector<float>& data) {
        // Implementation for trace visualization
        visualizationData_ = data;
    }
    
    void visualize3D(const std::vector<float>& data) {
        // Implementation for 3D visualization
        visualizationData_ = data;
    }
    
    std::vector<float> visualizationData_;
};

// DataVisualizer class for data visualization
class DataVisualizer {
public:
    DataVisualizer() = default;
    
    static std::vector<float> plotTimeSeries(const std::vector<double>& timestamps, 
                                           const std::vector<double>& values) {
        if (timestamps.size() != values.size()) {
            throw std::invalid_argument("Timestamps and values must have same size");
        }
        
        std::vector<float> result;
        double minVal = *std::min_element(values.begin(), values.end());
        double maxVal = *std::max_element(values.begin(), values.end());
        double range = maxVal - minVal;
        
        if (range < 0.0001f) range = 1.0f;
        
        for (double val : values) {
            float normalized = static_cast<float>((val - minVal) / range);
            result.push_back(normalized);
        }
        
        return result;
    }
    
    static std::vector<float> plotHistogram(const std::vector<float>& data, size_t bins = 10) {
        std::vector<float> histogram(bins, 0.0f);
        if (data.empty()) return histogram;
        
        float minVal = *std::min_element(data.begin(), data.end());
        float maxVal = *std::max_element(data.begin(), data.end());
        float range = maxVal - minVal;
        
        if (range < 0.0001f) range = 1.0f;
        
        for (float val : data) {
            size_t bin = static_cast<size_t>((val - minVal) / range * bins);
            if (bin >= bins) bin = bins - 1;
            histogram[bin] += 1.0f;
        }
        
        // Normalize
        float total = std::accumulate(histogram.begin(), histogram.end(), 0.0f);
        if (total > 0.0f) {
            for (float& count : histogram) {
                count /= total;
            }
        }
        
        return histogram;
    }
};

// ResultPlotter class for plotting results
class ResultPlotter {
public:
    ResultPlotter() = default;
    
    static std::string plotAccuracy(float accuracy, float confidence) {
        std::stringstream ss;
        ss << "Accuracy: " << accuracy * 100 << "%";
        if (confidence > 0.0f) {
            ss << " (conf: " << confidence * 100 << "%)";
        }
        return ss.str();
    }
    
    static std::string plotLoss(float loss, size_t iteration) {
        std::stringstream ss;
        ss << "Iter: " << iteration << ", Loss: " << loss;
        return ss.str();
    }
    
    static std::vector<float> plotTrainingCurve(const std::vector<float>& losses, 
                                                size_t width, size_t height) {
        std::vector<float> curve(width * height, 0.0f);
        if (losses.empty()) return curve;
        
        float minLoss = *std::min_element(losses.begin(), losses.end());
        float maxLoss = *std::max_element(losses.begin(), losses.end());
        float range = maxLoss - minLoss;
        if (range < 0.0001f) range = 1.0f;
        
        // Draw line graph
        for (size_t i = 0; i < losses.size() - 1; ++i) {
            size_t x1 = static_cast<size_t>(i * (width - 1) / (losses.size() - 1));
            size_t x2 = static_cast<size_t>((i + 1) * (width - 1) / (losses.size() - 1));
            
            float y1 = (losses[i] - minLoss) / range;
            float y2 = (losses[i + 1] - minLoss) / range;
            
            size_t y1Pix = static_cast<size_t>((1.0f - y1) * (height - 1));
            size_t y2Pix = static_cast<size_t>((1.0f - y2) * (height - 1));
            
            // Draw line (simplified)
            if (x1 == x2) {
                for (size_t y = std::min(y1Pix, y2Pix); y <= std::max(y1Pix, y2Pix); ++y) {
                    if (y < height) curve[y * width + x1] = 1.0f;
                }
            } else {
                curve[y1Pix * width + x1] = 1.0f;
                curve[y2Pix * width + x2] = 1.0f;
            }
        }
        
        return curve;
    }
};

// DebugMonitor class for real-time debugging
class DebugMonitor {
public:
    DebugMonitor() = default;
    
    struct DebugInfo {
        std::chrono::system_clock::time_point timestamp;
        std::string message;
        std::string level;  // DEBUG, INFO, WARNING, ERROR
        std::unordered_map<std::string, float> metrics;
    };
    
    void logDebug(const std::string& message, const std::string& level = "INFO") {
        DebugInfo info;
        info.timestamp = std::chrono::system_clock::now();
        info.message = message;
        info.level = level;
        
        std::lock_guard<std::mutex> lock(mutex_);
        debugInfo_.push_back(info);
        
        if (level == "ERROR") {
            std::cerr << "[DEBUG ERROR] " << message << std::endl;
        } else if (level == "WARNING") {
            std::cerr << "[DEBUG WARNING] " << message << std::endl;
        }
    }
    
    void addMetric(const std::string& name, float value) {
        std::lock_guard<std::mutex> lock(mutex_);
        metrics_[name] = value;
    }
    
    std::vector<DebugInfo> getDebugInfo() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return debugInfo_;
    }
    
    std::unordered_map<std::string, float> getMetrics() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return metrics_;
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        debugInfo_.clear();
        metrics_.clear();
    }
    
private:
    mutable std::mutex mutex_;
    std::vector<DebugInfo> debugInfo_;
    std::unordered_map<std::string, float> metrics_;
};

// BreakpointManager class for debugging control
class BreakpointManager {
public:
    BreakpointManager() = default;
    
    void addBreakpoint(size_t step) {
        breakpoints_.insert(step);
    }
    
    bool shouldBreak(size_t step) const {
        return breakpoints_.find(step) != breakpoints_.end();
    }
    
    void removeBreakpoint(size_t step) {
        breakpoints_.erase(step);
    }
    
    void clearBreakpoints() {
        breakpoints_.clear();
    }
    
    std::vector<size_t> getBreakpoints() const {
        return std::vector<size_t>(breakpoints_.begin(), breakpoints_.end());
    }
    
    void setAutoContinue(bool autoContinue) {
        autoContinue_ = autoContinue;
    }
    
    bool shouldAutoContinue() const { return autoContinue_; }
    
private:
    std::unordered_set<size_t> breakpoints_;
    bool autoContinue_ = false;
};

// Profiler class for performance profiling
class Profiler {
public:
    Profiler() = default;
    
    struct ProfileData {
        std::string functionName;
        double duration;
        size_t callCount;
        double totalTime;
    };
    
    void startProfile(const std::string& functionName) {
        profiles_[functionName].startTime = std::chrono::high_resolution_clock::now();
    }
    
    void endProfile(const std::string& functionName) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto it = profiles_.find(functionName);
        if (it != profiles_.end()) {
            auto duration = std::chrono::duration<double>(endTime - it->second.startTime).count();
            it->second.duration = duration;
            it->second.callCount++;
            it->second.totalTime += duration;
        }
    }
    
    std::vector<ProfileData> getProfiles() const {
        std::vector<ProfileData> result;
        for (const auto& pair : profiles_) {
            result.push_back(pair.second);
        }
        return result;
    }
    
    void reset() {
        profiles_.clear();
    }
    
private:
    struct InternalProfileData {
        std::chrono::high_resolution_clock::time_point startTime;
        double duration;
        size_t callCount;
        double totalTime;
    };
    
    std::unordered_map<std::string, InternalProfileData> profiles_;
};

// MemoryLeakDetector class for memory leak detection
class MemoryLeakDetector {
public:
    MemoryLeakDetector() = default;
    
    struct AllocationInfo {
        void* pointer;
        size_t size;
        std::string file;
        size_t line;
        std::chrono::system_clock::time_point timestamp;
    };
    
    void trackAllocation(void* pointer, size_t size, const std::string& file, size_t line) {
        AllocationInfo info;
        info.pointer = pointer;
        info.size = size;
        info.file = file;
        info.line = line;
        info.timestamp = std::chrono::system_clock::now();
        
        std::lock_guard<std::mutex> lock(mutex_);
        allocations_[pointer] = info;
    }
    
    void trackDeallocation(void* pointer) {
        std::lock_guard<std::mutex> lock(mutex_);
        allocations_.erase(pointer);
    }
    
    std::vector<AllocationInfo> getLeaks() const {
        std::vector<AllocationInfo> result;
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& pair : allocations_) {
            auto duration = std::chrono::duration<double>(now - pair.second.timestamp).count();
            if (duration > leakThreshold_) {  // Consider leaks that persist longer than threshold
                result.push_back(pair.second);
            }
        }
        
        return result;
    }
    
    void setLeakThreshold(double threshold) { leakThreshold_ = threshold; }
    
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        allocations_.clear();
    }
    
private:
    mutable std::mutex mutex_;
    std::unordered_map<void*, AllocationInfo> allocations_;
    double leakThreshold_ = 60.0;  // 60 seconds
};

    // ===== DEBUGGING AND ANALYSIS TOOLS =====

    // ===== Neural Activity Analysis =====

    // Neural Activity Analyzer class
    py::class_<NeuralActivityAnalyzer>(m, "NeuralActivityAnalyzer", R"pbdoc(Neural activity analysis)pbdoc")
        .def(py::init<>())
        .def("analyzeSpikes", &NeuralActivityAnalyzer::analyzeSpikes,
              py::arg("spikeTrain"), "Analyze spike train");

    // Network Topology Analyzer class
    py::class_<NetworkTopologyAnalyzer>(m, "NetworkTopologyAnalyzer", R"pbdoc(Neural network topology analysis)pbdoc")
        .def(py::init<>())
        .def("analyzeTopology", &NetworkTopologyAnalyzer::analyzeTopology,
              py::arg("brain"), "Analyze network topology");

    // Pattern Detector class
    py::class_<PatternDetector>(m, "PatternDetector", R"pbdoc(Pattern extraction from neural activity)pbdoc")
        .def(py::init<>())
        .def("detectPattern", &PatternDetector::detectPattern,
              py::arg("activityData"), "Detect patterns in neural activity");

    // Synchrony Detector class
    py::class_<SynchronyDetector>(m, "SynchronyDetector", R"pbdoc(Measuring neural synchrony)pbdoc")
        .def(py::init<>())
        .def("measureSynchrony", &SynchronyDetector::measureSynchrony,
              py::arg("spikeTrains"), "Measure synchrony between spike trains");

    // ===== Performance Optimization =====

    // Memory Manager class
    py::class_<MemoryManager>(m, "MemoryManager", R"pbdoc(Advanced memory management)pbdoc")
        .def(py::init<>())
        .def("allocateMemory", &MemoryManager::allocateMemory,
              py::arg("size"), "Allocate memory");

    // Performance Profiler class
    py::class_<PerformanceProfiler>(m, "PerformanceProfiler", R"pbdoc(Performance profiling)pbdoc")
        .def(py::init<>())
        .def("profile", &PerformanceProfiler::profile,
              py::arg("function"), "Profile function execution");

    // Memory Leak Detector class
    py::class_<MemoryLeakDetector>(m, "MemoryLeakDetector", R"pbdoc(Memory leak detection)pbdoc")
        .def(py::init<>())
        .def("detectLeaks", &MemoryLeakDetector::detectLeaks,
              "Detect memory leaks");

    // Cache Optimizer class
    py::class_<CacheOptimizer>(m, "CacheOptimizer", R"pbdoc(Cache optimization)pbdoc")
        .def(py::init<>())
        .def("optimizeCache", &CacheOptimizer::optimizeCache,
              "Optimize cache performance");

    // Advanced Simulation Features

// TimeStepManager class for time step management
class TimeStepManager {
public:
    TimeStepManager() = default;
    explicit TimeStepManager(size_t initialStep, double timestepDuration)
        : currentStep_(initialStep), timestepDuration_(timestepDuration) {}
    
    void setStep(size_t step) { currentStep_ = step; }
    size_t getStep() const { return currentStep_; }
    
    void setTimestep(double duration) { timestepDuration_ = duration; }
    double getTimestep() const { return timestepDuration_; }
    
    double getTime() const { return currentStep_ * timestepDuration_; }
    
    void nextStep() { ++currentStep_; }
    void advanceSteps(size_t steps) { currentStep_ += steps; }
    
    size_t getTotalSteps(double totalTime) const {
        return static_cast<size_t>(totalTime / timestepDuration_);
    }
    
    double timeAtStep(size_t step) const { return step * timestepDuration_; }
    
private:
    size_t currentStep_;
    double timestepDuration_;
};

// EventScheduler class for event-driven simulation
class EventScheduler {
public:
    EventScheduler() = default;
    
    struct Event {
        size_t timestamp;
        std::function<void()> callback;
        bool operator>(const Event& other) const { return timestamp > other.timestamp; }
    };
    
    void scheduleEvent(size_t timestamp, std::function<void()> callback) {
        events_.push({timestamp, callback});
    }
    
    Event nextEvent() {
        if (events_.empty()) throw std::runtime_error("No events scheduled");
        Event next = events_.top();
        events_.pop();
        return next;
    }
    
    bool hasEvents() const { return !events_.empty(); }
    size_t nextEventTime() const {
        return events_.empty() ? 0 : events_.top().timestamp;
    }
    
    void clear() {
        std::priority_queue<Event, std::vector<Event>, std::greater<Event>> empty;
        std::swap(events_, empty);
    }
    
private:
    std::priority_queue<Event, std::vector<Event>, std::greater<Event>> events_;
};

// RealTimeSimulator class for real-time simulation
class RealTimeSimulator {
public:
    RealTimeSimulator() = default;
    explicit RealTimeSimulator(double targetFPS) : targetFPS_(targetFPS), timestep_(1.0 / targetFPS) {}
    
    void setTargetFPS(double fps) { targetFPS_ = fps; timestep_ = 1.0 / fps; }
    double getTargetFPS() const { return targetFPS_; }
    
    double getTimestep() const { return timestep_; }
    
    double runStep() {
        if (lastFrameTime_ > 0) {
            double now = std::chrono::duration<double>(
                std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            double elapsed = now - lastFrameTime_;
            
            // Cap timestep to avoid large jumps
            if (elapsed > timestep_ * 10) elapsed = timestep_ * 10;
            
            lastFrameTime_ += timestep_;
            return elapsed;
        }
        lastFrameTime_ = std::chrono::duration<double>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        return timestep_;
    }
    
    void reset() {
        lastFrameTime_ = 0.0;
    }
    
private:
    double targetFPS_;
    double timestep_;
    double lastFrameTime_;
};

// ParticleWorld class for particle-based world simulation
class ParticleWorld {
public:
    struct Particle {
        std::vector<float> position;
        std::vector<float> velocity;
        std::vector<float> acceleration;
        float mass;
        float radius;
        
        Particle() : mass(1.0f), radius(1.0f) {}
    };
    
    ParticleWorld() = default;
    explicit ParticleWorld(size_t capacity) {
        particles_.reserve(capacity);
    }
    
    void addParticle(const Particle& particle) {
        particles_.push_back(particle);
    }
    
    void update(double dt) {
        // Simple Euler integration
        for (auto& particle : particles_) {
            for (size_t i = 0; i < particle.position.size(); ++i) {
                particle.position[i] += particle.velocity[i] * dt;
                particle.velocity[i] += particle.acceleration[i] * dt;
            }
        }
    }
    
    size_t particleCount() const { return particles_.size(); }
    const Particle& getParticle(size_t index) const { return particles_[index]; }
    
    std::vector<float> getAllPositions() const {
        std::vector<float> positions;
        for (const auto& particle : particles_) {
            positions.insert(positions.end(), particle.position.begin(), particle.position.end());
        }
        return positions;
    }
    
private:
    std::vector<Particle> particles_;
};

// FluidSimulation class for fluid dynamics simulation
class FluidSimulation {
public:
    FluidSimulation() = default;
    FluidSimulation(size_t width, size_t height) 
        : width_(width), height_(height), density_(width * height, 1.0f) {}
    
    void update(double dt) {
        // Simplified fluid simulation using pressure projection
        // This is a placeholder implementation
        computeVorticityConfinement();
        applyBoundaryConditions();
    }
    
    void setDensity(const std::vector<float>& density) {
        if (density.size() == width_ * height_) {
            density_ = density;
        }
    }
    
    const std::vector<float>& getDensity() const { return density_; }
    
    void addPressure(const std::vector<float>& pressure) {
        // Add pressure to the system
        pressure_ = pressure;
    }
    
    std::vector<float> getPressure() const { return pressure_; }
    
private:
    size_t width_;
    size_t height_;
    std::vector<float> density_;
    std::vector<float> pressure_;
    
    void computeVorticityConfinement() {
        // Placeholder for vorticity confinement computation
    }
    
    void applyBoundaryConditions() {
        // Apply periodic boundary conditions
        // Placeholder implementation
    }
};

// CellularAutomaton class for cellular automaton simulation
class CellularAutomaton {
public:
    CellularAutomaton() = default;
    CellularAutomaton(size_t width, size_t height, size_t numStates)
        : width_(width), height_(height), numStates_(numStates) {
        grid_.resize(height, std::vector<size_t>(width, 0));
        newGrid_.resize(height, std::vector<size_t>(width, 0));
    }
    
    void setRule(const std::vector<size_t>& rule) {
        rule_ = rule;
    }
    
    void setState(size_t x, size_t y, size_t state) {
        if (x < width_ && y < height_) {
            grid_[y][x] = state;
        }
    }
    
    size_t getState(size_t x, size_t y) const {
        if (x >= width_ || y >= height_) return 0;
        return grid_[y][x];
    }
    
    void update() {
        // Apply cellular automaton rules
        for (size_t y = 0; y < height_; ++y) {
            for (size_t x = 0; x < width_; ++x) {
                size_t state = grid_[y][x];
                size_t newState = applyRule(x, y, state);
                newGrid_[y][x] = newState;
            }
        }
        grid_ = newGrid_;
    }
    
    std::vector<std::vector<size_t>> getGrid() const { return grid_; }
    
private:
    size_t applyRule(size_t x, size_t y, size_t state) {
        // Simple rule: next state is average of neighbors
        size_t sum = 0;
        size_t count = 0;
        
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                size_t nx = x + dx;
                size_t ny = y + dy;
                if (nx < width_ && ny < height_) {
                    sum += grid_[ny][nx];
                    ++count;
                }
            }
        }
        
        return (count > 0) ? (sum / count) : state;
    }
    
    size_t width_;
    size_t height_;
    size_t numStates_;
    std::vector<std::vector<size_t>> grid_;
    std::vector<std::vector<size_t>> newGrid_;
    std::vector<size_t> rule_;
};

// MultiPhysicsWorld class for multi-physics simulation
class MultiPhysicsWorld {
public:
    MultiPhysicsWorld() = default;
    
    void addParticleWorld(const ParticleWorld& world) {
        particleWorlds_.push_back(world);
    }
    
    void addFluidSimulation(const FluidSimulation& fluid) {
        fluidSimulations_.push_back(fluid);
    }
    
    void addCellularAutomaton(const CellularAutomaton& ca) {
        cellularAutomata_.push_back(ca);
    }
    
    void update(double dt) {
        // Update all physics systems
        for (auto& world : particleWorlds_) {
            world.update(dt);
        }
        
        for (auto& fluid : fluidSimulations_) {
            fluid.update(dt);
        }
        
        for (auto& ca : cellularAutomata_) {
            ca.update();
        }
    }
    
    size_t particleWorldCount() const { return particleWorlds_.size(); }
    size_t fluidSimulationCount() const { return fluidSimulations_.size(); }
    size_t cellularAutomatonCount() const { return cellularAutomata_.size(); }
    
private:
    std::vector<ParticleWorld> particleWorlds_;
    std::vector<FluidSimulation> fluidSimulations_;
    std::vector<CellularAutomaton> cellularAutomata_;
};

// ===== WAVE 5: PLUGIN ARCHITECTURE AND COMPREHENSIVE TESTING =====

namespace pynlm {

// ===== PLUGIN BASE CLASSES =====

// Plugin base class for all plugins
class Plugin {
public:
    virtual ~Plugin() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    
    virtual bool initialize(const std::unordered_map<std::string, std::any>& config) = 0;
    virtual void shutdown() = 0;
    virtual bool isInitialized() const = 0;
    
    virtual std::string getStatus() const = 0;
    virtual std::unordered_map<std::string, std::any> getMetrics() const = 0;
};

// PluginManager class for plugin management and discovery
class PluginManager {
public:
    PluginManager() = default;
    
    template<typename T>
    bool registerPlugin(const std::string& name, const std::string& description = "") {
        // Plugin factory registration
        auto factory = []() -> Plugin* { return new T(); };
        pluginFactories_[name] = factory;
        
        PluginInfo info;
        info.name = name;
        info.type = getPluginType<T>();
        info.description = description;
        info.version = "1.0.0";
        info.isAvailable = true;
        
        pluginRegistry_[name] = info;
        return true;
    }
    
    std::unique_ptr<Plugin> createPlugin(const std::string& name) {
        auto it = pluginFactories_.find(name);
        if (it != pluginFactories_.end()) {
            return std::unique_ptr<Plugin>(it->second());
        }
        return nullptr;
    }
    
    bool activatePlugin(const std::string& name, const std::unordered_map<std::string, std::any>& config) {
        auto it = activePlugins_.find(name);
        if (it != activePlugins_.end()) return false;
        
        auto plugin = createPlugin(name);
        if (!plugin) return false;
        
        if (!plugin->initialize(config)) {
            return false;
        }
        
        activePlugins_[name] = std::move(plugin);
        return true;
    }
    
    bool deactivatePlugin(const std::string& name) {
        auto it = activePlugins_.find(name);
        if (it == activePlugins_.end()) return false;
        
        it->second->shutdown();
        activePlugins_.erase(it);
        return true;
    }
    
    std::vector<std::string> getAvailablePlugins() const {
        std::vector<std::string> available;
        for (const auto& pair : pluginRegistry_) {
            if (pair.second.isAvailable) {
                available.push_back(pair.first);
            }
        }
        return available;
    }
    
    std::vector<std::string> getActivePlugins() const {
        std::vector<std::string> active;
        for (const auto& pair : activePlugins_) {
            active.push_back(pair.first);
        }
        return active;
    }
    
    std::unordered_map<std::string, std::any> getPluginInfo(const std::string& name) const {
        auto it = pluginRegistry_.find(name);
        if (it == pluginRegistry_.end()) return {};
        
        std::unordered_map<std::string, std::any> info;
        info["name"] = it->first;
        info["description"] = it->second.description;
        info["version"] = it->second.version;
        info["type"] = static_cast<int>(it->second.type);
        info["isAvailable"] = it->second.isAvailable;
        
        if (activePlugins_.find(name) != activePlugins_.end()) {
            auto plugin = activePlugins_.at(name).get();
            info["status"] = plugin->getStatus();
            info["metrics"] = plugin->getMetrics();
        }
        
        return info;
    }
    
    std::string getPluginTypeName(PluginType type) const {
        switch (type) {
            case PluginType::LEARNING: return "LearningPlugin";
            case PluginType::SENSOR: return "SensorPlugin";
            case PluginType::WORLD: return "WorldPlugin";
            case PluginType::VISUALIZATION: return "VisualizationPlugin";
            case PluginType::ANALYSIS: return "AnalysisPlugin";
            default: return "Unknown";
        }
    }
    
    template<typename T>
    PluginType getPluginType() const {
        if (std::is_base_of<LearningPlugin, T>::value) return PluginType::LEARNING;
        if (std::is_base_of<SensorPlugin, T>::value) return PluginType::SENSOR;
        if (std::is_base_of<WorldPlugin, T>::value) return PluginType::WORLD;
        if (std::is_base_of<VisualizationPlugin, T>::value) return PluginType::VISUALIZATION;
        if (std::is_base_of<AnalysisPlugin, T>::value) return PluginType::ANALYSIS;
        return PluginType::LEARNING;  // Default fallback
    }

protected:
    struct PluginInfo {
        std::string name;
        std::string description;
        std::string version;
        PluginType type;
        bool isAvailable = true;
    };
    
    std::unordered_map<std::string, std::function<Plugin*()>> pluginFactories_;
    std::unordered_map<std::string, PluginInfo> pluginRegistry_;
    std::unordered_map<std::string, std::unique_ptr<Plugin>> activePlugins_;
};

// PluginLoader class for dynamic plugin loading
class PluginLoader {
public:
    PluginLoader() = default;
    
    bool loadPluginFromFile(const std::string& filepath, const std::string& pluginName) {
        // Dynamic library loading (placeholder)
        // In real implementation, would use dlopen, dlsym, dlclose
        try {
            // For now, just register a mock plugin
            pluginManager_.registerPlugin<STDPPlugin>(pluginName, "STDP plugin loaded from file");
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    bool loadPluginFromPath(const std::string& directoryPath, const std::string& pluginName) {
        // Scan directory for plugin files and load them
        std::string searchPath = directoryPath + "/" + pluginName + "*.so";
        // Placeholder for directory scanning
        return loadPluginFromFile(searchPath, pluginName);
    }
    
    static bool loadPluginsFromDirectory(const std::string& directoryPath) {
        // Scan directory and load all plugins
        // Placeholder implementation
        return true;
    }
    
    std::vector<std::string> getAvailablePluginFiles(const std::string& directoryPath) const {
        // Return list of plugin files in directory
        std::vector<std::string> files;
        return files;
    }
    
    static std::string getPluginLibraryExtension() {
        return ".so";  // Shared object extension
    }

protected:
    PluginManager pluginManager_;
};

// PluginRegistry class for plugin registration and discovery
class PluginRegistry {
public:
    PluginRegistry() = default;
    
    static bool registerPlugin(const std::string& name, const std::string& description, PluginType type) {
        // Global plugin registry (singleton-like)
        PluginInfo info;
        info.name = name;
        info.description = description;
        info.type = type;
        info.version = "1.0.0";
        info.isAvailable = true;
        info.lastUpdated = std::chrono::system_clock::now();
        
        instance().plugins_[name] = info;
        return true;
    }
    
    static std::unordered_map<std::string, PluginInfo> getAllPlugins() {
        return instance().plugins_;
    }
    
    static std::vector<std::string> getPluginsByType(PluginType type) {
        std::vector<std::string> result;
        for (const auto& pair : instance().plugins_) {
            if (pair.second.type == type) {
                result.push_back(pair.first);
            }
        }
        return result;
    }
    
    static PluginInfo getPluginInfo(const std::string& name) {
        auto it = instance().plugins_.find(name);
        if (it != instance().plugins_.end()) {
            return it->second;
        }
        return PluginInfo{};
    }

private:
    struct PluginInfo {
        std::string name;
        std::string description;
        std::string version;
        PluginType type;
        bool isAvailable;
        std::chrono::system_clock::time_point lastUpdated;
    };
    
    static PluginRegistry& instance() {
        static PluginRegistry registry;
        return registry;
    }
    
    std::unordered_map<std::string, PluginInfo> plugins_;
};

// PluginAdapter class for plugin integration
class PluginAdapter {
public:
    PluginAdapter() = default;
    
    template<typename PluginType>
    bool adaptPlugin(Plugin* plugin, const std::string& targetSystem) {
        // Plugin-specific adaptation logic
        if (targetSystem == "brain") {
            return adaptToBrainSystem(static_cast<LearningPlugin*>(plugin));
        } else if (targetSystem == "world") {
            return adaptToWorldSystem(static_cast<WorldPlugin*>(plugin));
        } else if (targetSystem == "visualization") {
            return adaptToVisualizationSystem(static_cast<VisualizationPlugin*>(plugin));
        }
        return false;
    }
    
    bool adaptToBrainSystem(LearningPlugin* plugin) {
        if (!plugin) return false;
        
        // Add plugin to brain's learning system
        plugin->setLearningRate(0.01f);
        plugin->set plasticityEnabled(true);
        return true;
    }
    
    bool adaptToWorldSystem(WorldPlugin* plugin) {
        if (!plugin) return false;
        
        // Add plugin to world's simulation
        plugin->setUpdateFrequency(60.0f);
        plugin->setEnabled(true);
        return true;
    }
    
    bool adaptToVisualizationSystem(VisualizationPlugin* plugin) {
        if (!plugin) return false;
        
        // Add plugin to visualization system
        plugin->setVisualizationMode("heatmap");
        plugin->setUpdateInterval(0.1f);
        return true;
    }
};

// Plugin category base classes

// LearningPlugin base class for learning plugins
class LearningPlugin : public Plugin {
public:
    virtual void setLearningRate(float rate) = 0;
    virtual float getLearningRate() const = 0;
    virtual void setplasticityEnabled(bool enabled) = 0;
    virtual bool isplasticityEnabled() const = 0;
    virtual void updateWeights(const std::vector<float>& input, const std::vector<float>& target) = 0;
    virtual float getError() const = 0;
};

// SensorPlugin base class for sensor plugins
class SensorPlugin : public Plugin {
public:
    virtual std::vector<float> getSensorData() const = 0;
    virtual size_t getSensorCount() const = 0;
    virtual std::string getSensorType(size_t index) const = 0;
    virtual void setSamplingRate(float rate) = 0;
    virtual float getSamplingRate() const = 0;
    virtual void calibrate() = 0;
    virtual std::unordered_map<std::string, float> getCalibrationParameters() const = 0;
};

// WorldPlugin base class for world plugins
class WorldPlugin : public Plugin {
public:
    virtual void setUpdateFrequency(float freq) = 0;
    virtual float getUpdateFrequency() const = 0;
    virtual void setEnabled(bool enabled) = 0;
    virtual bool isEnabled() const = 0;
    virtual void updateWorldState(float timestep) = 0;
    virtual std::unordered_map<std::string, float> getWorldMetrics() const = 0;
};

// VisualizationPlugin base class for visualization plugins
class VisualizationPlugin : public Plugin {
public:
    virtual void setVisualizationMode(const std::string& mode) = 0;
    virtual std::string getVisualizationMode() const = 0;
    virtual void setUpdateInterval(float interval) = 0;
    virtual float getUpdateInterval() const = 0;
    virtual std::vector<float> getVisualizationData() const = 0;
    virtual void setDataSource(const std::string& source) = 0;
    virtual std::string getDataSource() const = 0;
    virtual void updateVisualization() = 0;
};

// AnalysisPlugin base class for analysis plugins
class AnalysisPlugin : public Plugin {
public:
    virtual void setAnalysisType(const std::string& type) = 0;
    virtual std::string getAnalysisType() const = 0;
    virtual std::unordered_map<std::string, float> performAnalysis(const std::vector<float>& data) = 0;
    virtual std::vector<std::string> getAnalysisResults() const = 0;
    virtual void setThreshold(float threshold) = 0;
    virtual float getThreshold() const = 0;
    virtual bool isAnomaly(const std::vector<float>& data) = 0;
};

// Plugin examples

// STDPPlugin for spike-timing dependent plasticity plugins
class STDPPlugin : public LearningPlugin {
public:
    STDPPlugin() {
        name_ = "STDPPlugin";
        version_ = "1.0.0";
        description_ = "Spike-timing dependent plasticity plugin";
        learningRate_ = 0.01f;
        plasticityEnabled_ = true;
        isInitialized_ = false;
    }
    
    std::string getName() const override { return name_; }
    std::string getVersion() const override { return version_; }
    std::string getDescription() const override { return description_; }
    
    bool initialize(const std::unordered_map<std::string, std::any>& config) override {
        if (config.find("learning_rate") != config.end()) {
            learningRate_ = std::any_cast<float>(config.at("learning_rate"));
        }
        isInitialized_ = true;
        return true;
    }
    
    void shutdown() override {
        isInitialized_ = false;
    }
    
    bool isInitialized() const override {
        return isInitialized_;
    }
    
    std::string getStatus() const override {
        return isInitialized_ ? "Initialized" : "Not initialized";
    }
    
    std::unordered_map<std::string, std::any> getMetrics() const override {
        std::unordered_map<std::string, std::any> metrics;
        metrics["learning_rate"] = learningRate_;
        metrics["plasticity_enabled"] = plasticityEnabled_;
        metrics["is_initialized"] = isInitialized_;
        metrics["total_updates"] = totalUpdates_;
        return metrics;
    }
    
    void setLearningRate(float rate) override {
        learningRate_ = rate;
    }
    
    float getLearningRate() const override {
        return learningRate_;
    }
    
    void setplasticityEnabled(bool enabled) override {
        plasticityEnabled_ = enabled;
    }
    
    bool isplasticityEnabled() const override {
        return plasticityEnabled_;
    }
    
    void updateWeights(const std::vector<float>& input, const std::vector<float>& target) override {
        // STDP weight update implementation
        float error = 0.0f;
        for (size_t i = 0; i < input.size() && i < target.size(); ++i) {
            error += std::abs(target[i] - input[i]);
        }
        error /= static_cast<float>(input.size() > 0 ? input.size() : 1);
        
        // Simple weight update
        for (float& weight : weights_) {
            weight += learningRate_ * error;
        }
        
        totalUpdates_++;
    }
    
    float getError() const override {
        return error_;
    }
    
    void setError(float error) {
        error_ = error;
    }

private:
    std::string name_;
    std::string version_;
    std::string description_;
    float learningRate_;
    bool plasticityEnabled_;
    bool isInitialized_;
    float error_;
    size_t totalUpdates_ = 0;
    std::vector<float> weights_;
};

// MemoryPlugin for memory management plugins
class MemoryPlugin : public Plugin {
public:
    MemoryPlugin() {
        name_ = "MemoryPlugin";
        version_ = "1.0.0";
        description_ = "Memory management plugin";
        capacity_ = 1000;
        allocated_ = 0;
        isInitialized_ = false;
    }
    
    std::string getName() const override { return name_; }
    std::string getVersion() const override { return version_; }
    std::string getDescription() const override { return description_; }
    
    bool initialize(const std::unordered_map<std::string, std::any>& config) override {
        if (config.find("capacity") != config.end()) {
            capacity_ = std::any_cast<size_t>(config.at("capacity"));
        }
        isInitialized_ = true;
        return true;
    }
    
    void shutdown() override {
        isInitialized_ = false;
        allocated_ = 0;
    }
    
    bool isInitialized() const override {
        return isInitialized_;
    }
    
    std::string getStatus() const override {
        return isInitialized_ ? "Initialized" : "Not initialized";
    }
    
    std::unordered_map<std::string, std::any> getMetrics() const override {
        std::unordered_map<std::string, std::any> metrics;
        metrics["capacity"] = capacity_;
        metrics["allocated"] = allocated_;
        metrics["utilization"] = static_cast<float>(allocated_) / capacity_;
        metrics["is_initialized"] = isInitialized_;
        return metrics;
    }
    
    bool allocate(size_t size) {
        if (allocated_ + size <= capacity_) {
            allocated_ += size;
            return true;
        }
        return false;
    }
    
    void deallocate(size_t size) {
        allocated_ = std::max<size_t>(0, allocated_ - size);
    }
    
    size_t getAllocated() const {
        return allocated_;
    }
    
    size_t getCapacity() const {
        return capacity_;
    }
    
    float getUtilization() const {
        return static_cast<float>(allocated_) / capacity_;
    }

private:
    std::string name_;
    std::string version_;
    std::string description_;
    size_t capacity_;
    size_t allocated_;
    bool isInitialized_;
};

// NeuromodulationPlugin for neuromodulation plugins
class NeuromodulationPlugin : public Plugin {
public:
    NeuromodulationPlugin() {
        name_ = "NeuromodulationPlugin";
        version_ = "1.0.0";
        description_ = "Neuromodulation plugin";
        dopamineLevel_ = 0.5f;
        serotoninLevel_ = 0.5f;
        norepinephrineLevel_ = 0.5f;
        isInitialized_ = false;
    }
    
    std::string getName() const override { return name_; }
    std::string getVersion() const override { return version_; }
    std::string getDescription() const override { return description_; }
    
    bool initialize(const std::unordered_map<std::string, std::any>& config) override {
        if (config.find("dopamine_level") != config.end()) {
            dopamineLevel_ = std::any_cast<float>(config.at("dopamine_level"));
        }
        if (config.find("serotonin_level") != config.end()) {
            serotoninLevel_ = std::any_cast<float>(config.at("serotonin_level"));
        }
        if (config.find("norepinephrine_level") != config.end()) {
            norepinephrineLevel_ = std::any_cast<float>(config.at("norepinephrine_level"));
        }
        isInitialized_ = true;
        return true;
    }
    
    void shutdown() override {
        isInitialized_ = false;
    }
    
    bool isInitialized() const override {
        return isInitialized_;
    }
    
    std::string getStatus() const override {
        return isInitialized_ ? "Initialized" : "Not initialized";
    }
    
    std::unordered_map<std::string, std::any> getMetrics() const override {
        std::unordered_map<std::string, std::any> metrics;
        metrics["dopamine_level"] = dopamineLevel_;
        metrics["serotonin_level"] = serotoninLevel_;
        metrics["norepinephrine_level"] = norepinephrineLevel_;
        metrics["is_initialized"] = isInitialized_;
        metrics["total_modulations"] = totalModulations_;
        return metrics;
    }
    
    void modulateDopamine(float amount) {
        dopamineLevel_ = std::max(0.0f, std::min(1.0f, dopamineLevel_ + amount));
        totalModulations_++;
    }
    
    void modulateSerotonin(float amount) {
        serotoninLevel_ = std::max(0.0f, std::min(1.0f, serotoninLevel_ + amount));
        totalModulations_++;
    }
    
    void modulateNorepinephrine(float amount) {
        norepinephrineLevel_ = std::max(0.0f, std::min(1.0f, norepinephrineLevel_ + amount));
        totalModulations_++;
    }
    
    float getDopamineLevel() const {
        return dopamineLevel_;
    }
    
    float getSerotoninLevel() const {
        return serotoninLevel_;
    }
    
    float getNorepinephrineLevel() const {
        return norepinephrineLevel_;
    }

private:
    std::string name_;
    std::string version_;
    std::string description_;
    float dopamineLevel_;
    float serotoninLevel_;
    float norepinephrineLevel_;
    bool isInitialized_;
    size_t totalModulations_ = 0;
};

// PredictionPlugin for prediction plugins
class PredictionPlugin : public Plugin {
public:
    PredictionPlugin() {
        name_ = "PredictionPlugin";
        version_ = "1.0.0";
        description_ = "Prediction plugin";
        predictionHorizon_ = 10;
        predictionAccuracy_ = 0.8f;
        isInitialized_ = false;
    }
    
    std::string getName() const override { return name_; }
    std::string getVersion() const override { return version_; }
    std::string getDescription() const override { return description_; }
    
    bool initialize(const std::unordered_map<std::string, std::any>& config) override {
        if (config.find("prediction_horizon") != config.end()) {
            predictionHorizon_ = std::any_cast<size_t>(config.at("prediction_horizon"));
        }
        if (config.find("prediction_accuracy") != config.end()) {
            predictionAccuracy_ = std::any_cast<float>(config.at("prediction_accuracy"));
        }
        isInitialized_ = true;
        return true;
    }
    
    void shutdown() override {
        isInitialized_ = false;
    }
    
    bool isInitialized() const override {
        return isInitialized_;
    }
    
    std::string getStatus() const override {
        return isInitialized_ ? "Initialized" : "Not initialized";
    }
    
    std::unordered_map<std::string, std::any> getMetrics() const override {
        std::unordered_map<std::string, std::any> metrics;
        metrics["prediction_horizon"] = predictionHorizon_;
        metrics["prediction_accuracy"] = predictionAccuracy_;
        metrics["is_initialized"] = isInitialized_;
        metrics["total_predictions"] = totalPredictions_;
        return metrics;
    }
    
    std::vector<float> predict(const std::vector<float>& input) {
        totalPredictions_++;
        
        // Simple prediction: return input scaled by prediction accuracy
        std::vector<float> prediction = input;
        for (float& val : prediction) {
            val *= predictionAccuracy_;
        }
        
        return prediction;
    }
    
    size_t getPredictionHorizon() const {
        return predictionHorizon_;
    }
    
    float getPredictionAccuracy() const {
        return predictionAccuracy_;
    }
    
    float getPredictionError() const {
        return predictionError_;
    }
    
    void setPredictionError(float error) {
        predictionError_ = error;
    }

private:
    std::string name_;
    std::string version_;
    std::string description_;
    size_t predictionHorizon_;
    float predictionAccuracy_;
    float predictionError_;
    bool isInitialized_;
    size_t totalPredictions_ = 0;
};

// SocialPlugin for social behavior plugins
class SocialPlugin : public Plugin {
public:
    SocialPlugin() {
        name_ = "SocialPlugin";
        version_ = "1.0.0";
        description_ = "Social behavior plugin";
        socialWeight_ = 0.7f;
        imitationRate_ = 0.3f;
        coordinationThreshold_ = 0.6f;
        isInitialized_ = false;
    }
    
    std::string getName() const override { return name_; }
    std::string getVersion() const override { return version_; }
    std::string getDescription() const override { return description_; }
    
    bool initialize(const std::unordered_map<std::string, std::any>& config) override {
        if (config.find("social_weight") != config.end()) {
            socialWeight_ = std::any_cast<float>(config.at("social_weight"));
        }
        if (config.find("imitation_rate") != config.end()) {
            imitationRate_ = std::any_cast<float>(config.at("imitation_rate"));
        }
        if (config.find("coordination_threshold") != config.end()) {
            coordinationThreshold_ = std::any_cast<float>(config.at("coordination_threshold"));
        }
        isInitialized_ = true;
        return true;
    }
    
    void shutdown() override {
        isInitialized_ = false;
    }
    
    bool isInitialized() const override {
        return isInitialized_;
    }
    
    std::string getStatus() const override {
        return isInitialized_ ? "Initialized" : "Not initialized";
    }
    
    std::unordered_map<std::string, std::any> getMetrics() const override {
        std::unordered_map<std::string, std::any> metrics;
        metrics["social_weight"] = socialWeight_;
        metrics["imitation_rate"] = imitationRate_;
        metrics["coordination_threshold"] = coordinationThreshold_;
        metrics["is_initialized"] = isInitialized_;
        metrics["total_interactions"] = totalInteractions_;
        return metrics;
    }
    
    void socialize(const std::vector<float>& otherAgentState) {
        totalInteractions_++;
        
        // Simple social behavior: influence by other agent's state
        float influence = 0.0f;
        for (float val : otherAgentState) {
            influence += val;
        }
        influence /= static_cast<float>(otherAgentState.size() > 0 ? otherAgentState.size() : 1);
        
        // Apply social influence
        socialWeight_ = std::max(0.0f, std::min(1.0f, socialWeight_ + influence * 0.1f));
    }
    
    float getSocialWeight() const {
        return socialWeight_;
    }
    
    float getImitationRate() const {
        return imitationRate_;
    }
    
    float getCoordinationThreshold() const {
        return coordinationThreshold_;
    }
    
    bool shouldCoordinate(float socialSignal) const {
        return socialSignal >= coordinationThreshold_;
    }

private:
    std::string name_;
    std::string version_;
    std::string description_;
    float socialWeight_;
    float imitationRate_;
    float coordinationThreshold_;
    bool isInitialized_;
    size_t totalInteractions_ = 0;
};

// ===== COMPREHENSIVE TESTING FRAMEWORK =====

// TestFramework class for comprehensive unit testing
class TestFramework {
public:
    TestFramework() = default;
    
    template<typename TestCase>
    bool runTest(const std::string& testName, TestCase testCase) {
        TestResult result;
        result.testName = testName;
        result.passed = false;
        result.startTime = std::chrono::high_resolution_clock::now();
        
        try {
            testCase();
            result.passed = true;
            result.errorMessage = "";
        } catch (const std::exception& e) {
            result.passed = false;
            result.errorMessage = e.what();
        }
        
        result.endTime = std::chrono::high_resolution_clock::now();
        result.duration = std::chrono::duration<double>(result.endTime - result.startTime).count();
        
        addResult(result);
        return result.passed;
    }
    
    bool runTestSuite(const TestSuite& suite) {
        bool allPassed = true;
        for (const auto& test : suite.tests) {
            if (!runTest(test.name, test.testCase)) {
                allPassed = false;
            }
        }
        return allPassed;
    }
    
    TestReport generateReport() const {
        TestReport report;
        report.totalTests = testResults_.size();
        report.passedTests = 0;
        report.failedTests = 0;
        
        for (const auto& result : testResults_) {
            if (result.passed) {
                report.passedTests++;
            } else {
                report.failedTests++;
            }
            report.totalDuration += result.duration;
        }
        
        report.successRate = static_cast<float>(report.passedTests) / report.totalTests;
        report.timestamp = std::chrono::system_clock::now();
        report.testResults = testResults_;
        
        return report;
    }
    
    void reset() {
        testResults_.clear();
    }
    
    size_t getTotalTestsRun() const {
        return testResults_.size();
    }
    
    size_t getPassedTests() const {
        size_t passed = 0;
        for (const auto& result : testResults_) {
            if (result.passed) passed++;
        }
        return passed;
    }

protected:
    struct TestResult {
        std::string testName;
        bool passed;
        std::string errorMessage;
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
        double duration;
    };
    
    struct TestSuite {
        std::string suiteName;
        std::vector<TestResult> tests;
        std::function<bool()> setup;
        std::function<bool()> teardown;
    };
    
    struct TestReport {
        size_t totalTests;
        size_t passedTests;
        size_t failedTests;
        float successRate;
        double totalDuration;
        std::chrono::system_clock::time_point timestamp;
        std::vector<TestResult> testResults;
    };
    
    void addResult(const TestResult& result) {
        testResults_.push_back(result);
    }
    
    std::vector<TestResult> testResults_;
};

// TestRunner class for test execution
class TestRunner {
public:
    TestRunner() = default;
    
    void setTestFramework(TestFramework& framework) {
        testFramework_ = &framework;
    }
    
    template<typename TestCase>
    bool runTest(const std::string& testName, TestCase testCase) {
        return testFramework_ ? testFramework_->runTest(testName, testCase) : false;
    }
    
    bool runTestSuite(const TestFramework::TestSuite& suite) {
        return testFramework_ ? testFramework_->runTestSuite(suite) : false;
    }
    
    TestFramework::TestReport generateReport() const {
        return testFramework_ ? testFramework_->generateReport() : TestFramework::TestReport{};
    }

private:
    TestFramework* testFramework_ = nullptr;
};

// TestReporter class for test reporting
class TestReporter {
public:
    TestReporter() = default;
    
    std::string generateTextReport(const TestFramework::TestReport& report) const {
        std::stringstream ss;
        ss << "Test Report" << std::endl;
        ss << "===========" << std::endl;
        ss << "Total Tests: " << report.totalTests << std::endl;
        ss << "Passed Tests: " << report.passedTests << std::endl;
        ss << "Failed Tests: " << report.failedTests << std::endl;
        ss << "Success Rate: " << (report.successRate * 100) << "%" << std::endl;
        ss << "Total Duration: " << report.totalDuration << " seconds" << std::endl;
        ss << "Timestamp: " << std::chrono::system_clock::to_time_t(report.timestamp) << std::endl;\n        ss << std::endl << "Detailed Results:" << std::endl;
        ss << "----------------" << std::endl;
        for (const auto& result : report.testResults) {
            ss << (result.passed ? "✓ PASS" : "✗ FAIL") << " " << result.testName << std::endl;
            if (!result.passed) {
                ss << "  Error: " << result.errorMessage << std::endl;
            }
            ss << "  Duration: " << result.duration << " seconds" << std::endl;
        }
        
        return ss.str();
    }
    
    std::string generateJSONReport(const TestFramework::TestReport& report) const {
        std::stringstream ss;
        ss << "{" << std::endl;
        ss << "  \"totalTests\": " << report.totalTests << "," << std::endl;
        ss << "  \"passedTests\": " << report.passedTests << "," << std::endl;
        ss << "  \"failedTests\": " << report.failedTests << "," << std::endl;
        ss << "  \"successRate\": " << report.successRate << "," << std::endl;
        ss << "  \"totalDuration\": " << report.totalDuration << "," << std::endl;
        ss << "  \"timestamp\": \"" << std::chrono::system_clock::to_time_t(report.timestamp) << "\"," << std::endl;
        ss << "  \"testResults\": [" << std::endl;
        
        for (size_t i = 0; i < report.testResults.size(); ++i) {
            const auto& result = report.testResults[i];
            ss << "    {\"name\": \"" << result.testName << "\", \"passed\": " << (result.passed ? "true" : "false")
               << ", \"duration\": " << result.duration;
            if (!result.passed) {
                ss << ", \"error\": \"" << result.errorMessage << "\"";
            }
            if (i < report.testResults.size() - 1) {
                ss << "," << std::endl;
            } else {
                ss << std::endl;
            }
        }
        
        ss << "  ]" << std::endl;
        ss << "}" << std::endl;
        
        return ss.str();
    }
};
