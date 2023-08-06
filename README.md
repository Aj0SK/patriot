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