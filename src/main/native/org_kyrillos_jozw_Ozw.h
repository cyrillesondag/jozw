/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class org_kyrillos_jozw_Ozw */

#ifndef _Included_org_kyrillos_jozw_Ozw
#define _Included_org_kyrillos_jozw_Ozw
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getVersionAsString
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getVersionAsString
  (JNIEnv *, jclass);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    doCreate
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_org_kyrillos_jozw_Ozw_doCreate
  (JNIEnv *, jobject, jstring, jstring, jstring);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    destroy
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_destroy
  (JNIEnv *, jclass);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    addDriver
 * Signature: (Ljava/lang/String;I)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_doAddDriver
  (JNIEnv *, jobject, jstring, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    writeConfig
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_writeConfig
  (JNIEnv *, jobject, jlong);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    removeDriver
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_removeDriver
  (JNIEnv *, jobject, jstring);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getControllerNodeId
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getControllerNodeId
  (JNIEnv *, jobject, jlong);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getSUCNodeId
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getSUCNodeId
  (JNIEnv *, jobject, jlong);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    isPrimaryController
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isPrimaryController
  (JNIEnv *, jobject, jlong);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    isStaticUpdateController
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isStaticUpdateController
  (JNIEnv *, jobject, jlong);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    isBridgeController
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isBridgeController
  (JNIEnv *, jobject, jlong);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getLibraryVersion
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getLibraryVersion
  (JNIEnv *, jobject, jlong);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getLibraryTypeName
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getLibraryTypeName
  (JNIEnv *, jobject, jlong);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getSendQueueCount
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_org_kyrillos_jozw_Ozw_getSendQueueCount
  (JNIEnv *, jobject, jlong);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    logDriverStatistics
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_logDriverStatistics
  (JNIEnv *, jobject, jlong);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getControllerInterfaceType
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getControllerInterfaceType
  (JNIEnv *, jobject, jlong);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getControllerPath
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getControllerPath
  (JNIEnv *, jobject, jlong);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getPollInterval
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_org_kyrillos_jozw_Ozw_getPollInterval
  (JNIEnv *, jobject);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    setPollInterval
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setPollInterval
  (JNIEnv *, jobject, jlong);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    enablePoll
 * Signature: (Lorg/kyrillos/jozw/ValueId;I)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_enablePoll
  (JNIEnv *, jobject, jobject, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    disablePoll
 * Signature: (Lorg/kyrillos/jozw/ValueId;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_disablePoll
  (JNIEnv *, jobject, jobject);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    isPolled
 * Signature: (Lorg/kyrillos/jozw/ValueId;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isPolled
  (JNIEnv *, jobject, jobject);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    setPollIntensity
 * Signature: (Lorg/kyrillos/jozw/ValueId;I)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setPollIntensity
  (JNIEnv *, jobject, jobject, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getPollIntensity
 * Signature: (Lorg/kyrillos/jozw/ValueId;)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getPollIntensity
  (JNIEnv *, jobject, jobject);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    refreshNodeInfo
 * Signature: (JS)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_refreshNodeInfo
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    requestNodeState
 * Signature: (JS)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_requestNodeState
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    requestNodeDynamic
 * Signature: (JS)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_requestNodeDynamic
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    isNodeListeningDevice
 * Signature: (JS)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isNodeListeningDevice
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    isNodeFrequentListeningDevice
 * Signature: (JS)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isNodeFrequentListeningDevice
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    isNodeBeamingDevice
 * Signature: (JS)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isNodeBeamingDevice
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    isNodeRoutingDevice
 * Signature: (JS)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isNodeRoutingDevice
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    isNodeSecurityDevice
 * Signature: (JS)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isNodeSecurityDevice
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getNodeMaxBaudRate
 * Signature: (JS)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getNodeMaxBaudRate
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getNodeVersion
 * Signature: (JS)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getNodeVersion
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getNodeSecurity
 * Signature: (JS)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getNodeSecurity
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    isNodeZWavePlus
 * Signature: (JS)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isNodeZWavePlus
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getNodeBasic
 * Signature: (JS)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getNodeBasic
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getNodeGeneric
 * Signature: (JS)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getNodeGeneric
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getNodeSpecific
 * Signature: (JS)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getNodeSpecific
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getNodeType
 * Signature: (JS)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getNodeType
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getNodeNeighbors
 * Signature: (JS)[I
 */
JNIEXPORT jintArray JNICALL Java_org_kyrillos_jozw_Ozw_getNodeNeighbors
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getNodeManufacturerName
 * Signature: (JS)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getNodeManufacturerName
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getNodeProductName
 * Signature: (JS)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getNodeProductName
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getNodeName
 * Signature: (JS)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getNodeName
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getNodeLocation
 * Signature: (JS)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getNodeLocation
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getNodeManufacturerId
 * Signature: (JS)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getNodeManufacturerId
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getNodeProductType
 * Signature: (JS)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getNodeProductType
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getNodeProductId
 * Signature: (JS)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getNodeProductId
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    setNodeManufacturerName
 * Signature: (JSLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setNodeManufacturerName
  (JNIEnv *, jobject, jlong, jint, jstring);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    setNodeProductName
 * Signature: (JSLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setNodeProductName
  (JNIEnv *, jobject, jlong, jint, jstring);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    setNodeName
 * Signature: (JSLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setNodeName
  (JNIEnv *, jobject, jlong, jint, jstring);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    setNodeLocation
 * Signature: (JSLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setNodeLocation
  (JNIEnv *, jobject, jlong, jint, jstring);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    setNodeOn
 * Signature: (JS)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setNodeOn
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    setNodeOff
 * Signature: (JS)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setNodeOff
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    setNodeLevel
 * Signature: (JSI)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setNodeLevel
  (JNIEnv *, jobject, jlong, jint, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    isNodeInfoReceived
 * Signature: (JS)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isNodeInfoReceived
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getNodeClassInformation
 * Signature: (JSILjava/lang/String;I)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_getNodeClassInformation
  (JNIEnv *, jobject, jlong, jint, jint, jstring, jobject);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    isNodeAwake
 * Signature: (JS)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isNodeAwake
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    isNodeFailed
 * Signature: (JS)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isNodeFailed
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getNodeQueryStage
 * Signature: (JS)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getNodeQueryStage
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getNodeDeviceType
 * Signature: (JS)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getNodeDeviceType
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getNodeDeviceTypeString
 * Signature: (JS)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getNodeDeviceTypeString
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getNodeRole
 * Signature: (JS)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getNodeRole
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getNodeRoleString
 * Signature: (JS)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getNodeRoleString
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getNodePlusType
 * Signature: (JS)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getNodePlusType
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getNodePlusTypeString
 * Signature: (JS)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getNodePlusTypeString
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getValueLabel
 * Signature: (Lorg/kyrillos/jozw/ValueId;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getValueLabel
  (JNIEnv *, jobject, jobject);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    setValueLabel
 * Signature: (Lorg/kyrillos/jozw/ValueId;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setValueLabel
  (JNIEnv *, jobject, jobject, jstring);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getValueUnits
 * Signature: (Lorg/kyrillos/jozw/ValueId;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getValueUnits
  (JNIEnv *, jobject, jobject);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    setValueUnits
 * Signature: (Lorg/kyrillos/jozw/ValueId;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setValueUnits
  (JNIEnv *, jobject, jobject, jstring);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getValueHelp
 * Signature: (Lorg/kyrillos/jozw/ValueId;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getValueHelp
  (JNIEnv *, jobject, jobject);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    setValueHelp
 * Signature: (Lorg/kyrillos/jozw/ValueId;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setValueHelp
  (JNIEnv *, jobject, jobject, jstring);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getValueMin
 * Signature: (Lorg/kyrillos/jozw/ValueId;)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getValueMin
  (JNIEnv *, jobject, jobject);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getValueMax
 * Signature: (Lorg/kyrillos/jozw/ValueId;)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getValueMax
  (JNIEnv *, jobject, jobject);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    isValueReadOnly
 * Signature: (Lorg/kyrillos/jozw/ValueId;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isValueReadOnly
  (JNIEnv *, jobject, jobject);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    isValueWriteOnly
 * Signature: (Lorg/kyrillos/jozw/ValueId;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isValueWriteOnly
  (JNIEnv *, jobject, jobject);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    isValueSet
 * Signature: (Lorg/kyrillos/jozw/ValueId;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isValueSet
  (JNIEnv *, jobject, jobject);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    isValuePolled
 * Signature: (Lorg/kyrillos/jozw/ValueId;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isValuePolled
  (JNIEnv *, jobject, jobject);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getValueListItemsAsString
 * Signature: (Lorg/kyrillos/jozw/ValueId;)Ljava/util/List;
 */
JNIEXPORT jobject JNICALL Java_org_kyrillos_jozw_Ozw_getValueListItemsAsString
  (JNIEnv *, jobject, jobject);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getValueListValuesAsInt
 * Signature: (Lorg/kyrillos/jozw/ValueId;)Ljava/util/List;
 */
JNIEXPORT jobject JNICALL Java_org_kyrillos_jozw_Ozw_getValueListValuesAsInt
  (JNIEnv *, jobject, jobject);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getValueFloatPrecision
 * Signature: (Lorg/kyrillos/jozw/ValueId;)Ljava/lang/Float;
 */
JNIEXPORT jobject JNICALL Java_org_kyrillos_jozw_Ozw_getValueFloatPrecision
  (JNIEnv *, jobject, jobject);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    refreshValue
 * Signature: (Lorg/kyrillos/jozw/ValueId;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_refreshValue
  (JNIEnv *, jobject, jobject);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    setChangeVerified
 * Signature: (Lorg/kyrillos/jozw/ValueId;Z)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setChangeVerified
  (JNIEnv *, jobject, jobject, jboolean);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getChangeVerified
 * Signature: (Lorg/kyrillos/jozw/ValueId;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_getChangeVerified
  (JNIEnv *, jobject, jobject);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    pressButton
 * Signature: (Lorg/kyrillos/jozw/ValueId;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_pressButton
  (JNIEnv *, jobject, jobject);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    releaseButton
 * Signature: (Lorg/kyrillos/jozw/ValueId;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_releaseButton
  (JNIEnv *, jobject, jobject);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getNumSwitchPoints
 * Signature: (Lorg/kyrillos/jozw/ValueId;)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getNumSwitchPoints
  (JNIEnv *, jobject, jobject);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    setSwitchPoint
 * Signature: (Lorg/kyrillos/jozw/ValueId;III)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_setSwitchPoint
  (JNIEnv *, jobject, jobject, jint, jint, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    RemoveSwitchPoint
 * Signature: (Lorg/kyrillos/jozw/ValueId;II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_RemoveSwitchPoint
  (JNIEnv *, jobject, jobject, jint, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    ClearSwitchPoints
 * Signature: (Lorg/kyrillos/jozw/ValueId;)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_ClearSwitchPoints
  (JNIEnv *, jobject, jobject);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getSwitchPoint
 * Signature: (Lorg/kyrillos/jozw/ValueId;IIII)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_getSwitchPoint
  (JNIEnv *, jobject, jobject, jint, jint, jint, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    SwitchAllOn
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_SwitchAllOn
  (JNIEnv *, jobject, jlong);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    SwitchAllOff
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_SwitchAllOff
  (JNIEnv *, jobject, jlong);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    setConfigParam
 * Signature: (JSIII)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_setConfigParam
  (JNIEnv *, jobject, jlong, jint, jint, jint, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    requestConfigParam
 * Signature: (JSI)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_requestConfigParam
  (JNIEnv *, jobject, jlong, jint, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    requestAllConfigParams
 * Signature: (JS)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_requestAllConfigParams
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getNumGroups
 * Signature: (JS)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getNumGroups
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getMaxAssociations
 * Signature: (JSI)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getMaxAssociations
  (JNIEnv *, jobject, jlong, jint, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getGroupLabel
 * Signature: (JSI)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getGroupLabel
  (JNIEnv *, jobject, jlong, jint, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    addAssociation
 * Signature: (JSIII)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_addAssociation
  (JNIEnv *, jobject, jlong, jint, jint, jint, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    removeAssociation
 * Signature: (JSIII)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_removeAssociation
  (JNIEnv *, jobject, jlong, jint, jint, jint, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    resetController
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_resetController
  (JNIEnv *, jobject, jlong);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    softReset
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_softReset
  (JNIEnv *, jobject, jlong);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    cancelControllerCommand
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_cancelControllerCommand
  (JNIEnv *, jobject, jlong);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    testNetworkNode
 * Signature: (JSI)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_testNetworkNode
  (JNIEnv *, jobject, jlong, jint, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    testNetwork
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_testNetwork
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    healNetworkNode
 * Signature: (JSZ)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_healNetworkNode
  (JNIEnv *, jobject, jlong, jint, jboolean);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    healNetwork
 * Signature: (JZ)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_healNetwork
  (JNIEnv *, jobject, jlong, jboolean);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    addNode
 * Signature: (JZ)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_addNode
  (JNIEnv *, jobject, jlong, jboolean);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    removeNode
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_removeNode
  (JNIEnv *, jobject, jlong);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    removeFailedNode
 * Signature: (JS)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_removeFailedNode
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    hasNodeFailed
 * Signature: (JS)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_hasNodeFailed
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    requestNodeNeighborUpdate
 * Signature: (JS)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_requestNodeNeighborUpdate
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    assignReturnRoute
 * Signature: (JS)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_assignReturnRoute
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    deleteAllReturnRoutes
 * Signature: (JS)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_deleteAllReturnRoutes
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    sendNodeInformation
 * Signature: (JS)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_sendNodeInformation
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    createNewPrimary
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_createNewPrimary
  (JNIEnv *, jobject, jlong);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    receiveConfiguration
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_receiveConfiguration
  (JNIEnv *, jobject, jlong);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    replaceFailedNode
 * Signature: (JS)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_replaceFailedNode
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    transferPrimaryRole
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_transferPrimaryRole
  (JNIEnv *, jobject, jlong);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    requestNetworkUpdate
 * Signature: (JS)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_requestNetworkUpdate
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    replicationSend
 * Signature: (JS)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_replicationSend
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    createButton
 * Signature: (JSS)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_createButton
  (JNIEnv *, jobject, jlong, jint, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    deleteButton
 * Signature: (JSS)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_deleteButton
  (JNIEnv *, jobject, jlong, jint, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getNumScenes
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getNumScenes
  (JNIEnv *, jobject);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getAllScenes
 * Signature: ()[S
 */
JNIEXPORT jintArray JNICALL Java_org_kyrillos_jozw_Ozw_getAllScenes
  (JNIEnv *, jobject);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    removeAllScenes
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_removeAllScenes
  (JNIEnv *, jobject, jlong);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    createScene
 * Signature: ()S
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_createScene
  (JNIEnv *, jobject);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    removeScene
 * Signature: (S)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_removeScene
  (JNIEnv *, jobject, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    removeSceneValue
 * Signature: (SLorg/kyrillos/jozw/ValueId;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_removeSceneValue
  (JNIEnv *, jobject, jint, jobject);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    sceneGetValues
 * Signature: (S[Lorg/kyrillos/jozw/ValueId;)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_sceneGetValues
  (JNIEnv *, jobject, jint, jobjectArray); //TODO

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    getSceneLabel
 * Signature: (S)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getSceneLabel
  (JNIEnv *, jobject, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    setSceneLabel
 * Signature: (SLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setSceneLabel
  (JNIEnv *, jobject, jint, jstring);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    sceneExists
 * Signature: (S)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_sceneExists
  (JNIEnv *, jobject, jint);

/*
 * Class:     org_kyrillos_jozw_Ozw
 * Method:    activateScene
 * Signature: (S)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_activateScene
  (JNIEnv *, jobject, jint);

#ifdef __cplusplus
}
#endif
#endif
