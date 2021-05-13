# TorqueScript

This repository is a reimplementation of the Torque Script scripting language (version used in Tribes 2). The intent is to be able to use the scripting
language in an easily embeddable manner outside of Torque Game Engine.

## Building

The project utilizes CMake to perform automatic build generation. However, there are two dependencies you must setup on your machine.

### ANTLR4

See https://github.com/antlr/antlr4/blob/master/doc/getting-started.md for setting up ANTLR4 on your platform. If setup appropriately, CMake should autodetect
your ANTLR4 tool. If it does not, you may set he ANTLR_EXECUTABLE variable in your CMake invocation like:

```
-DANTLR_EXECUTABLE=/usr/local/bin/antlr-4.9.2-complete.jar
```

### GTest

Google Test is used for verification that various parts of the compiler and interpreter are working correctly.

See https://github.com/google/googletest

If built and installed to your system, CMake should automatically pick up on your installation.

## Testing

After CMake has generated the makefiles and you have built the project, you may run tests using:

```
ctest
```

## Building Documentation

After CMake has generated the makefiles, you may use the following to build the doxygen documentation:

```
make TorqueCompilerDocs
```
