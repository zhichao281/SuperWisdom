#!/bin/bash
PWD=$(dirname $0)
export LD_LIBRARY_PATH=$PWD/lib:$LD_LIBRARY_PATH
$PWD/rtsp_server.bin