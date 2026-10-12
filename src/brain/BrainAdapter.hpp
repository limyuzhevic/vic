#pragma once

/**
 * BrainAdapter - Adapter pattern for integrating performance optimizations
 * 
 * This adapter class wraps the OptimizedBrain to maintain backward compatibility
 * while providing access to all performance optimizations.
 * 
 * Features:
 * - Transparently delegates all Brain calls to OptimizedBrain
 * - Maintains full backward compatibility with existing Brain interface
 * - Provides access to performance monitoring and statistics
 * - Allows enabling/disabling of optimizations dynamically
 * - Manages memory pool and other resources
 * 
 * Usage:
 *   std::shared_ptr<Brain> brain = std::make_shared<BrainAdapter>(config);
 *   brain->initialize();  // Uses OptimizedBrain internally
 *   brain->step(step, time);  // Uses optimized implementations
 *   // Access performance stats
 *   brain->getPerformanceStats();  // Available via adapter
 */

#include "brain/Brain.hpp"
#include "brain/MemoryPoolManager.hpp"
#include "brain/OptimizedBrain.hpp"
#include <memory>
#include <string>

namespace nlm {

class BrainAdapter : public Brain {
public:
    /**
     * Create brain adapter with configuration
     * @param config Configuration for both Brain and optimizations
     */
    explicit BrainAdapter(std::shared_ptr<Config> config)
        : config_(config)
    {
        // Create optimized brain internally
        optimizedBrain_ = std::make_unique<OptimizedBrain>(config);
        
        // Initialize optimized brain
        if (!optimizedBrain_->initialize()) {
            throw std::runtime_error("Failed to initialize OptimizedBrain");
        }
        
        // Setup memory pool manager
        memoryPoolManager_ = &MemoryPoolManager::getInstance();
        memoryPoolManager_->setConfig(config);
        
        // Setup event system
        eventSystem_ = optimizedBrain_->getEventSystem();
        
        // Setup connectivity manager
        connectivity_ = optimizedBrain_->getConnectivityManager();
        
        // Setup parallel processor
        parallelProcessor_ = optimizedBrain_->getParallelProcessor();
        
        // Setup SIMD vectorizer
        simdVectorizer_ = optimizedBrain_->getSIMDVectorizer();
    }
    
    ~BrainAdapter() override {
        // Cleanup is handled by smart pointers
    }
    
    // Disable copying
    BrainAdapter(const BrainAdapter&) = delete;
    BrainAdapter& operator=(const BrainAdapter&) = delete;
    
    // Enable moving
    BrainAdapter(BrainAdapter&&) noexcept = default;
    BrainAdapter& operator=(BrainAdapter&&) noexcept = default;
    
    /**
     * Initialize brain with configuration
     */
    bool initialize() override {
        return optimizedBrain_->initialize();
    }
    
    /**
     * Main simulation step with optimized implementation
     */
    void step(SimulationStep currentStep) override {
        optimizedBrain_->step(currentStep);
    }
    
    /**
     * Main simulation step with time parameter
     */
    void step(SimulationStep currentStep, Timestamp currentTime) override {
        optimizedBrain_->step(currentStep, currentTime);
    }
    
    /**
     * Receive sensory input from environment
     */
    void receiveSensoryInput(const SensoryInput& input) override {
        optimizedBrain_->receiveSensoryInput(input);
    }
    
    /**
     * Inject current directly into a specific neuron
     */
    void injectCurrent(NeuronId neuron, MembranePotential current) override {
        optimizedBrain_->injectCurrent(neuron, current);
    }
    
    /**
     * Inject current into all neurons of a specific type
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current) override {
        optimizedBrain_->injectCurrentToNeurons(type, current);
    }
    
    /**
     * Get spike system
     */
    SpikeSystem* getSpikeSystem() override {
        return optimizedBrain_->getSpikeSystem();
    }
    
    const SpikeSystem* getSpikeSystem() const override {
        return optimizedBrain_->getSpikeSystem();
    }
    
    /**
     * Get STDP system
     */
    STDP* getSTDP() override {
        return optimizedBrain_->getSTDP();
    }
    
