#ifndef NLM_CLI_HPP
#define NLM_CLI_HPP

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {
namespace cli {

class NLMCLI {
private:
    std::string config_file_;
    std::string output_dir_;
    bool quiet_mode_;
    bool verbose_mode_;
    
public:
    NLMCLI();
    ~NLMCLI() = default;
    
    int run(int argc, char** argv);
    
private:
    void printBanner(const std::string& title) const;
    std::string getTimestamp() const;
    bool parseArguments(int argc, char** argv, std::map<std::string, std::string>& args);
    bool executeCommand(const std::string& command, const std::map<std::string, std::string>& args);
};

} // namespace cli
} // namespace nlm

#endif // NLM_CLI_HPP
