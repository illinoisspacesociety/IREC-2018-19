import sys
import time
from Phidget22.Devices.VoltageRatioInput import *
from Phidget22.PhidgetException import *
from Phidget22.Phidget import *
from Phidget22.Net import *

#%% Paul DeTrempe8, Hybrid rocket load cell data collection script
# Big shoutout to Phidget for writing 90% of this
# Change data collection time duration in %%Collect Data section
# Change output text file name in %%Store Voltage Ratios section



# define storage vector of voltage ratios
Vratios = [0.0]

try:
    ch = VoltageRatioInput() 
    #ch.open()
except RuntimeError as e:
    print("Runtime Exception %s" % e.details)
    print("Press Enter to Exit...\n")
    readin = sys.stdin.read(1)
    exit(1)

def VoltageRatioInputAttached(e):
    try:
        attached = e
        print("\nAttach Event Detected (Information Below)")
        print("===========================================")
        print("Library Version: %s" % attached.getLibraryVersion())
        print("Serial Number: %d" % attached.getDeviceSerialNumber())
        print("Channel: %d" % attached.getChannel())
        print("Channel Class: %s" % attached.getChannelClass())
        print("Channel Name: %s" % attached.getChannelName())
        print("Device ID: %d" % attached.getDeviceID())
        print("Device Version: %d" % attached.getDeviceVersion())
        print("Device Name: %s" % attached.getDeviceName())
        print("Device Class: %d" % attached.getDeviceClass())
        print("\n")

    except PhidgetException as e:
        print("Phidget Exception %i: %s" % (e.code, e.details))
        print("Press Enter to Exit...\n")
        readin = sys.stdin.read(1)
        exit(1)

def VoltageRatioInputDetached(e):
    detached = e
    try:
        print("\nDetach event on Port %d Channel %d" % (detached.getHubPort(), detached.getChannel()))
    except PhidgetException as e:
        print("Phidget Exception %i: %s" % (e.code, e.details))
        print("Press Enter to Exit...\n")
        readin = sys.stdin.read(1)
        exit(1)

def ErrorEvent(e, eCode, description):
    print("Error %i : %s" % (eCode, description))

# Write voltage ratios to command window and store in a vector to be filed away
# in a text file later
def VoltageRatioChangeHandler(e, voltageRatio):
    print("VoltageRatio: %f" % voltageRatio)
    Vratios.append(voltageRatio)

def SensorChangeHandler(e, sensorValue, sensorUnit):
    print("Sensor Value: %f" % sensorValue)

#%% This is Phidget's code that does the collecting
# We only have 1 load cell hooked up, so this should be all good for now
# If we ever have more than one, use the Phidget control panel to specify hub ports
# and serial numbers
try:
    ch.setOnAttachHandler(VoltageRatioInputAttached)
    ch.setOnDetachHandler(VoltageRatioInputDetached)
    ch.setOnErrorHandler(ErrorEvent)

    ch.setOnVoltageRatioChangeHandler(VoltageRatioChangeHandler)
    ch.setOnSensorChangeHandler(SensorChangeHandler)

    # Please review the Phidget22 channel matching documentation for details on the device
    # and class architecture of Phidget22, and how channels are matched to device features.

    # Specifies the serial number of the device to attach to.
    # For VINT devices, this is the hub serial number.
    #
    # The default is any device.
    #
    # ch.setDeviceSerialNumber(470070)

    # For VINT devices, this specifies the port the VINT device must be plugged into.
    #
    # The default is any port.
    #
    # ch.setHubPort(0)

    # Specifies that the channel should only match a VINT hub port.
    # The only valid channel id is 0.
    #
    # The default is 0 (false), meaning VINT hub ports will never match
    #
    # ch.setIsHubPortDevice(1)

    # Specifies which channel to attach to.  It is important that the channel of
    # the device is the same class as the channel that is being opened.
    #
    # The default is any channel.
    #
    # ch.setChannel(0)

    # In order to attach to a network Phidget, the program must connect to a Phidget22 Network Server.
    # In a normal environment this can be done automatically by enabling server discovery, which
    # will cause the client to discovery and connect to available servers.
    #
    # To force the channel to only match a network Phidget, set remote to 1.
    #
    # Net.enableServerDiscovery(PhidgetServerType.PHIDGETSERVER_DEVICE);
    # ch.setIsRemote(1)

    print("Waiting for the Phidget VoltageRatioInput Object to be attached...")
    ch.openWaitForAttachment(5000)
except PhidgetException as e:
    print("Phidget Exception %i: %s" % (e.code, e.details))
    print("Press Enter to Exit...\n")
    readin = sys.stdin.read(1)
    exit(1)

if(ch.getChannelSubclass() == ChannelSubclass.PHIDCHSUBCLASS_VOLTAGERATIOINPUT_BRIDGE):
    ch.setBridgeEnabled(1)

#%% Collect Data (set collection time in here)
print("Gathering data...")
time.sleep(30) # sleep(collection time in seconds)

#%% Store Voltage Ratios into text file
# to change name, change following line to open('desiredname.txt','w')
f = open('voltageRatios.txt','w')
f.write(str(Vratios))
f.close()

#%% close channel
try:
    ch.close()
except PhidgetException as e:
    print("Phidget Exception %i: %s" % (e.code, e.details))
    print("Press Enter to Exit...\n")
    readin = sys.stdin.read(1)
    exit(1)
print("Closed VoltageRatioInput device")
exit(0)
