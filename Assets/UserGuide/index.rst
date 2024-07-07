
.. title:: TrackRunner User Guide

.. toctree::
   :maxdepth: 2
   :hidden:

.. contents:: User Guide Contents
   :depth: 3
   :local:
   :backlinks: none



Writing Scripts
---------------


TrackRunner will set the working directory and pass any selected files/folders as arguments to the command.  This
allows you to create small Tracker add-ons using many interpreted languages.  See the `Other Command & Script Examples`_ section
for more.

.. code-block:: bash
   :caption: A simple bash example to display the selected arguments

   #!/bin/bash
   echo "PWD: $PWD"

   if (($# == 0)); then
      echo "No command line arguments given"
   else
      for ((i = 1; i <= $#; i++)); do
         echo "Arg $i: ${!i}"
      done
   fi



Tracker Context Menu
--------------------

.. image:: ../Screenshots/TrackRunnerMenu.png
   :alt: Tracker context menu
   :align: center


The context menu allows you to directly execute one of your saved commands as well as open the settings or
management window.

..
  .. note::
     Menuitem icons are only available when TrackRunner is built from source.  They are not available in packaged releases.

Command Management Window
-------------------------

.. image:: ../Screenshots/TrackRunnerCommands.png
   :alt: Command management window
   :align: center


The command management window allows you to add/edit/remove/sort shortcuts.
You can drag'n'drop a script to replace the currently shown command path.
If the command is a script, the `Show in Tracker` button will open its location in a Tracker window. From there you can rename it or open it in an editor etc.


Command List
^^^^^^^^^^^^

The list of commands can be sorted by drag and drop.  This will change the order in the `Tracker Context Menu`_.


Command
^^^^^^^

In addition to passing command line arguments it is possible to use simple shell redirects (`>`) and the comment
character (`#`).  The python web server example from the screenshot above shows the comment character being used to
prevent any selected files from being read as arguments.  In this case the command only needs to start up in the
current folder.

*Commands with spaces in the file path will have to be escaped or quoted.*


Run in Terminal
^^^^^^^^^^^^^^^

Use the Terminal application to run the command so that the output can be viewed.



Command Select Window
---------------------

.. image:: ../Screenshots/CommandSelectWindow.png
   :alt: Command select window
   :align: center


The command select window is activated by using the add-on in the traditional BeOS/Haiku way.  This is done by using
the shortcut `opt+alt+x` or `right click -> Add-ons -> TrackRunner` from a Tracker window or the desktop.  In this
window you may select and edit one of the existing commands or run a completely new command.

*Changes made to the command are temporary and not saved*


Command
^^^^^^^

Allows you to edit the command that will be executed.


Directory
^^^^^^^^^

Allows you to change the working directory to something other than where the add-on was executed from.


Run in Terminal
^^^^^^^^^^^^^^^

Use the Terminal application to run the command so that the output can be viewed.



Settings
-----------

.. image:: ../Screenshots/TrackRunnerPrefs.png
   :alt: Settings window
   :align: center


Menu label
^^^^^^^^^^

Allows you to change the menu label for the `Tracker Context Menu`_.

*The label must be at least 4 characters long*


..
  Use icons in menus
  ^^^^^^^^^^^^^^^^^^

  Use the TrackerRunner logo in the top level menu and attempt to load icons for any custom commands.

  *This feature is experimental and only enabled when TrackRunner is built from source*



Other Command & Script Examples
-------------------------------

Basic Commands
^^^^^^^^^^^^^^

+-----------------------------------------+----------------------------------------------------+
| Command                                 | Description                                        |
+=========================================+====================================================+
| `java -jar`                             | Run a java application                             |
+-----------------------------------------+----------------------------------------------------+
| `python3.9 -m http.server -b 0.0.0.0 #` | Start a python web server in the current directory |
+-----------------------------------------+----------------------------------------------------+
| `emacsclient -n -c -a ""`               | Open a file for editing with emacsclient           |
+-----------------------------------------+----------------------------------------------------+


Basic Scripts
^^^^^^^^^^^^^


.. literalinclude:: ../../TestCommands/generate_md5.sh
   :language: bash
   :caption: Generate MD5 checksum file `[generate_md5.sh @ github] <https://github.com/augiedoggie/TrackRunner/TestCommands/generate_md5.sh>`_


.. code-block:: bash
   :caption: Use scp to copy selected files/folders to a remote host


   #!/usr/bin/env bash

   if (($# == 0)); then
      echo "Nothing selected"
      exit
   fi

   scp -r "$@" myhost:


.. literalinclude:: ../../TestCommands/copy_paths_to_clipboard.sh
   :language: bash
   :caption: Copy the selected file paths to the clipboard `[copy_paths_to_clipboard.sh @ github] <https://github.com/augiedoggie/TrackRunner/TestCommands/copy_paths_to_clipboard.sh>`_


Intermediate Scripts
^^^^^^^^^^^^^^^^^^^^


.. literalinclude:: ../../TestCommands/toggle_bfs_attribute.sh
   :language: bash
   :caption: Toggle a BFS attribute on/off `[toggle_bfs_attribute.sh @ github] <https://github.com/augiedoggie/TrackRunner/TestCommands/toggle_bfs_attribute.sh>`_

.. literalinclude:: ../../TestCommands/create_video_thumbnails.sh
   :language: bash
   :caption: Create Tracker thumbnail icons for video files `[create_video_thumbnails.sh @ github] <https://github.com/augiedoggie/TrackRunner/TestCommands/create_video_thumbnails.sh>`_

.. literalinclude:: ../../TestCommands/extract_archive.sh
   :language: bash
   :caption: Extract various archive types using the tar/zip/package commands `[extract_archive.sh @ github] <https://github.com/augiedoggie/TrackRunner/TestCommands/extract_archive.sh>`_
