#include <limits>
#include <unordered_map>
#include <functional>
#include <ctime>
#include <filesystem>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <unordered_set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <random>
#include <cstring>
#include <limits>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <thread>

// Core types
namespace nlm {
    using NeuronId = uint64_t;
    using RegionId = uint64_t;
    using SimulationStep = uint64_t;
    using Timestamp = double;
    using MembranePotential = float;
    using Delay = uint32_t;
    
    // Enumerations
    enum class FiringState { Resting, Active, Refractory };
    enum class NeuronType { Excitatory, Inhibitory, Sensory, Motor, Modulatory, Internal };
    enum class ActionType { MoveForward, MoveBackward, TurnLeft, TurnRight, LookLeft, LookRight, Interact, Wait };
    enum class DevelopmentalStage { Initial, CriticalPeriod, Maturation, Adult, Aging };
    
    // Configuration
    class Config;
    class RandomGenerator;
    class SimulationClock;
    class Logger;
    
    // Neural components
    class NeuralRegion;
    class NeuralPopulation;
    class Neuron;
    class Synapse;
    
    // Systems
    class SpikeSystem;
    class STDP;
    class Hebbian;
    class StructuralPlasticity;
    class NeuralWorkingMemory;
    class NeuralEpisodicMemory;
    class NeuralAssociativeMemory;
    class PredictionSystem;
    class NeuralPlanner;
    class ConceptFormation;
    class AttentionalSelection;
    class DevelopmentSystem;
    class Dopamine;
    class Curiosity;
    class Novelty;
    class PredictionError;
    class Environment;
    
    // Motor system
    struct MotorCommand;
    struct Action;
    struct AgentBody;
    struct SensoryPercept;
    
    // Experiment framework
    class Experiment;
    class ExperimentRunner;
    
    // Checkpointing
    class CheckpointManager;
    
    // Performance
    class MemoryPool;
    class EventQueue;
    class PerformanceProfiler;
    
    // For now, create forward declarations for missing includes
    class Neuron {
    public:
        Neuron(NeuronId id) : id_(id) {}
        
        NeuronId getId() const { return id_; }
        bool isFiring() const { return state_.firingState == FiringState::Active; }
        void setState(FiringState state) { state_.firingState = state; }
        void setLastSpikeTime(Timestamp time) { state_.lastSpikeTime = time; }
        void setRefractoryRemaining(uint32_t remaining) { state_.refractoryRemaining = remaining; }
        bool isInRefractory() const { return state_.firingState == FiringState::Refractory; }
        
        void injectCurrent(MembranePotential current) {}
        void stepLIF(Timestamp currentTime, TimestepDuration dt) {}
        void receiveExcitatoryInput(MembranePotential current) {}
        void receiveInhibitoryInput(MembranePotential current) {}
        
    private:
        struct State {
            FiringState firingState = FiringState::Resting;
            Timestamp lastSpikeTime = -1.0f;
            float membranePotential = -70.0f;
            float restingPotential = -70.0f;
            float threshold = -50.0f;
            float resetPotential = -75.0f;
            float leakConductance = 10.0f;
            uint32_t refractoryRemaining = 0;
            uint32_t refractoryPeriod = 2;
        };
        
        NeuronId id_;
        State state_;
    };
    
    // Create a simple mock RandomGenerator for testing
    class RandomGenerator {
    public:
        float uniformReal(float min, float max) {
            std::uniform_real_distribution<float> dist(min, max);
            return dist(rng_);
        }
        
        int uniformInt(int min, int max) {
            std::uniform_int_distribution<int> dist(min, max);
            return dist(rng_);
        }
        
        void seed(uint64_t seed) {
            rng_.seed(seed);
        }
        
    private:
        std::mt19937 rng_{std::random_device{}()};
    };
    
    // Forward declare important classes
    class Config {
    public:
        template<typename T>
        void set(const std::string&, const T&) {}
        
        template<typename T>
        T getOr(const std::string&, const T& default) const { return default; }
    };
    
    // For now, create stub implementations for missing includes
    class CheckpointManager {
    public:
        void configure(const std::string&, uint64_t, size_t, bool) {}
        bool update(uint64_t, double) { return false; }
        bool saveImmediately(const std::string& = "") { return false; }
    };
    
