// Abstract Base Classes for NLM Components
// Provides clean interfaces and abstractions between components

#ifndef NLM_ABSTRACTIONS_HPP
#define NLM_ABSTRACTIONS_HPP

#include <memory>
#include <string>
#include <vector>
#include <chrono>

// Forward declarations
class Brain;
class Config;
class Logger;

namespace nlm {

// Abstract Brain Interface
class IBrain {
public:
    virtual ~IBrain() = default;
    
    virtual bool initialize() = 0;
    virtual void reset() = 0;
    virtual void step(SimulationStep step, double time) = 0;
    virtual size_t getTotalNeuronCount() const = 0;
    virtual size_t getTotalSynapseCount() const = 0;
    virtual size_t getTotalSpikeCount() const = 0;
    virtual size_t getFiringNeuronCount() const = 0;
    virtual void injectCurrentToNeurons(NeuronType type, float current) = 0;
    virtual const SpikeSystem* getSpikeSystem() const = 0;
    virtual void logStatus() const = 0;
    
    // Factory method pattern for creating different brain types
    static std::unique_ptr<IBrain> createBrain(const Config& config);
};

// Abstract Config Interface
class IConfig {
public:
    virtual ~IConfig() = default;
    
    virtual bool loadFromFile(const std::string& filename) = 0;
    virtual void loadFromArgs(int argc, char** argv) = 0;
    virtual void set(const std::string& key, const std::string& value, ConfigSource source) = 0;
    
    // Type-safe getter methods
    virtual std::string getAsString(const std::string& key) const = 0;
    virtual int64_t getAsInt(const std::string& key) const = 0;
    virtual double getAsDouble(const std::string& key) const = 0;
    virtual float getAsFloat(const std::string& key) const = 0;
    
    // Type-safe getOr methods
    virtual std::string getOr(const std::string& key, const std::string& defaultValue) const = 0;
    virtual int64_t getOr(const std::string& key, int64_t defaultValue) const = 0;
    virtual double getOr(const std::string& key, double defaultValue) const = 0;
    virtual float getOr(const std::string& key, float defaultValue) const = 0;
    
    // Configuration management
    virtual size_t getKeyCount() const = 0;
    virtual bool hasKey(const std::string& key) const = 0;
};

// Abstract Logger Interface
class ILogger {
public:
    virtual ~ILogger() = default;
    
    virtual void log(const std::string& message, LogLevel level = LogLevel::Info) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual void addLogger(std::shared_ptr<ILogger> logger) = 0;
    virtual std::string getName() const = 0;
    
    // Static logger management
    static std::shared_ptr<ILogger> getGlobalLogger();
    static void setGlobalLogger(std::shared_ptr<ILogger> logger);
};

// Abstract Factory Interface
class IComponentFactory {
public:
    virtual ~IComponentFactory() = default;
    
    virtual std::unique_ptr<IBrain> createBrain(const IConfig& config) = 0;
    virtual std::unique_ptr<IConfig> createConfig() = 0;
    virtual std::unique_ptr<IConfig> createConfig(const std::string& filename) = 0;
    virtual std::unique_ptr<ILogger> createLogger() = 0;
    
    // Singleton pattern
    static IComponentFactory& getInstance();
    static void setFactory(std::unique_ptr<IComponentFactory> factory);
};

// Abstract Test Interface
class ITest {
public:
    virtual ~ITest() = default;
    
    virtual std::string getName() const = 0;
    virtual bool run(std::shared_ptr<IBrain> brain) = 0;
    virtual void setUp(std::shared_ptr<IBrain> brain) = 0;
    virtual void tearDown(std::shared_ptr<IBrain> brain) = 0;
    
    // Timing and statistics
    virtual double getExecutionTime() const = 0;
    virtual size_t getNeuronCount() const = 0;
    virtual size_t getSynapseCount() const = 0;
    virtual TestStatus getStatus() const = 0;
    virtual std::string getErrorMessage() const = 0;
};

// Abstract Memory System Interface
class IMemorySystem {
public:
    virtual ~IMemorySystem() = default;
    
    virtual void store(const std::string& key, const std::vector<float>& data) = 0;
    virtual std::vector<float> retrieve(const std::string& key) const = 0;
    virtual bool exists(const std::string& key) const = 0;
    virtual void clear() = 0;
    virtual size_t getItemCount() const = 0;
    virtual void remove(const std::string& key) = 0;
    
    // Memory management
    virtual void compact() = 0;
    virtual size_t getMemoryUsage() const = 0;
};

// Abstract Event System Interface
class IEventSystem {
public:
    virtual ~IEventSystem() = default;
    
    virtual void subscribe(const std::string& eventType, std::function<void(const Event&)> callback) = 0;
    virtual void unsubscribe(const std::string& eventType, std::function<void(const Event&)> callback) = 0;
    virtual void publish(const std::string& eventType, const Event& event) = 0;
    virtual bool hasSubscribers(const std::string& eventType) const = 0;
    virtual size_t getSubscriberCount(const std::string& eventType) const = 0;
    
    // Event management
    virtual void clear(const std::string& eventType = "") = 0;
};

// Abstract Simulation Clock Interface
class IClock {
public:
    virtual ~IClock() = default;
    
    virtual double getCurrentTime() const = 0;
    virtual SimulationStep getCurrentStep() const = 0;
    virtual double getTimeStep() const = 0;
    virtual void setTimeStep(double dt) = 0;
    virtual void advance(SimulationStep steps) = 0;
    virtual void reset() = 0;
    
    // Time utilities
    virtual double elapsedSince(double time) const = 0;
    virtual std::chrono::milliseconds getUptime() const = 0;
};

// Configuration Source enum
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Programmatic
};

// Event structure
struct Event {
    std::string type;
    double timestamp;
    std::map<std::string, std::string> data;
    std::shared_ptr<void> payload;
};

// Test Status enum
enum class TestStatus {
    PASS,
    FAIL,
    SKIP,
    ERROR
};

// Neuron Type enum
enum class NeuronType {
    Excitatory,
    Inhibitory,
    Sensory,
    Motor,
    Internuncial
};

// Simulation Step type
using SimulationStep = uint64_t;

// Neuron ID type
using NeuronId = uint64_t;

// Synapse ID type
using SynapseId = uint64_t;

// Region ID type
using RegionId = uint64_t;

// Spike System interface
class ISpikeSystem {
public:
    virtual ~ISpikeSystem() = default;
    
    virtual void processSpikes() = 0;
    virtual void reset() = 0;
    virtual size_t getActiveNeuronCount() const = 0;
    virtual std::vector<NeuronId> getMostActiveNeurons(size_t count) const = 0;
    virtual double getLastFiringRate() const = 0;
    virtual void setFiringRate(double rate) = 0;
};

} // namespace nlm

#endif // NLM_ABSTRACTIONS_HPP