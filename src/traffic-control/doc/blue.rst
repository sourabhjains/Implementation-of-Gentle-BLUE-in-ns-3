.. include:: replace.txt
.. highlight:: cpp

BLUE queue disc
---------------

This chapter describes the BLUE [Feng99]_ queue disc implementation in |ns3|. 

Like RED, BLUE is a queuing discipline that aims to control the queue length
at routers.

Model Description
*****************

The source code for the BLUE model is located in the directory ``src/traffic-control/model``
and consists of 2 files `blue-queue-disc.h` and `blue-queue-disc.cc` defining a BlueQueueDisc
class. The code was ported to |ns3| by Mohit P. Tahiliani, Vivek Jain and Sandeep Singh
based on ns-1.1 code implemented by Wu-chang Feng and the Linux kernel code of Stochastic Fair
BLUE.  

* class :cpp:class:`BlueQueueDisc`: This class implements the main BLUE algorithm:

  * ``BlueQueueDisc::DoEnqueue ()``: This method checks whether the queue is full, and if so, drops the packets and records the number of drops due to queue overflow and calls method ``BlueQueueDisc::IncrementPmark()``. If queue is not full, this method calls ``BlueQueueDisc::DropEarly()``, and depending on the value returned, the incoming packet is either enqueued or dropped.

  * ``BlueQueueDisc::DropEarly ()``: The decision to enqueue or drop the packet is taken by invoking this method, which returns a boolean value; false indicates enqueue and true indicates drop.

  * ``BlueQueueDisc::IncrementPmark ()``: This method increases the drop probability when there is heavy congestion in queue.

  * ``BlueQueueDisc::DecrementPmark ()``: This method decrements the drop probability when link is idle for certain time.

  * ``BlueQueueDisc::DoDequeue ()``: This method dequeues the packet from queue and if queue is idle, this initializes idleStartTime.  

References
==========

.. [Feng99] W. Feng, D. Kandlur, D. Saha, K. Shin (1999, April). The Blue Queue Management Algorithms, IEEE/ACM Transactions on Networking, Vol. 10, No. 4.  Available online at `<http://www.thefengs.com/wuchang/blue/ToN-02.pdf>`_.

Attributes
==========

The key attributes that the BlueQueueDisc class holds include the following: 

* ``Mode:`` BLUE operating mode (BYTES or PACKETS). The default mode is PACKETS. 
* ``QueueLimit:`` The maximum number of bytes or packets the queue can hold. The default value is 25 bytes / packets.
* ``MeanPktSize:`` Mean packet size in bytes. The default value is 1000 bytes.
* ``Increment:`` Decrement value for marking probability. The default value is 0.0025.
* ``Decrement:`` Increment value for marking probability. The default value is 0.00025.
* ``FreezeTime:`` Time interval during which Pmark cannot be updated. The default value is 100 ms. 
* ``LastUpdateTime:`` Last time at which drop probability is changed.
* ``PMark:`` Value of drop probability.

Examples
========

The example for BLUE is `red-vs-blue.cc` located in ``src/traffic-control/examples``.  To run the file (the first invocation below shows the available command-line options):

:: 

   $ ./waf --run "red-vs-blue --PrintHelp"
   $ ./waf --run "red-vs-blue --queueDiscType=BLUE"

Validation
**********

The BLUE model is tested using :cpp:class:`BlueQueueDiscTestSuite` class defined in `src/traffic-control/test/blue-queue-disc-test-suite.cc`. The suite includes 4 test cases:

* Test 1: enqueue/dequeue with no drops and makes sure that BLUE attributes can be set correctly.
* Test 2: default values for BLUE parameters
* Test 3: higher increment value for Pmark
* Test 4: lesser time interval for updating Pmark

The test suite can be run using the following commands: 

::

  $ ./waf configure --enable-examples --enable-tests
  $ ./waf build
  $ ./test.py -s blue-queue-disc

or  

::

  $ NS_LOG="BlueQueueDisc" ./waf --run "test-runner --suite=blue-queue-disc"

