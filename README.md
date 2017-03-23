# Hardware related (hyper-)graphs

This library derives from the generic concepts formulated in the hypergraph library concepts related to the hardware components and architecture of (robotic) systems.

For example, the processing hardware of a robot can be described by the means of
* Devices
* Interfaces
* Busses
These entities have the following general relations defined:
* Devices have Interfaces
* Busses connect Interfaces
With these concepts both processing devices, there interfaces and their interconnection can be described.

## Status

* Defined Computational Hardware Graph
* First tests work (but are not yet complete)

## ToDo

* Complete the tests
* Use relation/set specific queries when they become available in the hypergraph library
* Make the experimental generator for toplevel VHDL code work again
