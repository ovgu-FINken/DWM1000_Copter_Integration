#include "mlat.h"
MLat<NUMBER_ANCHORS> mlat; // multilateration object
void initialiseMlat() {
    mlat.anchors << .0, .0,        .0,
                    .0, -10.0 ,     .0,
                     10.0, -10.0  , .0,
                     10.0, .0  ,    .0,
                     20.0 , .0   ,  .0,
                     20.0 , -10.0  , .0,
                     20.0, 10.0  ,  .0,
                     10.0, 10.0   ,  .0;
    mlat.position << .5,.5,.5;
    mlat.m << 1, 1, 1, 1, 1, 1, 1, 1;
}
