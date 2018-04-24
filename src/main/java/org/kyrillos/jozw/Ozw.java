/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package org.kyrillos.jozw;

import java.util.List;

/**
 *
 * @author cyrille
 */
public class Ozw {

    enum ControllerInterface {
        SERIAL(1),
        USB(2),
        UNKWNON(3);

        private final int id;

        ControllerInterface(int i) {
            id = i;
        }
    }
    
    private static Ozw INSTANCE;
       
    private final long nativePtr;
    private volatile boolean attached = false;
    
    static {
        // FIXME lib path....
        System.load("/home/cyrille/CLionProjects/openzwave-jni/cmake-build-release/libopenzwave-jni.so");
    }

    public static native String getVersionAsString();
    
    public synchronized static Ozw getInstance(){
        if (INSTANCE == null){
            INSTANCE = new Ozw();
        }
        return INSTANCE;
    }
    
    private Ozw(){
        String configPath = System.getProperty("ozw.config.path", "");
        String userPath = System.getProperty("ozw.user.path", "");
        String args = System.getProperty("ozw.arguments", "");
        this.nativePtr = doCreate(configPath, userPath, args);
    }

    /**
     * Init Ozw.
     *
     * Instanciate Openzwave::Config and Openzwave::Manager
     * Openzwave::Manager.addWatcher() is created
     *
     * @param config config file.
     * @param userPath user config file.
     * @param arguments user arguments.
     * @return native Ptr
     * @see #onNotification(Notification)
     */
    private native long doCreate(String config, String userPath, String arguments);

    /**
     * This method is required to be notified by Native lib watcher.
     * @param notification OpenZWave notification.
     */
    private void onNotification(Notification notification){
        System.out.print("notification: " + notification);
        attached = true;
    }

    /**
     * Deletes the Manager and cleans up any associated objects.
     */
    private native static void destroy();

    /**
     * Creates a new driver for a Z-Wave controller. This method creates a Driver object for handling communications with a single Z-Wave controller. In the background, the driver first tries to read configuration data saved during a previous run. It then queries the controller directly for any missing information, and a refresh of the list of nodes that it controls. Once this information has been received, a DriverReady notification callback is sent, containing the Home ID of the controller. This Home ID is required by most of the OpenZWave Manager class methods.
     */
    public synchronized boolean addDriver(String controllerPath, ControllerInterface interfaceType) throws InterruptedException {
        boolean ret = doAddDriver(controllerPath, interfaceType != null ? interfaceType.id : ControllerInterface.UNKWNON.id);
        // Native pthread isn't attached yet (and hold) to the JVM,
        // we have to wait until the first notification was recevied.
        while (!attached){
            this.wait(100);
        }
        return ret;
    }

    private native boolean doAddDriver(String controllerPath, int interfaceType);

    /**
     * Saves the configuration of a PC Controller's Z-Wave network to the application's user data folder. This method does not normally need to be called, since OpenZWave will save the state automatically during the shutdown process. It is provided here only as an aid to development. The configuration of each PC Controller's Z-Wave network is stored in a separate file. The filename consists of the 8 digit hexadecimal version of the controller's Home ID, prefixed with the String 'zwcfg'. This convention allows OpenZWave to find the correct configuration file for a controller, even if it is attached to a different serial port, USB device path, etc.
     */
    public native void writeConfig(long homeId);
    // Configuration :

    // For saving the Z-Wave network configuration so that the entire network does not need to be polled every time the application starts.

    // Drivers :
    // Methods for adding and removing drivers and obtaining basic controller information.

    /**
     * Removes the driver for a Z-Wave controller, and closes the controller. Drivers do not need to be explicitly removed before calling Destroy - this is handled automatically.
     */
    public native boolean removeDriver(String controllerPath);

    /**
     * get the node ID of the Z-Wave controller.
     */
    public native int getControllerNodeId(long homeId);

