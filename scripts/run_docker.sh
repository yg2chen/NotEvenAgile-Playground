#!/usr/bin/sh

docker run --rm -it -v "$(pwd):/workdir" crops/poky --workdir=/workdir
