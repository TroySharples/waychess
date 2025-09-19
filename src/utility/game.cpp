#include "game.hpp"

#include "search/search.hpp"

#include <mutex>
#include <stdexcept>

game::game()
{
    _t = std::thread(&game::run_loop, this);
}

game::~game()
{
    stop();

    {
        std::lock_guard<std::mutex> lk(_m);
        _run = false;
    }
    _c.notify_one();

    _t.join();
}

void game::search(search_type type, std::size_t max_depth, std::chrono::duration<double> max_time)
{
    {
        std::lock_guard<std::mutex> lk(_m);
        if (_search_params.has_value())
            throw std::runtime_error("Search already ongoing");

        _search_params = { .max_depth=max_depth, .max_time=max_time };
        _type = type;
    }
    _c.notify_one();
}

void game::stop()
{
    gs.stop_search = true;
}

void game::run_loop()
{
    while (true)
    {
        {
            std::unique_lock<std::mutex> lk(_m);
            _c.wait(lk, [this] () { return !_run || _search_params.has_value(); });
        }

        // Return immediately if we're stopping the run thread (shutting down).
        if (!_run)
            return;

        // Otherwise kick-off a search.
        const std::uint64_t move { search::recommend_move(gs, _search_params->max_depth, _search_params->max_time).move };
        if (_type == search_go)
            callback_best_move(move);

        _search_params.reset();
    }
}