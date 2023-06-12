# Ringen

**Ringen is a C++20 header-only implementation of the disruptor pattern used to communicate between threads in a high-performance producer/consumer arrangement.**

- See the LMAX [technical paper](https://lmax-exchange.github.io/disruptor/files/Disruptor-1.0.pdf) for a description of the theory behind the disruptor.
- See the [Java LMAX Disruptor project](http://lmax-exchange.github.io/disruptor/) for more resources relating to the disruptor.
- See the [Martin Fowler's article](https://martinfowler.com/articles/lmax.html) about LMAX architecture.
 
## Description

A disruptor data structure is essentially a circular buffer queue ring buffer that uses different cursors to keep track of where consumers or producers have processed up to.

A disruptor can be used with either single producer thread or multiple producer
threads. Multiple producer threads are able to publish items out-of-sequence
so that producer threads do not hold up other producer threads unless the
entire ring buffer fills up while waiting for a slow producer to publish its item. Consumers, however, always process items in the sequence they were enqueued.

The disruptor data structure supports batched operations with producers able to
enqueue multiple items with a single synchronisation operation and consumers
that fall behind are able to catch up by consuming batches of consecutive
items with a single synchronisation operation.

When used with a single producer thread with the spin wait-strategy the disruptor uses only atomic reads/writes of integers and acquire/release memory barriers for synchronisation. It does not use CAS operations or locks that require kernel arbitration.

This implementation of the disruptor data-structure is defined fully in header files. There are no libraries to link against and many functions should be able to be inlined. Also, this implementation does not make use of abstract interfaces or virtual function calls, which can inhibit inlining and incur additional runtime overhead, but instead prefers to use templates for compile-time polymorphism.

## Examples

- [Example1](http://www.wavelet.space/ringen/example1.html): A single producer/single consumer use of a disruptor for communication.

## License

The Ringen library is available under the [MIT](./LICENSE.txt) license.

## Building

As this library is a header-only library there is no library component
that needs to be built separately. Simply add the include/ directory
to your include path and include the appropriate headers.

If you want to build the samples/tests then you can use the
[CMake](https://cmake.org/) build system to build this code.

Once you have installed cake you can simply run `cmake` in the root
directory to build everything.

## Performance

*Put some performance results in here.*

## Development

- Generate documentation with Sphinx.

    ```shell
    sphinx-build doc/source doc/build
    ```

- Build examples with CMake one by one.

    ```shell
    cmake --build build --target example1
    cmake --build build --target example2
    ...
    ```

- Format source code.

    ```shell
    clang-format -i .\include\ringen\*.hpp .\examples\*.cpp
    ```

- Lint source code.

    ```shell
    clang-tidy ...
    ```
