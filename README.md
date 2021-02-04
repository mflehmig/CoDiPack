# CoDiPack

[CoDiPack](http://www.scicomp.uni-kl.de/software/codi/) (Code Differentiation Package) is a tool for gradient evaluation in computer programs. It supports the features:
  - Forward mode of Algorithmic Differentiation(AD)
  - Reverse mode of Algorithmic Differentiation(AD)
  - Different tape implementations
  - An AdjointMPI interface
  - External functions
  - Higher order derivatives

The design principle for CoDiPack is that it is easy to use.
However, it also gives experienced AD developers the full access to all the data structures.

The [Scientific Computing Group](http://www.scicomp.uni-kl.de) at the TU Kaiserslautern develops CoDiPack and will enhance and extend CoDiPack in the future.
There is a newsletter available at [codi-info@uni-kl.de](https://lists.uni-kl.de/uni-kl/subscribe/codi-info) and if you want to contact us please write a mail to [codi@scicomp.uni-kl.de](mailto:codi@scicomp.uni-kl.de).

[![Build Status](https://travis-ci.org/SciCompKL/CoDiPack.svg?branch=develop)](https://travis-ci.org/SciCompKL/CoDiPack)
[![DOI](https://zenodo.org/badge/37602249.svg)](https://zenodo.org/badge/latestdoi/37602249)

## Usage

CoDiPack is a header only library.
The only file the user needs to include is `codi.hpp`.
The only other requirement is a C++11 compliant compiler
where one usually needs to specify '--std=c++11' in the compiler arguments.
CoDiPack is tested with GCC and the Intel compiler.

The file `codi.hpp` defines the data types `RealForward`, `RealReverse`, `RealReverseUnchecked` and several others.
The `RealForward` type implements the forward mode of AD and
the `RealReverse` type implements the reverse mode of AD.
The third type is also an implementation of the reverse mode of AD but it should only be used by experienced users.
For each type there is also a type with single precession e.g. `RealForwardFloat`.

For further details please visit our [CoDiPack](http://www.scicomp.uni-kl.de/software/codi/) web page.

## Tutorials and Documentation

CoDiPack is shipped with various **tutorials** showing its usage. The tutorials can be found in the directory `documentation`. Invoking `make doc` will compile the tutorials and output the binaries to `build`. The makefile `Makefile` may be adapted to your own needs. All tutorials are documented in great [detail](https://www.scicomp.uni-kl.de/codi/db/d3c/tutorialPage.html).

The **documentation** can be build locally by `make doc` (requires the [Doxygen](https://www.doxygen.nl/index.html) package). The html files are outputted to `build/documentation/html` and the documentation landing page is `index.html`.

## Miscellaneous Information

### Debugging with gdb

The ActiveReal type contains the tape as a static member.
GDB prints the information of these members in its default settings, which makes the output quite verbose.
We recommend to disable the output of the static class members.
This can be done with
~~~~{.txt}
set print static-members off
~~~~

### Intel Compiler Options

Because CoDiPack relies on inlining of the compiler the performance can drop if it is not done or ignored.
Therefore we recommend to force inlining of CoDiPack with the option
~~~~{.txt}
-DCODI_UseForcedInlines 
~~~~

## Hello World Example

A very small and simple example for the use of the RealForward type is the code:

~~~~{.cpp}
    #include <codi.hpp>
    #include <iostream>

    int main(int nargs, char** args) {
      codi::RealForward x = 4.0;
      x.setGradient(1.0);

      codi::RealForward y = x * x;

      std::cout << "f(4.0) = " << y << std::endl;
      std::cout << "df/dx(4.0) = " << y.getGradient() << std::endl;

      return 0;
    }
~~~~

It is compiled with
~~~~{.txt}
  g++  -I<path to codi>/include --std=c++11 -g -o forward forward.cpp
~~~~
for the gcc compiler or with
~~~~{.txt}
  icpc  -I<path to codi>/include --std=c++11 -g -o forward forward.cpp
~~~~
for the Intel compiler.

Please visit the [tutorial page](http://www.scicomp.uni-kl.de/codi/db/d3c/tutorialPage.html) for further information.


## Citation

If you use CoDiPack in one of your applications and write a paper it would be nice if you could cite the paper
[High-Performance Derivative Computations using CoDiPack](https://dl.acm.org/doi/abs/10.1145/3356900).
~~~~{.txt}
@article{SaAlGauTOMS2019,
  title = {High-Performance Derivative Computations using CoDiPack},
  author = {M. Sagebaum, T. Albring, N.R. Gauger},
  url = {https://dl.acm.org/doi/abs/10.1145/3356900},
  year = {2019},
  date = {2019-12-01},
  journal = {ACM Transactions on Mathematical Software (TOMS)},
  volume = {45},
  number = {4},
}
~~~~
