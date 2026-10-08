namespace nlm {

class Maturation {
public:
    Maturation(std::shared_ptr<Config> config) {}
    ~Maturation() = default;
    
    bool initialize() { return true; }
    void update(double timestep) {}
};

} // namespace nlm