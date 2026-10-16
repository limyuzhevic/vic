// Enhanced STDP implementation with triplet STDP support
class AdvancedSTDP : public PlasticityRule {
public:
    AdvancedSTDP();
    ~AdvancedSTDP() override;
    
    // Basic STDP update (from parent class)
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override { return "AdvancedSTDP"; }
    
    // Advanced STDP parameters
    void enableTripletSTDP(bool enable) { tripletSTDPEnabled_ = enable; }
    bool isTripletSTDP() const { return tripletSTDPEnabled_; }
    
    void enableRewardModulatedSTDP(bool enable) { rewardModulatedSTDP_ = enable; }
    bool isRewardModulatedSTDP() const { return rewardModulatedSTDP_; }
    
    void setTripletWeights(float A1, float A2, float B1, float B2) {
        A1_ = A1; A2_ = A2; B1_ = B1; B2_ = B2;
    }
    
    void setRewardScale(float scale) { rewardScale_ = scale; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Triplet STDP parameters
    bool tripletSTDPEnabled_;
    bool rewardModulatedSTDP_;
    float A1_, A2_, B1_, B2_;
    float rewardScale_;
};