#include "mlat.h"
MLat<NUMBER_ANCHORS> mlat; // multilateration object
void initialiseMlat() {
    mlat.anchors << .0, .0,        .0,
                    .0, 1.0 ,     .0,
                     1.0, 1.0  , .0,
                     1.0, .0  ,    .0,
                     1.0 , 1.0   ,  .0,
                      1.9 , 0  , .0,
                    0, 0.5  ,  .0,
                    1.9, 0.5   ,  .0;
    mlat.position << .5,.5,.5;
    mlat.m << 1, 1, 1, 1, 1, 1, 1, 1;
}
