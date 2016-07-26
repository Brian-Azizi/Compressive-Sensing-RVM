#! /bin/bash

./leaf Foreman_L3D_lines.cfg > Data/Simulations/foreman_288-352-288/Foreman_L3D_lines.log &

./leaf Foreman_L3_lines.cfg > Data/Simulations/foreman_288-352-288/Foreman_L3_lines.log &

./leaf Lenna_L6_30.cfg > Data/Simulations/lenna_512-512-1/Lenna_L6_30.log &

./leaf Lenna_L6_50.cfg > Data/Simulations/lenna_512-512-1/Lenna_L6_50.log &

./leaf Lenna_L6D_50.cfg > Data/Simulations/lenna_512-512-1/Lenna_L6D_50.log &

./leaf Foreman_L4_95.cfg > Data/Simulations/foreman_288-352-288/Foreman_L4_95.log &