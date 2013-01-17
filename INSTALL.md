Installation and Usage Instructions for the OmniVM
==================================================

This file briefly describes how to compile and use the OmniVM.


Installation
------------

Requirements:

 - GCC or Clang (Clang is used for development)
 - Ubuntu: libx11-dev, libxext-dev
 - Mac OS X: Xcode Developer Tools

Compilation:

In the standard case, calling `./configure ; make` in the build directory
should be sufficient to compile the rvm executable:
```sh
  $ cd vm/build
  $ ./configure
  $ make
```

Compilation for Debugging:
```sh
  $ cd vm/build
  $ ./configure --debug
  $ make
```


Usage
-----

The OmniVM executable supports the following command-line interface:

```
  ./omnivm [options] <image-file> [app-params]
  
 [options]       optional command-line parameters as detailed below 
 <image-file>    a relative path to a Smalltalk image
 [app-params]    parameters given the application executed by the OmniVM
```

Command-line Parameters:

```
 -headless       initializes the OmniVM with a dummy display to avoid opening
                 an X11 session, useful for command-line applications or
                 benchmarks
               
 -min_heap_MB N  sets the lower limit for the overall heap size
```


Preparing a Smalltalk Image
---------------------------

The OmniVM has been developed with a [Pharo 1.4][pharo-download] image.
Prebuilt images can be downloaded from the [Omni project page][omni-project].
To load the code into a fresh Pharo image evaluate the following expression in
a workspace:

```Smalltalk
Gofer new
    squeaksource3: #Omni;
    package: #ConfigurationOfOmniVM;
    load.
(Smalltalk at: #ConfigurationOfOmniVM) loadPhD2013
```

The resulting image relies on the OmniVM to provide the semantics of the OMOP.
To experiment on a standard interpreter, the RoarVM, or CogVM, an alternative
implementation is available as well. It uses AST transformation and does not
require the OmniVM. It can be loaded with:
```Smalltalk
"Lower performance, but does not require special VM support:"
Gofer new
    squeaksource3: #Omni;
    package: #ConfigurationOfOmni;
    load.
(Smalltalk at: #ConfigurationOfOmni) loadPhD2013
```


[pharo-download]: http://www.pharo-project.org/pharo-download/release-1-4
[omni-project]:   http://www.stefan-marr.de/research/omni/
