#pragma once

namespace nlm {

/**
 * SensoryPercept - Represents sensory input to the brain
 * 
 * A percept contains all the information the brain receives from the environment,
 * processed into different modalities (vision, touch, internal signals, proprioception).
 */
class SensoryPercept {
public:
    SensoryPercept() = default;
    ~SensoryPercept() = default;
    
    /**
     * Get vision data (256 values - 16x16 grid)
     * 
     * Returns normalized intensity values where:
     * 0.0 = empty/far away
     * 1.0 = bright/close
     * 
     * @return Vector of vision intensities
     */
    const std::vector<float>& getVision() const { return vision_; }
    
    /**
     * Set vision data
     * 
     * @param vision Vision intensity vector
     */
    void setVision(const std::vector<float>& vision) { vision_ = vision; }
    
    /**
     * Get touch sensor data (8 directions)
     * 
     * Returns proximity values where:
     * 0.0 = far from obstacle
     * 1.0 = close to obstacle
     * 
     * @return Vector of touch proximity values
     */
    const std::vector<float>& getTouch() const { return touch_; }
    
    /**
     * Set touch data
     * 
     * @param touch Touch proximity vector
     */
    void setTouch(const std::vector<float>& touch) { touch_ = touch; }
    
    /**
     * Get internal state signals (energy, health, movement)
     * 
     * @return Vector of internal signal values
     */
    const std::vector<float>& getInternal() const { return internal_; }
    
    /**
     * Set internal signals
     * 
     * @param internal Internal signal vector
     */
    void setInternal(const std::vector<float>& internal) { internal_ = internal; }
    
    /**
     * Get proprioception (body position, velocity, orientation)
     * 
     * Returns normalized values where:
     * 0.0 = left/bottom/min orientation
     * 1.0 = right/top/max orientation
     * 
     * @return Vector of proprioceptive values
     */
    const std::vector<float>& getProprioception() const { return proprioception_; }
    
    /**
     * Set proprioception data
     * 
     * @param proprioception Proprioception vector
     */
    void setProprioception(const std::vector<float>& proprioception) { 
        proprioception_ = proprioception; }
    
    /**
     * Set timestamp for when this percept was generated
     * 
     * @param timestamp Simulation timestamp
     */
    void setTimestamp(double timestamp) { timestamp_ = timestamp; }
    
    /**
     * Get timestamp of percept generation
     * 
     * @return Simulation timestamp
     */
    double getTimestamp() const { return timestamp_; }
    
    /**
     * Get size of vision data
     * 
     * @return Number of vision values (typically 256)
     */
    size_t getVisionSize() const { return vision_.size(); }
    
    /**
     * Get size of touch data
     * 
     * @return Number of touch values (typically 8)
     */
    size_t getTouchSize() const { return touch_.size(); }
    
    /**
     * Get size of internal signals
     * 
     * @return Number of internal values (typically 4)
     */
    size_t getInternalSize() const { return internal_.size(); }
    
    /**
     * Get size of proprioception data
     * 
     * @return Number of proprioception values (typically 6)
     */
    size_t getProprioceptionSize() const { return proprioception_.size(); }
    
    /**
     * Check if percept is valid (contains reasonable data)
     * 
     * @return True if percept has valid data
     */
    bool isValid() const {
        return !vision_.empty() && !touch_.empty() && 
               !internal_.empty() && !proprioception_.empty();
    }
    
    /**
     * Clear all sensory data
     */
    void clear() {
        vision_.clear();
        touch_.clear();
        internal_.clear();
        proprioception_.clear();
        timestamp_ = 0.0;
    }
    
    /**
     * Merge two percepts (element-wise combination)
     * 
     * @param other Other percept to merge with
     * @return New percept with merged data
     */
    SensoryPercept merge(const SensoryPercept& other) const {
        SensoryPercept result;
        
        // Merge vision (weighted average)
        const size_t minVision = std::min(vision_.size(), other.vision_.size());
        result.vision_.reserve(minVision);
        for (size_t i = 0; i < minVision; ++i) {
            result.vision_.push_back((vision_[i] + other.vision_[i]) * 0.5f);
        }
        
        // Merge touch (take max)
        const size_t minTouch = std::min(touch_.size(), other.touch_.size());
        result.touch_.reserve(minTouch);
        for (size_t i = 0; i < minTouch; ++i) {
            result.touch_.push_back(std::max(touch_[i], other.touch_[i]));
        }
        
        // Merge internal (weighted average)
        const size_t minInternal = std::min(internal_.size(), other.internal_.size());
        result.internal_.reserve(minInternal);
        for (size_t i = 0; i < minInternal; ++i) {
            result.internal_.push_back((internal_[i] + other.internal_[i]) * 0.5f);
        }
        
        // Merge proprioception (take max for position, average for orientation)
        const size_t minProprio = std::min(proprioception_.size(), other.proprioception_.size());
        result.proprioception_.reserve(minProprio);
        for (size_t i = 0; i < minProprio; ++i) {
            if (i >= 2) { // Position indices
                result.proprioception_.push_back(std::max(proprioception_[i], other.proprioception_[i]));
            } else { // Orientation/speed indices
                result.proprioception_.push_back((proprioception_[i] + other.proprioception_[i]) * 0.5f);
            }
        }
        
        return result;
    }
    
    /**
     * Apply weighting to percept (for attention/filtering)
     * 
     * @param visionWeight Weight for vision data
     * @param touchWeight Weight for touch data
     * @param internalWeight Weight for internal signals
     * @param proprioceptionWeight Weight for proprioception
     * @return Weighted percept
     */
    SensoryPercept applyWeight(float visionWeight, float touchWeight, 
                               float internalWeight, float proprioceptionWeight) const {
        SensoryPercept result;
        
        result.vision_ = vision_;
        for (float& v : result.vision_) v *= visionWeight;
        
        result.touch_ = touch_;
        for (float& t : result.touch_) t *= touchWeight;
        
        result.internal_ = internal_;
        for (float& i : result.internal_) i *= internalWeight;
        
        result.proprioception_ = proprioception_;
        for (float& p : result.proprioception_) p *= proprioceptionWeight;
        
        return result;
    }
    
private:
    std::vector<float> vision_;           // Vision data (256 values)
    std::vector<float> touch_;            // Touch data (8 values)
    std::vector<float> internal_;         // Internal signals (4 values)
    std::vector<float> proprioception_;   // Proprioception (6 values)
    double timestamp_;                    // Time when percept was generated
};

} // namespace nlm
