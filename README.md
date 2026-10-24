# Asteroid Escape solver

Single-agent search algorithms (BFS, A\*, IDA\*, and variants that find every shortest solution) for
**Asteroid Escape**, the 3×3 sliding puzzle by SmartGames (product SG 426), plus the entropy-based
difficulty scores (MUSE / ReMUSE) described in del Solar-Zavala, Schaa and Barriga (2023).

The repository includes the 60 challenges of the official booklet (`Boards/`) and the booklet itself.

## The puzzle and how it is modelled

The board has 3×3 cells, numbered row by row:

```
 0 | 1 | 2
---+---+---
 3 | 4 | 5
---+---+---
 6 | 7 | 8
       v
     exit
```

Eight pieces and one empty cell fill the board. A move slides a piece into the empty cell (up, down,
left or right). The goal is to slide the spaceship out through the exit, below cell 7. The pieces
are not squares: they interlock, and a piece can only slide if its shape and the shapes around it allow it.

Each piece is stored as an 8×8 bitmap (`PRECALC_PIECES` in `Common.cpp`). The centre 4×4 is
the piece's own cell, and the 2-pixel border is the part that reaches into the neighbouring cells.
Collisions and slides are checked on these bitmaps.

| Letter | Piece |
|---|---|
| `A` | spaceship |
| `B` | big asteroid |
| `C` | big asteroid (corner) |
| `D`, `E`, `F` | small asteroid (D, E and F have the same shape; any of the three letters can be used) |
| `G` | two small asteroids, diagonal |
| `H` | two small asteroids, aligned |
| `-` | empty cell |

## Board files

A board is a text file with two lines of 9 characters each, one per cell (cells 0 to 8):

1. the piece in each cell;
2. the orientation of each piece, `0` to `3`, in steps of 90° (`-` or any digit for the empty cell).

Example (`Boards/starter/starter1`):

```
BDGACDH-D
021023300
```

There must be exactly one spaceship and one empty cell. Malformed boards are rejected with a
message that says what is wrong. Windows line endings are accepted.

`Boards/` contains the 60 challenges of the booklet, 12 per level: `starter`, `junior`, `expert`,
`master` and `wizard`.

## Building

A C++20 compiler is needed (GCC 10 or later, Clang 12 or later, or MinGW-w64 on Windows).

```
make            # produces AsteroidEscape.exe
make clean
```

Without `make`:

```
g++ -std=c++20 -O3 main.cpp Game.cpp State.cpp Piece.cpp Common.cpp -o AsteroidEscape.exe
```

## Usage

```
AsteroidEscape.exe <board_file> <algorithm> [heuristic] [LOOKBACK]
```

Example:

```
AsteroidEscape.exe Boards/starter/starter1 ASTAR DISTANCE
```

The program first prints the board (see below) and then runs the chosen algorithm.

### Algorithms

| Algorithm | What it does |
|---|---|
| `BFS` | Breadth-first search. Returns one shortest solution. |
| `ASTAR` | A\* with the chosen heuristic. Returns one solution (shortest when the heuristic is admissible). |
| `IDASTAR` | Iterative-deepening A\*. |
| `ALLSTAR` | Variant of A\* that keeps searching until it has found every shortest solution. |
| `ALLPATHS` | Variant of BFS that finds every shortest solution. |
| `HUMAN` | Interactive play: the board and the legal moves are shown, and you pick one by number. |
| `MUSE` | Runs `ALLSTAR` and then computes the MUSE entropy score of the puzzle (see the paper below). |
| `REMUSE` | Same, with the ReMUSE variant of the score (softmin / KL divergence). |

`BFS` and `ASTAR` print the solution as the sequence of directions in which the pieces were slid,
its length, and search counters (`ASTAR`: closed-list size and the number of generated
successors, "All iterations").

### Heuristics

All the heuristics below are admissible and consistent, so `ASTAR` and `IDASTAR` return shortest
solutions.

| Heuristic | Value |
|---|---|
| `ZERO` (default) | 0 (A\* behaves as uniform-cost search) |
| `TWOONE` | +1 if the ship is not in the exit column, +1 if it is not out of the board |
| `OBSTACLES0` | `TWOONE` + 1 if the cell above the exit (cell 7) holds an asteroid |
| `OBSTACLES1` | `TWOONE` + number of occupied cells in the exit column between the ship's row and the exit |
| `DISTANCE` | Manhattan distance, in cells, from the ship to the exit cell |
| `ROWDISTANCE` | Number of rows between the ship and the exit (the column is ignored) |

This was checked exactly on the 60 boards. For every state reachable from each board (about
1.7 million in total), the shortest distance to the goal was computed, and no heuristic is ever
larger than that distance. No move changes any heuristic by more than 1.

