#!/bin/bash
cat "$1" | grep '[et]ly lost:' | grep '\([0-9][0-9]\|[1-9]\) bytes' &> /dev/null

if [ "$?" -eq "0" ]; then
  echo -e '\n\n===================================='
  echo 'CHECK VALGRIND MEMORY LEAKS ANALYSIS'
  echo -e '====================================\n\n'
  exit 1
fi
