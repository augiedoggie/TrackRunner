## ![TrackRunner Logo](Assets/Icons/TrackRunner.svg) TrackRunner

![TrackRunner Menu ScreenShot](Assets/Screenshots/TrackRunnerMenu.png)

_TrackRunner_ is a Tracker add-on for Haiku which runs commands on selected files and folders.  Each command is run with the working directory set and any selected files/folders are passed as arguments to the script.  This allows you to create small Tracker add-ons using interpreted languages like ruby, python, shell scripts, etc...

### Command Management Window

![Commands ScreenShot](Assets/Screenshots/TrackRunnerCommands.png)

In addition to passing command line arguments it is possible to use simple shell redirects (`>`) and the comment character (`#`).  The python web server example from the screenshot above shows the comment character being used to prevent any selected files from being read as arguments.  In this case the command only needs to start up in the current folder.

*Commands with spaces in the file path will have to be escaped or quoted.*

### Command Select Window

![Command Select ScreenShot](Assets/Screenshots/CommandSelectWindow.png)

When the add-on is executed the traditional way it will display a command selection window.  This is done by using the shortcut `opt+alt+x` or `right click -> Add-ons -> TrackRunner` from a Tracker window or the desktop.  In this window you may select and edit one of the existing commands or run a completely new command.  You may also change the working directory or adjust other options.

*Changes made to commands and options in this window are temporary. They do not get saved.*


------------------------------------------------------------


### Build Instructions

In order to build TrackRunner you must have cmake installed.  Optionally, if you wish to build the userguide then you will need the sphinx documentation generator.  These can be installed with the HaikuDepot application or by using the command:
```
~> pkgman install cmake sphinx_python310
```

Once the dependencies are installed, a standard release build of TrackRunner can be created with:
```
~> cd TrackRunner
~/TrackRunner> cmake .
~/TrackRunner> make
~/TrackRunner> make install
```

There are several common options that can also be passed to the cmake command or by using the cmake-gui application.  For example, to create a debug build with the userguide disabled and menuitem icons:
```
~/TrackRunner> cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_USERGUIDE=OFF -DUSE_MENUITEM_ICONS=ON .
```

*Menuitem icons are an experimental hack.  If the internal Tracker menu code changes then this add-on will cause Tracker to repeatedly crash until it is disabled or recompiled against newer Tracker sources code.  This should only be enabled by advanced users.*
