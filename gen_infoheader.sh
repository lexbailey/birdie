#!/usr/bin/env sh

echo "#ifndef INFOHEADER_INCLUDE"
echo "#define INFOHEADER_INCLUDE"

echo "#define MAX_FILENAME_WIDTH (`ls *.c | awk '{ print length, $0 }' | sort -n | cut -d" " -f2- | tail -n1 | wc -m`)"

echo "#endif //INFOHEADER_INCLUDE"