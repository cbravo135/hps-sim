# HPS Sim

The HPS Sim project provides a physics detector simulation tool for the HPS project.  It reads an LCDD geometry description and simulates particle interactions using LHE, StdHep or LCIO generator input files.  There is also a built-in generator for overlaying beam background events.  There is a flexible set of tools for event overlay and merging, as well as components for transforming the vertices and momenta of the input particle data.

## Build Instructions

***Warning:*** Building all of this is not trivial, and it is time consuming. 

The following instructions assume that you are using some flavor of Linux, or POSIX compliant operating system. This has been successfully tested on Centos 7 and MacOS (10.13 other will probably work as well).

### Prerequisites

* A modern compiler (gcc 4.8.5 or later, must be c++11 compliant)
* cmake version 3 or later.
* A moderate level of Linux skills.


#### CMake 
You should have at least CMake 3.0 installed on your machine, and preferably a current version from the [CMake website](https://cmake.org). The installation will not work with any 2.x version of cmake, which is too old. 

On many system, you may need to invoke the version 3 cmake with "cmake3".

##### Cmake usage.

Details of cmake are in the [cmake documentation](https://cmake.org/cmake/help/v3.8/index.html). There are a number of general cmake options you should be aware of. You activate these as -DCMAKE_INSTALL_PREFIX=/my/output/dir, or by setting this as an environment variable:

* CMAKE\_INSTALL\_PREFIX  - determines where the produced libraries and binaries are installed on a "make install". This can be some common location for all your packages, or individual directories. It defaults to `/usr/local`, which is probably _not_ what you want. 
* CMAKE\_PREFIX\_PATH       - Points to a set of locations, separated by :, where cmake should look for additional configuration files. You use this for instance to find the correct version of GEANT4, or Qt.

#### GCC

Support for the C++11 standard is required so you will need a version of gcc 4.8 or greater.

### External Dependencies

You will first need to install or have available at your site a number of external dependencies before building the actual framework.

#### CLHEP

The install of CLHEP is optional, since this now come bundled with GEANT4. It is good to install anyway. 

For Linux or MacOS:

``` bash
git clone https://gitlab.cern.ch/CLHEP/CLHEP.git
git checkout CLHEP_2_4_1_0
mkdir build; cd build
cmake -DCMAKE_INSTALL_PREFIX=../install ..
make install
```

On MacOS you can alternatively install CLHEP with ```brew install clhep```, once you have [brew installed](https://brew.sh)

#### Xerces-c

You can install Xerces-c on Centos with: ```yum install xerces-c-devel```
If you are not so fortunate to have control of the system, or a decent sysadmin, you may need to build it yourself. You get the Xerces-c XML parser from: [Xerces C++](https://xerces.apache.org/xerces-c/). To build it (SLAC example):

```
tar xzvf xerces-c-3.2.2.tar.gz
cd xerces-c-3.2.2
mkdir build
cd build
cmake --DCMAKE_INSTALL_PREFIX=/nfs/slac/g/hps3/software/simulation/xerces-c-3.2.2
make -j8
make install
export XercesC_DIR=../
```
Setting of the variable ```XercesC_DIR``` is completely optional and for convinience. It makes installation of other dependencies easier.  It will be used throughout this README. 


#### GEANT4

GEANT4 is the underlying library that does all the heavy lifting for particle transport and particle-matter interactions. It is a beast to install, but required.
On JLab or SLAC machines, you can find pre-compiled versions, however these may not have the features (graphics) you are looking for.

On a Mac, if you do not want to configure this yourself, you can install it with ```brew install geant4```. It takes a while to complete.

Details for installing GEANT4 yourself are at: [Geant4](http://geant4.cern.ch/)

For a fully graphics capable version of GEANT4, I recommend going through the effort of installing Qt5 _first_, and then compiling GEANT4. 

##### Qt5

You can get a pre-compiled version of Qt with a Qt installer from [Qt.io](https://www.qt.io/download) (These are multi-GB downloads). They now make it difficult to get the sources and compile it yourself. [You can find the details here](https://wiki.qt.io/Building_Qt_5_from_Git). Make sure you get the sub-modules.

To build Qt5 it not so bad:
```bash
configure --prefix=/nfs/slac/g/hps3/software/simulation/Qt5.9.1
make -j 16
make install
```
It takes several hours, depending on your system.

#### Installing GEANT4

Once you have Qt5 installed, you build and install GEANT4 with these commands (example from the SLAC install). We want to make sure the source and install directories are _separate_!

Some options to consider:

* If you have a system installed Xerces-c, then cmake will find it for you, so no need for the -DXERCESC flags. 
* You must give the CMAKE\_PREFIX\_PATH for cmake to find the correct Qt5 version.
* You can install the data by hand, then set `-DGEANT4_INSTALL_DATA=OFF`.

```
tar xzvf geant4.10.03.p03.tar.gz
mv  geant4.10.03.p03 geant4.10.03.p03_src
cd geant4.10.03.p03_src
mkdir build
cd build
cmake3 -DCMAKE_PREFIX_PATH=/nfs/slac/g/hps3/software/simulation/Qt5.9.1 -DXERCESC_INCLUDE_DIR=/nfs/slac/g/hps3/software/simulation/xerces-c-3.2.2/include -DXERCESC_LIBRARY=/nfs/slac/g/hps3/software/simulation/xerces-c-3.2.2/lib64/libxerces-c-3.2.so -DGEANT4_INSTALL_DATA=ON -DGEANT4_USE_GDML=ON -DGEANT4_USE_QT=ON -DGEANT4_USE_OPENGL_X11=ON -DBUILD_SHARED_LIBS=ON -DBUILD_STATIC_LIBS=ON -DCMAKE_INSTALL_PREFIX=/nfs/slac/g/hps3/software/simulation/geant4.10.03.p03 ..
```

#### Install GDML

You get the code here: [GDML](https://github.com/slaclab/gdml)

Example install (SLAC):

```bash
git clone https://github.com/slaclab/gdml
cd gdml
mkdir build
cd build
cmake3 -DCMAKE_INSTALL_PREFIX=/nfs/slac/g/hps3/software/simulation/gdml -DCMAKE_PREFIX_PATH="/nfs/slac/g/hps3/software/simulation/xerces-c-3.2.2;/nfs/slac/g/hps3/software/simulation/geant4.10.03.p03/lib64/Geant4-10.3.3"  .. 
make -j 8
make install
```

An alternative to using the variable ```CMAKE_PREFIX_PATH``` is to just pass the location of Xerces-c directly as follows: 

```bash

cmake3 -DXERCES_DIR=$XercesC_DIR -DCMAKE_INSTALL_PREFIX=../install ..
export GDML_DIR=../install
```
where ```install``` is the installation passed to CMake via the command ```CMAKE_INSTALL_PREFIX```. Setting of the environmental vairable ```GDML_DIR``` is optional but faciliates the installation of other packages. 

#### Install LCDD

You get the code here: [LCDD](https://github.com/slaclab/lcdd)
Note that on some (not all) systems, LCDD will not compile properly because the compiler does not default to a C++11 compiler. You need to add the flag `-DCMAKE_CXX_FLAGS="-std=c++11"` to the cmake command. Since LCDD depends on GDML, XERCESC and GEANT4, we need to tell it where those are.

NOTE: If you have ROOT in your path (i.e. you initialized ROOT), then it will often happen that cmake picks up the ROOT version of the GDML library, which will not lead to functional code. (but you won't know until much later.) Make sure that ROOT is not initialized.

Example install (SLAC):

```bash
git clone https://github.com/slaclab/lcdd
cd lcdd
mkdir build
cd build
cmake3 -DCMAKE_INSTALL_PREFIX=/nfs/slac/g/hps3/software/simulation/lcdd  -DGDML_DIR=/nfs/slac/g/hps3/software/simulation/gdml -DGDML_LIBRARY=/nfs/slac/g/hps3/software/simulation/gdml/lib/libgdml.so -DCMAKE_PREFIX_PATH="/nfs/slac/g/hps3/software/simulation/xerces-c-3.2.2;/nfs/slac/g/hps3/software/simulation/geant4.10.03.p03/lib64/Geant4-10.3.3"  -DCMAKE_CXX_FLAGS="-std=c++11"  ..
make -j 8
make install
```

#### Install LCIO

For LCIO, we _must_ get a _specific version_: v02-07-05. If not, then the C++ produces lcio files cannot be read by the Java version. The code comes from: [LCIO](http://geant4.cern.ch/). If you want to use the ROOT interface, make sure it is installed and initialized.

Example install (SLAC):
```bash
git clone https://github.com/iLCSoft/LCIO
git checkout v02-07-05
cd lcio
mkdir build
cmake  -DBUILD_ROOTDICT=on  -DCMAKE_CXX_FLAGS="`root-config --cflags`"  -DCMAKE_INSTALL_PREFIX=/nfs/slac/g/hps3/software/simulation/lcio  -DCMAKE_SHARED_LINKER_FLAGS="`root-config --libs`" -DCMAKE_BUILD_TYPE=Release ..
make -j 8
make install
```

You should now have the pre-requisites to:

## Building hps-sim

To start, clone the project to your machine.

```
git clone https://github.com/jeffersonlab/hps-sim
```

Next, a build dir should be created.

```
cd hps-sim
mkdir build
```

From the build dir, run CMake to configure the build.

```
cmake [args] ..
```

The particulars of the CMake arguments will depend on your software installations.

For the install at SLAC:

```
hps_sim_dir=/nfs/slac/g/hps3/software/simulation
cmake3 .. \
        -DCMAKE_INSTALL_PREFIX=${hps_sim_dir}  \
        -DXERCES_DIR=${hps_sim_dir}/xerces-c-3.2.2 \
        -DGDML_DIR=${hps_sim_dir}/gdml \
        -DGDML_LIBRARY=${hps_sim_dir}/gdml/lib/libgdml.so \
        -DLCDD_DIR=${hps_sim_dir}/lcdd \
        -DGeant4_DIR=${hps_sim_dir}/geant4.10.03.p03/lib/Geant4-10.3.2 \
        -DLCIO_DIR=${hps_sim_dir}/lcio \
		 -DCMAKE_BUILD_TYPE=Release
```

Now, you can run Make to build the project, also from your build directory:

```
make -j4 install
```

You should now be able to run the `hps-sim` program if this completes successfully.

## Running the Application

There is a shell script that is automatically created which can be used to setup the run environment:

```
cd hps-sim
. hps-sim/install/bin/hps-sim-env.sh
```

Now you can check if the program is available from the command line:

```
which hps-sim
```

There are two modes available for running the application.

In the batch mode, a macro is provided to run a non-interactive job.

```
hps-sim run.mac
```

You can also run the simulation interactively by not providing any arguments:

```
hps-sim
```

In this mode you will get a Geant4 command prompt where macro commands can be executed:

```
PreInit> /control/execute run.mac
```

After the session is over, you may type `exit` in the prompt to quit the Geant4 shell.

## Macro Commands

HPS Sim is controlled by a macro command language defined in Geant4.  Many custom commands are available for loading data, transforming it, and configuring the output.

Here is a very simple example of running a test job and writing some LCIO output:

```
# load the detector
/lcdd/url detector.lcdd

# use a test particle generator
/hps/generators/create BeamGen TEST

# initialize the run
/run/initialize

# configure output settings
/hps/lcio/file test.slcio
/hps/lcio/recreate

# run events
/run/beamOn
```

There are many other macro examples in the [macros directory](https://github.com/JeffersonLab/hps-sim/tree/master/macros) of the project.

## Additional References

[New HPS Sim Application](https://confluence.slac.stanford.edu/download/attachments/227174909/HPS%20New%20Sim%20Application.pptx?version=1&modificationDate=1508272655371&api=v2) - slides on new HPS simulation application
