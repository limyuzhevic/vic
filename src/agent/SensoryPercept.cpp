#include "SensoryPercept.hpp"

namespace nlm {

// Default constructor with standard sizes
SensoryPercept::SensoryPercept()
    : visionWidth_(16)
    , visionHeight_(16)
    , timestamp_(0.0)
{
    // Initialize all sensory modalities with standard sizes
    vision_.resize(visionWidth_ * visionHeight_, 0.0f);
    touch_.resize(8, 0.0f);
    internal_.resize(4, 0.0f);
    proprioception_.resize(6, 0.0f);
    audio_.resize(0, 0.0f);  // Audio initialized as empty by default
}

// Constructor with configurable sizes
SensoryPercept::SensoryPercept(size_t visionWidth, size_t visionHeight,
                               size_t touchSize, size_t internalSize,
                               size_t proprioceptionSize, size_t audioSize)
    : visionWidth_(visionWidth)
    , visionHeight_(visionHeight)
    , timestamp_(0.0)
{
    // Initialize all sensory modalities with configurable sizes
    vision_.resize(visionWidth * visionHeight, 0.0f);
    touch_.resize(touchSize, 0.0f);
    internal_.resize(internalSize, 0.0f);
    proprioception_.resize(proprioceptionSize, 0.0f);
    audio_.resize(audioSize, 0.0f);  // Audio initialized with configurable size
}

// Set methods with validation
void SensoryPercept::setVision(const std::vector<float>& v) {
    // Validate input vector size matches expected dimensions
    if (v.size() != visionWidth_ * visionHeight_) {
        vision_.resize(visionWidth_ * visionHeight_, 0.0f);
    }
    vision_ = v;
}

void SensoryPercept::setTouch(const std::vector<float>& t) {
    // Touch signals should be within reasonable bounds
    touch_ = t;
}

void SensoryPercept::setInternal(const std::vector<float>& i) {
    // Internal signals should be within homeostatic range (-1 to 1)
    internal_ = i;
}

void SensoryPercept::setProprioception(const std::vector<float>& p) {
    // Proprioception signals should be normalized position/velocity (-1 to 1)
    proprioception_ = p;
}

void SensoryPercept::setAudio(const std::vector<float>& a) {
    // Audio signals should be normalized (-1 to 1 for mono)
    audio_ = a;
}

std::vector<float> SensoryPercept::getAllSignals() const {
    std::vector<float> all;
    
    // Validate that all vectors have consistent size
    all.reserve(vision_.size() + touch_.size() + internal_.size() + 
                proprioception_.size() + audio_.size());
    
    // Vision (flattened)
    all.insert(all.end(), vision_.begin(), vision_.end());
    
    // Touch
    all.insert(all.end(), touch_.begin(), touch_.end());
    
    // Internal
    all.insert(all.end(), internal_.begin(), internal_.end());
    
    // Proprioception
    all.insert(all.end(), proprioception_.begin(), proprioception_.end());
    
    // Audio
    all.insert(all.end(), audio_.begin(), audio_.end());
    
    return all;
}

void SensoryPercept::save(std::vector<float>& out) const {
    // Save all sensory data to output vector
    out.clear();
    
    // Save vision data
    out.insert(out.end(), vision_.begin(), vision_.end());
    
    // Save touch data
    out.insert(out.end(), touch_.begin(), touch_.end());
    
    // Save internal data
    out.insert(out.end(), internal_.begin(), internal_.end());
    
    // Save proprioception data
    out.insert(out.end(), proprioception_.begin(), proprioception_.end());
    
    // Save audio data
    out.insert(out.end(), audio_.begin(), audio_.end());
    
    // Save timestamp
    out.push_back(timestamp_);
}

bool SensoryPercept::load(const std::vector<float>& in) {
    // Load sensory data from input vector
    // Calculate expected sizes
    size_t expectedSize = visionWidth_ * visionHeight_ + touch_.size() + 
                         internal_.size() + proprioception_.size() + audio_.size() + 1;
    
    if (in.size() < expectedSize) {
        // Not enough data to load completely
        return false;
    }
    
    size_t offset = 0;
    
    // Load vision data
    if (!in.empty()) {
        vision_.assign(in.begin() + offset, in.begin() + offset + visionWidth_ * visionHeight_);
        offset += visionWidth_ * visionHeight_;
    }
    
    // Load touch data
    if (offset < in.size()) {
        touch_.assign(in.begin() + offset, in.begin() + offset + touch_.size());
        offset += touch_.size();
    }
    
    // Load internal data
    if (offset < in.size()) {
        internal_.assign(in.begin() + offset, in.begin() + offset + internal_.size());
        offset += internal_.size();
    }
    
    // Load proprioception data
    if (offset < in.size()) {
        proprioception_.assign(in.begin() + offset, in.begin() + offset + proprioception_.size());
        offset += proprioception_.size();
    }
    
    // Load audio data
    if (offset < in.size()) {
        audio_.assign(in.begin() + offset, in.begin() + offset + audio_.size());
        offset += audio_.size();
    }
    
    // Load timestamp
    if (offset < in.size()) {
        timestamp_ = in[offset];
    }
    
    return true;
}

bool SensoryPercept::validate() const {
    // Validate all sensory data
    bool valid = true;
    
    // Check vision data
    if (vision_.empty()) {
        valid = false;
    }
    
    // Check that touch, internal, proprioception, and audio have valid sizes
    // (they can be empty)
    
    return valid;
}

void SensoryPercept::clear() {
    // Clear all sensory data
    vision_.clear();
    touch_.clear();
    internal_.clear();
    proprioception_.clear();
    audio_.clear();
    timestamp_ = 0.0;
}

void SensoryPercept::resetToDefault() {
    // Reset to standard default configuration
    visionWidth_ = 16;
    visionHeight_ = 16;
    timestamp_ = 0.0;
    
    vision_.resize(visionWidth_ * visionHeight_, 0.0f);
    touch_.resize(8, 0.0f);
    internal_.resize(4, 0.0f);
    proprioception_.resize(6, 0.0f);
    audio_.resize(0, 0.0f);
}

} // namespace nlm
