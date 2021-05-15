#!/bin/bash
PROCESS_ID=$(pgrep test-mmap)
kill -SIGUSR1 $PROCESS_ID
echo "kill -SIGUSR1 $PROCESS_ID"