namespace nlm {

SensoryPercept::SensoryPercept()
    : visionWidth_(16)
    , visionHeight_(16)
    , timestamp_(0.0)
{
    // Initialize all vectors with proper capacity
    vision_.reserve(16 * 16);  // 256 elements for vision
    touch_.reserve(8);
    internal_.reserve(4);
    proprioception_.reserve(6);
    audio_.reserve(0);
    
    // Initialize with zeros using reserve+resize (more efficient than direct resize)
    vision_.resize(16 * 16, 0.0f);
    touch_.resize(8, 0.0f);
    internal_.resize(4, 0.0f);
    proprioception_.resize(6, 0.0f);
    // Audio remains empty as it's variable-sized
}

std::vector<float> SensoryPercept::getAllSignals() const {
    std::vector<float> all;
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

void SensoryPercept::clearAll() {
    // Clear all vectors and reset to default state
    vision_.assign(vision_.size(), 0.0f);
    touch_.assign(touch_.size(), 0.0f);
    internal_.assign(internal_.size(), 0.0f);
    proprioception_.assign(proprioception_.size(), 0.0f);
    audio_.clear();
    timestamp_ = 0.0;
}

float SensoryPercept::getTotalSignalCount() const {
    return static_cast<float>(vision_.size() + touch_.size() + internal_.size() +
                              proprioception_.size() + audio_.size());
}

float SensoryPercept::getMaxSignalValue() const {
    float max = 0.0f;
    for (float val : vision_) max = std::max(max, std::abs(val));
    for (float val : touch_) max = std::max(max, std::abs(val));
    for (float val : internal_) max = std::max(max, std::abs(val));
    for (float val : proprioception_) max = std::max(max, std::abs(val));
    for (float val : audio_) max = std::max(max, std::abs(val));
    return max;
}

} // namespace nlm
