#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <unordered_map>
#include "Visualizer.hpp"

namespace nlm {

class Brain;
class Neuron;
class Synapse;
class SpikeEvent;

/**
 * Animation generation system for NLM
 * 
 * Provides comprehensive animation capabilities for:
 * - Neural network dynamics visualization
 * - Spike activity animations
 * * Learning progress visualization
 * - Performance metrics animation
 * - Time-series data animation
 * - Interactive animations
 * - Batch processing animations
 * - Export animations in multiple formats
 * - Real-time animation rendering
 * - Frame-based animation generation
 * - Interactive timeline control
 * 
 * Supported animation formats:
 * - GIF: Graphics Interchange Format
 * - MP4: MPEG-4 Video
 * - WebM: WebM Video
 * - HTML5: HTML5 Canvas animations
 * - JS: JavaScript animations
 * - Flash: Adobe Flash (deprecated)
 * - QuickTime: Apple QuickTime
 * - AVI: Audio Video Interleave
 * - MOV: Apple QuickTime Movie
 * - MKV: Matroska Video
 * - WEBM: WebM Video
 * 
 * Features:
 * - Frame-based animation generation
 * - Time interpolation and easing
 * - Multiple animation types
 * - Real-time rendering
 * - Progress tracking
 * - Error handling
 * - Export control
 * - Memory-efficient streaming
 * - Multi-format support
 * 
 * Integration with:
 * - All visualization systems
 * - Data processing pipeline
 * - Export systems
 * - Experiment management
 * 
 * @see Visualizer
 * @see InteractiveVisualizer
 * @see DataExporter
 * @see SpikeVisualizer
 * @see NetworkVisualizer
 */
class AnimationGenerator {
public:
    /**
     * Animation format enumeration
     */
    enum class AnimationFormat {
        GIF,
        MP4,
        WEBM,
        HTML5,
        JS,
        FLASH,
        QUICKTIME,
        AVI,
        MOV,
        MKV,
        WEBM_VP8,
        WEBM_VP9,
        AV1,
        H.264,
        H.265,
        VP8,
        VP9,
        CUSTOM
    };

    /**
     * Animation type enumeration
     */
    enum class AnimationType {
        NEURAL_DYNAMICS,      // Neural network dynamics
        SPIKE_ACTIVITY,       // Spike activity
        LEARNING_PROGRESS,    // Learning over time
        PERFORMANCE,          // Performance metrics
        TIME_SERIES,          // Generic time series
        COMPARISON,           // Multiple conditions
        SUBSET,               // Subset of data
        FULL_SIMULATION       // Complete simulation
    };

    /**
     * Animation frame information
     */
    struct AnimationFrame {
        int frameIndex;
        double timeStamp;
        std::vector<uint8_t> frameData; // Raw pixel data
        std::unordered_map<std::string, std::string> metadata;
        std::string format;              // Image format (PNG, JPEG, etc.)
        size_t sizeBytes;
        std::chrono::system_clock::time_point captured;
    };

    /**
     * Animation configuration
     */
    struct AnimationConfig {
        AnimationType type = AnimationType::NEURAL_DYNAMICS;
        AnimationFormat format = AnimationFormat::GIF;
        double frameRate = 30.0;              // Frames per second
        double duration = 60.0;              // Total duration (seconds)
        int width = 1920;                    // Frame width
        int height = 1080;                   // Frame height
        std::string quality = "medium";     // quality: low, medium, high, ultra
        bool loop = true;                    // Whether to loop
        int loops = 0;                       // Number of loops (0 = infinite)
        bool enableAudio = false;            // Audio track
        std::string audioFile;              // Audio file path
        double audioVolume = 1.0;            // Audio volume
        bool enableInteractive = false;     // Interactive controls
        double transitionTime = 0.5;        // Transition between frames
        std::string easingFunction = "linear"; // linear, ease-in, ease-out, ease-in-out
        bool enableProgressReporting = true;
        bool overwrite = false;
        std::string outputPath = ".";
        std::string filename = "animation";
        std::unordered_map<std::string, std::string> metadata;
        bool compress = true;
        double compressionLevel = 0.8;
    };

    /**
     * Animation statistics
     */
    struct AnimationStats {
        int totalFrames = 0;
        double totalDuration = 0.0;
        double fileSizeMB = 0.0;
        double averageFrameTime = 0.0;
        double maxFrameTime = 0.0;
        double minFrameTime = 0.0;
        double generationTime = 0.0;
        int fps = 0;
        bool completed = false;
        std::string formatUsed;
        std::vector<std::pair<int, double>> frameTimes; // frameIndex, timeTaken
    };

    /**
     * Animation callback function
     */
    using AnimationCallback = std::function<void(double progress, const std::string& status)>;

    /**
     * Frame renderer function
     */
    using FrameRenderer = std::function<std::vector<uint8_t>(int frameIndex, double time)>;

    /**
     * AnimationGenerator constructor
     */
    AnimationGenerator();
    
    /**
     * AnimationGenerator constructor with configuration
     */
    explicit AnimationGenerator(const AnimationConfig& config);
    
    /**
     * AnimationGenerator destructor
     */
    ~AnimationGenerator();

    /**
     * Configure animation generation
     */
    void configure(const AnimationConfig& config);
    
    /**
     * Initialize animation generator
     */
    bool initialize(const Brain& brain);
    
    /**
     * Generate animation
     */
    bool generate(const Brain& brain, const std::vector<SpikeEvent>& spikes, const AnimationCallback& callback = nullptr);
    
    /**
     * Add frame renderer
     */
    void addFrameRenderer(const FrameRenderer& renderer);
    
    /**
     * Set animation data source
     */
    void setDataSource(std::function<std::vector<std::vector<double>>(double time)> dataSource);
    
    /**
     * Get animation statistics
     */
    AnimationStats getStats() const;
    
    /**
     * Check if animation generation is complete
     */
    bool isComplete() const;
    
    /**
     * Get animation frames
     */
    std::vector<AnimationFrame> getFrames() const;
    
    /**
     * Export frames to files
     */
    bool exportFrames(const std::string& directory) const;
    
    /**
     * Compile animation
     */
    bool compileAnimation(const std::string& outputPath) const;
    
    /**
     * Stop animation generation
     */
    void stop();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Suppress warnings about unused private members
    AnimationGenerator(const AnimationGenerator&) = delete;
    AnimationGenerator& operator=(const AnimationGenerator&) = delete;
};

} // namespace nlm
