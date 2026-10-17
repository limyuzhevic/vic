// Anti-Hebbian rule: decrease weight when neurons fire together
// Enhanced with better parameter handling and validation
class AntiHebbianRule : public PlasticityRule {
public:
    AntiHebbianRule();
    ~AntiHebbianRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    const char* getDescription() const override;
    const char* getVersion() const override;
    
    // Parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    void setMinWeight(float minWeight);
    float getMinWeight() const;
    
    // Enhanced parameter handling
    std::unordered_map<std::string, std::string> getParameters() const override;
    void setParameters(const std::unordered_map<std::string, std::string>& params) override;
    bool hasParameter(const std::string& name) const override;
    const char* getParameterName(size_t index) const override;
    std::unique_ptr<PlasticityRule> clone() const override;
    bool validate() const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Bienenstock-Cooper-Munro (BCM) rule
// Enhanced with better parameter handling and validation
class BCMRule : public PlasticityRule {
public:
    BCMRule();
    ~BCMRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    const char* getDescription() const override;
    const char* getVersion() const override;
    
    // Parameters
    void setTheta(float theta);  // BCM threshold
    float getTheta() const;
    void setTau(float tau);      // Adaptation time constant
    float getTau() const;
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setPin(float pin);      // BCM pinning constant
    float getPin() const;
    
    std::unordered_map<std::string, std::string> getParameters() const override;
    void setParameters(const std::unordered_map<std::string, std::string>& params) override;
    bool hasParameter(const std::string& name) const override;
    const char* getParameterName(size_t index) const override;
    std::unique_ptr<PlasticityRule> clone() const override;
    bool validate() const override;
    
    // BCM-specific methods
    void setThetaUpdateRule(const std::string& rule);
    const char* getThetaUpdateRule() const;
    void setHebbianModulation(bool modulate);
    bool getHebbianModulation() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward-modulated STDP (R-STDP)
// Enhanced with better parameter handling and validation
class RewardModulatedSTDP : public PlasticityRule {
public:
    RewardModulatedSTDP();
    ~RewardModulatedSTDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    const char* getDescription() const override;
    const char* getVersion() const override;
    
    // Parameters
    void setBaseSTDPWeight(float weight);
    float getBaseSTDPWeight() const;
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setRewardScalar(float scalar);
    float getRewardScalar() const;
    void setEligibilityDecay(float decay);
    float getEligibilityDecay() const;
    
    std::unordered_map<std::string, std::string> getParameters() const override;
    void setParameters(const std::unordered_map<std::string, std::string>& params) override;
    bool hasParameter(const std::string& name) const override;
    const char* getParameterName(size_t index) const override;
    std::unique_ptr<PlasticityRule> clone() const override;
    bool validate() const override;
    void initialize(Brain* brain) override;
    void shutdown() override;
    
    // Reward-modulated specific methods
    void setRewardSystem(Dopamine* dopamine);
    Dopamine* getRewardSystem() const;
    void setPredictionError(PredictionError* error);
    PredictionError* getPredictionError() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Competition-based plasticity rule
class CompetitionRule : public PlasticityRule {
public:
    CompetitionRule();
    ~CompetitionRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    const char* getDescription() const override;
    const char* getVersion() const override;
    
    // Parameters
    void setStrength(float strength);
    float getStrength() const;
    void setDecayRate(float decay);
    float getDecayRate() const;
    void setWinnerTakesAll(bool wta);
    bool getWinnerTakesAll() const;
    
    std::unordered_map<std::string, std::string> getParameters() const override;
    void setParameters(const std::unordered_map<std::string, std::string>& params) override;
    bool hasParameter(const std::string& name) const override;
    const char* getParameterName(size_t index) const override;
    std::unique_ptr<PlasticityRule> clone() const override;
    bool validate() const override;
    
    // Competition-specific methods
    void setCompetitionScope(RegionId region);
    RegionId getCompetitionScope() const;
    void setNormalizeWeights(bool normalize);
    bool getNormalizeWeights() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm