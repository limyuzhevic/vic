namespace nlm {

// Sensory data packet - what the agent perceives
// Contains only low-level sensory signals, NO semantic labels
class SensoryPercept {
public:
    /**
     * @brief Default constructor
     * Creates empty sensory data with default dimensions:
     * - Vision: 16x16 grayscale grid (256 values)
     * - Touch: 8 proximity sensors (binary collision)
     * - Internal: 4 homeostatic signals
     * - Proprioception: 6 body position/velocity signals
     * - Audio: 0 signals (disabled by default)
     */
    SensoryPercept();
    
    /**
     * @brief Default destructor
     */
    ~SensoryPercept() = default;
    
    // Vision: simple grayscale intensity grid (no RGB, no object labels)
    // Format: row-major, values 0.0-1.0 representing light intensity
    /**
     * @brief Get vision data as a reference to the internal vector
     * @return Reference to vision data vector (16x16 = 256 values)
     */
    const std::vector<float>& getVision() const { return vision_; }
    
    /**
     * @brief Set vision data
     * @param v New vision data (must be 256 values for 16x16 grid)
     */
    void setVision(const std::vector<float>& v) { vision_ = v; }
    
    /**
     * @brief Get vision grid width
     * @return Vision width in pixels (always 16 for standard vision)
     */
    size_t getVisionWidth() const { return visionWidth_; }
    
    /**
     * @brief Get vision grid height
     * @return Vision height in pixels (always 16 for standard vision)
     */
    size_t getVisionHeight() const { return visionHeight_; }
    
    // Touch/proximity sensors (binary collision signals)
    /**
     * @brief Get touch/proximity sensor data
     * @return Reference to touch sensor vector (8 values, binary collision)
     */
    const std::vector<float>& getTouch() const { return touch_; }
    
    /**
     * @brief Set touch sensor data
     * @param t New touch data (must be 8 values)
     */
    void setTouch(const std::vector<float>& t) { touch_ = t; }
    
    // Internal state signals (homeostatic signals)
    /**
     * @brief Get internal homeostatic signal data
     * @return Reference to internal signal vector (4 values)
     */
    const std::vector<float>& getInternal() const { return internal_; }
    
    /**
     * @brief Set internal homeostatic signal data
     * @param i New internal signal data (must be 4 values)
     */
    void setInternal(const std::vector<float>& i) { internal_ = i; }
    
    // Proprioception: body position/velocity signals
    /**
     * @brief Get proprioception (body position/velocity) data
     * @return Reference to proprioception vector (6 values)
     */
    const std::vector<float>& getProprioception() const { return proprioception_; }
    
    /**
     * @brief Set proprioception data
     * @param p New proprioception data (must be 6 values)
     */
    void setProprioception(const std::vector<float>& p) { proprioception_ = p; }
    
    // Audio-like signals (if enabled)
    /**
     * @brief Get audio signal data (if audio processing enabled)
     * @return Reference to audio signal vector (typically 0 values)
     */
    const std::vector<float>& getAudio() const { return audio_; }
    
    /**
     * @brief Set audio signal data
     * @param a New audio signal data
     */
    void setAudio(const std::vector<float>& a) { audio_ = a; }
    
    // Get all signals concatenated (for brain input)
    /**
     * @brief Get all sensory signals concatenated into a single vector
     * @return Vector containing all signals: [vision][touch][internal][proprioception][audio]
     */
    std::vector<float> getAllSignals() const;
    
    // Timestamp
    /**
     * @brief Get the timestamp of this sensory percept
     * @return Simulation timestamp when this percept was created
     */
    double getTimestamp() const { return timestamp_; }
    
    /**
     * @brief Set the timestamp of this sensory percept
     * @param t New timestamp
     */
    void setTimestamp(double t) { timestamp_ = t; }
    
private:
    std::vector<float> vision_;
    size_t visionWidth_;
    size_t visionHeight_;
    std::vector<float> touch_;
    std::vector<float> internal_;
    std::vector<float> proprioception_;
    std::vector<float> audio_;
    double timestamp_;
};

} // namespace nlm
