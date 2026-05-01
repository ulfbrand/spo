#!/bin/bash

set -e

LOGIN="u309510"
PASSWORD="7b00bf29-9d3a-42f8-ae48-5877f3d97450"

cd ..

./main input/input2/ output/output2/ cfg/cfg2/ 1 vm/src/listing.asm

cd vm

mono Portable.RemoteTasks.Manager.exe \
  -ul "$LOGIN" \
  -up "$PASSWORD" \
  -w -id \
  -s AssembleDebug \
  asmListing src/listing.asm \
  definitionFile StackVM.pdsl \
  archName StackVM \
  sourcesDir src > out.txt

TASK_ID=$(head -n 1 out.txt)

mono Portable.RemoteTasks.Manager.exe \
  -ul "$LOGIN" \
  -up "$PASSWORD" \
  -g "$TASK_ID" \
  -r out.ptptb > binary_debug.ptptb

mono Portable.RemoteTasks.Manager.exe \
  -ul "$LOGIN" \
  -up "$PASSWORD" \
  -il \
  -s ExecuteBinaryWithInteractiveInput \
  definitionFile StackVM.pdsl \
  archName StackVM \
  binaryFileToRun binary_debug.ptptb \
  codeRamBankName code \
  ipRegStorageName IP \
  finishMnemonicName HLT \
  stdinRegStName IN_PORT \
  stdoutRegStName OUT_PORT