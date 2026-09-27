#ifndef NLM_ENHANCED_VISION_PLUGIN_H
#define NLM_ENHANCED_VISION_PLUGIN_H

#include "ISensorPlugin.h"
#include <vector>
#include <string>
#include <mutex>
#include <random>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

/**
 * @class EnhancedVisionPlugin
 * @brief High-resolution vision sensor with spectral sensitivity and advanced image processing
 * 
 * The EnhancedVisionPlugin provides advanced vision capabilities for NLM agents,
 * including high-resolution imaging, spectral sensitivity, and real-time image processing.
 * 
 * This plugin demonstrates the extensibility of the NLM plugin system by providing
 * features that go beyond the standard vision capabilities.
 * 
 * Key Features:
 * - High-resolution imaging (configurable)
 * - Spectral sensitivity across multiple bands
 * - Advanced image processing (contrast enhancement, noise reduction)
 * - Real-time updates and motion blur simulation
 * - Auto exposure and gain control
 * - Heatmap generation
 * - Configurable field of view
 * 
 * Usage:
 * @code
 * #include "EnhancedVisionPlugin.h"
 * 
 * // Create plugin instance
 * auto plugin = std::make_shared<EnhancedVisionPlugin>();
 * 
 * // Configure plugin
 * Config config;
 * config.set("vision.resolution", 512);
 * config.set("vision.spectral_bands", 3);
 * 
 * // Initialize
 * plugin->initialize(config);
 * plugin->setActive(true);
 * 
 * // Read data
 * ISensorPlugin::SensorData data = plugin->read();
 * @endcode
 * 
 * Configuration Options:
 * - vision.resolution: Image resolution (square)
 * - vision.spectral_bands: Number of spectral bands (default: 3)
 * - vision.field_of_view: Field of view in degrees (default: 60.0)
 * - vision.color_space: Color space (RGB, HSV, etc.)
 * - vision.enhance_contrast: Enable contrast enhancement (default: true)
 * - vision.noise_reduction: Enable noise reduction (default: true)
 * - vision.adaptive_thresholding: Enable adaptive thresholding (default: true)
 * - vision.enable_infrared: Enable infrared vision (default: false)
 * - vision.enable_depth: Enable depth sensing (default: false)
 * - vision.enable_heatmap: Enable heatmap generation (default: false)
 * - vision.kernel_size: Convolution kernel size (default: 5)
 * - vision.learning_rate: Learning rate for vision processing (default: 0.01)
 * - vision.auto_exposure: Enable auto exposure (default: true)
 * - vision.gain_control: Enable gain control (default: true)
 */
class EnhancedVisionPlugin : public ISensorPlugin {
private:
    // Plugin configuration
    size_t width_;                    //!< Image width
    size_t height_;                   //!< Image height
    int spectralBands_;              //!< Number of spectral bands
    double fieldOfView_;             //!< Field of view in degrees
    std::string colorSpace_;         //!< Color space (RGB, HSV, etc.)
    
    // Processing flags
    bool contrastEnhancement_;       //!< Enable contrast enhancement
    bool noiseReduction_;            //!< Enable noise reduction
    bool adaptiveThresholding_;      //!< Enable adaptive thresholding
    bool enableInfrared_;            //!< Enable infrared vision
    bool enableDepth_;               //!< Enable depth sensing
    bool enableHeatmap_;             //!< Enable heatmap generation
    
    // Processing parameters
    int kernelSize_;                  //!< Convolution kernel size
    double learningRate_;             //!< Learning rate for vision processing
    bool autoExposure_;              //!< Enable auto exposure
    bool gainControl_;               //!< Enable gain control
    
    // State
    bool isActive_;                  //!< Whether plugin is active
    bool initialized_;               //!< Whether plugin is initialized
    
    // Vision data
    std::vector<float> visionData_;  //!< Current vision data
    
    // Random number generator
    std::mt19937 rng_;                //!< Random number generator
    
    // Mutex for thread safety
    mutable std::mutex dataMutex_;   //!< Mutex for accessing vision data
    
public:
    /**
     * @brief Constructor
     */
    EnhancedVisionPlugin();
    
    /**
     * @brief Destructor
     */
    ~EnhancedVisionPlugin() override;
    
    // IPlugin interface implementation
    std::string getName() const override;
    std::string getVersion() const override;
    std::string getDescription() const override;
    bool initialize(const Config& config) override;
    void shutdown() override;
    bool isEnabled() const override;
    void setEnabled(bool enabled) override;
    
    // ISensorPlugin interface implementation
    SensorData read() override;
    size_t getExpectedSize() const override;
    std::string getSensorType() const override;
    bool isActive() const override;
    void setActive(bool active) override;
    
    // Enhanced Vision Plugin specific methods
    
    /**
     * @brief Set image resolution
     * 
     * @param width Image width (height will be same for square aspect ratio)
     * @param height Image height
     */
    void setResolution(size_t width, size_t height);
    
