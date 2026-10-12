#pragma once

/**
 * CompleteCheckpointSystem - Comprehensive brain state checkpointing
 * 
 * Provides robust and complete checkpointing for neural simulations with:
 * - Full brain state serialization including neurons, synapses, connectivity, plasticity
 * - Memory systems integration (working memory, episodic memory, associative memory)
 * - Development system checkpointing
 * - Neuromodulation systems checkpointing
 * - Incremental checkpointing for large-scale simulations
 * - Compression support for efficient storage
 * - Performance optimization with zero-copy where possible
 * - Validation and error recovery
 * 
 * Features:
 * - Complete brain state extraction and restoration
 * - Memory system state preservation
 * - Development system state tracking
 * - Neuromodulation system checkpointing
 * - Efficient serialization with compression
 * - Incremental checkpointing for long simulations
 * - Checkpoint validation and integrity checks
 * - Backward compatibility support
 * 
 * Biological motivation:
 * - Brain state preservation across simulation sessions
 * - Memory consolidation and replay across checkpoints
 * - Developmental trajectories across checkpoints
 * - Neuromodulation state influence on behavior
 * - Learning transfer across simulation sessions
 */

#include <cstddef>
#include <cstdint>
#include <string>
#include <memory>
#include <vector>
#include <chrono>
#include <filesystem>

namespace nlm {

class Config;
class Logger;
class CheckpointWriter;
class CheckpointReader;
class Brain;

// Forward declarations
class Brain;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class DevelopmentSystem;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;

// Checkpoint version for compatibility
constexpr uint32_t CHECKPOINT_VERSION_MAJOR = 1;
constexpr uint32_t CHECKPOINT_VERSION_MINOR = 0;
constexpr uint32_t CHECKPOINT_VERSION_PATCH = 0;

// Compression options
enum class CompressionLevel {
    None,
    Fast,
    Balanced,
    Best
};

// Brain state for complete checkpointing
struct BrainState {
    // Neuron state
    struct NeuronCheckpointData {
        std::vector<float> membranePotential;
        std::vector<float> restingPotential;
        std::vector<float> threshold;
        std::vector<float> resetPotential;
        std::vector<float> leakConductance;
        std::vector<uint8_t> firingState;
        std::vector<uint32_t> refractoryRemaining;
        std::vector<uint32_t> refractoryPeriod;
        std::vector<float> lastSpikeTime;
        std::vector<uint64_t> neuronType;
        std::vector<uint64_t> regionId;
        std::vector<uint64_t> populationId;
    } neuronData;
    
    // Synapse state
    struct SynapseCheckpointData {
        std::vector<uint64_t> sourceNeuron;
        std::vector<uint64_t> destinationNeuron;
        std::vector<float> weight;
        std::vector<uint32_t> delay;
        std::vector<uint8_t> synapseType;
        std::vector<float> eligibilityTrace;
    } synapseData;
    
    // Connectivity state
    struct ConnectivityData {
        size_t neuronCount;
        float connectionProbability;
        size_t maxConnectionsPerNeuron;
    } connectivityData;
    
    // Plasticity state
    struct PlasticityData {
        float stdpLtpWeight;
        float stdpLtdWeight;
        float stdpTau;
        float hebbianLearningRate;
        float plasticityModulation;
    } plasticityData;
    
    // Memory system state
    struct MemoryData {
        size_t workingMemoryTraces;
        size_t episodicMemorySize;
        size_t associativeMemorySize;
    } memoryData;
    
    // Development system state
    struct DevelopmentData {
        DevelopmentalStage developmentalStage;
        float synaptogenesisRate;
        float pruningRate;
        float maturationProgress;
    } developmentData;
    
    // Neuromodulation system state
    struct NeuromodulationData {
        float dopamineLevel;
        float curiosityLevel;
        float noveltyLevel;
        float predictionError;
    } neuromodulationData;
    
    // Simulation state
    struct SimulationState {
        SimulationStep simulationStep;
        Timestamp simulationTime;
        size_t totalSpikes;
        float averageFiringRate;
    } simulationState;
    
    // Statistics
    size_t neuronCount;
    size_t synapseCount;
    size_t regionCount;
};

/**
 * Complete checkpoint system for neural simulations
 */
class CompleteCheckpointSystem {
public:
    /**
     * Create complete checkpoint system
     */
    CompleteCheckpointSystem();
    
    ~CompleteCheckpointSystem();
    
    // Disable copying
    CompleteCheckpointSystem(const CompleteCheckpointSystem&) = delete;
    CompleteCheckpointSystem& operator=(const CompleteCheckpointSystem&) = delete;
    
    /**
     * Configure checkpoint system
     */
    void configure(const std::string& checkpointPath, 
                   bool compression = true,
                   bool incremental = false);
    
