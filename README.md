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