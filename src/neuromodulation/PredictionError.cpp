namespace nlm {

class PredictionError {
public:
    PredictionError() {}
    ~PredictionError() = default;
    
    void initialize() {}
    void update(double timestep) {}
    
    float getLevel() const { return 0.0f; }
};

} // namespace nlm