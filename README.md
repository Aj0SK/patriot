# Installation

Source of steps is [blog](https://web.archive.org/web/20230806114409/https://arielm.github.io/cross-blog/2022/10/06/hello-world.html).

```
sudo apt-get install git
sudo apt-get install g++
sudo apt-get install cmake ninja-build
sudo apt-get install libboost-system-dev libboost-filesystem-dev
sudo apt-get install xorg-dev
```

Assumess storing installations in `~/installs`:

```
cd ~/installs
git clone --recurse-submodules https://github.com/arielm/chronotext-cross
cd chronotext-cross
source setup.sh
cd $CROSS_PATH/core/src
./build.linux.sh
```

To check chronotext-cross installation:

```
cd $CROSS_PATH/tests/TestingTriangle
RUN_TEST -DPLATFORM=linux
```

and... you should see a triangle.

## Installation to run in a web browser

Source is the same blog, different [blogpost](https://web.archive.org/web/20221008124053/https://arielm.github.io/cross-blog/2022/10/06/running-in-the-browser.html).

Again this assumption of storing stuff in `~/installs`:

```
cd ~/installs
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
```

now a little more personal stuff (replace `JOZEF` with the actual user name):

```
export EMSCRIPTEN_PATH=/home/JOZEF/installs/emsdk/upstream/emscripten
cd $EMSCRIPTEN_PATH
source ../../emsdk_env.sh
```

and again testing of successful installation:

```
cd $EMSCRIPTEN_PATH
emcc test/hello_world.cpp -s USE_ZLIB=1
```

Again some cross stuff:

```
cd $CROSS_PATH/core/src
./build.emscripten.sh
```

and final test (with little hack for Linux to change word `safari` to `chrome` in the setup):

```
cd $CROSS_PATH/tests/TestingTriangle
sed -i 's/safari/chrome/g' $CROSS_PATH/core/cmake/emscripten/run.browser.sh.in
RUN_TEST -DPLATFORM=emscripten
```

# Running

```
RUN_TEST -DPLATFORM=linux
```