    /**
     * Get Hebbian system
     */
    Hebbian* getHebbian() override {
        return optimizedBrain_->getHebbian();
    }
    
    /**
     * Get structural plasticity system
     */
    StructuralPlasticity* getStructuralPlasticity() override {
        return optimizedBrain_->getStructuralPlasticity();
    }
    
    /**
     * Get statistics
     */
    float getExcitationInhibitionRatio() const override {
        return optimizedBrain_->getExcitationInhibitionRatio();
    }
    
    size_t getTotalSpikeCount() const override {
        return optimizedBrain_->getTotalSpikeCount();
    }
    
    size_t getPendingSpikeEventCount() const override {
        return optimizedBrain_->getPendingSpikeEventCount();
    }
    
    /**
     * Produce motor/action output
     */
    std::unique_ptr<Action> produceAction() override {
        return optimizedBrain_->produceAction();
    }
    
    /**
     * Apply neuromodulatory signals
     */
    void applyNeuromodulation(const Neuromodulator& signal) override {
        optimizedBrain_->applyNeuromodulation(signal);
    }
    
    /**
     * Update plasticity rules
     */
    void updatePlasticity() override {
        optimizedBrain_->updatePlasticity();
    }
    
    /**
     * Apply developmental changes
     */
    void develop() override {
        optimizedBrain_->develop();
    }
    
    /**
     * Reset brain state
     */
    void reset() override {
        optimizedBrain_->reset();
    }
    
    /**
     * Save brain state to file (checkpointing)
     */
    bool save(const std::string& filepath) const override {
        return optimizedBrain_->save(filepath);
    }
    
    /**
     * Load brain state from file
     */
    bool load(const std::string& filepath) override {
        return optimizedBrain_->load(filepath);
    }
    
    /**
     * Region management
     */
    RegionId addRegion(const std::string& name = "") override {
        return optimizedBrain_->addRegion(name);
    }
    
    NeuralRegion* getRegion(RegionId id) override {
        return optimizedBrain_->getRegion(id);
    }
    
    const NeuralRegion* getRegion(RegionId id) const override {
        return optimizedBrain_->getRegion(id);
    }
    
    size_t getRegionCount() const override {
        return optimizedBrain_->getRegionCount();
    }
    
    std::vector<RegionId> getRegionIds() const override {
        return optimizedBrain_->getRegionIds();
    }
    
    /**
     * Get all regions
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const override {
        return optimizedBrain_->getRegions();
    }
    
    /**
     * Inter-region connection management
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1) override {
        optimizedBrain_->addInterRegionConnection(source, target, weight, delay);
    }
    
    void removeInterRegionConnection(RegionId source, RegionId target) override {
        optimizedBrain_->removeInterRegionConnection(source, target);
    }
    
    /**
     * Global statistics
     */
    size_t getTotalNeuronCount() const override {
        return optimizedBrain_->getTotalNeuronCount();
    }
    
    size_t getTotalSynapseCount() const override {
        return optimizedBrain_->getTotalSynapseCount();
    }
    
    size_t getActiveNeuronCount() const override {
        return optimizedBrain_->getActiveNeuronCount();
    }
    
    size_t getFiringNeuronCount() const override {
        return optimizedBrain_->getFiringNeuronCount();
    }
    
    float getAverageFiringRate() const override {
        return optimizedBrain_->getAverageFiringRate();
    }
    
    // ========== MEMORY SYSTEMS ==========
    
    /**
     * Working memory access
     */
    NeuralWorkingMemory* getWorkingMemory() override {
        return optimizedBrain_->getWorkingMemory();
    }
    
    /**
     * Episodic memory access
     */
    NeuralEpisodicMemory* getEpisodicMemory() override {
        return optimizedBrain_->getEpisodicMemory();
    }
    
    /**
     * Associative memory access
     */
    NeuralAssociativeMemory* getAssociativeMemory() override {
        return optimizedBrain_->getAssociativeMemory();
    }
    
    // ========== PREDICTION SYSTEM ==========
    
    /**
     * Prediction system access
     */
    PredictionSystem* getPredictionSystem() override {
        return optimizedBrain_->getPredictionSystem();
    }
    
