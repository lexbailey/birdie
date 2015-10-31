#!/usr/bin/env sh
make -s clean
make -j -s verification.log
less verification.log