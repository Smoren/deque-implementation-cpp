#include "profiler.h"

namespace Smoren::Tools {
    Profiler::Profiler(std::string name) : name(name), timeStart(std::chrono::steady_clock::now()), isStopped(false) {

    }

    Profiler::~Profiler() {
        if(!isStopped) {
            stop();
        }
    }

    void Profiler::stop() {
        auto timeFinish = std::chrono::steady_clock::now();
        auto duration = timeFinish - timeStart;
        std::cerr << name << ": " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << " ms" << std::endl;

        timeStart = timeFinish;
        isStopped = true;
    }
}
