
# Change Log

## [Unreleased] - yyyy-mm-dd

### Added

- Added quiescent search to mitigate the horizon effect.

### Changed

- Restructured transposition-table implementation in search.

### Fixed

- Misevaluation of drawn positions as decisive by ignoring the three-fold repetition rule.

## [1.2.0] - 2025-08-24

### Added
- Configurable and extensible logging framework.
- Version number in UCI ID.
- WhyChess as a standalone application (instead of an old version of WayChess).
- Lichess config files.

### Changed

- Restructured UCI handling.
- Made search algorithms use hash-tables by default.

### Fixed

- Misevaluation of drawn positions as decisive by ignoring the 50-move draw rule.
- Misevaluation of drawn positions as decisive by ignoring stalemate resources.
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