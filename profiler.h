#pragma once

#include <chrono>
#include <iostream>
#include <string>

namespace Smoren::Tools {
    #define UNIQ_ID_IMPL(lineno) _a_local_var_##lineno
    #define UNIQ_ID(lineno) UNIQ_ID_IMPL(lineno)

    #define LOG_DURATION(message) \
        Profiler UNIQ_ID(__LINE__)(message)

    class Profiler {
    public:
        Profiler(std::string name);
        ~Profiler();
        void stop();
    protected:
        std::string name;
        std::chrono::time_point<std::chrono::steady_clock> timeStart;
        bool isStopped;
    };
}
