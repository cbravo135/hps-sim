# HPS Sim

The HPS Sim project provides a physics detector simulation tool for the HPS project.  It reads an LCDD geometry description and simulates particle interactions using LHE, StdHep or LCIO generator input files.  There is also a built-in generator for overlaying beam background events.  There is a flexible set of tools for event overlay and merging, as well as components for transforming the vertices and momenta of the input particle data.

## Build Instructions

The following instructions assume that you are using some flavor of Linux. Mac/OSX may also work but has not been tested yet.

### Prerequisites

#### CMake

You should have at least CMake 3.0 installed on your machine, and preferably a current version from the [CMake website](https://cmake.org). The installation will not work with any 2.x version of cmake, which is too old.

#### GCC

Support for the C++11 standard is required so you will need a version of gcc 4.8 or greater.

### External Dependencies

You will first need to install or have available at your site a number of external dependencies before building the actual framework.

#### CLHEP

``` bash
git clone https://gitlab.cern.ch/CLHEP/CLHEP.git
git checkout CLHEP_2_4_1_0
mkdir build; cd build
cmake -DCMAKE_INSTALL_PREFIX=../install ..
make install
```

HPS Sim also has the following software dependencies:

- [GDML](https://github.com/slaclab/gdml)
- [LCDD](https://github.com/slaclab/lcdd)
- [Xerces C++](https://xerces.apache.org/xerces-c/)
- [Geant4](http://geant4.cern.ch/)
- [LCIO](http://geant4.cern.ch/)

For now, these must each be installed separately.

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

My CMake command looks this this:

```
# dependency root dir
depdir=/u/ey/jeremym/hps-dev/

# cmake command with all dependencies specified
cmake .. \
        -DCMAKE_INSTALL_PREFIX=../install \
        -DGDML_DIR=$depdir/gdml/install \
        -DLCDD_DIR=$depdir/lcdd/install \
        -DXERCES_LIBRARY=$depdir/xercesc/xerces-c-3.2.0-install/lib/libxerces-c.so -DXERCES_INCLUDE_DIR=$depdir/xercesc/xerces-c-3.2.0-install/include \
        -DGeant4_DIR=$depdir/geant4/geant4.10.02.p03-install/lib64/Geant4-10.2.3/ \
        -DLCIO_DIR=$depdir/LCIO/install \
        -DCMAKE_BUILD_TYPE=Debug
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
