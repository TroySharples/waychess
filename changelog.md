# Change Log

## [1.5.0] - 2025-09-18

### Added

- Static Exchange Evaluation (SEE) pruning in quiescent search.
- Evaluate UCI-extension for inspecting terminal evaluation.
- Ability to configure search internals via compile-time config.
- History heuristic for quiet move-ordering (currently turned off in default configuration).
- SPRT testing framework using fastchess.
- Null-move pruning.
- Late-Move Reduction (LMR).

### Changed

- Hash-moves that fail high are now added to the killer tables and history heuristic.
- Drawing moves are no longer included in the PV.
- Search threading model - search can now be interrupted, as required by UCI.
- Widened move-history buffer to longest possible legal game - we came dangerously close to overflowing in a few games.
- Removed WhyChess variant, as this was becoming difficult to maintain with low-strength. May add it back in the future.
- Better search logging.

### Fixed

- Killer-move replacement strategy.
- Bug in piece-square tables where we were playing anti-positionally (primary reason for large increase in playing strength).
- Order promotions before quiet moves.
- Bug in aspiration window where we were returning early upon possible checkmating moves instead of widening the window out to the maximum - this was causing the occasional illegal move recommendation.

## [1.4.0] - 2025-09-07

### Added

- Piece-square tables to the terminal evaluation heuristic.
- Middle-game / end-game evaluation interpolation so that we can score various stages of the game differently depending on material on board.
- MVV-LVA-sort to the main search.
- Killer-move tables.

### Changed

- Greatly improved logging code to show additional interesting information about the search.

### Fixed

- Bug in depth-1 negamax with alpha/beta code where we weren't properly applying various optimisation techniques.

## [1.3.0] - 2025-09-05

### Added

- Principle variation collection and ordering in main search.
- Hash-move ordering in main search.
- Aspiration window in main search.
- Quiescent search to mitigate the horizon effect.
  - MVV-LVA-sort.
  - Delta-pruning.
- UCI informational printing of nodes, depth, and PV.

### Changed

- Refactored general search and negamax code.
- Early return on checkmate.

### Fixed

## [1.2.3] - 2025-08-29

### Added

### Changed

- Saved the type of alpha/beta node in the transposition-table.

### Fixed

- Misevaluation of drawn positions as decisive by ignoring the three-fold repetition rule.
- Critical error in transposition-table depth where we were preferring positions that were evaluated to a shallower depth.

## [1.2.2] - 2025-08-26

### Added

### Changed

### Fixed

- Erroneous early-return from search by implementing transposition-table-entry ageing (hacky implementation for now).

## [1.2.1] - 2025-08-25

### Added

### Changed

- Changed WhyChess simulated thinking time to use the Lichess standard simulation time.
- Allow up to two take-backs when playing WhyChess.
- Changed the WayChess default hash-size to 256 MB.

### Fixed

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