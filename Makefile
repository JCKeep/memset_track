plugin_cxxflags	= -shared -fPIC \
		  -DPLUGIN_VERSION=\"0.0.1\" \
		  -fno-rtti -fno-exceptions -fasynchronous-unwind-tables \
		  -ggdb -Wno-narrowing -Wno-unused-variable \
		  -Wno-format-diag

all: target_dir
	g++ $(plugin_cxxflags) -o target/memset_track_plugin.so src/memset_track_plugin.c \
		-I/usr/lib/gcc/x86_64-linux-gnu/11/plugin/include \
		-I./include \
		-lstdc++
	gcc -O0 -o target/main src/main.c \
		-fplugin=target/memset_track_plugin.so \
		-I./include

target_dir:
	@mkdir -p target

.PHONY: clean

clean:
	rm -rf target