    /**
     * @brief Get the learning rate
     * 
     * @return Current learning rate
     */
    double getLearningRate() const;
    
    /**
     * @brief Set the learning rate
     * 
     * @param rate New learning rate
     */
    void setLearningRate(double rate);
    
    /**
     * @brief Apply a convolution filter to the vision data
     * 
     * @param kernel Convolution kernel (size kernelSize_ × kernelSize_)
     */
    void applyFilter(const std::vector<float>& kernel);
    
    /**
     * @brief Get current vision data (for testing/debugging)
     * 
     * @return Reference to internal vision data
     */
    const std::vector<float>& getVisionData() const {
        return visionData_;
    }
    
    /**
     * @brief Get current resolution
     * 
     * @return Pair of (width, height)
     */
    std::pair<size_t, size_t> getResolution() const {
        return {width_, height_};
    }
    
    /**
     * @brief Check if plugin is initialized
     * 
     * @return true if plugin is initialized
     */
    bool isInitialized() const {
        return initialized_;
    }
private:
    // Private helper methods
    
    /**
     * @brief Generate test patterns for initial data
     */
    void generateTestPattern();
    
    /**
     * @brief Generate more realistic test patterns
     */
    void generateRealisticTestPattern();
    
    /**
     * @brief Generate brightness variation pattern
     * 
     * @param x X coordinate
     * @param y Y coordinate
     * @return Brightness value (0.0 to 1.0)
     */
    double generateBrightnessPattern(size_t x, size_t y);
    
    /**
     * @brief Generate texture pattern
     * 
     * @param x X coordinate
     * @param y Y coordinate
     * @return Texture value (0.0 to 1.0)
     */
    double generateTexturePattern(size_t x, size_t y);
    
    /**
     * @brief Generate edge pattern
     * 
     * @param x X coordinate
     * @param y Y coordinate
     * @return Edge value (0.0 to 1.0)
     */
    double generateEdgePattern(size_t x, size_t y);
    
    /**
     * @brief Generate spectral pattern
     * 
     * @param x X coordinate
     * @param y Y coordinate
     * @param band Spectral band index
     * @return Spectral value (0.0 to 1.0)
     */
    double generateSpectralPattern(size_t x, size_t y, int band);
    
    /**
     * @brief Generate noise pattern
     * 
     * @param x X coordinate
     * @param y Y coordinate
     * @param frequency Noise frequency
     * @return Noise value (0.0 to 1.0)
     */
    double generateNoisePattern(size_t x, size_t y, double frequency);
    
    /**
     * @brief Generate structure pattern
     * 
     * @param x X coordinate
     * @param y Y coordinate
     * @return Structure value (0.0 to 1.0)
     */
    double generateStructurePattern(size_t x, size_t y);
    
    /**
     * @brief Generate Gaussian noise
     * 
     * @param mean Mean value
     * @param stddev Standard deviation
     * @return Gaussian noise value
     */
    double generateGaussianNoise(double mean, double stddev);
    
    /**
     * @brief Enhance contrast
     * 
     * @param value Input value
     * @return Enhanced value
     */
    static double enhanceContrast(double value);
    
    /**
     * @brief Apply contrast enhancement to all data
     */
    void enhanceContrast();
    
    /**
     * @brief Apply noise reduction
     */
    void reduceNoise();
    
    /**
     * @brief Apply adaptive thresholding
     */
    void applyAdaptiveThreshold();
    
    /**
     * @brief Update vision data (simulate real-time updates)
     */
    void updateVisionData();
    
    /**
     * @brief Generate motion blurred value
     * 
     * @param x X coordinate
     * @param y Y coordinate
     * @return Motion blurred value
     */
    double generateMotionBlurredValue(size_t x, size_t y);
    
    /**
     * @brief Update cell value with temporal evolution
     * 
     * @param x X coordinate
     * @param y Y coordinate
     * @return Updated value
     */
    double updateCellValue(size_t x, size_t y);
    
    /**
     * @brief Apply auto exposure
     */
    void applyAutoExposure();
    
    /**
     * @brief Apply gain control
     */
    void applyGainControl();
    
    /**
     * @brief Calculate mean value of vision data
     * 
     * @return Mean value
     */
    double calculateMean() const;
    
    /**
     * @brief Generate heatmap overlay
     */
    void generateHeatmap();
    
    /**
     * @brief Apply convolution to data
     * 
     * @param data Data to convolve
     * @param kernel Convolution kernel
     */
    void applyConvolution(std::vector<float>& data, const std::vector<float>& kernel);
    
    /**
     * @brief Get current timestamp
     * 
     * @return Current timestamp as seconds since epoch
     */
    static double getCurrentTimestamp();
};

} // namespace nlm

#endif // NLM_ENHANCED_VISION_PLUGIN_H