    // ========== COGNITION SYSTEMS ==========
    
    /**
     * Neural planner access
     */
    NeuralPlanner* getPlanner() override {
        return optimizedBrain_->getPlanner();
    }
    
    /**
     * Concept formation access
     */
    ConceptFormation* getConceptFormation() override {
        return optimizedBrain_->getConceptFormation();
    }
    
    /**
     * Attentional selection access
     */
    AttentionalSelection* getAttention() override {
        return optimizedBrain_->getAttention();
    }
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    /**
     * Development system access
     */
    DevelopmentSystem* getDevelopmentSystem() override {
        return optimizedBrain_->getDevelopmentSystem();
    }
    
    /**
     * Get developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const override {
        return optimizedBrain_->getDevelopmentalStage();
    }
    
    /**
     * Set developmental stage
     */
    void setDevelopmentalStage(DevelopmentalStage stage) override {
        optimizedBrain_->setDevelopmentalStage(stage);
    }
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    /**
     * Dopamine access
     */
    Dopamine* getDopamine() override {
        return optimizedBrain_->getDopamine();
    }
    
    /**
     * Curiosity access
     */
    Curiosity* getCuriosity() override {
        return optimizedBrain_->getCuriosity();
    }
    
    /**
     * Novelty access
     */
    Novelty* getNovelty() override {
        return optimizedBrain_->getNovelty();
    }
    
    /**
     * Prediction error signal access
     */
    PredictionError* getPredictionErrorSignal() override {
        return optimizedBrain_->getPredictionErrorSignal();
    }
    
    /**
     * Get current configuration
     */
    std::shared_ptr<const Config> getConfig() const override {
        return optimizedBrain_->getConfig();
    }
    
    /**
     * Get random generator
     */
    RandomGenerator* getRandomGenerator() override {
        return optimizedBrain_->getRandomGenerator();
    }
    
    /**
     * Logging
     */
    void logStatus() const override {
        optimizedBrain_->logStatus();
    }
    
    // ========== PERFORMANCE OPTIMIZATION ACCESSORS ==========
    
    /**
     * Get performance statistics
     */
    OptimizedBrain::PerformanceStats getPerformanceStats() const {
        return optimizedBrain_->getPerformanceStats();
    }
    
    /**
     * Enable/disable event-driven processing
     */
    void enableEventDriven(bool enable) {
        optimizedBrain_->enableEventDriven(enable);
    }
    
    /**
     * Enable/disable multithreading
     */
    void enableMultithreading(bool enable) {
        optimizedBrain_->enableMultithreading(enable);
    }
    
    /**
     * Enable/disable SIMD
     */
    void enableSIMD(bool enable) {
        optimizedBrain_->enableSIMD(enable);
    }
    
    /**
     * Enable/disable sparse connectivity
     */
    void enableSparseConnectivity(bool enable) {
        optimizedBrain_->enableSparseConnectivity(enable);
    }
    
    /**
     * Set thread count
     */
    void setThreadCount(size_t count) {
        optimizedBrain_->setThreadCount(count);
    }
    
    /**
     * Get thread count
     */
    size_t getThreadCount() const {
        return optimizedBrain_->getThreadCount();
    }
    
    /**
     * Enable/disable performance monitoring
     */
    void enablePerformanceMonitoring(bool enable) {
        optimizedBrain_->enablePerformanceMonitoring(enable);
    }
    
    /**
     * Report performance statistics
     */
    void reportPerformance() {
        optimizedBrain_->reportPerformance();
    }
    
    /**
     * Get memory pool manager
     */
    MemoryPoolManager& getMemoryPoolManager() {
        return *memoryPoolManager_;
    }
    
    /**
     * Get event system
     */
    EventSystem& getEventSystem() {
        return *eventSystem_;
    }
    
    /**
     * Get connectivity manager
     */
    SparseConnectivity& getConnectivityManager() {
        return *connectivity_;
    }
    
    /**
     * Get parallel processor
     */
    ParallelProcessor& getParallelProcessor() {
        return *parallelProcessor_;
    }
    
