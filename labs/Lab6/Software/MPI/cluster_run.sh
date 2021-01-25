#!/bin/bash
time mpirun --mca btl_tcp_if_include eth0 -n 4 ./next_prime

