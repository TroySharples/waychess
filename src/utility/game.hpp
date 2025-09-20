#pragma once

#include "position/game_state.hpp"

#include <chrono>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <optional>

class game
{
public:
    game();

    game(const game&) = delete;
    game& operator=(const game&) = delete;

    game(game&&) = delete;
    game& operator=(game&&) = delete;

    ~game();

    enum search_type : std::uint8_t { search_go, search_evaluate };
    void search(search_type type, std::size_t max_depth, std::chrono::duration<double> max_time = std::chrono::days(2));

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
    search_type _type;


    void run_loop();
};