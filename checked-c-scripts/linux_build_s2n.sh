echo "This Script Should Be Called From Root Directory, Not s2n-tls/checked-c-scripts"
#rm -rf build

CC=/home/dhananjay/CheckedC/build/bin/clang cmake . -Bbuild \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_C_FLAGS="-D_GNU_SOURCE" \
    -DBUILD_TESTING=OFF \
    -GNinja
cmake --build ./build -- -j12
echo "Finished generating compilation database for s2n-tls. The compile_commands.json file should be at build/compile_commands.json"

