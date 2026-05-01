[section constants]

c1: dw 49        ; '1'
c2: dw 50        ; '2'
c3: dw 51        ; '3'
nl: dw 10

[section code]

; RR(2): quantum = 2 output operations.
; Contexts are stored in ram by INIT_CTX and CTX_SWITCH.
; Context layout: IP, SP, FP, DSP, each 16-bit.
; ctx1=256, ctx2=272, ctx3=288, boot_ctx=240.

INIT_CTX 256 thread1 61440 0 36864
INIT_CTX 272 thread2 57344 0 37120
INIT_CTX 288 thread3 53248 0 37376
CTX_SWITCH 240 256
HLT

thread1:
; first quantum: 2 of 4
load_const c1
OUT
load_const c1
OUT
CTX_SWITCH 256 272

; second quantum: 2 of 4, task1 finished
load_const c1
OUT
load_const c1
OUT
CTX_SWITCH 256 272
HLT

thread2:
; first quantum: 2 of 8
load_const c2
OUT
load_const c2
OUT
CTX_SWITCH 272 288

; second quantum: 2 of 8
load_const c2
OUT
load_const c2
OUT
CTX_SWITCH 272 288

; third quantum: 2 of 8
load_const c2
OUT
load_const c2
OUT
CTX_SWITCH 272 288

; fourth quantum: 2 of 8, task2 finished; all tasks are finished here
load_const c2
OUT
load_const c2
OUT
load_const nl
OUT
HLT

thread3:
; first quantum: 2 of 5
load_const c3
OUT
load_const c3
OUT
CTX_SWITCH 288 256

; second quantum: 2 of 5
load_const c3
OUT
load_const c3
OUT
CTX_SWITCH 288 272

; final quantum: 1 of 5, task3 finished
load_const c3
OUT
CTX_SWITCH 288 272
HLT
