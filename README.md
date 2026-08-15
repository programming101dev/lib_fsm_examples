# lib_fsm examples

Small executable examples for `lib_fsm`. Their p101 library dependencies must
be installed first or supplied by the workspace's exact dependency lane.

## Build

```bash
git clone https://github.com/programming101dev/lib_fsm_examples.git
cd lib_fsm_examples
cmake -S . -B build -DCMAKE_C_COMPILER=clang -DP101_BUILD_LEVEL=1
cmake --build build
```

Workspace compiler discovery writes other available choices to
`supported_c_compilers.txt`.
