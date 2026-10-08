namespace nlm {

class Novelty {
public:
    Novelty() {}
    ~Novelty() = default;
    
    void initialize() {}
    void update(double timestep) {}
    
    float getLevel() const { return 0.0f; }
};

} // namespace nlm