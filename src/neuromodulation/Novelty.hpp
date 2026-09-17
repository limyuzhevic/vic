class Novelty {
public:
    Novelty();
    ~Novelty();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Get novelty level
    float getLevel() const;
    
    // Detect and update novelty from input
    void update(TimestepDuration dt);
    
    // Check if something is novel
    bool isNovel(const std::vector<float>& pattern) const;
    
    // Reset novelty detector
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
