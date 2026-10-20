#!/usr/bin/env python3

"""
Usage: python3 make_learnables.py INPUT_JSON_OR_DIR OUTPUT_FILE

Build a primary store of learnable moves for each species based on input documents. This script
is meant to be run to generate a pre-processed store of data that should not change very much;
thus, it can safely be pre-computed in order to speed up incremental builds for end-users.
The game build supplies only porymoves_files/custom.json. A directory argument
is still supported for manually importing historical PoryMoves exports.
"""

from functools import reduce

import json
import pathlib
import sys


def from_single(fname: pathlib.Path) -> dict[str, set[str]]:
    with open(fname, "r") as fp:
        data = json.load(fp)
        required = {"LevelMoves", "TMMoves", "EggMoves", "TutorMoves"}
        for species, by_method in data.items():
            if not isinstance(by_method, dict) or not required.issubset(by_method):
                raise ValueError(f"{fname}: {species} must use the PoryMoves per-method format")
        return {
            species: set([level_up["Move"] for level_up in by_method["LevelMoves"]])
            | set([move for move in by_method["TMMoves"]])
            | set([move for move in by_method["EggMoves"]])
            | set([move for move in by_method["TutorMoves"]])
            for species, by_method in data.items()
        }


def from_batch(dir: pathlib.Path) -> dict[str, set[str]]:
    learnables = reduce(
        lambda acc, single: {
            species: acc.get(species, set()) | single.get(species, set())
            for species in acc.keys() | single.keys()
        },
        map(from_single, dir.glob("*.json")),
        {},
    )
    # Historical imports retain Acid; this fork replaced it with Corrode.
    return {
        species: {"MOVE_CORRODE" if move == "MOVE_ACID" else move for move in moves}
        for species, moves in learnables.items()
    }


def main():
    if len(sys.argv) < 3:
        print("Missing required arguments", file=sys.stderr)
        print(__doc__, file=sys.stderr)
        quit(1)

    INPUT_PATH = pathlib.Path(sys.argv[1])
    OUTPUT_FILE = pathlib.Path(sys.argv[2])

    assert INPUT_PATH.exists(), f"{INPUT_PATH=} does not exist"
    assert OUTPUT_FILE.parent.exists(), f"parent of {OUTPUT_FILE=} does not exist"

    learnables = from_batch(INPUT_PATH) if INPUT_PATH.is_dir() else from_single(INPUT_PATH)
    batch = {
        species: list(sorted(learnables))
        for species, learnables in sorted(learnables.items())
    }
    with open(OUTPUT_FILE, "w") as fp:
        json.dump(batch, fp, indent=2)
        fp.write("\n")


if __name__ == "__main__":
    main()
