plugin_cxxflags	= -shared -fPIC \
		  -DPLUGIN_VERSION=\"0.0.1\" \
		  -fno-rtti -fno-exceptions -fasynchronous-unwind-tables \
		  -ggdb -Wno-narrowing -Wno-unused-variable \
		  -Wno-format-diag

plugin: target_dir
	g++ $(plugin_cxxflags) -o target/memset_track_plugin.so src/memset_track_plugin.c \
		-I/usr/lib/gcc/x86_64-linux-gnu/11/plugin/include \
		-I./include \
		-lstdc++
	gcc -O0 -o target/memset_track.o -c src/memset_track.c -fplugin=target/memset_track_plugin.so -I./include

target_dir:
	@mkdir -p target

target_example_dir:
	@mkdir -p target/example

.PHONY: clean example

clean:
	rm -rf target

example: target_example_dir plugin
	gcc -O0 -o target/example/example1 example/example1.c \
		-fplugin=target/memset_track_plugin.so \
		-I./include