> **Note on `DISTANCE` and `ROWDISTANCE`.** Before this version, `DISTANCE` ignored the column
> because of a typo (`current_ship%3 - current_ship%3`, which is always 0), so it was equal to
> what is now `ROWDISTANCE`. With the fixed `DISTANCE`, the closed list is about 10 % smaller
> (median over the 60 boards). Solution lengths do not change. Use `ROWDISTANCE` to reproduce
> results obtained with the old `DISTANCE`.

### Options

`LOOKBACK` allows a piece to undo the previous move. By default, the move that would bring
the board back to the previous state is not generated. This does not change the solutions found,
because that state has already been visited.

## Reading the board print

```
        col 0    col 1    col 2
       b b b b
      +--------+--------+--------+
      |B B B B :. . . . :G G . . |
      |B B B B :. . . . :G G . . |      row 0
      |. . . . :D D . . :. . G G |
      |. . . . :D D . . :. . G G |
      +- - - - +- - - - +- - - - +
  a a |A A A A :a a . . :. . . . |
  a a |A A A A :a a . . :. . . . |      row 1
      |. A A c :C C . . :. . D D |
      |. A A c :C C . . :. . D D |
      +- - - - +- - - - +- - - - +
      |H H . . :c c . . :. . D D |
      |H H . . :. . . . :. . D D |      row 2
      |H H . . :. . . . :. . . . |
      |H H . . :. . . . :. . . . |
      +--------+  EXIT  +--------+

  cell  (r,c)  piece  name                        orientation
  0     (0,0)    B    big asteroid                0 deg
  ...
  7     (2,1)    -    (empty)
  ...

  Ship: cell 3 (row 1, col 0)   exit: below cell 7
  Empty cell: 7
  g = 0   h = 3 (DISTANCE)   f = 3
```

- Every cell is drawn as 4×4 characters.
  - An **uppercase** letter is a piece inside its own cell.
  - A **lowercase** letter is the same piece reaching into a neighbouring cell. For example, the
    `a a` to the right of the ship is its wing inside cell 4.
  - `.` is free space.
  - `*` would mean two pieces overlapping, which a legal board never has.
- Letters outside the frame are parts of pieces that stick out of the board.
- When the ship leaves the board, it is drawn below `EXIT`.
- The table lists every cell, and the last lines give the position of the ship, the empty cell
  and the values of g, h and f for the chosen heuristic.

## Files

| File | Contents |
|---|---|
| `main.cpp` | Command-line parsing |
| `Game.cpp`, `Game.h` | Search algorithms, interactive play, MUSE / ReMUSE |
| `State.cpp`, `State.h` | Board state: parsing, legal moves, sliding, collisions, heuristics, board print |
| `Piece.cpp`, `Piece.h` | One piece: type, orientation and bitmap |
| `Common.cpp`, `Common.h` | Enumerations, piece bitmaps and board geometry |
| `Boards/` | The 60 challenges of the official booklet |
| `Challenge Booklet - Asteroid Escape - Smart Games.pdf` | Official rules and challenges |
| `T2_BusquedaHeuristica.pdf` | Report describing the implementation of the search algorithms |

## Reproducibility notes

- Search counters (closed list, generated nodes) depend on how ties between nodes with the same
  f are broken. Ties are broken by `std::priority_queue`, which is implementation-specific, so
  counts can differ slightly between compilers or standard libraries. Solution lengths do not.
- Time and "nodes per second" are wall-clock measurements. On very small boards the time can be
  0 ms, and nodes per second is then printed as `inf`.

## Reference

J. A. del Solar-Zavala, H. Schaa and N. A. Barriga, "Using Entropy for Modeling Difficulty in the
Asteroid Escape Sliding Puzzle," *2023 IEEE CHILEAN Conference on Electrical, Electronics
Engineering, Information and Communication Technologies (CHILECON)*, 2023.
doi:[10.1109/CHILECON60335.2023.10418707](https://doi.org/10.1109/CHILECON60335.2023.10418707)

```bibtex
@inproceedings{delsolar2023entropy,
  author    = {del Solar-Zavala, Jose A. and Schaa, Hans and Barriga, Nicolas A.},
  title     = {Using Entropy for Modeling Difficulty in the Asteroid Escape Sliding Puzzle},
  booktitle = {2023 IEEE CHILEAN Conference on Electrical, Electronics Engineering,
               Information and Communication Technologies (CHILECON)},
  year      = {2023},
  doi       = {10.1109/CHILECON60335.2023.10418707}
}
```

Asteroid Escape is a product of SmartGames. This repository is not affiliated with SmartGames.