    /**
     * get the node ID of the Static Update Controller.
     */
    public native int getSUCNodeId(long homeId);

    /**
     * Query if the controller is a primary controller. The primary controller is the main device used to configure and control a Z-Wave network. There can only be one primary controller - all other controllers are secondary controllers.
     */
    public native boolean isPrimaryController(long homeId);

    /**
     * Query if the controller is a static update controller. A Static Update Controller (SUC); is a controller that must never be moved in normal operation and which can be used by other nodes to receive information about network changes.
     */
    public native boolean isStaticUpdateController(long homeId);

    /**
     * Query if the controller is using the bridge controller library. A bridge controller is able to create virtual nodes that can be associated with other controllers to enable events to be passed on.
     */
    public native boolean isBridgeController(long homeId);

    /**
     * get the version of the Z-Wave API library used by a controller.
     */
    public native String getLibraryVersion(long homeId);

    /**
     * get a String containing the Z-Wave API library type used by a controller. The possible library types are:
     */
    public native String getLibraryTypeName(long homeId);

    /**
     * get count of messages in the outgoing send queue.
     */
    public native long getSendQueueCount(long homeId);

    /**
     * Send current driver statistics to the log file.
     */
    public native void logDriverStatistics(long homeId);

    /**
     * Obtain controller interface type.
     */
    public native int getControllerInterfaceType(long homeId);

    /**
     * Obtain controller interface path.
     */
    public native String getControllerPath(long homeId);

    // Polling Z-Wave devices */
    //Methods for controlling the polling of Z-Wave devices. Modern devices will not require polling. Some old devices need to be polled as the only way to detect status changes. */

    /**
     * get the time period between polls of a node's state.
     */
    public native long getPollInterval();

    /**
     * set the time period between polls of a node's state
     * Due to patent concerns, some devices do not report state changes automatically to the controller. These devices need to have their state polled at regular intervals. The length of the interval is the same for all devices. To even out the Z-Wave network traffic generated by polling, OpenZWave divides the polling interval by the number of devices that have polling enabled, and polls each in turn. It is recommended that if possible, the interval should not be set shorter than the number of polled devices in seconds (so that the network does not have to cope with more than one poll per second);.
     */
    public native void setPollInterval(long milliseconds);

    /**
     * Enable the polling of a device's state.
     */
    // TODO poll intensity
    public native boolean enablePoll(ValueId valueId, int intensity);

    /**
     * Disable the polling of a device's state.
     */
    public native boolean disablePoll(ValueId valueId);

    /**
     * Determine the polling of a device's state.
     */
    public native boolean isPolled(ValueId valueId);

    /**
     * set the frequency of polling (0=none, 1=every time through the list, 2-every other time, etc);
     */
    public native void setPollIntensity(ValueId valueId, int intensity);

    /**
     * get the polling intensity of a device's state.
     */
    public native int getPollIntensity(ValueId valueId);

    // Methods for accessing information on individual nodes. */
    // Node information */

    /**
     * Trigger the fetching of fixed data about a node. Causes the node's data to be obtained from the Z-Wave network in the same way as if it had just been added. This method would normally be called automatically by OpenZWave, but if you know that a node has been changed, calling this method will force a refresh of all of the data held by the library. This can be especially useful for devices that were asleep when the application was first run. This is the same as the query state starting from the beginning.
     */
    public native boolean refreshNodeInfo(long homeId, int nodeId);

    /**
     * Trigger the fetching of dynamic value data for a node. Causes the node's values to be requested from the Z-Wave network. This is the same as the query state starting from the associations state.
     */
    public native boolean requestNodeState(long homeId, int nodeId);

    /**
     * Trigger the fetching of just the dynamic value data for a node. Causes the node's values to be requested from the Z-Wave network. This is the same as the query state starting from the dynamic state.
     */
    public native boolean requestNodeDynamic(long homeId, int nodeId);

    /**
     * get whether the node is a listening device that does not go to sleep.
     */
    public native boolean isNodeListeningDevice(long homeId, int nodeId);

