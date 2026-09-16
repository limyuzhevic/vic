#ifndef NLM_CNNBRAIN_HPP
#define NLM_CNNBRAIN_HPP

#include "../brain/Brain.hpp"
#include "../sensory/Vision.hpp"
#include "../core/Config/Constants.hpp"
#include <memory>
#include <vector>
#include <array>

namespace nlm {

// Forward declarations
class Config;

// Convolutional Layer structure
struct ConvolutionalLayer {
    std::vector<std::vector<float>> kernel;  // Small 3x3 kernel
    float bias;
    std::vector<std::vector<float>> activation;  // ReLU activation
    
    ConvolutionalLayer() : bias(0.0f) {
        kernel = std::vector<std::vector<float>>(3, std::vector<float>(3, 0.0f));
        activation = std::vector<std::vector<float>>(64, std::vector<float>(64, 0.0f));
    }
    
    ConvolutionalLayer(const std::vector<std::vector<std::vector<float>>>& kernels,
                      const std::vector<float>& biases)
        : bias(biases[0]) {
        kernel = kernels[0];
        activation = std::vector<std::vector<float>>(64, std::vector<float>(64, 0.0f));
    }
    
    void applyConvolution(const std::vector<std::vector<float>>& input);
    void applyReLU(float threshold = 0.0f);
};

// Pooling Layer structure
struct PoolingLayer {
    std::vector<std::vector<float>> output;
    
    PoolingLayer() {
        output = std::vector<std::vector<float>>(32, std::vector<float>(32, 0.0f));
    }
    
    void applyMaxPooling(const std::vector<std::vector<float>>& input);
    void applyAveragePooling(const std::vector<std::vector<float>>& input);
};

// CNNBrain: Implements convolutional neural network inspired processing
// Extends the base Brain class with visual feature extraction capabilities
class CNNBrain : public Brain {
public:
    /**
     * @brief Constructor - creates a CNN-inspired brain with visual processing
     * 
     * @param config Shared pointer to the brain configuration
     * @param receptiveFieldSize Size of the convolutional receptive field
     * @note Implements biologically plausible convolutional processing
     *       inspired by mammalian visual cortex architecture
     */
    CNNBrain(std::shared_ptr<Config> config, size_t receptiveFieldSize = 5);
    
    /**
     * @brief Destructor - cleans up CNN-specific resources
     */
    ~CNNBrain() override;
    
    /**
     * @brief Process visual input through convolutional layers
     * 
     * @param vision Vision input from the environment
     * @note Extracts visual features using convolutional filters
     *       mimicking early visual processing in the brain
     */
    void processVisualInput(const Vision& vision);
    
    /**
     * @brief Get the current feature map from convolutional processing
     * 
     * @return Feature map representing high-level visual features
     * @note These features are used for object recognition and scene understanding
     */
    const std::vector<std::vector<float>>& getFeatureMap() const { return featureMap_; }
    
    /**
     * @brief Get the current activation from the final convolutional layer
     * 
     * @return Activation map of the last processing layer
     */
    const std::vector<std::vector<float>>& getActivationMap() const { return activationMap_; }
    
    /**
     * @brief Get the number of feature maps (depth)
     * 
     * @return Number of feature maps extracted by the convolutional network
     */
    size_t getFeatureMapDepth() const { return featureMap_.size(); }
    
    /**
     * @brief Get the spatial dimensions of feature maps
     * 
     * @return Spatial dimension (assuming square feature maps)
     */
    size_t getFeatureMapSize() const { return featureMap_.empty() ? 0 : featureMap_[0].size(); }
    
    /**
     * @brief Set the receptive field size for convolutional processing
     * 
     * @param size New receptive field size (in pixels)
     * @note Changes the scale of visual feature extraction
     */
    void setReceptiveFieldSize(size_t size) { receptiveFieldSize_ = size; }
    
    /**
     * @brief Get the current receptive field size
     * 
     * @return Current receptive field size
     */
    size_t getReceptiveFieldSize() const { return receptiveFieldSize_; }
    
    /**
     * @brief Configure the number of convolutional layers
     * 
     * @param layers Number of convolutional layers to create
     * @note More layers increase feature abstraction but require more computation
     */
    void setConvolutionalLayersCount(size_t layers) { convolutionalLayersCount_ = layers; }
    
    /**
     * @brief Get the number of convolutional layers
     * 
     * @return Current number of convolutional layers
     */
    size_t getConvolutionalLayersCount() const { return convolutionalLayersCount_; }
    
    /**
     * @brief Get the current learning rate for convolutional weights
     * 
     * @return Learning rate for synaptic weight updates in convolutional layers
     */
    float getConvolutionalLearningRate() const { return convolutionalLearningRate_; }
    
    /**
     * @brief Set the learning rate for convolutional weight updates
     * 
     * @param rate New learning rate for convolutional weight updates
     */
    void setConvolutionalLearningRate(float rate) { convolutionalLearningRate_ = rate; }
    
