namespace nlm {

class Environment {
public:
    Environment(std::shared_ptr<Config> config) {}
    ~Environment() = default;
    
    bool initialize() { return true; }
    void update(double timestep) {}
};

} // namespace nlm