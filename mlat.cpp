#include "mlat.h"
MLat<6> mlat; // multilateration object
void initialiseMlat() {
    mlat.anchors << 0    , 0,  2.77,
                    4.42 , 0,  2.77,
                    0.17 , 0.16, 0.12,
                    0.18 , 2.88, 0.12,
                    4.92 , 0.14, 0.22,
                    4.92 , 2.95, 0.22;
    mlat.position << .5,.5,.5;
    mlat.m << 0, 1, 1, 1, 1.41, 1;
}
