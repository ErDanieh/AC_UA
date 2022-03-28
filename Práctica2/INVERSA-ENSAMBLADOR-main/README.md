# INVERSA-ENSAMBLADOR

La parte 2 falla. Hay algo hecho pero no se porque falla.

El codigo en C funciona correctamente.
El parte1 funciona perfectamente.

La parte2 falla. Podeis usar testeo para ir probando cosas.


## POR QUÉ IBA MÁS LENTO EN SSE?
Mixing AVX code and legacy SSE code incurs a performance penalty. The most reasonable solution is to execute the VZEROALL instruction after an AVX segment of code, especially just before executing SSE code.

As per Intel's diagram, the penalty when transitioning into or out of state C (legacy SSE with upper half of AVX registers saved) is in the order of 100 clock cycles. The other transitions are only 1 cycle:
