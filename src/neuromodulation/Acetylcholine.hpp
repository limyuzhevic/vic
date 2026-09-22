class Brain;

namespace nlm {

// Acetylcholine implementation with full attention modulation and memory encoding
class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Attention modulation
    void enhanceAttention(float focusLevel, Brain* brain);
    
    // Memory encoding
    void enhanceEncoding(float novelty, Brain* brain);
    
    // Signal processing
    void signalNovelty(float novelty);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
