---
layout: post
tags: [embedded memory]
title: What consumes the ROM
---
arm-none-eabi/bin/ld: region `rom' overflowed by 462256 bytes

That's what you get for trying to program embedded systems with
C++ :) Turns out gcc is not terribly good at printing out statistics
of the code before the linking step. `-Wl,-M` does give something,
but its output is a bit difficult to decipher.

### Step 1: increse memory
Introduce a "debug" target device, one with insane amounts of memory.
This will make the linking step succeed, and give access to the statistics.

### Step 2: get rid of '\_\_dso\_handle'
If linking stil fails with a missing symbol `__dso_handle` 
(dso is short for Dynamic Shared Object, i.e. a shared library), get rid
of it. 

In this case the culprit was an `#include <iostream>` which caused gcc
to internally link against a shared library.

### Step 3: find the whales
Once it links (to ELF format, of course)

	arm-none-eabi-readelf -s output.elf |tr -s [:space:] |cut -d' ' -f 4- |sort -n

gives a nice list of big objects that remain.

	...
	266 FUNC GLOBAL HIDDEN 1 __udivsi3
	368 FUNC GLOBAL DEFAULT 1 _Z4fft0iibPSt7complexIlES
	436 FUNC GLOBAL DEFAULT 1 __ieee754_sqrt
	460 FUNC GLOBAL HIDDEN 1 __divsi3
	556 FUNC GLOBAL HIDDEN 1 __aeabi_fmul
	560 FUNC GLOBAL HIDDEN 1 __aeabi_fdiv
	1064 OBJECT LOCAL DEFAULT 5 impure_data
	1252 FUNC GLOBAL HIDDEN 1 __aeabi_dmul
	1464 FUNC GLOBAL HIDDEN 1 __aeabi_ddiv
	1772 FUNC GLOBAL HIDDEN 1 __aeabi_dadd
	1848 FUNC GLOBAL HIDDEN 1 __aeabi_dsub


So here it is integer division, a square root and arithmetics with doubles.

### Step 4: find the callsites

	arm-none-eabi-objdump -S -l output.elf |less

and search for the big symbols.

### Step 5: callgraphs
Why is there no de-facto tool to produce callgraphs of functions, compile-time?
There seems to be a lot of alternatives out there, but which is the best?
I'll probably write about this in some future post...

