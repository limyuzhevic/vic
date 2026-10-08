namespace nlm {

class DevelopmentSystem {
public:
    DevelopmentSystem(std::shared_ptr<Config> config) {}
    ~DevelopmentSystem() = default;
    
    bool initialize() { return true; }
    void update(double timestep) {}
};

} // namespace nlm