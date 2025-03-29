# memset_track
tracking the memset_s function for debugging some simple memory faulty. This is just a simple way to tracking memory problom without ASAN.

## Usage

1. Register the memory region you want to tracking in your product.
   ```c
   uint8_t tracked_mem[16];

   memset_track_register((uintptr_t)&tracked_mem, sizeof(tracked_mem));
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
ERROR: memset_s hit the track memory at RET_IP = 0x63a64f2f866c
./target/example/example1(+0x1450)[0x63a64f2f8450]
./target/example/example1(+0x15d9)[0x63a64f2f85d9]
./target/example/example1(+0x166c)[0x63a64f2f866c]
/lib/x86_64-linux-gnu/libc.so.6(+0x29d90)[0x790bf6229d90]
/lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0x80)[0x790bf6229e40]
./target/example/example1(+0x1185)[0x63a64f2f8185]
buf: fe fe fe fe fe fe fe fe fe fe fe fe fe fe fe fe 
tracked_mem: fe fe 00 00 00 00 00 00 00 00 00 00 00 00 00 00
```