    /**
     * get whether the node is a frequent listening device that goes to sleep but can be woken up by a beam. Useful to determine node and controller consistency.
     */
    public native boolean isNodeFrequentListeningDevice(long homeId, int nodeId);

    /**
     * get whether the node is a beam capable device.
     */
    public native boolean isNodeBeamingDevice(long homeId, int nodeId);

    /**
     * get whether the node is a routing device that passes messages to other nodes.
     */
    public native boolean isNodeRoutingDevice(long homeId, int nodeId);

    /**
     * get the security attribute for a node. True if node supports security features.
     */
    public native boolean isNodeSecurityDevice(long homeId, int nodeId);

    /**
     * get the maximum baud rate of a node's communications.
     */
    public native int getNodeMaxBaudRate(long homeId, int nodeId);

    /**
     * get the version number of a node.
     */
    public native int getNodeVersion(long homeId, int nodeId);

    /**
     * get the security int of a node.
     */
    public native int getNodeSecurity(long homeId, int nodeId);

    /**
     * Is this a ZWave+ Supported Node?
     */
    public native boolean isNodeZWavePlus(long homeId, int nodeId);

    /**
     * get the basic type of a node.
     */
    public native int getNodeBasic(long homeId, int nodeId);

    /**
     * get the generic type of a node.
     */
    public native int getNodeGeneric(long homeId, int nodeId);

    /**
     * get the specific type of a node.
     */
    public native int getNodeSpecific(long homeId, int nodeId);

    /**
     * get a human-readable label describing the node The label is taken from the Z-Wave specific, generic or basic type, depending on which of those values are specified by the node.
     */
    public native String getNodeType(long homeId, int nodeId);

    /**
     * get the bitmap of this node's neighbors.
     */
    public native int[] getNodeNeighbors(long homeId, int nodeId);

    /**
     * get the manufacturer name of a device The manufacturer name would normally be handled by the Manufacturer Specific command class, taking the manufacturer ID reported by the device and using it to look up the name from the manufacturerspecific.xml file in the OpenZWave config folder. However, there are some devices that do not support the command class, so to enable the user to manually set the name, it is stored with the node data and accessed via this method rather than being reported via a command class Value object.
     */
    public native String getNodeManufacturerName(long homeId, int nodeId);

    /**
     * get the product name of a device The product name would normally be handled by the Manufacturer Specific command class, taking the product Type and ID reported by the device and using it to look up the name from the manufacturerspecific.xml file in the OpenZWave config folder. However, there are some devices that do not support the command class, so to enable the user to manually set the name, it is stored with the node data and accessed via this method rather than being reported via a command class Value object.
     */
    public native String getNodeProductName(long homeId, int nodeId);

    /**
     * get the name of a node The node name is a user-editable label for the node that would normally be handled by the Node Naming command class, but many devices do not support it. So that a node can always be named, OpenZWave stores it with the node data, and provides access through this method and setNodeName, rather than reporting it via a command class Value object. The maximum length of a node name is 16 characters.
     */
    public native String getNodeName(long homeId, int nodeId);

    /**
     * get the location of a node The node location is a user-editable String that would normally be handled by the Node Naming command class, but many devices do not support it. So that a node can always report its location, OpenZWave stores it with the node data, and provides access through this method and setNodeLocation, rather than reporting it via a command class Value object.
     */
    public native String getNodeLocation(long homeId, int nodeId);

    /**
     * get the manufacturer ID of a device The manufacturer ID is a four digit hex code and would normally be handled by the Manufacturer Specific command class, but not all devices support it. Although the value reported by this method will be an empty String if the command class is not supported and cannot be set by the user, the manufacturer ID is still stored with the node data (rather than being reported via a command class Value object); to retain a consistent approach with the other manufacturer specific data.
     */
    public native String getNodeManufacturerId(long homeId, int nodeId);

