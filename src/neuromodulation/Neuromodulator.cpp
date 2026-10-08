namespace nlm {

class Neuromodulator {
public:
    Neuromodulator() {}
    ~Neuromodulator() = default;
    
    void initialize() {}
    void update(double timestep) {}
    
    float getLevel() const { return 0.0f; }
};

} // namespace nlm