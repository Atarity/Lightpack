
AmbilightUSB
============
                                                            
Very simple implementation of the backlight for a laptop (as in Philips TV).

Linux
=====

Download and install Qt4 libs:  
`$ sudo apt-get install libqt4-gui`  
   
Binary
------
 
 If you using `x86_64` you can simply goto [downloads](https://github.com/brunql/AmbilightUSB/downloads) and download latest `AmbilightUSB_vN.N.N`, and may be this will work =) overwise compile from sources.   
 This builds created on Ubuntu 10.04 LTS (x86_64), AMD Turion X2 (64 bit), using gcc4.4.3 and Qt4.6.  

Sources
-------
 
 Installing from sources (Qt4 and gcc must be already installed): 
 
1. Download sources:

   * Get only latest source from github: [tar.gz](https://github.com/brunql/AmbilightUSB/tarball/master) or [zip](https://github.com/brunql/AmbilightUSB/zipball/master)  
   * Get full git tree from github: `$ git clone git@github.com:brunql/AmbilightUSB.git`  
     
2. Compile:  
   
       $ cd AmbilightUSB/Software/
       $ ./update_locales.sh
       $ qmake AmbilightUSB.pro
       $ make
       $ ./build/AmbilightUSB

Icon for linux users: [ambilight_icon.png](https://github.com/brunql/AmbilightUSB/blob/master/Software/res/icons/ambilight_icon.png)
 
If AmbilightUSB shows `AmbilightUSB. Error state.` and write to logs `Access to device denied` or `The specified device was not found`, this may be problem with permissions, try to add `UDEV` rules [50-usb.rules](https://github.com/brunql/AmbilightUSB/raw/master/50-usb.rules) to `/etc/udev/rules.d/`  

    $ sudo cp 50-usb.rules /etc/udev/rules.d/
    $ sudo restart udev

Unplug and plug, if needed, AmbilightUSB device.  


Windows
=======
 
Binary
------

1. Download: [Libs Win32](https://github.com/downloads/brunql/AmbilightUSB/Libs_Win32.zip)  
2. Goto: [downloads](https://github.com/brunql/AmbilightUSB/downloads) find there latest version of available builds (`AmbilightUSB_vN.N.N.exe`, like this AmbilightUSB_v2.7.6.exe)  
3. Extract archive in directory `AmbilightUSB`.  
4. Copy `AmbilightUSB_vN.N.N.exe` to `AmbilightUSB` directory.  
 
 Icon for windows users included in resources.
 
