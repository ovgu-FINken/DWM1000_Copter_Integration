#include "mlat.h"
MLat<NUMBER_ANCHORS> mlat; // multilateration object
void initialiseMlat() {
    mlat.anchors << 7.18, -3.99,   1.2,
                    0.4, -3.7 ,    1.55,
                    0.4, -1.54,    1.55,
                    7.18, -0.84 ,  1.2;
    mlat.position << .5,.5,.5;
    mlat.m << 1, 1, 1, 1;
}
