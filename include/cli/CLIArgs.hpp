#ifndef NLM_CLARGS_HPP
#define NLM_CLARGS_HPP

#include <string>
#include <map>

namespace CLI {
    enum class Mode {
        NONE,
        DEMO,
        EXPERIMENT,
        BATCH,
        CHECKPOINT,
        VISUALIZE,
        TEST,
        CONFIG,
        HELP
    };

    struct CLIArgs {
        Mode mode = Mode::NONE;
        std::string configFile = "configs/default.cfg";
        std::string experimentName;
        std::string batchFile;
        std::string checkpointCommand;
        std::string checkpointPath;
        std::string vizType;
        std::string testSuite;
        std::string configTemplate;
        int verbosity = 1;
        std::string logFile;
        bool quiet = false;
        bool autoSaveCheckpoint = false;
        std::map<std::string, std::string> extraArgs;
    };
}

#endif // NLM_CLARGS_HPP
