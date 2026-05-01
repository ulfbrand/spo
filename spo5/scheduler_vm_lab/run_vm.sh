#!/bin/bash

MODE=$1

if [ -z "$MODE" ]; then
  echo "Usage: ./run_vm.sh [fcfs|rr]"
  exit 1
fi

LOGIN="u309510"
PASSWORD="7b00bf29-9d3a-42f8-ae48-5877f3d97450"

# Переходим в vm
cd vm || exit 1

PDSL="../StackVM_scheduler.pdsl"

echo "=== ASSEMBLE ==="

mono Portable.RemoteTasks.Manager.exe \
-ul $LOGIN \
-up $PASSWORD \
-w -id \
-s AssembleDebug \
asmListing ../src/scheduler_${MODE}.asm \
definitionFile $PDSL \
archName StackVM \
sourcesDir ../src > out.txt

echo "Assemble result:"
cat out.txt

# Берём только ID задачи
TASK_ID=$(head -n 1 out.txt | tr -d '\r')

echo "TASK_ID=$TASK_ID"

echo "=== GET BINARY ==="

mono Portable.RemoteTasks.Manager.exe \
-ul $LOGIN \
-up $PASSWORD \
-g "$TASK_ID" \
-r out.ptptb > binary_debug.ptptb

echo "Binary file:"
ls -l binary_debug.ptptb

echo "=== RUN ==="

mono Portable.RemoteTasks.Manager.exe \
-ul $LOGIN \
-up $PASSWORD \
-il \
-s ExecuteBinaryWithInteractiveInput \
definitionFile $PDSL \
archName StackVM \
binaryFileToRun binary_debug.ptptb \
codeRamBankName code \
ipRegStorageName IP \
finishMnemonicName HLT \
stdinRegStName IN_PORT \
stdoutRegStName OUT_PORT