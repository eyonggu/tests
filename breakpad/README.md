Example of using breakpad
===================

Steps
========

1. Build breakpad

```bash
git clone https://chromium.googlesource.com/breakpad/breakpad
cd breakpad/
cd src/third_party/
git clone https://chromium.googlesource.com/linux-syscall-support lss
./configure
make -j4
```

2. copy this example to <breakpad>/example

3. make and run

4. Analyze coredump

```c
../src/tools/linux/dump_syms/dump_syms ./crash > crash.sym
head -n1 crash.sym
mv crash.sym ./symbols/crash/<954DC2D5916FD1DDF9745670C1D709AF0>
../src/processor/minidump_stackwalk /tmp/<1c3fd3e8-afcb-4164-d38edd91-45f49ce0.dmp> ./symbols
```

