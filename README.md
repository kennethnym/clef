# clef

clef is an experimental web engine to enable the next generation of distributed and decentralized web apps. it is not built on the existing web standard, but rather a brand new set of technologies optimized for not only static content but also interactive applications.

- clef is not just a browser, but also a cross-platform app runtime that can run web apps on demand, much like how most of the web is today.
- clef should be lightweight to minimize resource consumption while maintaining runtime performance.
- clef will have a universal VM that can execute (multithreaded) applications written in programming languages that can be compiled to the VM's bytecode. WASM is looking promising, but unsure about its performance characteristic. Also, WASM is single-threaded, so I need to somehow add multithreading support to it. or i can just build a new VM.
- all standards of clef will not only be open-source, but also feasibly sized (unlike the current web standard) so that *anyone* can build their own web engine based on the new standard.