    /**
     * Get SIMD vectorizer
     */n    VectorizedLIF& getSIMDVectorizer() {
        return *simdVectorizer_;
    }
    
    /**
     * Get optimized brain internal pointer
     */
    OptimizedBrain* getOptimizedBrain() {
        return optimizedBrain_.get();
    }
    
    /**
     * Check if performance optimizations are enabled
     */
    bool areOptimizationsEnabled() const {
        return eventDrivenEnabled_ && multithreadingEnabled_ && 
               simdEnabled_ && sparseEnabled_;
    }
    
    /**
     * Get optimization configuration
     */
    struct OptimizationConfig {
        bool eventDrivenEnabled = false;
        bool multithreadingEnabled = false;
        bool simdEnabled = false;
        bool sparseConnectivityEnabled = false;
        size_t threadCount = 1;
        bool performanceMonitoringEnabled = false;
        size_t neuronPoolSize = 1024;
        size_t synapsePoolSize = 4096;
        size_t eventQueueCapacity = 100000;
        bool enableZeroCopy = false;
        bool enableWorkStealing = false;
        bool enableCacheAlignment = false;
        bool enableEventRecycling = false;
        bool compressionEnabled = false;
        int compressionLevel = 1;
        bool speedOptimized = false;
        size_t maxEventsPerStep = 1000;
        bool enableDynamicScheduling = false;
        size_t synapseBatchThreshold = 5000;
        size_t plasticityBatchThreshold = 5000;
        bool bulkProcessingEnabled = false;
        int vectorizationWidth = 8;
        bool enableAlignmentAlloc = false;
        size_t cacheLineSize = 64;
        bool numaAware = false;
        bool hardwareDetection = false;
        size_t cpuAffinity = 1;
    };
    
    OptimizationConfig getOptimizationConfig() const {
        OptimizationConfig config;
        config.eventDrivenEnabled = optimizedBrain_->isEventDrivenEnabled();
        config.multithreadingEnabled = optimizedBrain_->isMultithreadingEnabled();
        config.simdEnabled = optimizedBrain_->isSIMDEnabled();
        config.sparseConnectivityEnabled = optimizedBrain_->isSparseEnabled();
        config.threadCount = optimizedBrain_->getThreadCount();
        config.performanceMonitoringEnabled = performanceMonitoringEnabled_;
        
        // Get pool sizes from memory pool manager
        if (memoryPoolManager_) {
            config.neuronPoolSize = memoryPoolManager_->getExcitatoryNeuronPool().size();
            config.synapsePoolSize = memoryPoolManager_->getExcitatorySynapsePool().size();
        }
        
        return config;
    }
    
    /**
     * Update optimization configuration
     */
    void updateOptimizationConfig(const OptimizationConfig& config) {
        optimizedBrain_->enableEventDriven(config.eventDrivenEnabled);
        optimizedBrain_->enableMultithreading(config.multithreadingEnabled);
        optimizedBrain_->enableSIMD(config.simdEnabled);
        optimizedBrain_->enableSparseConnectivity(config.sparseConnectivityEnabled);
        optimizedBrain_->setThreadCount(config.threadCount);
        optimizedBrain_->enablePerformanceMonitoring(config.performanceMonitoringEnabled);
        
        // Update memory pool manager configuration
        if (memoryPoolManager_) {
            // In a real implementation, you would update pool sizes here
            // This would require adding configuration methods to the pool manager
        }
        
        // Note: Event system, connectivity manager, etc. would need to be updated
        // as well if their configuration changes
    }
    
private:
    std::shared_ptr<Config> config_;
    std::unique_ptr<OptimizedBrain> optimizedBrain_;
    MemoryPoolManager* memoryPoolManager_;
    EventSystem* eventSystem_;
    SparseConnectivity* connectivity_;
    ParallelProcessor* parallelProcessor_;
    VectorizedLIF* simdVectorizer_;
    bool eventDrivenEnabled_;
    bool multithreadingEnabled_;
    bool simdEnabled_;
    bool sparseEnabled_;
    bool performanceMonitoringEnabled_;
};

} // namespace nlm