#!/bin/bash
PROCESS_ID=$(pgrep test-mmap)
echo "Antes: $(<blocks.txt)"
nano blocks.txt
echo "Despues: $(<blocks.txt)"
kill -SIGUSR1 $PROCESS_ID
echo "kill -SIGUSR1 $PROCESS_ID"