    /**
     * get the product type of a device The product type is a four digit hex code and would normally be handled by the Manufacturer Specific command class, but not all devices support it. Although the value reported by this method will be an empty String if the command class is not supported and cannot be set by the user, the product type is still stored with the node data (rather than being reported via a command class Value object); to retain a consistent approach with the other manufacturer specific data.
     */
    public native String getNodeProductType(long homeId, int nodeId);

    /**
     * get the product ID of a device The product ID is a four digit hex code and would normally be handled by the Manufacturer Specific command class, but not all devices support it. Although the value reported by this method will be an empty String if the command class is not supported and cannot be set by the user, the product ID is still stored with the node data (rather than being reported via a command class Value object); to retain a consistent approach with the other manufacturer specific data.
     */
    public native String getNodeProductId(long homeId, int nodeId);

    /**
     * set the manufacturer name of a device The manufacturer name would normally be handled by the Manufacturer Specific command class, taking the manufacturer ID reported by the device and using it to look up the name from the manufacturerspecific.xml file in the OpenZWave config folder. However, there are some devices that do not support the command class, so to enable the user to manually set the name, it is stored with the node data and accessed via this method rather than being reported via a command class Value object.
     */
    public native void setNodeManufacturerName(long homeId, int nodeId, String manufacturerName);

    /**
     * set the product name of a device The product name would normally be handled by the Manufacturer Specific command class, taking the product Type and ID reported by the device and using it to look up the name from the manufacturerspecific.xml file in the OpenZWave config folder. However, there are some devices that do not support the command class, so to enable the user to manually set the name, it is stored with the node data and accessed via this method rather than being reported via a command class Value object.
     */
    public native void setNodeProductName(long homeId, int nodeId, String productName);

    /**
     * set the name of a node The node name is a user-editable label for the node that would normally be handled by the Node Naming command class, but many devices do not support it. So that a node can always be named, OpenZWave stores it with the node data, and provides access through this method and getNodeName, rather than reporting it via a command class Value object. If the device does support the Node Naming command class, the new name will be sent to the node. The maximum length of a node name is 16 characters.
     */
    public native void setNodeName(long homeId, int nodeId, String nodeName);

    /**
     * set the location of a node The node location is a user-editable String that would normally be handled by the Node Naming command class, but many devices do not support it. So that a node can always report its location, OpenZWave stores it with the node data, and provides access through this method and getNodeLocation, rather than reporting it via a command class Value object. If the device does support the Node Naming command class, the new location will be sent to the node.
     */
    public native void setNodeLocation(long homeId, int nodeId, String location);

    /**
     * Turns a node on This is a helper method to simplify basic control of a node. It is the equivalent of changing the level reported by the node's Basic command class to 255, and will generate a ValueChanged notification from that class. This command will turn on the device at its last known level, if supported by the device, otherwise it will turn it on at 100%.
     */
    public native void setNodeOn(long homeId, int nodeId);

    /**
     * Turns a node off This is a helper method to simplify basic control of a node. It is the equivalent of changing the level reported by the node's Basic command class to zero, and will generate a ValueChanged notification from that class.
     */
    public native void setNodeOff(long homeId, int nodeId);

    /**
     * sets the basic level of a node This is a helper method to simplify basic control of a node. It is the equivalent of changing the value reported by the node's Basic command class and will generate a ValueChanged notification from that class.
     */
    public native void setNodeLevel(long homeId, int nodeId, int level);

    /**
     * get whether the node information has been received.
     */
    public native boolean isNodeInfoReceived(long homeId, int nodeId);

    /**
     * get whether the node has the defined class available or not.
     */
    public native boolean getNodeClassInformation(long homeId, int nodeId, int commandClassId, String className, Integer classVersion);

    /**
     * get whether the node is awake or asleep.
     */
    public native boolean isNodeAwake(long homeId, int nodeId);

