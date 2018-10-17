#pragma once

#include <string>

namespace logger
{
    void start(const std::string &name);
    void log(const std::string &message);
}