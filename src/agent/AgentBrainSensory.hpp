// AgentBrain sensory processing header
// Handles sensory input processing and injection

#pragma once

#include <string>
#include <vector>
#include <functional>

namespace nlm {

class Neuron;

/**
 * @class AgentBrainSensory
 * @brief Manages sensory input processing and neural injection
 * 
 * This module handles all aspects of sensory processing:
 * - Vision input processing and current injection
 * - Touch input processing and maximum touch detection
 * - Text input processing for text-as-a-sense functionality
 * - Novelty and prediction update mechanisms
 * - Prediction maintenance using exponential moving averages
 * 
 * The sensory processing module is responsible for converting environmental
 * data (vision, touch, text) into neural signals that can be processed by
 * the brain. It also maintains predictions and calculates novelty to drive
 * curiosity and learning.
 */
class AgentBrainSensory {
public:
    /**
     * @brief Process vision input to sensory neurons
     * 
     * Converts visual sensor data into neural currents by injecting appropriate
     * current values into vision sensory neurons. This represents the transduction
     * of external visual information into neural signals.
     * 
     * @param vision Vector of vision intensity values (0.0 to 1.0) from world
     * @param visionNeurons Collection of vision sensory neurons to receive input
     * @param visionMean Output parameter that receives calculated vision mean
     * @param lastVision Output parameter that receives current vision state
     */
    static void processVisionInput(
        const std::vector<float>& vision,
        const std::vector<Neuron*>& visionNeurons,
        float& visionMean,
        std::vector<float>& lastVision);
    
    /**
     * @brief Process touch input to sensory neurons
     * 
     * Converts tactile sensor data into neural currents by detecting maximum
     * touch intensity and identifying forward touch directions. This represents
     * the transduction of physical contact information into neural signals.
     * 
     * @param touch Vector of touch intensity values (0.0 to 1.0) from world
     * @param maxTouch Output parameter that receives maximum touch detected
     * @param forwardTouch Output parameter that receives forward touch strength
     * @param lastTouch Output parameter that receives current touch state
     */
    static void processTouchInput(
        const std::vector<float>& touch,
        float& maxTouch,
        float& forwardTouch,
        std::vector<float>& lastTouch);
    
    /**
     * @brief Process text input for text-as-a-sense
     * 
     * Implements the text-as-a-sense functionality by converting text strings
     * into sparse neural patterns using the encodeText method. This allows
     * the brain to process learned text associations without string matching.
     * 
     * @param text The text string to process as sensory input
     * @param encodeText Function pointer to text encoding method
     * @param visionNeurons Collection of vision sensory neurons for text injection
     */
    static void processTextInput(
        const std::string& text,
        const std::function<std::vector<size_t>(const std::string&)>& encodeText,
        const std::vector<Neuron*>& visionNeurons);
    
    /**
     * @brief Update novelty level and predicted vision
     * 
     * Calculates novelty as the prediction error between current vision and
     * the brain's predicted vision. This drives curiosity and exploration behavior
     * by signaling when the environment is novel or unexpected.
     * 
     * @param vision Current vision input from world
     * @param touch Current touch input from world  
     * @param noveltyLevel Output parameter that receives calculated novelty
     * @param forwardTouch Output parameter that receives forward touch strength
     * @param forwardObjectVision Output parameter that receives forward object vision
     * @param predictedVision Output parameter that receives updated prediction
     * @param sensoryNoveltyDecay Decay factor for novelty computation
     */
    static void updateNoveltyAndPrediction(
        const std�่ง float>& vision,
        const std::vector<float>& touch,
        float& noveltyLevel,
        float& forwardTouch,
        float& forwardObjectVision,
        std::vector<float>& predictedVision,
        float sensoryNoveltyDecay);
    
    /**
     * @brief Gradually update prediction towards current input
     * 
     * Uses exponential moving average to maintain a slowly adapting prediction
     * of the visual scene. This enables habituation and provides a baseline
     * for novelty detection. The prediction gradually approaches the actual
     * input over time.
     * 
     * @param vision Current vision input from world
     * @param predictedVision Output parameter that receives updated prediction
     * @param decayRate Decay rate for prediction update (higher = faster adaptation)
     */
    static void updatePrediction(const std::vector<float>& vision,
                                 std::vector<float>& predictedVision,
                                 float decayRate);
};

} // namespace nlm