    // Other missing stubs...
    
    // Create mock logger for testing
    class Logger {
    public:
        void addLogger(const std::shared_ptr<Logger>&) {}
        void setLevel(int) {}
        void log(int, const std::string&) {}
    };
    
    class ConsoleLogger : public Logger {
    public:
        ConsoleLogger(int level) {}
    };
    
    class StreamLogger : public Logger {
    public:
        StreamLogger(std::stringstream&) {}
    };
    
    // Mock action classes
    struct Action {
        Action(ActionType type) : type(type) {}
        ActionType type;
    };
    
    struct MotorCommand {
        ActionType command = ActionType::Wait;
    };
    
    // Mock world classes
    struct SensoryInput {
        std::vector<float> getData() const { return {}; }
    };
    
    struct WorldObject {
        WorldObject(float x, float y, int type, float value) {}
    };
    
    enum class WorldObjectType { Empty, Resource, Hazard, Wall, Marker };
    
    // Mock memory system classes
    class NeuralWorkingMemory {
    public:
        void initialize(class Brain*) {}
        void setCapacity(size_t) {}
        void storeToNeuron(NeuronId, float) {}
        void update(double) {}
        void clear() {}
        std::vector<NeuronId> getMemoryNeurons() const { return {}; }
    };
    
    struct EpisodicMemoryItem {
        SimulationStep timestamp;
        float reward;
        std::vector<NeuronId> activeNeurons;
        std::vector<float> neuronActivations;
    };
    
