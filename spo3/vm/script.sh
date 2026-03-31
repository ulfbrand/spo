#!/bin/bash

cd ..
./main input/input2/ output/output2/ cfg/cfg2/ 1 vm/src/listing.asm 
cd vm
mono Portable.RemoteTasks.Manager.exe -ul u309510 -up 7b00bf29-9d3a-42f8-ae48-5877f3d97450 -w -id -s AssembleDebug asmListing src/listing.asm definitionFile StackVM.pdsl archName StackVM sourcesDir src > out.txt
mono Portable.RemoteTasks.Manager.exe -ul u309510 -up 7b00bf29-9d3a-42f8-ae48-5877f3d97450  -g $(< out.txt) -r out.ptptb > binary_debug.ptptb
mono Portable.RemoteTasks.Manager.exe -ul u309510 -up 7b00bf29-9d3a-42f8-ae48-5877f3d97450  -il -s ExecuteBinaryWithInteractiveInput definitionFile StackVM.pdsl archName StackVM binaryFileToRun binary_debug.ptptb codeRamBankName code ipRegStorageName IP finishMnemonicName HLT stdinRegStName IN_PORT stdoutRegStName OUT_PORT
# mono Portable.RemoteTasks.Manager.exe -ul u309510 -up 7b00bf29-9d3a-42f8-ae48-5877f3d97450  -il -s DebugBinary definitionFile StackVM.pdsl archName StackVM binaryFileToRun binary_debug.ptptb codeRamBankName code ipRegStorageName IP finishMnemonicName HLT