    /**
     * get whether the node is working or has failed.
     */
    public native boolean isNodeFailed(long homeId, int nodeId);

    /**
     * get whether the node's query stage as a String.
     */
    public native String getNodeQueryStage(long homeId, int nodeId);

    /**
     * get the node device type as reported in the Z-Wave+ Info report.
     */
    public native int getNodeDeviceType(long homeId, int nodeId);

    /**
     * get the node device type as reported in the Z-Wave+ Info report.
     */
    public native String getNodeDeviceTypeString(long homeId, int nodeId);

    /**
     * get the node role as reported in the Z-Wave+ Info report.
     */
    public native int getNodeRole(long homeId, int nodeId);

    /**
     * get the node role as reported in the Z-Wave+ Info report.
     */
    public native String getNodeRoleString(long homeId, int nodeId);

    /**
     * get the node PlusType as reported in the Z-Wave+ Info report.
     */
    public native int getNodePlusType(long homeId, int nodeId);

    /**
     * get the node PlusType as reported in the Z-Wave+ Info report.
     */
    public native String getNodePlusTypeString(long homeId, int nodeId);

    // Values
    // Methods for accessing device values. All the methods require a ValueId, which will have been provided in the ValueAdded Notification callback when the the value was first discovered by OpenZWave.

    /**
     * gets the user-friendly label for the value.
     */
    public native String getValueLabel(ValueId id);

    /**
     * sets the user-friendly label for the value.
     */
    public native void setValueLabel(ValueId id, String value);

    /**
     * gets the units that the value is measured in.
     */
    public native String getValueUnits(ValueId id);

    /**
     * sets the units that the value is measured in.
     */
    public native void setValueUnits(ValueId id, String value);

    /**
     * gets a help String describing the value's purpose and usage.
     */
    public native String getValueHelp(ValueId id);

    /**
     * sets a help String describing the value's purpose and usage.
     */
    public native void setValueHelp(ValueId id, String value);

    /**
     * gets the minimum that this value may contain.
     */
    public native int getValueMin(ValueId id);

    /**
     * gets the maximum that this value may contain.
     */
    public native int getValueMax(ValueId id);

    /**
     * Test whether the value is read-only.
     */
    public native boolean isValueReadOnly(ValueId id);

    /**
     * Test whether the value is write-only.
     */
    public native boolean isValueWriteOnly(ValueId id);

    /**
     * Test whether the value has been set.
     */
    public native boolean isValueSet(ValueId id);

    /**
     * Test whether the value is currently being polled.
     */
    public native boolean isValuePolled(ValueId id);

    /**
     * gets the list of items from a list value.
     */
    public native List<String> getValueListItemsAsString(ValueId id);

    /**
     * gets the list of values from a list value.
     */
    public native List<Integer> getValueListValuesAsInt(ValueId id);

    /**
     * gets a float value's precision.
     */
    public native Float getValueFloatPrecision(ValueId id);

    /**
     * Refreshes the specified value from the Z-Wave network. A call to this function causes the library to send a message to the network to retrieve the current value of the specified ValueId (just like a poll, except only one-time, not recurring);.
     */
    public native boolean refreshValue(ValueId id);

    /**
     * sets a flag indicating whether value changes noted upon a refresh should be verified. If so, the library will immediately refresh the value a second time whenever a change is observed. This helps to filter out spurious data reported occasionally by some devices.
     */
    public native void setChangeVerified(ValueId id, boolean verify);

    /**
     * determine if value changes upon a refresh should be verified. If so, the library will immediately refresh the value a second time whenever a change is observed. This helps to filter out spurious data reported occasionally by some devices.
     */
    public native boolean getChangeVerified(ValueId id);

    /**
     * Starts an activity in a device. Since buttons are write-only values that do not report a state, no notification callbacks are sent.
     */
    public native boolean pressButton(ValueId id);

    /**
     * Stops an activity in a device. Since buttons are write-only values that do not report a state, no notification callbacks are sent.
     */
    public native boolean releaseButton(ValueId id);

