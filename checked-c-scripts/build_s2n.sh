echo "This Script Should Be Called From Root Directory, Not s2n-tls/checked-c-scripts"
cmake . -Bbuild -GNinja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_PREFIX_PATH=$(dirname $(dirname $(brew list openssl@1.1|grep libcrypto.dylib))) \
   -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build ./build -j $(nproc)
echo "Finished Building s2n-tls. The compile_commands.json file should be at build/compile_commands.json"

