namespace nlm {

class Curiosity {
public:
    Curiosity() {}
    ~Curiosity() = default;
    
    void initialize() {}
    void update(double timestep) {}
    
    float getLevel() const { return 0.0f; }
};

} // namespace nlm