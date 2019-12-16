# Weathering

A student project (made alone) based on *Single Image Weathering via Exemplar Propagation* of Satoshi Iizuka, Yuki Endo, Yoshihiro Kanamori and Jun Mitani. I took a simpler approach of their algorithm to get a firt working version in time.

Exemple :
![exemple](https://raw.githubusercontent.com/nealith/weathering/develop/exemple.jpg)

100 iterations :

![exemple](https://raw.githubusercontent.com/nealith/weathering/develop/exemple_weathered100.png)

200 iterations :

![exemple](https://raw.githubusercontent.com/nealith/weathering/develop/exemple_weathered200.png)


This project use  [TextureQuilting](https://github.com/nealith/texture-quilting.git)

## Requirements :

- OpenCV version > 3.4
- [NLOpt](https://github.com/stevengj/nlopt)

## To build :

```bash

$ git clone https://github.com/nealith/weathering.git
$ cd weathering
$ git checkout develop
$ git submodule init
$ git submodule update
$ mkdir build
$ cd build
$ cmake ..
$ make

```

## To use :

```bash

$ ./testWeathering <image path> [enable grabcut]

```

enable grabcut is 0 or 1 (1 to enable), by default enabled
