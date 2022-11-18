New Project setup:

-------------------------------------
1. C/C++ Additional Include Directories:
-------------------------------------
Add ..\Common (or Absolute Path to Common)

--------------------------------------
2. Linker Additional Library Directories:
--------------------------------------
Add ..\Common (or Absolute Path to Common)

-----------
3. Libraries:
-----------
Debug libraries:
d3d11.lib;D3DCompiler.lib;Effects11d.lib;dxgi.lib;dxguid.lib;%(AdditionalDependencies)

Release libraries:
d3d11.lib;D3DCompiler.lib;Effects11.lib;dxgi.lib;dxguid.lib;%(AdditionalDependencies)

--------
4. FXC Call - compile shaders
--------

a) Debug mode:   fxc /Fc /Od /Zi /T fx_5_0 /Fo "%(RelativeDir)\%(Filename).fxo" "%(FullPath)"
b) Release mode: fxc /T fx_5_0 /Fo "%(RelativeDir)\%(Filename).fxo" "%(FullPath)"

a) Debug Description: fxc compile for debug: %(FullPath)
b) Release Description: fxc compile for release: %(FullPath)

Outputs: %(RelativeDir)\%(Filename).fxo