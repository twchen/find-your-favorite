if [ ! -d emsdk ]; then
    git clone https://github.com/emscripten-core/emsdk.git
    cd emsdk
    ./emsdk install latest
    ./emsdk activate latest
else
    cd emsdk
fi
source ./emsdk_env.sh