    /**
     * Save complete brain state to checkpoint
     */
    bool save(const std::shared_ptr<Brain>& brain, const std::string& filepath);
    
    /**
     * Load complete brain state from checkpoint
     */
    bool load(std::shared_ptr<Brain>& brain, const std::string& filepath);
    
    /**
     * Create incremental checkpoint during simulation
     */
    void checkpoint(std::shared_ptr<Brain>& brain, SimulationStep step);
    
    /**
     * Resume simulation from checkpoint
     */
    bool resume(std::shared_ptr<Brain>& brain, const std::string& filepath);
    
    /**
     * Check if checkpoint file exists
     */
    bool hasCheckpoint(const std::string& filepath) const;
    
    /**
     * Get list of checkpoint files in directory
     */
    std::vector<std::string> getCheckpointFiles(const std::string& directory) const;
    
    /**
     * Clean up old checkpoints (keep most recent N)
     */
    void cleanupOldCheckpoints(const std::string& directory, int keepCount = 5);
    
    /**
     * Save checkpoint metadata
     */
    bool saveMetadata(const std::string& filepath, const std::string& metadata);
    
    /**
     * Load checkpoint metadata
     */
    std::string loadMetadata(const std::string& filepath);
    
    // Performance control
    
    /**
     * Enable/disable compression
     */
    void enableCompression(bool enable);
    
    /**
     * Set compression level
     */
    void setCompressionLevel(int level);
    
    /**
     * Optimize for speed (tradeoff storage for speed)
     */
    void optimizeForSpeed(bool optimize);
    
    // Statistics
    
    /**
     * Get last checkpoint save time (milliseconds)
     */
    double getSaveTime() const;
    
    /**
     * Get last checkpoint load time (milliseconds)
     */
    double getLoadTime() const;
    
    /**
     * Get total number of checkpoints created
     */
    uint64_t getCheckpointSize() const;
    
    /**
     * Get number of checkpoints
     */
    size_t getCheckpointCount() const;
    
private:
    /**
     * Extract complete brain state for checkpointing
     */
    bool extractBrainState(std::shared_ptr<Brain>& brain, BrainState& state);
    
    /**
     * Read brain state from checkpoint file
     */
    bool readBrainState(CheckpointReader& reader, BrainState& state);
    
    /**
     * Restore brain state from checkpoint data
     */
    bool restoreBrainState(std::shared_ptr<Brain>& brain, const BrainState& state);
    
    /**
     * Update incremental checkpoint metadata
     */
    void updateIncrementalMetadata(std::shared_ptr<Brain>& brain, SimulationStep step, const std::string& filepath);
    
    /**
     * Get path to latest checkpoint
     */
    std::string getLatestCheckpointPath() const;
    
    /**
     * Write brain state to checkpoint
     */
    bool writeBrainState(CheckpointWriter& writer, const BrainState& state);
    
    // Component state extraction and restoration
    
    void extractNeuronData(std::shared_ptr<Brain>& brain, BrainState& state);
    void restoreNeuronData(std::shared_ptr<Brain>& brain, const BrainState& state);
    
    void extractSynapseData(std::shared_ptr<Brain>& brain, BrainState& state);
    void restoreSynapseData(std::shared_ptr<Brain>& brain, const BrainState& state);
    
    void extractConnectivityData(std::shared_ptr<Brain>& brain, BrainState& state);
    void restoreConnectivityData(std::shared_ptr<Brain>& brain, const BrainState& state);
    
    void extractPlasticityData(std::shared_ptr<Brain>& brain, BrainState& state);
    void restorePlasticityData(std::shared_ptr<Brain>& brain, const BrainState& state);
    
    void extractMemoryData(std::shared_ptr<Brain>& brain, BrainState& state);
    void restoreMemoryData(std::shared_ptr<Brain>& brain, const BrainState& state);
    
    void extractDevelopmentData(std::shared_ptr<Brain>& brain, BrainState& state);
    void restoreDevelopmentData(std::shared_ptr<Brain>& brain, const BrainState& state);
    
    void extractNeuromodulationData(std::shared_ptr<Brain>& brain, BrainState& state);
    void restoreNeuromodulationData(std::shared_ptr<Brain>& brain, const BrainState& state);
    
    void extractSimulationState(std::shared_ptr<Brain>& brain, BrainState& state);
    
    // Configuration
    std::string checkpointDir_;
    bool compressionEnabled_;
    bool incremental_;
    CompressionLevel compressionLevel_;
    bool speedOptimized_;
    
    // Statistics
    double lastSaveTime_;
    double lastLoadTime_;
    uint64_t totalCheckpoints_;
    double totalSaveTime_;
    double totalLoadTime_;
    
    // Manager for checkpoint operations
    class CheckpointManager* manager_;
    
    // Current checkpoint counter
    uint64_t checkpointCounter_;
};

} // namespace nlm