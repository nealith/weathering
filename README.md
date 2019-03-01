# Weathering

Based on *Single Image Weathering via Exemplar Propagation* of Satoshi Iizuka, Yuki Endo, Yoshihiro Kanamori and Jun Mitani

This project include TextureQuilting, another projet done in 2017-2018

## Requirements :

- OpenCV version > 3.4
- [NLOpt](https://github.com/stevengj/nlopt)

## To build :

```bash

$ git clone https://github.com/nealith/weathering.git
$ cd weathering
$ git checkout develop
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