    // Climate Control Schedules */
    // Methods for accessing schedule values. All the methods require a ValueId, which will have been provided in the ValueAdded Notification callback when the the value was first discovered by OpenZWave. */
    // The ValueTypeSchedule is a specialized Value used to simplify access to the switch point schedule information held by a setback thermostat that supports the Climate Control Schedule command class. Each schedule contains up to nine switch points for a single day, consisting of a time in hours and minutes (24 hour clock); and a setback in tenths of a degree Celsius. The setback value can range from -128 (-12.8C); to 120 (12.0C);. There are two special setback values - 121 is used to set Frost Protection mode, and 122 is used to set Energy Saving mode. */
    // The switch point methods only modify OpenZWave's copy of the schedule information. Once all changes have been made, they are sent to the device by calling setSchedule. */

    /**
     * get the number of switch points defined in a schedule.
     */
    public native int getNumSwitchPoints(ValueId id);

    /**
     * set a switch point in the schedule. Inserts a new switch point into the schedule, unless a switch point already exists at the specified time in which case that switch point is updated with the new setback value instead. A maximum of nine switch points can be set in the schedule.
     */
    public native boolean setSwitchPoint(ValueId id, int hours, int minutes, int setback);

    /**
     * Remove a switch point from the schedule. Removes the switch point at the specified time from the schedule.
     */
    public native boolean RemoveSwitchPoint(ValueId id, int hours, int minutes);

    /**
     * Clears all switch points from the schedule.
     */
    public native void ClearSwitchPoints(ValueId id);

    /**
     * gets switch point data from the schedule. Retrieves the time and setback values from a switch point in the schedule.
     */
    public native boolean getSwitchPoint(ValueId id, int idx, int ohours, int ominutes, int osetback);

    // Methods for switching all devices on or off together. The devices must support the SwitchAll command class. The command is first broadcast to all nodes, and then followed up with individual commands to each node (because broadcasts are not routed, the message might not otherwise reach all the nodes);. *
    // SwitchAll

    /**
     * Switch all devices on. All devices that support the SwitchAll command class will be turned on.
     */
    public native void SwitchAllOn(long homeId);

    /**
     * Switch all devices off. All devices that support the SwitchAll command class will be turned off.
     */
    public native void SwitchAllOff(long homeId);

    // Configuration Parameters */
    // Methods for accessing device configuration parameters. Configuration parameters are values that are managed by the Configuration command class. The values are device-specific and are not reported by the devices. Information on parameters is provided only in the device user manual. */
    // An ongoing task for the OpenZWave project is to create XML files describing the available parameters for every Z-Wave. See the config folder in the project source code for examples. */

    /**
     * set the value of a configurable parameter in a device. Some devices have various parameters that can be configured to control the device behavior. These are not reported by the device over the Z-Wave network, but can usually be found in the device's user manual. This method returns immediately, without waiting for confirmation from the device that the change has been made.
     */
    public native boolean setConfigParam(long homeId, int nodeId, int param, int value, int size);

    /**
     * Request the value of a configurable parameter from a device. Some devices have various parameters that can be configured to control the device behavior. These are not reported by the device over the Z-Wave network, but can usually be found in the device's user manual. This method requests the value of a parameter from the device, and then returns immediately, without waiting for a response. If the parameter index is valid for this device, and the device is awake, the value will eventually be reported via a ValueChanged notification callback. The ValueId reported in the callback will have an index set the same as param and a command class set to the same value as returned by a call to Configuration::StaticGetCommandClassId.
     */
    public native void requestConfigParam(long homeId, int nodeId, int param);

    /**
     * Request the values of all known configurable parameters from a device.
     */
    public native void requestAllConfigParams(long homeId, int nodeId);

    // Groups
    // Methods for accessing device association groups.

