#! /bin/bash

make

./leaf ConfigFiles/Foreman_L3D_lines.cfg &
./leaf ConfigFiles/Foreman_L3_lines.cfg &
./leaf ConfigFiles/Lenna_L6_30.cfg &
./leaf ConfigFiles/Lenna_L6_50.cfg &
./leaf ConfigFiles/Lenna_L6D_50.cfg &
./leaf ConfigFiles/Foreman_L4_95.cfg &

./leaf ConfigFiles/Lola.cfg &
./leaf ConfigFiles/Fibi.cfg &
