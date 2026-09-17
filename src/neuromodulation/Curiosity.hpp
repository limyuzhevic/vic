class Curiosity {
public:
    Curiosity();
    ~Curiosity();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Get curiosity level
    float getLevel() const;
    
    // Update curiosity from novel experiences
    void update(TimestepDuration dt);
    
    // Increase curiosity for new information
    void increaseCuriosity(float amount);
    
    // Decrease curiosity
    void decreaseCuriosity(float amount);
    
    // Reset curiosity
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
