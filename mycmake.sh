depdir=/u/ey/jeremym/hps-dev/
cmake .. \
        -DCMAKE_INSTALL_PREFIX=../install \
        -DGDML_DIR=$depdir/gdml/install \
        -DLCDD_DIR=$depdir/lcdd/install \
        -DXERCES_LIBRARY=$depdir/xercesc/xerces-c-3.2.0-install/lib/libxerces-c.so -DXERCES_INCLUDE_DIR=$depdir/xercesc/xerces-c-3.2.0-install/include \
        -DGeant4_DIR=$depdir/geant4/geant4.10.03.p02-install/lib64/Geant4-10.3.2/ \
        -DLCIO_DIR=$depdir/LCIO/install \
        -DCMAKE_BUILD_TYPE=Debug
