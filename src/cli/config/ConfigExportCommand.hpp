class ConfigExportCommand : public Command {
public:
    std::string getName() const override { return "export"; }
    std::string getDescription() const override { 
        return "Export configuration to file"; 
    }
    std::string getUsage() const override { 
        return "nlm config export <format> [path]"; 
    }
    
    bool validateArgs(const std::vector<std::string>& args) override {
        return args.size() >= 1 && args.size() <= 2;
    }
    
    bool execute(const std::vector<std::string>& args) override;
};