    /**
     * gets the number of association groups reported by this node In Z-Wave, groups are numbered starting from one. For example, if a call to getNumGroups returns 4, the groupIdx value to use in calls to getAssociations, AddAssociation and RemoveAssociation will be a number between 1 and 4.
     */
    public native int getNumGroups(long homeId, int nodeId);

    /**
     * gets the maximum number of associations for a group.
     */
    public native int getMaxAssociations(long homeId, int nodeId, int groupIdx);

    /**
     * Returns a label for the particular group of a node. This label is populated by the device specific configuration files.
     */
    public native String getGroupLabel(long homeId, int nodeId, int groupIdx);

    /**
     * Adds a node to an association group. Due to the possibility of a device being asleep, the command is assumed to succeed, and the association data held in this class is updated directly. This will be reverted by a future Association message from the device if the Z-Wave message actually failed to get through. Notification callbacks will be sent in both cases.
     */
    public native void addAssociation(long homeId, int nodeId, int groupIdx, int targetNodeId, int instance);

    /**
     * Removes a node from an association group. Due to the possibility of a device being asleep, the command is assumed to succeed, and the association data held in this class is updated directly. This will be reverted by a future Association message from the device if the Z-Wave message actually failed to get through. Notification callbacks will be sent in both cases.
     */
    public native void removeAssociation(long homeId, int nodeId, int groupIdx, int targetNodeId, int instance);

    // Controller Commands */
    // Commands for Z-Wave network management using the PC Controller. */

    /**
     * Hard Reset a PC Z-Wave Controller. Resets a controller and erases its network configuration settings. The controller becomes a primary controller ready to add devices to a new network.
     */
    public native void resetController(long homeId);

    /**
     * Soft Reset a PC Z-Wave Controller. Resets a controller without erasing its network configuration settings.
     */
    public native void softReset(long homeId);

    ///**
    // * Start a controller command process. Most Controller Commands are implemented via Other Manager methods, you should only use this method if you need advanced control over a existing Controller Command or if a ControllerCommand is not implemented.
    // */
    //@Deprecated
    //public native boolean BeginControllerCommand(long homeId, Driver::ControllerCommand command, Driver::pfnControllerCallbackt callback=NULL, void context=NULL, boolean highPower=false, int nodeId=0xff, int arg=0);

    /**
     * Cancels any in-progress command running on a controller.
     */
    public native boolean cancelControllerCommand(long homeId);

    // Network Commands
    // Commands for Z-Wave network for testing, routing and other internal operations.

    /**
     * Test network node. Sends a series of messages to a network node for testing network reliability.
     */
    public native void testNetworkNode(long homeId, int nodeId, int count);

    /**
     * Test network. Sends a series of messages to every node on the network for testing network reliability.
     */
    public native void testNetwork(long homeId, int count);

    /**
     * Heal network node by requesting the node rediscover their neighbors. Sends a ControllerCommandRequestNodeNeighborUpdate to the node.
     */
    public native void healNetworkNode(long homeId, int nodeId, boolean doRR);

    /**
     * Heal network by requesting node's rediscover their neighbors. Sends a ControllerCommandRequestNodeNeighborUpdate to every node. Can take a while on larger networks.
     */
    public native void healNetwork(long homeId, boolean doRR);

    /**
     * Start the Inclusion Process to add a Node to the Network. The Status of the Node Inclusion is communicated via Notifications. Specifically, you should monitor ControllerCommand Notifications.
     */
    public native boolean addNode(long homeId, boolean doSecurity);

    /**
     * Remove a Device from the Z-Wave Network The Status of the Node Removal is communicated via Notifications. Specifically, you should monitor ControllerCommand Notifications.
     */
    public native boolean removeNode(long homeId);

    /**
     * Remove a Failed Device from the Z-Wave Network This Command will remove a failed node from the network. The Node should be on the Controllers Failed Node List, otherwise this command will fail. You can use the HasNodeFailed function below to test if the Controller believes the Node has Failed. The Status of the Node Removal is communicated via Notifications. Specifically, you should monitor ControllerCommand Notifications.
     */
    public native boolean removeFailedNode(long homeId, int nodeId);

