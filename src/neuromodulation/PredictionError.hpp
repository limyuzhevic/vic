class PredictionError {
public:
    PredictionError();
    ~PredictionError();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Get prediction error level
    float getLevel() const;
    
    // Update prediction error from sensory input
    void update(TimestepDuration dt);
    
    // Set predicted value
    void setPredicted(float prediction);
    
    // Update with actual value (computes error)
    void updateWithActual(float actual);
    
    // Reset prediction error
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
