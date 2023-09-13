Crashpad example
================
Demo program from https://github.com/backtrace-labs/crashpad/tree/backtrace/examples/linux/demo

Howto
==================
Some instruction: https://docs.bugsplat.com/introduction/getting-started/integrations/cross-platform/crashpad/how-to-build-google-crashpad

Still use tools from breakpad to analyze the coredump:

```bash
export BREAKPAD=<breakpad>

${BREAKPAD}/src/tools/linux/dump_syms/dump_syms ./crash > crash.sym

#read build ID from the symbol file
head -n1 crash.sym

#the symobol file must be located under <some dir>/<program name>/<build id>/
mkdir -p ./symbols/crash/<build id>
mv crash.sym ./symbols/crash/<build id>

${BREAKPAD}/src/processor/minidump_stackwalk /tmp/<1c3fd3e8-afcb-4164-d38edd91-45f49ce0.dmp> ./symbols
```


