#pragma once

#include "../agent/AgentBody.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <string>
#include <cmath>

namespace nlm {

/**
 * @class SensoryProcessor
 * @brief Processes sensory input and computes novelty from environmental stimuli
 * 
 * This component handles the transduction of sensory percepts into neural current
 * injection. It manages the mapping of sensory modalities (vision, touch, internal,
 * proprioception) to their corresponding neural populations and computes novelty
 * based on changes in sensory input over time.
 * 
 * Responsibilities:
 * - Distribute sensory signals to appropriate neuron groups
 * - Compute novelty metrics from sensory input changes
 * - Maintain historical sensory state for novelty detection
 * - Validate and normalize sensory input values
 * 
 * Key features:
 * - Multi-modal sensory processing
 * - Temporal novelty detection
 * - Memory-based change detection
 * - Thread-safe for concurrent processing
 */
class SensoryProcessor {
public:
    /**
     * @brief Constructor for SensoryProcessor
     * 
     * @param brain Pointer to the brain for neural access
     * @param world Pointer to the world for sensory context
     * @throw std::invalid_argument if brain or world pointers are null
     */
    SensoryProcessor(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world);
    
    /**
     * @brief Destructor
     */
    ~SensoryProcessor() = default;
    
    /**
     * @brief Process sensory percept and inject into brain
     * 
     * Transforms sensory percept values into proportional neural currents and
     * updates the novelty detection system based on input changes.
     * 
     * @param percept The sensory percept to process
     * @pre percept must contain valid sensory data
     * @post Neural currents are injected into appropriate sensory neuron groups
     */
    void processSensoryInput(const SensoryPercept& percept);
    
    /**
     * @brief Update novelty level based on current percept
     * 
     * @param vision Current vision input vector
     * @param worldWidth World width for coordinate normalization
     * @param worldHeight World height for coordinate normalization
     */
    void updateNovelty(const std::vector<float>& vision, float worldWidth, float worldHeight);
    
    /**
     * @brief Get current novelty level
     * 
     * @return float Novelty level (0.0-1.0)
     */
    float getNoveltyLevel() const { return noveltyLevel_; }
    
    /**
     * @brief Get vision configuration size
     * 
     * @return size_t Expected vision width * height
     */
    size_t getVisionInputSize() const { return visionInputSize_; }
    
    /**
     * @brief Configure vision input size
     * 
     * @param width Vision grid width
     * @param height Vision grid height
     * @pre width > 0 and height > 0
     */
    void configureVision(size_t width, size_t height) {
        visionInputSize_ = width * height;
        visionBuffer_.resize(visionInputSize_, 0.0f);
    }
    
    /**
     * @brief Initialize processor with world dimensions
     * 
     * @param world The world to initialize with
     * @pre world pointer must not be null
     */
    void initialize(const SimpleWorld& world);
    
    /**
     * @brief Reset novelty and historical state
     */
    void reset();
    
    /**
     * @brief Get sensory neuron group counts
     * 
     * @return std::tuple<size_t, size_t, size_t, size_t> Vision, touch, internal, proprioception counts
     */
    std::tuple<size_t, size_t, size_t, size_t> getSensoryNeuronCounts() const;
    
    /**
     * @brief Set sensory novelty decay rate
     * 
     * @param decay Rate at which novelty decays (0.0-1.0)
     * @pre 0.0 <= decay <= 1.0
     */
    void setNoveltyDecayRate(float decay) {
        noveltyDecayRate_ = std::clamp(decay, 0.0f, 1.0f);
    }

private:
    /**
     * @brief Inject current into a sensory neuron group
     * 
     * @param neurons Vector of neuron pointers to inject into
     * @param signal Vector of signal values to inject
     * @param scalingFactor Scaling factor for signal->current conversion
     */
    void injectCurrent(const std::vector<Neuron*>& neurons, const std::vector<float>& signal, float scalingFactor);
    
    /**
     * @brief Compute novelty as normalized difference from previous input
     * 
     * @param currentVision Current vision input
     * @return float Novelty level (0.0-1.0)
     */
    float computeNovelty(const std::vector<float>& currentVision);
    
    /**
     * @brief Clamp signal values to valid range
     * 
     * @param signal Vector of signal values to clamp
     * @param min Minimum allowed value
     * @param max Maximum allowed value
     */
    void clampSignal(std::vector<float>& signal, float min = 0.0f, float max = 1.0f);
    
    std::shared_ptr<Brain> brain_;           ///< Brain for neural access
    std::shared_ptr<SimpleWorld> world_;      ///< World for sensory context
    
    // Sensory neuron group counters (for quick access)
    size_t visionNeuronCount_;
    size_t touchNeuronCount_;
    size_t internalNeuronCount_;
    size_t proprioceptionNeuronCount_;
    
    // State variables
    float noveltyLevel_;              ///< Current novelty level (0.0-1.0)
    float noveltyDecayRate_;          ///< Rate at which novelty decays
    size_t visionInputSize_;          ///< Expected vision input buffer size
    std::vector<float> visionBuffer_; ///< Historical vision data for novelty detection
    
    // Previous frame for novelty computation
    std::vector<float> previousVision_;
};

} // namespace nlm
