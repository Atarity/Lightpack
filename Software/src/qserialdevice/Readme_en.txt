== QSerialSevice ==

Contents:
1. About
2. Building
3. Installation
4. License
5. Authors
6. Links
----------------




1. About the project.
~~~~~~~~~~~~

QSerialDevice - a cross-platform library for serial
devices (ports) based on Qt4 (http://qt.nokia.com/).

Attention:
In the current version - guaranteed free operation (tested by the author) only in operating systems:
- Windows XP Pro Sp3 / Win 7 32bit.
- GNU/Linux.

So in other cases, the work is not guaranteed!

1.1 About the classes.

The library contains the following classes:

AbstractSerial      - a class that provides an interface for
                      with serial devices.
SerialDeviceEnumerator - a class that provides an interface
                      notification (tracking) for serial devices,
					  as well as for information on serial devices.
                          
(Detailed description see the documentation)

Note:
Classes SerialDeviceWatcher and SerialDeviceInfo outdated and no longer supported.
Instead, use the class SerialDeviceEnumerator.


1.2 About the examples.

The library in this part of its console contains examples of programs in the directory /examples,
Utilities:

writer                  - is intended to demonstrate the use of libraries when writing data to the serial device.

reader                  - is intended to demonstrate the use of the library when reading data from the
                          serial device using the method: waitForReadyRead().

sreader                 - intended to demonstrate the use of the library when reading data from the
                          serial device using a signal: readyRead().
    
enumerator              - designed to demonstrate the use of libraries for the notification (tracking)
                          for serial devices, as well as to demonstrate the use of the library for
                          information about the serial devices.
                          
anymaster               - intended to demonstrate the use of libraries as an abstract
                          communication protocol via the serial interface as a master
                          (Implemented techniques such as: request/response to the slave anyslave)
                          
anyslave                - intended to demonstrate the use of libraries as an abstract
                          communication protocol via the serial interface as a slave
                          (Implemented techniques such as: reception/response to the master anymaster)

(More details on the building of the library and examples, see below in the section number 2 "Building")

1.3 About the Documentation.

Documentation: (doc/ru/html or doc/en/html) performed in two languages: English and Russian.
(note: generated using Doxygen)




2. Building
~~~~~~~~~~~~

2.1 Requirements.

To build the project in OC Windows and GNU / Linux (for example) require mandatory conditions:
- Must be installed library Qt4> 4.6.0 (and 4.5.0 can be, but the ability to edit the source code of some classes)
  and properly configured its environment variables.
- Must be installed and configured the compiler its environment variables.

Note: For OS GNU/Linux to build class SerialDeviceEnumerator
must be installed UDEV (http://www.kernel.org/pub/linux/utils/kernel/hotplug/udev.html)

2.2 Building the Library.

Note: By default, the library is going with option: staticlib, so
all the recommendations of assembling the library and the examples that follow,
are given only for a static build of the library.

    2.2.1 Assembling the only library using qmake.
    
		2.2.1.1 Using QtCreator
		1. Unpack the source tarball of the project.
		2. Start QtCreator
		3. The menu QtCreator: "File-> Open file or project", select the file: BuildLibrary.pro
		4. Click to QtCreator "Build", etc.

		2.2.1.2 Using the command line.
		1. Unpack the source tarball of the project.
		2. Go to the source directory of the project:
			#cd qserialdevice-qserialdevice
		3. Execute the following commands:
			#qmake BuildLibrary.pro
			#make
        
    2.2.2 Assembling the only library using CMake.

		2.2.2.1 Using QtCreator
		Assembling the only library using CMake untested.
		You may have to fix the file: src/CMakeLists.txt.
        
		2.2.2.2 Using the command line.
		Assembling the only library using CMake untested.
		You may have to fix the file: src/CMakeLists.txt.

    2.2.3 Compiling examples using qmake.
    
    Note: when assembling the examples will also be collected and the library itself!

		2.2.3.1 Using QtCreator
		1. Unpack the source tarball of the project.
		2. Start QtCreator
		3. The menu QtCreator: "File-> Open file or project", select the file: BuildExamples.pro
		4. Click to QtCreator "Build", etc.
    
		2.2.3.2 Using the command line.
		1. Unpack the source tarball of the project.
		2. Go to the source directory of the project:
			#cd qserialdevice-qserialdevice
		3. Execute the following commands:
			#qmake BuildExamples.pro
			#make

    2.2.4 Compiling examples using CMake.
    
    Note: when assembling the examples will also be collected and the library itself!

		2.2.4.1 Using QtCreator
		1. Unpack the source tarball of the project.
		2. Start QtCreator
		3. The menu QtCreator: "File-> Open file or project", select the file: CMakeLists.txt
		4. Click to QtCreator "Build", etc.
    
		2.2.4.2 Using the command line.
		1. Unpack the source tarball of the project.
		2. In parallel with the source code directory, create a project
			directory "cmake-build".
			Example:
				/
				|->/qserialdevice-qserialdevice
				|->/cmake-build

		3. Navigate to the directory with "cmake-build":
			#cd cmake-build
		4. Execute the following commands:
			#cmake .. /qserialdevice-qserialdevice
			#make

		Note: may need to add some parameters to the command line
		to build, depending on the compiler (read dokumentayiyu for CMake).
    
2.3 Building the documentation.

To assemble the documentation necessary to:
1. Install Doxygen: http://www.stack.nl/~dimitri/doxygen/ .
2. Run the utility Doxywizard (it appears after installing Doxygen).
3. Select the menu Doxywizard "File-> Open" file "Doxyfile_en" or "Doxyfile_ru" depending on the desired language.
4. Click to Doxywizard tab "Run" button "Run Doxygen".
5. If Doxywizard notified that that no directory /doc - then create the directory at the root of the unpacked archive library.




3. Installation
~~~~~~~~~~~~




4. License
~~~~~~~~~~~~~
GPL

Copyright (C) 2009 Denis Shienkov <scapig2@yandex.ru>

See file: copying




5. Authors
~~~~~~~~~~~~~

Developer: Denis Shienkov
ICQ: 321789831
e-mail: scapig2@yandex.ru




6. Links
~~~~~~~~~~~~~

