#include "mlat.h"
MLat<8> mlat; // multilateration object
void initialiseMlat() {
    mlat.anchors << .0  , .0   ,  .0,
                    .0  , 10.0 ,  .0,
                    10.0, 10.0 ,  .0,
                    10.0, .0   ,  .0,
                    5.0 , 2.5  ,  .0,
                    2.5 , 5.0  ,  .0,
                    5.0 , 7.5  ,  .0,
                    7.5 , 5.0  ,  .0;                    ;
    mlat.position << .5,.5,.5;
    mlat.m << 0, 1, 1, 1, 1.41, 1, 1, 1;
}
