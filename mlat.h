#pragma once
#include <eigen/Eigen/Dense>
#include "node_config.h"
using namespace Eigen;
template<const uint8_t NUM_ANCHORS> class MLat {

    private:
        Matrix<double, NUM_ANCHORS, 1> m0;
        Matrix<double, NUM_ANCHORS, 3> G;
        void computeM0() {
            for(size_t i = 0; i < NUM_ANCHORS; i++) {
                m0[i] = (anchors.row(i) - position.transpose()).norm();
            }

        }
        void computeG() {
            for(size_t i = 0; i < NUM_ANCHORS; i++) {
                Matrix<double, 1, 3> row = position.transpose() - anchors.row(i);
                G.row(i) = row.normalized();
            }
        }

    public:
        double residual() {
            return (this->m - this->m0).norm();
        }
        Matrix<double, NUM_ANCHORS, 1> m;
        Matrix<double, 3, 1> position;
        Matrix<double, NUM_ANCHORS, 3> anchors;
        double iterative_step() {
            this->computeG();
            this->computeM0();
            position = G.completeOrthogonalDecomposition().solve(m - m0) + position;

            this->computeM0();
            return this->residual();

        }
        double iterative_solve() {
            size_t count = 0;
            double precision = 1e-5;
            double res;
            while(count++ < 10) {
                res = iterative_step();
                if(res < precision)
                    break;
            }
            return res;

        }
};
extern MLat<6> mlat; // multilateration object
void initialiseMlat();
