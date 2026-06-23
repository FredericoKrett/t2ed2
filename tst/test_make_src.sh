#!/bin/sh

set -eu

project_root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)

cd "$project_root"
make -C src ted
test -x src/ted
rm -f src/ted
