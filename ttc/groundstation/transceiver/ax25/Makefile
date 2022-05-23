# NOTE: ensure your G++ and Python bitlevels match
# (eg both 64 or both 32 - I (adam) use 32)

ax25: ax25.cpp
	g++ -c -fPIC ax25.cpp -o ax25.o
	g++ -shared -Wl,-soname,ax25.so -o ax25.so ax25.o