if [ ! -d build ]; then
	mkdir build
else echo "Directory 'build' exists" 
fi

echo "Building using cmake..."

cmake -B build -S . -G "Unix Makefiles"
cmake --build build -j 6



