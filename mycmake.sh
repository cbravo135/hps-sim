#!/bin/bash

depdir=/u/ey/jeremym/hps-dev/hps-sim-deps

cmake .. \
        -DXERCES_DIR=$depdir/xercesc/xerces-c-3.2.0/install/ \
        -DCMAKE_INSTALL_PREFIX=../install \
        -DGDML_DIR=$depdir/gdml/install \
        -DLCDD_DIR=$depdir/lcdd/install \
        -DGeant4_DIR=$depdir/geant4/geant4.10.02.p03-install/lib64/Geant4-10.2.3/ \
        -DLCIO_DIR=$depdir/LCIO/install \
        -DCMAKE_BUILD_TYPE=Debug
