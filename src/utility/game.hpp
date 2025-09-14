#pragma once

#include "position/game_state.hpp"

#include <chrono>
#include <thread>
#include <condition_variable>
#include <mutex>

class game
{
public:
    game();

    game(const game&) = delete;
    game& operator=(const game&) = delete;

    game(game&&) = delete;
    game& operator=(game&&) = delete;

    ~game();

    void search(std::size_t max_depth, std::chrono::duration<double> max_time);
    void stop();

    game_state gs;

    void (*callback_best_move)(std::uint32_t);

private:
    std::thread _t;
    std::condition_variable _c;
    bool _run { true };
    std::mutex _m;

    struct search_parameters
    {
        std::size_t max_depth;
        std::chrono::duration<double> max_time;
    };
    std::optional<search_parameters> _search_params;

    void run_loop();
};