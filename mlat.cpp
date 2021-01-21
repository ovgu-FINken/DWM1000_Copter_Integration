#include "mlat.h"
MLat<NUMBER_ANCHORS> mlat; // multilateration object
void initialiseMlat() {
    mlat.anchors << .0, .0,        .0,
                    .0, -17.2 ,     .0,
                     12.7, -17.2  , .0,
                     12.7, .0  ,    .0,
                     27.7 , .0   ,  .0,
                     27.7 , -17.2  , .0,
                     27.7, 7.3  ,  .0,
                     12.7, 7.3   ,  .0;
    mlat.position << .5,.5,.5;
    mlat.m << 1, 1, 1, 1, 1, 1, 1, 1;
}
