# memset_track
tracking the memset_s function for debugging some simple memory faulty. This is just a simple way to tracking memory problom whthout ASAN.

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
$ make && ./target/main
ERROR: memset_s hit the track memory at RET_IP = 0x57943f8f266c
./target/main(+0x1450)[0x57943f8f2450]
./target/main(+0x15d9)[0x57943f8f25d9]
./target/main(+0x166c)[0x57943f8f266c]
/lib/x86_64-linux-gnu/libc.so.6(+0x29d90)[0x7263ed629d90]
/lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0x80)[0x7263ed629e40]
./target/main(+0x1185)[0x57943f8f2185]
buf: fe fe fe fe fe fe fe fe fe fe fe fe fe fe fe fe 
tracked_mem: fe fe 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
```