#pragma once

#include <chrono>
#include <iostream>

namespace timing
{

struct Timer
{
    using clock         = std::chrono::system_clock;
    using time_point    = clock::time_point;
    using ms            = std::chrono::duration<double, std::milli>;
    std::string message;
    time_point start;
    int cluster;
    Timer(const std::string& message) :
            message(message),
            start(clock::now())
    {
    }
    ~Timer()
    {
        auto delta = clock::now() - start;
        std::cout << message << ": " << cluster << " cluster in " << std::chrono::duration_cast<ms>(delta).count() << "ms" << std::endl;
    }
};

struct Benchmark
{
    template<std::size_t Itr, typename Call>
    static void timing(const std::string& prefix, Call&& callable)
    {
        auto start = Timer::clock::now();
        for (std::size_t i = 0; i < Itr; ++i)
            callable();
        auto end = Timer::clock::now();
        auto delta = end - start;
        auto in_ms = std::chrono::duration_cast<Timer::ms>(delta).count();
        std::cout << prefix << ": Total: " << in_ms << "ms Avg: " << in_ms / Itr << std::endl;
    }
};

}