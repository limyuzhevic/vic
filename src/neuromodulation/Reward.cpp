namespace nlm {

class Reward {
public:
    Reward() {}
    ~Reward() = default;
    
    void initialize() {}
    void update(double timestep) {}
    
    float getLevel() const { return 0.0f; }
};

} // namespace nlm