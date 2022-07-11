gprof example
==============
This example shows how profiling works.

The main trick is "-pg" options specified during compling.

This is the code generated without "-pg" option.
```
000000000000113d <f2>:
    113d:       f3 0f 1e fa             endbr64
    1141:       55                      push   %rbp
    1142:       48 89 e5                mov    %rsp,%rbp
    1145:       48 83 ec 10             sub    $0x10,%rsp
    1149:       89 7d fc                mov    %edi,-0x4(%rbp)
    114c:       89 75 f8                mov    %esi,-0x8(%rbp)

    ....
```

With "-pg" option, calling of mcount (glibc library) is inserted into each function call.
```
00000000000011e7 <f2>:
    11e7:       f3 0f 1e fa             endbr64
    11eb:       55                      push   %rbp
    11ec:       48 89 e5                mov    %rsp,%rbp
    11ef:       48 83 ec 10             sub    $0x10,%rsp
    11f3:       ff 15 ef 2d 00 00       callq  *0x2def(%rip)        # 3fe8 <mcount@GLIBC_2.2.5>
    11f9:       89 7d fc                mov    %edi,-0x4(%rbp)
    ...
```

This trick is also the base of ftrace feature in Linux kernel.

On X86_64, "-mfentry" option coupled with "-pg" can be used to call fentry instead of mcount. Note the difference in where fentry is called.

```
00000000000011e7 <f2>:
    11e7:       f3 0f 1e fa             endbr64
    11eb:       ff 15 07 2e 00 00       callq  *0x2e07(%rip)        # 3ff8 <__fentry__@GLIBC_2.13>
    11f1:       55                      push   %rbp
    11f2:       48 89 e5                mov    %rsp,%rbp
    11f5:       48 83 ec 10             sub    $0x10,%rsp
    11f9:       89 7d fc                mov    %edi,-0x4(%rbp)

    ...

```



##Links
- [https://ftp.gnu.org/old-gnu/Manuals/gprof-2.9.1/html_mono/gprof.html]
- [https://www.linuxjournal.com/content/simplifying-function-tracing-modern-gcc]
