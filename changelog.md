
# Change Log

## [Unreleased] - yyyy-mm-dd

### Added
- Configurable and extensible logging framework.
- Version number in UCI ID.
- WhyChess as a standalone application (instead of an old version of WayChess).
- Lichess config files.

### Changed

- Restructured transposition-table implementation in search.
- Restructured UCI handling.
- Made search algorithms use hash-tables by default.

### Fixed

- Misevaluation of drawn positions as winning by ignoring three-fold repetition rules.
- Misevaluation of drawn positions as winning by ignoring stalemate resources.
- FEN-serialisation bug.

## [1.1.0] - 2025-08-10

### Added

- Negamax implementation of minimax algorithm.
- Iterative-deepening search framework.
- Alpha/beta pruning search algorithm.
- Variable-sized hash-table options for perft and alpha/beta search.
- Regression tests for puzzles solved.

### Changed

- Made alpha/beta pruning (without hash-tables) default search algorithm.
- Made iterative-deepening the chosen time-management strategy.

### Fixed

## [1.0.0] - 2025-08-02

Initial release.