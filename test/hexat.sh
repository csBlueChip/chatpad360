#!/bin/bash

for p in "$@" ; do
    ./push.pl $p
done