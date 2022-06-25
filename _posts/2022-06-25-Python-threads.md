---
layout: post
tags: [parallelism python]
---
A continuation to the [C++ threads post]({{ site.url}}/2021/07/11/C++-threads.html). Read that first to make sense of this.

I got a Raspberry Pi RP2040 (dual core ARM Cortex M0) that can run MicroPython, so I thought I should have a look on threading in Python.

Porting the (buggy) version of the code to python gives this:
```
import time, _thread
import io

value=0
def worker():
	global value
	for i in range(10):
		value = value + 1

_thread.start_new_thread(worker, ()); worker()
```

### Raspberry Pi RP2040
MicroPython can run only one thread per processor core,
which means the two threads run simultaneously in their own cores.

The race condition is clearly visible, i.e. the overhead of starting a thread must be pretty small.

Since both threads run independently, the rather specific conditions needed to get sub 10 values cannot (_I think..._) occur:

![plot_raspberry]({{ site.url }}/assets/benari/plot_raspberry.png)


### Laptop with Intel CPU
Running the same python code on the Intel CPU shows the overhead of Python.
Starting a thread takes much longer than executing it does, and the race is not visible at all:

![plot_python10]({{ site.url }}/assets/benari/plot_python10.png)


When the run time of the threads is increased, the bug becomes visible. First signs can be seen if the thread increments 10k times instead of just 10 times. The effect is pretty clear at 100k increments per thread (values scaled back down to range 0-20):

![plot_python100k]({{ site.url }}/assets/benari/plot_python100k.png)


### Python GIL

In none of the above cases did a sub-10 value show up with Python.
This seems to be because of Python's Global Interpreter Lock ('GIL')
which forces all Python threads to run concurrently in one operating system thread.
Indeed, on the Intel CPU, the load was never over 100%, i.e. everything ran concurrent, but not in parallel.

On the RaspberryPi, MicroPython runs with the GIL disabled.
And since the threads both run in parallel (and alone on each core) the uneven scheduling of the two concurrent threads that is needed for sub-10 values, becomes impossible.


Interestingly, by changing: 
```
value = value + 1
```
to 
```
tmp = value
value = tmp + 1
```
(and running 100k increments) does sub 10 values start to show up on the Intel CPU version.

Perhaps the size of the bytecode chunks the Python executer runs no longer is a even multiple of the increment loop body size? Understanding what causes the above would need some deep insight into Python's implementation.

And even more interestingly, the following variant did not cause sub-10 values to show up:
```
tmp = value; value = tmp + 1
```
(NB: this does NOT mean lines are atomic in threaded Python. Only accesses to variables are guaranteed to be atomic according to Python's documentation. If the entire line would be atomic, the value calculated would always be 20. Nevertheless, there clearly is some correlation with lines and variables.)


None of these three versions would show sub-10 values on the RaspberryPi.
