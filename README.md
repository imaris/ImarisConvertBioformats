# ImarisConvertBioformats




### Usage

ImarisConvertBioformats is an executable. To get the arguments list and usage examples:

On Windows
```bash
ImarisConvertBioformats.exe --help
 ```
On MacOS and Linux
```bash
./ImarisConvertBioformats --help
 ```

### Dependencies

1. hdf5 version >= 1.10.4: https://www.hdfgroup.org/downloads/hdf5/ (compile with default options, only base C module is required)
1. zlib: https://www.zlib.net/ (compile with default options)
1. lz4: https://github.com/lz4/lz4 (compile with default options)
1. FreeImage: version >= 3.17: https://freeimage.sourceforge.io/ (compile with default options)
1. boost: version >= 1.65 (sucessfully tested with both <1.69 and >=1.69)
	We suggest cmake version >=3.13 for correctly finding new boost versions (starting from boost 1.69, boost::system became a header only library)
	On Mac and Windows compile boost with: 
	> --with-atomic --with-filesystem link=static
	
	On Linux we link dinamically, we do not need
	> link=static

1. java: tested succesfully with both java8 and java11 (we also need jre, or jre-like functionality from jdk)
1. BioFormats: download _bioformats_package.jar_ from https://www.openmicroscopy.org/bio-formats/downloads/  and copy it to the existing empty _bioformats_ folder, contained in the _Imaris Convert Bioformats_ top folder.
1. ImarisWriter: download and extract the source code from https://github.com/imaris/ImarisWriter. Rename the folder containing  _CMakeLists.txt_ as _ImarisWriter_ and copy it in the same folder as bioformats,  fileiobase,  fileiobioformats and  ImarisConvertBioformats. It will be automatically built during ImarisConvertBioformats' building process.

### Build

In order for CMake to find the thirdparty (custom) installations, some or all of the following CMake path variables should be properly set for your system:

>-DHDF5_ROOT
-DZLIB_ROOT
-DLZ4_ROOT
-DBOOST_ROOT
-DJAVA_HOME
-DJRE_HOME
-DFreeImage_ROOT
-DFreeImage_LIBRARIES

For most of the above dependencies, default system installation on Mac and Linux will work and in many cases, ```-D<LibraryName>_ROOT``` will not be necessary.
In case of FreeImage, however, even for system install, one has to define in the cmake command: 
>-DFreeImage_ROOT

and/or 
>-DFreeImage_LIBRARIES


```-DJRE_HOME``` should be set to the folder that contains, depending on the system:
 >lib/server/<dynamic_jvm_library>
 
 or 
> bin/server/<dynamic_jvm_library>

```-DJRE_HOME``` can point to a separate jre installation, or to a relative path to ```JAVA_HOME``` (assuming ```JAVA_HOME``` is set to the _jdk_ folder).

In most installations of Java 8, _jre_ is a subfolder of the _jdk_ folder, but that is not (necessarily) the case for java 11.
For most Java 11 installations, ```JRE_HOME``` could be set to ```$JAVA_HOME```


The ```cmake``` command should be run from _ImarisConvertBioformats_ subfolder.

####Building example on Windows
```Release``` build using Visual Studio, with custom installation for all dependencies:
```bash
cd ImarisConvertBioformats
mkdir build
cd build
cmake -G"Visual Studio 14 Win64" -DHDF5_ROOT="<libs>/hdf5" -DZLIB_ROOT="<libs>/zlib" -DLZ4_ROOT="<libs>/lz4" -DBOOST_ROOT="<libs>/boost" -DJAVA_HOME="<libs>/jdk" -DJRE_HOME="<libs>/jre" -DFreeImage_ROOT="<libs>/FreeImage" ..
devenv.com "ImarisConvertBioformats.sln" /Build "Release|x64" 
devenv.com "ImarisConvertBioformats.sln" /Deploy "Release|x64" /project "INSTALL"
```

A ```debug``` build is similar, one just has to set _Debug_, instead of _Release_ in the build and install steps.
The installation folder is ```build/<build_type>```.

####Building example on Linux/MacOS
```Release``` build, with system path installations for all thirdparty dependencies (so ```<Library_Name>_ROOT``` not necessary):
```bash
cd ImarisConvertBioformats
mkdir build
cd build
cmake .. -DFreeImage_LIBRARIES="<path_to_dynamic_FreeImage_library>" -DJAVA_HOME="<jdk_folder>" -DJRE_HOME="<jre_folder>" ..
make
make install
```
For a ```debug``` build, one has to set:
>-DCMAKE_BUILD_TYPE=Debug 

If ```CMAKE_BUILD_TYPE``` is defined, the installation folder is ```build/{$CMAKE_BUILD_TYPE}```, otherwise just ```build```.
On some MacOS systems, the build target architecture(s) should be set with
>-DCMAKE_OSX_ARCHITECTURES

For example:
>-DCMAKE_OSX_ARCHITECTURES=x86_64

