namespace nlm {

class Synaptogenesis {
public:
    Synaptogenesis(std::shared_ptr<Config> config) {}
    ~Synaptogenesis() = default;
    
    bool initialize() { return true; }
    void update(double timestep) {}
};

} // namespace nlm