    /**
     * Check if the Controller Believes a Node has Failed. This is different from the IsNodeFailed call in that we test the Controllers Failed Node List, whereas the IsNodeFailed is testing our list of Failed Nodes, which might be different. The Results will be communicated via Notifications. Specifically, you should monitor the ControllerCommand notifications.
     */
    public native boolean hasNodeFailed(long homeId, int nodeId);

    /**
     * Ask a Node to update its Neighbor Tables This command will ask a Node to update its Neighbor Tables.
     */
    public native boolean requestNodeNeighborUpdate(long homeId, int nodeId);

    /**
     * Ask a Node to update its update its Return Route to the Controller This command will ask a Node to update its Return Route to the Controller.
     */
    public native boolean assignReturnRoute(long homeId, int nodeId);

    /**
     * Ask a Node to delete all Return Route. This command will ask a Node to delete all its return routes, and will rediscover when needed.
     */
    public native boolean deleteAllReturnRoutes(long homeId, int nodeId);

    /**
     * Send a NIF frame from the Controller to a Node. This command send a NIF frame from the Controller to a Node.
     */
    public native boolean sendNodeInformation(long homeId, int nodeId);

    /**
     * Create a new primary controller when old primary fails. Requires SUC. This command Creates a new Primary Controller when the Old Primary has Failed. Requires a SUC on the network to function.
     */
    public native boolean createNewPrimary(long homeId);

    /**
     * Receive network configuration information from primary controller. Requires secondary. This command prepares the controller to receive Network Configuration from a Secondary Controller.
     */
    public native boolean receiveConfiguration(long homeId);

    /**
     * Replace a failed device with another. If the node is not in the controller's failed nodes list, or the node responds, this command will fail. You can check if a Node is in the Controllers Failed node list by using the HasNodeFailed method.
     */
    public native boolean replaceFailedNode(long homeId, int nodeId);

    /**
     * Add a new controller to the network and make it the primary. The existing primary will become a secondary controller.
     */
    public native boolean transferPrimaryRole(long homeId);

    /**
     * Update the controller with network information from the SUC/SIS.
     */
    public native boolean requestNetworkUpdate(long homeId, int nodeId);

    /**
     * Send information from primary to secondary.
     */
    public native boolean replicationSend(long homeId, int nodeId);

    /**
     * Create a handheld button id.
     */
    public native boolean createButton(long homeId, int nodeId, short buttonId);

    /**
     * Delete a handheld button id.
     */
    public native boolean deleteButton(long homeId, int nodeId, short buttonId);


    // Commands for Z-Wave scene interface.
    // Scene Commands

    /**
     * gets the number of scenes that have been defined.
     */
    public native int getNumScenes();

    /**
     * gets a list of all the SceneIds.
     */
    public native short[] getAllScenes();

    /**
     * Remove all the SceneIds.
     */
    public native void removeAllScenes(long homeId);

    /**
     * Create a new Scene passing in Scene ID.
     */
    public native short createScene();

    /**
     * Remove an existing Scene.
     */
    public native boolean removeScene(short sceneId);

    /**
     * Remove the Value ID from an existing scene.
     */
    public native boolean removeSceneValue(short sceneId, ValueId valueId);

    /**
     * Retrieves the scene's list of values.
     */
    public native int sceneGetValues(short sceneId, ValueId[] ovalue);

    /**
     * Returns a label for the particular scene.
     */
    public native String getSceneLabel(short sceneId);

    /**
     * sets a label for the particular scene.
     */
    public native void setSceneLabel(short sceneId, String value);

    /**
     * Check if a Scene ID is defined.
     */
    public native boolean sceneExists(short sceneId);

    /**
     * Activate given scene to perform all its actions.
     */
    public native boolean activateScene(short sceneId);

}
