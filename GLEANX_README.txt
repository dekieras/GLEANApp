You need the contents of three repositories: GLEANKernel, GLEANApp, and TLD_model, which is a sample device model. These are three separate projects;the kernel and app are build-once, but a particular model needs to be built and linked against the kernel, and then run with the app.

First please open and read the file GLEANkernel/GLEANLib/Glean_Legal_notice.txt.  This contains the terms and conditions under which I supply this code to you, and that you need to conform to.

GLEANKernel - this is an Xcode project which contains the platform-idependent C++ class library GLEANLib that implements GLEAN. The Xcode project creates a Mac OS X "Framework" - a dynamic lib with headers that can be linked to by other projects. This framework is copied to a convenient place so that other projects can link to it. The idea is that the only time you have to open and rebuild the GLEANKernel project is if the GLEAN kernel code itself is changed (probably only by me).

GLEANLib contains a folder named "Device Support" that has Widgets.h, .cpp in it. The Widgets are a very simple set of class that correspond to typical GUI class library classes, that simplify creating GUI-like device models.

GLEANApp - which is a Mac OS X Carbon API wrapper for GLEANkernel. You run this app, load a device(environment) model and a .gomsl file, and then run the simulation. It links to the GLEANKernel framework. The idea is that you build this once, then launch it whenever you run your model.

TLD_device.  TLD is for "Top Level Demo" which is a very simple model in which a few colored buttons appear on the simulated display and the user has to click on the red ones. The purpose of the demo is to illustrate how the top level of the user GOMS model might be organized - there are several possibilities documented in the GOMSL code file. The Xcode project builds a dynamic library for the device, which puts up the colored buttons and responds to the simulated user's clicks on them.  This is the simplest demo that I have on hand, and hopefully will allow you to quickly see the basics of how you program a device model.

Normally, the only coding you will do is to program the device model, and once that is settled, you write the .gomsl code that represents the GOMS model for using the device to accomplish tasks. 

I checked the builds of these using Xcode 7.3, Mac OS 10.1.5. The projects must be built at 32-bit applications because the Carbon API is still being used.

Here's how to get started.

I recommend setting Xcode Preferences so that the build results are more easily accessible by being located in the Project folder instead of the default hidden ~/Library

Launch Xcode, ->Preferences ->Locations, set Derived Data: Relative, should show "Project or Workspace Folder/DerivedData.

Then

Open the GLEANKernel project
Clean, then build

Look at your home directory Library folder.
This is hidden from ordinary users in current Mac OS. A way to see this in Mac OS X Finder is to hold down option & command while opening the "Go" menu - you should see "Library" as an option.  Otherwise, open a terminal window and cd to ~/Library/Frameworks. Either way you should see an entry for the just built and copied GLEANKernel.framework.

Close the GLEANKernel project.

Open the GLEANApp project, clean, then build. Should be no problem. 

Now open the TLD_device project, clean, then build. Should be no problem. Close the TLD_device project.

Launch GLEANApp either from the Xcode window, or by finding and double-clicking GleanApp in the DerivedData/Build/Products/Debug or Release - you should be able to drag the app icon out somewhere convenient, such as the desktop.

A set of windows should open on the display, see included first screen shot. The initial window layout stinks, but you can move and resize them as you please.

Click in one of the windows to select GLEANX (the old name of the App), select Control/Load Device.  Navigate to the TLD_device DerivedData/Build/Products/Debug or Release
and select the libTLD_device.dylib and open - this is the dynamic library created by the TLD_device project. You should see a bunch of path information appear in the "Normal output" window. 

Now select GLEANX/Control/Compile and navigate to TLD_device and select and open TopLevelDemo.gomsl. You should see "Model was parsed successfully" and "compiled and executable.  

Now select GLEANX/Control/Go or Run and stuff should start happening. See second screen shot for what it looks like after one Run command.  If you repeat this, eventually the simulation will stop and you'll get some run-time numbers. 

You should be able to play with the different controls, and the .gomsl file.