    /**
     * @brief Get the number of learned visual features
     * 
     * @return Number of distinct visual features (kernels) learned
     */
    size_t getLearnedFeaturesCount() const { return learnedFeaturesCount_; }
    
    /**
     * @brief Apply Hebbian learning to convolutional weights based on visual experience
     * 
     * @param visualInput Current visual input from environment
     * @note Implements Hebbian learning for visual feature extraction
     */
    void learnVisualFeatures(const Vision& visualInput);
    
    /**
     * @brief Reset the convolutional processing pipeline
     * 
     * @note Clears all feature maps and resets receptive field processing
     */
    void reset() override;
    
    /**
     * @brief Get a summary of the current CNN processing capabilities
     * 
     * @return String description of CNN brain characteristics
     */
    std::string getCNNDescription() const;
    
    /**
     * @brief Enable or disable sparse connectivity in convolutional layers
     * 
     * @param enable Whether to use sparse connectivity for efficiency
     */
    void enableSparseConnectivity(bool enable) { sparseConnectivityEnabled_ = enable; }
    
    /**
     * @brief Check if sparse connectivity is enabled
     * 
     * @return True if sparse connectivity is active
     */
    bool isSparseConnectivityEnabled() const { return sparseConnectivityEnabled_; }
    
    /**
     * @brief Get the current neural activity from feature processing
     * 
     * @return Vector of activation values across all feature maps
     */
    std::vector<float> getNeuralActivityFromFeatures() const;
    
    /**
     * @brief Apply lateral inhibition between feature maps
     * 
     * @param strength Inhibition strength between feature maps
     */
    void applyLateralInhibition(float strength);
    
private:
    /**
     * @brief Initialize the convolutional processing pipeline
     * 
     * @param config Brain configuration parameters
     */
    void initializeConvolutionalPipeline(const std::shared_ptr<Config>& config);
    
    /**
     * @brief Create convolutional layers based on configuration
     */
    void createConvolutionalLayers();
    
    /**
     * @brief Process input through all convolutional layers sequentially
     */
    void processThroughAllLayers();
    
    /**
     * @brief Extract spatial features using convolution operation
     */
    void extractSpatialFeatures();
    
    /**
     * @brief Apply pooling to reduce spatial resolution and increase invariance
     */
    void applySpatialPooling();
    
    /**
     * @brief Update learned weights based on visual experience (Hebbian learning)
     */
    void updateConvolutionalWeights(const Vision& visualInput);
    
    /**
     * @brief Generate biologically plausible receptive fields
     */
    void generateReceptiveFields();
    
    /**
     * @brief Apply non-linear activation functions to convolutional outputs
     */
    void applyNonlinearities();
    
    /**
     * @brief Integrate features with higher-level cognitive processing
     */
    void integrateWithCognition();
    
    // CNN-specific data structures
    std::vector<ConvolutionalLayer> convolutionalLayers_;
    std::vector<PoolingLayer> poolingLayers_;
    
    // Feature maps (output of each layer)
    std::vector<std::vector<float>> featureMap_;
    std::vector<std::vector<float>> activationMap_;
    std::vector<std::vector<std::vector<float>>> multiScaleFeatures_;
    
    // Configuration parameters
    size_t receptiveFieldSize_;
    size_t convolutionalLayersCount_;
    float convolutionalLearningRate_;
    size_t learnedFeaturesCount_;
    
    // State management
    bool sparseConnectivityEnabled_;
    bool receptiveFieldInitialized_;
    size_t featureExtractionStep_;
    
    // Receptive field templates (biologically inspired)
    std::vector<std::vector<float>> centerSurroundReceptiveField_;
    std::vector<std::vector<float>> edgeDetectionReceptiveField_;
    std::vector<std::vector<float>> motionDetectionReceptiveField_;
    
    // Hebbian learning for feature maps
    std::vector<std::vector<float>> hebbianWeights_;
    std::vector<float> featurePotentiation_;
    std::vector<float> featureDepression_;
    
    // Energy-based competition between feature maps
    std::vector<float> featureEnergies_;
    float globalInhibitionStrength_;
    
    // Temporal processing for dynamic visual input
    std::vector<std::vector<float>> previousFeatureMap_;
    double temporalFilterAlpha_;
    bool enableTemporalFiltering_;
    
    // Multi-scale processing for different visual scales
    std::vector<std::vector<std::vector<float>>> pyramidFeatures_;
    size_t pyramidLevels_;
    
    // Feedback connections from higher to lower layers (predictive coding)
    std::vector<std::vector<float>> feedbackPredictions_;
    std::vector<std::vector<float>> predictionErrors_;
    bool enablePredictiveCoding_;
};

} // namespace nlm

#endif // NLM_CNNBRAIN_HPP
