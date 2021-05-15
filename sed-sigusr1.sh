#!/bin/bash
PROCESS_ID=$(pgrep test-mmap)
echo "Antes: $(<blocks.txt)"
sed -i 's/./#/g' blocks.txt
echo "Despues: $(<blocks.txt)"
kill -SIGUSR1 $PROCESS_ID
echo "kill -SIGUSR1 $PROCESS_ID"