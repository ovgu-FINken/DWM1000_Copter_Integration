#include "mlat.h"
MLat<5> mlat; // multilateration object
void initialiseMlat() {
    mlat.anchors << 0  ,  0,  0,
                    .45,.60,  0,
                    .0 ,.60,  0,
                    .0 ,  0,.30,
                    .45,  0,.30;
    mlat.position << .5,.5,.5;
    mlat.m << 0, 1, 1, 1, 1.41;
}