    class NeuralEpisodicMemory {
    public:
        void initialize(class Brain*) {}
        void setMaxEpisodes(size_t) {}
        void storeEpisode(const EpisodicMemoryItem&) {}
        std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t) const { return {}; }
        void replayEpisode(const EpisodicMemoryItem*) {}
        void consolidate(float) {}
        void clear() {}
    };
    
    class NeuralAssociativeMemory {
    public:
        void initialize(class Brain*) {}
        void clear() {}
    };
    
    // Mock prediction system
    class PredictionSystem {
    public:
        void initialize(class Brain*) {}
    };
    
    // Mock cognition systems
    class NeuralPlanner {
    public:
        void initialize(class Brain*) {}
        void setPlanningDepth(int) {}
    };
    
    class ConceptFormation {
    public:
        void initialize(class Brain*) {}
    };
    
    class AttentionalSelection {
    public:
        void initialize(class Brain*) {}
        void setInhibitionStrength(float) {}
        void setExcitationStrength(float) {}
        void update(double) {}
        void processCompetition(const std::vector<NeuronId>&) {}
        void reset() {}
    };
    
    // Mock development system
    class DevelopmentSystem {
    public:
        void update(class Brain*, class RandomGenerator&, double) {}
    };
    
    // Mock neuromodulation systems
    class Dopamine {
    public:
        void initialize(class Brain*) {}
        void update(double) {}
        float getLevel() const { return 0.0f; }
        float getPlasticityFactor() const { return 1.0f; }
    };
    
    class Curiosity {
    public:
        void initialize(class Brain*) {}
        void update(double) {}
        float getLevel() const { return 0.0f; }
    };
    
    class Novelty {
    public:
        void initialize(class Brain*) {}
        void update(double) {}
        float getLevel() const { return 0.0f; }
    };
    
    class PredictionError {
    public:
        void initialize(class Brain*) {}
        void update(double) {}
        float getLevel() const { return 0.0f; }
    };
    
    // Mock environment
    class Environment {
    public:
        virtual ~Environment() {}
        virtual void update(double) {}
        virtual bool applyMotorCommand(const MotorCommand&, double) { return true; }
        virtual std::shared_ptr<SensoryPercept> getSensoryPercept() const { return nullptr; }
        virtual std::shared_ptr<AgentBody> getAgentBody() const { return nullptr; }
        virtual bool isValidPosition(float, float) const { return true; }
        virtual void setAgentStart(float, float) {}
        virtual double getSimulationTime() const { return 0.0; }
    };
    
    class SimpleWorld : public Environment {
    public:
        void configure(int, int, int, int) {}
        void reset() {}
        void setRandomSeed(uint64_t) {}
        void setAgentStart(float, float) {}
        bool applyMotorCommand(const MotorCommand&, double) override { return true; }
        std::shared_ptr<SensoryPercept> getSensoryPercept() const override { return nullptr; }
        std::shared_ptr<AgentBody> getAgentBody() const override { return nullptr; }
        bool isValidPosition(float, float) const override { return true; }
        float getWidth() const { return 10.0f; }
        float getHeight() const { return 10.0f; }
        float getSimulationTime() const override { return 0.0; }
    };
    
    // Mock spike system
    struct SpikeEvent {
        NeuronId neuron_id;
        Timestamp time;
        SimulationStep step;
    };
    
    struct DelayedSpikeEvent {
        NeuronId source_neuron;
        NeuronId destination_neuron;
        NeuronId synapse_id;
        float weight;
        uint8_t is_excitatory;
        Timestamp current_time;
        Timestamp delivery_time;
        SimulationStep current_step;
        Delay delay;
    };
    
    struct DetailedSpikeEvent {
        NeuronId neuron_id;
        float weight;
        Timestamp time;
        SimulationStep step;
    };
    
    class SpikeSystem {
    public:
        void queueSpike(const SpikeEvent&) {}
        void queueDelayedSpike(const DelayedSpikeEvent&) {}
        void processSpikes(SimulationStep) {}
        void processDelayedSpikes(SimulationStep, Timestamp) {}
        void registerHandler(const std::function<void(const DetailedSpikeEvent&)>&) {}
        void registerDelayedHandler(const std::function<void(const DelayedSpikeEvent&)>&) {}
        void reset() {}
        
        size_t getPendingSpikeCount() const { return 0; }
        size_t getPendingDelayedCount() const { return 0; }
    };
    
    // Mock plasticity systems
    struct PlasticityFlags {
        bool stdp = false;
        bool hebbian = false;
    };
    
    class STDP {
    public:
        void update(Synapse*, const std::vector<Timestamp>&, const std::vector<Timestamp>&, double) {}
        void setLTPWeight(float) {}
        void setLTDWeight(float) {}
    };
    
    class Hebbian {
    public:
        void update(Synapse*, const std::vector<Timestamp>&, const std::vector<Timestamp>&, double) {}
    };
    
    class StructuralPlasticity {
    public:
        void update(class Brain*, class RandomGenerator&) {}
        void setSynaptogenesisRate(float) {}
        void setPruningRate(float) {}
    };
    
    // Mock synapse
    class Synapse {
    public:
        Synapse(NeuronId src, NeuronId dest, float weight) {}
        
        NeuronId getSourceNeuron() const { return 0; }
        NeuronId getDestinationNeuron() const { return 0; }
        NeuronId getId() const { return 0; }
        float getWeight() const { return 0.0f; }
        void setWeight(float) {}
        Delay getDelay() const { return 1; }
        uint8_t getType() const { return 0; }
        PlasticityFlags getPlasticityFlags() const { return PlasticityFlags(); }
        void setPlasticityFlags(const PlasticityFlags&) {}
        float getEligibilityTrace() const { return 0.0f; }
        void decayEligibilityTrace(float) {}
        float getEfficacy() const { return 0.0f;
        float getShortTermDepression() const { return 0.0f; }
        float getShortTermFacilitation() const { return 0.0f; }
        
        std::vector<Timestamp> getPreSpikeHistory() const { return {}; }
        std::vector<Timestamp> getPostSpikeHistory() const { return {}; }
        void recordPreSpike(Timestamp) {}
        void recordPostSpike(Timestamp) {}
        void step(Timestamp) {}
    };
    
    // Mock brain interface for testing
    class Brain {
    public:
        virtual ~Brain() {}
        virtual bool initialize() { return true; }
        virtual void step(SimulationStep) {}
        virtual void step(SimulationStep, Timestamp) {}
        virtual size_t getTotalNeuronCount() const { return 0; }
        virtual size_t getTotalSynapseCount() const { return 0; }
        virtual RandomGenerator* getRandomGenerator() { return nullptr; }
    };
    
    // Mock neural region and population
    class NeuralRegion {
    public:
        virtual ~NeuralRegion() {}
        virtual RegionId getId() const { return 0; }
        virtual size_t getPopulationCount() const { return 0; }
        virtual size_t getTotalNeuronCount() const { return 0; }
        virtual size_t getTotalSynapseCount() const { return 0; }
        virtual size_t getActiveNeuronCount() const { return 0; }
        virtual size_t getFiringNeuronCount() const { return 0; }
        virtual void reset() {}
        
        virtual NeuralPopulation* getPopulation(RegionId) { return nullptr; }
        virtual std::vector<Neuron*> getAllNeurons() const { return {}; }
        virtual std::vector<Synapse*> getSynapses() const { return {}; }
        virtual std::vector<Synapse*> getSynapsesTo(NeuronId) const { return {}; }
        virtual std::vector<Synapse*> getSynapsesFrom(NeuronId) const { return {}; }
        
        virtual void addPopulation(RegionId, size_t, NeuronType) {}
        virtual void initializeRandomConnectivity(class RandomGenerator&, float, float, float) {}
    };
    
    class NeuralPopulation {
    public:
        virtual ~NeuralPopulation() {}
        virtual RegionId getId() const { return 0; }
        virtual NeuronType getNeuronType() const { return NeuronType::Internal; }
        virtual size_t getNeuronCount() const { return 0; }
        virtual size_t getSynapseCount() const { return 0; }
        virtual size_t getActiveNeuronCount() const { return 0; }
        virtual size_t getFiringNeuronCount() const { return 0; }
        virtual void resetNeurons() {}
        
        virtual std::vector<Neuron*> getNeurons() const { return {}; }
    };
    
    // Global configuration for testing
    class GlobalConfig {
    public:
        static GlobalConfig& getInstance() {
            static GlobalConfig instance;
            return instance;
        }
        
        void setLevel(int) {}
        int getLevel() const { return 0; }
        
    private:
        GlobalConfig() {}
    };
    
    // Function to get global logger
    inline Logger& getLogger() {
        static Logger logger;
        return logger;
    }
    
    inline Logger& getGlobal() {
        static Logger logger;
        return logger;
    }
    
    // Logging macros
    #define NLM_LOG_INFO(msg) nlm::getLogger().log(0, msg)
    #define NLM_LOG_ERROR(msg) nlm::getLogger().log(1, msg)
    
    // Simple string utilities
    namespace StringUtils {
        std::string toLower(const std::string& str) {
            std::string result = str;
            for (char& c : result) {
                c = std::tolower(c);
            }
            return result;
        }
        
        bool startsWith(const std::string& str, const std::string& prefix) {
            return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
        }
        
        size_t find(const std::string& str, const std::string& substr) {
            size_t pos = str.find(substr);
            return pos == std::string::npos ? 0 : pos;
        }
    }
    
    // Simple filesystem utilities
    namespace FileUtils {
        bool exists(const std::string& path) {
            return std::ifstream(path).good();
        }
        
        bool remove(const std::string& path) {
            return std::remove(path.c_str()) == 0;
        }
        
        void create_directories(const std::string& path) {
            // Simple implementation - in real code would use proper filesystem
        }
    }
    
    // Numeric utilities
    namespace NumericUtils {
        inline float abs(float x) { return std::abs(x); }
        inline float clamp(float x, float min, float max) { return std::max(min, std::min(x, max)); }
        inline float exp(float x) { return std::exp(x); }
    }
    
    // Standard headers to include
    #include <iostream>
    #include <fstream>
    #include <sstream>
    #include <string>
    #include <vector>
    #include <map>
    #include <unordered_map>
    #include <algorithm>
    #include <cmath>
    #include <random>
    #include <memory>
    #include <functional>
    #include <limits>
    #include <mutex>
    #include <atomic>
    #include <thread>
    #include <condition_variable>
    #include <stdexcept>
    #include <iomanip>
    #include <numeric>
    #include <chrono>
    #include <cstring>
    #include <cstdint>
    
    // Ensure the include guard is at the end
    #define BRAIN_TEST_UTILS_HPP
} // namespace nlm
