# Weathering

A student project (made alone) based on *Single Image Weathering via Exemplar Propagation* of Satoshi Iizuka, Yuki Endo, Yoshihiro Kanamori and Jun Mitani. I took a simpler approach of their algorithm to get a firt working version in time.

Exemple : 
![exemple](https://raw.githubusercontent.com/nealith/weathering/develop/exemple.jpg)

100 iterations :

![exemple](https://raw.githubusercontent.com/nealith/weathering/develop/exemple_weathered100.png)

200 iterations :

![exemple](https://raw.githubusercontent.com/nealith/weathering/develop/exemple_weathered200.png)


This project include TextureQuilting, another projet done in 2017-2018, based on *Image Quilting for Texture Synthesis and Transfer* from Alexei A. Efros and William T. Freeman (and made by [fonspa](https://github.com/fonspa), [Hyanaki](https://github.com/Hyanaki), [biscotteman](https://github.com/biscotteman) and me)

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
