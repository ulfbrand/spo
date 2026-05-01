[section constants]

c1: dw 49        ; '1'
c2: dw 50        ; '2'
c3: dw 51        ; '3'
space: dw 32
nl: dw 10

[section code]

; FCFS: tasks are executed to completion in FIFO order.
; Workload bursts: T1=4, T2=8, T3=5. All work is visible through OUT.

CALL task1 0
CALL task2 0
CALL task3 0
load_const nl
OUT
HLT

task1:
load_const c1
OUT
load_const c1
OUT
load_const c1
OUT
load_const c1
OUT
RET

task2:
load_const c2
OUT
load_const c2
OUT
load_const c2
OUT
load_const c2
OUT
load_const c2
OUT
load_const c2
OUT
load_const c2
OUT
load_const c2
OUT
RET

task3:
load_const c3
OUT
load_const c3
OUT
load_const c3
OUT
load_const c3
OUT
load_const c3
OUT
RET
