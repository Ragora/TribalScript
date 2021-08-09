# TribalScript

This repository is a reimplementation of the Torque Script scripting language (version used in Tribes 2). The intent is to be able to use the scripting
language in an easily embeddable manner outside of Torque Game Engine.

## Building

The project utilizes CMake to perform automatic build generation. However, there is one dependency you must setup on your machine, listed below.

### ANTLR4

See https://github.com/antlr/antlr4/blob/master/doc/getting-started.md for setting up ANTLR4 on your platform. If setup appropriately, CMake should autodetect
your ANTLR4 tool. If it does not, you may set the ANTLR_EXECUTABLE variable in your CMake invocation like:

```
-DANTLR_EXECUTABLE=/usr/local/bin/antlr-4.9.2-complete.jar
```

## Testing

After CMake has generated the makefiles and you have built the project, you may run tests using:

```
ctest
```

## Building Documentation

After CMake has generated the makefiles, you may use the following to build the doxygen documentation:

```
make TribalScriptDocs
```
