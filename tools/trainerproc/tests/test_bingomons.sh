#!/bin/sh
set -eu

trainerproc=${1:-./trainerproc}
fixture=$(dirname "$0")/all_fields.bingomons
tmpdir=$(mktemp -d)
trap 'rm -rf "$tmpdir"' EXIT

"$trainerproc" -f bingo-constants -o "$tmpdir/constants.h" "$fixture"
"$trainerproc" -f bingo-data -o "$tmpdir/data.h" "$fixture"

assert_contains() {
    grep -F "$1" "$2" >/dev/null || {
        echo "missing expected output: $1" >&2
        exit 1
    }
}

assert_contains "BINGO_MON_ALL_FIELDS," "$tmpdir/constants.h"
assert_contains "BINGO_MON_DYNAMAX," "$tmpdir/constants.h"
assert_contains "BINGO_MON_COUNT," "$tmpdir/constants.h"

assert_contains "[BINGO_MON_ALL_FIELDS] =" "$tmpdir/data.h"
assert_contains '.nickname = COMPOUND_STRING("Bubbles"),' "$tmpdir/data.h"
assert_contains ".species = SPECIES_WOBBUFFET," "$tmpdir/data.h"
assert_contains ".gender = TRAINER_MON_FEMALE," "$tmpdir/data.h"
assert_contains ".heldItem = ITEM_ASSAULT_VEST," "$tmpdir/data.h"
assert_contains ".ev = TRAINER_PARTY_EVS(252, 0, 0, 252, 4, 0)," "$tmpdir/data.h"
assert_contains ".iv = TRAINER_PARTY_IVS(25, 26, 27, 28, 29, 30)," "$tmpdir/data.h"
assert_contains ".ability = ABILITY_TELEPATHY," "$tmpdir/data.h"
assert_contains ".lvl = 67," "$tmpdir/data.h"
assert_contains ".ball = BALL_MASTER," "$tmpdir/data.h"
assert_contains ".friendship = 42," "$tmpdir/data.h"
assert_contains ".nature = NATURE_HASTY," "$tmpdir/data.h"
assert_contains ".isShiny = TRUE," "$tmpdir/data.h"
assert_contains ".dynamaxLevel = MAX_DYNAMAX_LEVEL," "$tmpdir/data.h"
assert_contains ".teraType = TYPE_PSYCHIC," "$tmpdir/data.h"
assert_contains ".tags = MON_POOL_TAG_LEAD | MON_POOL_TAG_TERA," "$tmpdir/data.h"
assert_contains "MOVE_AIR_SLASH," "$tmpdir/data.h"
assert_contains "MOVE_BARRIER," "$tmpdir/data.h"
assert_contains "MOVE_SOLAR_BEAM," "$tmpdir/data.h"
assert_contains "MOVE_EXPLOSION," "$tmpdir/data.h"

assert_contains "[BINGO_MON_DYNAMAX] =" "$tmpdir/data.h"
assert_contains ".gender = TRAINER_MON_MALE," "$tmpdir/data.h"
assert_contains ".dynamaxLevel = 5," "$tmpdir/data.h"
assert_contains ".gigantamaxFactor = TRUE," "$tmpdir/data.h"
assert_contains ".shouldUseDynamax = TRUE," "$tmpdir/data.h"

printf '%s\n' '=== INVALID_ID ===' 'Pikachu' > "$tmpdir/invalid.bingomons"
if "$trainerproc" -f bingo-data -o "$tmpdir/invalid.h" "$tmpdir/invalid.bingomons" 2>/dev/null; then
    echo "trainerproc accepted a bingo mon ID without the BINGO_MON_ prefix" >&2
    exit 1
fi

echo "trainerproc bingo mon tests passed"
