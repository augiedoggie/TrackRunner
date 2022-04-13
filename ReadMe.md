## ![TrackRunner Logo](Assets/Icons/TrackRunner.svg) TrackRunner

![TrackRunner Menu ScreenShot](Assets/Screenshots/TrackRunnerMenu.png)

_TrackRunner_ is a Tracker addon for Haiku which runs commands on selected files and folders.  Each command is run with the working directory set and any selected files/folders are passed as arguments to the script.  This allows you to create small Tracker addons using interpreted languages like ruby, python, shell scripts, etc...

![TrackRunner Commands ScreenShot](Assets/Screenshots/TrackRunnerCommands.png)

In addition to passing command line arguments it is possible to use simple shell redirects (`>`) and the comment character (`#`).  The python web server example from the screenshot above shows the comment character being used to prevent any selected files from being read as arguments.  In this case the command only needs to start up in the current folder.

*Commands with spaces in the file path will have to be escaped or quoted*


------------------------------------------------------------


### Build Instructions

```
~> cd TrackRunner
~/TrackRunner> cmake .
~/TrackRunner> make
```
