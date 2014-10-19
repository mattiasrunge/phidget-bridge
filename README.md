Phidget Bridge node.js addon
==================================
Node.js bindings for the Bridge parts of the Phidget library.
This module requires that the libphidget library is available in the system already.

# Dependencies
```
sudo apt-get install libusb-dev
wget http://www.phidgets.com/downloads/libraries/libphidget.tar.gz
tar zxvf libphidget.tar.gz
cd libphidget-2.1.8.20140319
./configure
make
sudo make install
```

# API
The module mimics the bridge parts of the phidget library API. So examples based on that API should be easy to convert to C++ versions.
All functions are synchronous and will block if they take time, they will throw if errors occur. Five events are available via the EventEmitter API which phidget module extends.

```
var phidget = require("phidget-bridge");

try {
  var phid = phidget.create();

  phidget.on("attach", function(phid) {
    console.log("Phidget attached!");
  });

  phidget.on("detach", function(phid) {
    console.log("Phidget detached!");
  });

  phidget.on("error", function(phid, errorString) {
    console.log("Phidget error!", errorString);
  });

  phidget.open(phid, -1);

  console.log("Waiting for interface kit to be attached....");

  phidget.waitForAttachment(10000);

  console.log("Device Type: " + phidget.getDeviceType(phid));
  console.log("Device Serial Number: " + phidget.getSerialNumber(phid));
  console.log("Device Version: " + phidget.getDeviceVersionphid));
  console.log("Device Input Count: " + phidget.getInputCount(phid));
} catch (e) {
  console.error(e);
}

// Set output 1
phidget.setOutputState(phid, 1, 1);

// Available methods
/*
phidget.create                 = function();
phidget.open                   = function(handle);
phidget.waitForAttachment      = function(handle, milliseconds);
phidget.close                  = function(handle);
phidget.remove                 = function(handle);
phidget.getDeviceName          = function(handle);
phidget.getSerialNumber        = function(handle);
phidget.getDeviceVersion       = function(handle);
phidget.getDeviceStatus        = function(handle);
phidget.getLibraryVersion      = function();
phidget.getDeviceType          = function(handle);
phidget.getInputCount          = function(handle);
phidget.getBridgeValue         = function(handle, index)
phidget.getBridgeMax           = function(handle, index)
phidget.getBridgeMin           = function(handle, index)
phidget.setEnabled             = function(handle, index, state)
phidget.getEnabled             = function(handle)
phidget.getGain                = function(handle, index)
phidget.setGain                = function(handle, index, gain)
phidget.getDataRate            = function(handle, index);
phidget.setDataRate            = function(handle, index, milliseconds);
phidget.getDataRateMax         = function(handle, index);
phidget.getDataRateMin         = function(handle, index);
*/

```
