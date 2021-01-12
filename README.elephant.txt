To build in windows, follow these guidline():
https://wiki.documentfoundation.org/Development/BuildingOnWindows

Build configure option (change the path follow your build setup):
- Release build

$ /cygdrive/h/sources/libo-core/autogen.sh --with-distro=LibreOfficeWin64 --enable-release-build --with-external-tar=/cygdrive/h/sources/lo-externalsrc --with-junit=/cygdrive/c/sources/junit-4.10.jar --with-ant-home=/cygdrive/c/sources/apache-ant-1.9.5 --enable-pch --disable-ccache --disable-dependency-tracking --without-ucrt-dir --with-product-name="Elephant Office"
$ /opt/lo/bin/make
The output will be located at: workdir\installation\ElephantOffice\msi\install\en-US in build folder
(This is msi package, convert it to msix by MSIX Packaging Tool (free app in windows store))

- Debug build:

$ /cygdrive/h/sources/libo-core/autogen.sh --host=x86_64-pc-cygwin --with-external-tar=/cygdrive/h/sources/lo-externalsrc-db --without-junit --with-ant-home=/cygdrive/c/sources/apache-ant-1.9.5 --enable-pch --disable-ccache --without-helppack-integration -disable-dependency-tracking --without-ucrt-dir --with-myspell-dicts --enable-mergelibs --with-product-name="Elephan Office"
$ /opt/lo/bin/make
To run after debug build, run: instdir/program/soffice.exe in build folder
