# memset_track

For debugging some simple memory problom cause by `memset` or `memcpy`. 
**This is just a simple way to tracking memory problom without ASAN**.

## Usage

1. Register the memory region you want to tracking in your product.
   ```c
   uint8_t tracked_mem[16];

   memset_track_register("test1", (uintptr_t)&tracked_mem, sizeof(tracked_mem));
   ```
   
   If you did not want to track it anymore, you could unregister it.

   ```c
   memset_track_unregister((uintptr_t)&tracked_mem);
   ```

2. Build this GCC Plugin `memset_track_plugin`
   
   ```shell
   # Just run make, or modify Makefile for your custem gcc build.
   $ make
   ```
   If it works, you will got a *memset_track_plugin.so* in target dir 

3. Compile your product with GCC Plugin `memset_track_plugin`
   
   ```
   # Just add param to your gcc
   -fplugin=$(YOUR_CUSTEM_PATH)/memset_track_plugin.so
   ```

## Example

```shell
$ make example && ./target/example/example1
ERROR: memset_s overlaps the memory [tracked_mem] at IP = 0x5a234d6706b1
./target/example/example1(print_stack_trace+0x49)[0x5a234d67047d]
./target/example/example1(memset_track_s+0xd6)[0x5a234d670614]
./target/example/example1(main+0x51)[0x5a234d6706b1]
/lib/x86_64-linux-gnu/libc.so.6(+0x29d90)[0x74d230229d90]
/lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0x80)[0x74d230229e40]
./target/example/example1(_start+0x25)[0x5a234d670185]
buf:         fe fe fe fe fe fe fe fe fe fe fe fe fe fe fe fe 
tracked_mem: fe fe 00 00 00 00 00 00 00 00 00 00 00 00 00 00
```