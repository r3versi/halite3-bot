#include "log.h"

#include <fstream>
#include <iostream>
#include <chrono>

static std::ofstream log_file;

void logger::start(const std::string& name)
{
    auto now_in_nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    std::string filename = "logs/" + name + "_" + std::to_string(now_in_nanos) + ".log";
    log_file.open(filename, std::ios::trunc | std::ios::out);
}

void logger::log(const std::string& message)
{
    log_file << message << std::endl;
}