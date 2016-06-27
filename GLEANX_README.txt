How to install and use GLEANX.
INSTALLATION AND TEST

Get an exported copy of the GLEAN DeviceProjTemplate from the server. Follow the README instructions on how to install it. You won't be able to build a device dylib using the template until the GLEANKernel framework has been built and installed.

Download either an exported or version-controlled (Checkout) copy of the GLEANKernel and GLEANApp projects.

Start Xcode and open the GLEANKernel project and hit build (either debug or release). The result should be the GLEANKernal.framework in both the build folder and /Library/Frameworks.

Close the GLEANKernel project. 

Open the GLEANApp project, select the GLEANApp target, and hit build (either debug or release). The result should be the GLEANApp Debug (or Release) in the build folder. Do the same for the Dummy_device target, and at your option, for the CLGLEAN (Command-line interface GLEAN) targets.

Close the GLEANApp project.

Create a new project in Xcode; the GLEAN device dylib template should appear in the Custom Projects section. Choose it and Call it "TestDevice" or similar. The template includes a Demo_device that implements a simple device suitable for demonstrating different top-level organization for GOMS models. Build the device dylib by hitting build (either Debug or Release).
 
Launch GLEANX either by double-clicking on the GLEANX.app icon or running it inside the GLEANApp project.  Load the demo device dylib and compile the Demo_device.gomsl file (included in the GLEAN DeviceProjTemplate directory). It should run successfully.

Quit and relaunch GLEANX, load the Dummy_device dylib, and a gomsl file such as Mac_file_tasks.gomsl. It should run successfully.

You should be able to copy GLEANX.app to any location of your choice (e.g. the Applications folder), and likewise you can move the device dylibs to a convenient location, such as in the same directory as the gomsl and other relevant files. You can clean the GLEANKernel and GLEANApp project or delete its build directory. 

CREATING YOUR OWN DEVICE

Start a new project with the GLEAN device dylib template, and add your own source code files to it, and delete (and trash) the Demo_device ones. 

Note that any #includes of the GLEANKernel files, like "Symbol.h", will have to be modified to specify the GLEANKernel framework directory:

#include "GLEANKernel/Symbol.h"

or similar. Note that Xcode automatically searches /Library/Frameworks for GLEANKernel, so a complete path is not needed. If you get a zillion errors on GLEANKernel symbols, Device_base functions, or the like, it is probably because the framework directory is missing from your includes.

If you open the GLEANKernel framework icon in Xcode, you can see which GLEANKernel headers are available and have to be referred to in this way.

DEBUGGING YOUR DEVICE
You can't execute your device dylib by itself, so you can't simply run or debug from your device project. There are two ways to handle this:

1. Open the GLEANApp project and build and run in Debug mode. Open your device project to set breakpoints in the device code. Note that the debug symbol mode is set to DWARF and debug symbols are created in both Debug and Release mode with the supplied project settings for GLEANApp, GLEANKernel and the device project template.

2. You can add an execution environment to your device project: In your device project, select Project/New custom executable ... and provide a name like "GLEAN debug" and then navigate to and select the GLEANX.app executable that you want to use. Set any other settings, especially the path, that might be useful. This enables the build & run and build & debug options, which will start GLEANX.app for you. Choose Build and debug, and then load the device dylib created from the same project, where you can set break points.  



