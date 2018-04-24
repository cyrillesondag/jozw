/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <jni.h>
#include <Manager.h>
#include <Options.h>
#include <Notification.h>
#include "org_kyrillos_jozw_Ozw.h"

/**************************************************************
 * JNI hooks
 **************************************************************/
static JavaVM* cachedJVM;
static jint JNI_VERSION = JNI_VERSION_1_2;

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    std::cout<<"JNI_OnLoad"<< std::endl;
    cachedJVM = vm;
    return JNI_VERSION;
};

/**************************************************************
 * Helpers class to java object
 **************************************************************/

static void throwIllegalException(JNIEnv *env, const char *msg){
    jclass clazz;
    clazz = env->FindClass("java.lang.IllegalStateException");
    env->ThrowNew(clazz, msg);
}

class ValueIdWrapper {
public:
    explicit ValueIdWrapper(JNIEnv *env);
    void destroy(JNIEnv *env);
    jobject toJava(JNIEnv *env, const OpenZWave::ValueID *valueID);
    OpenZWave::ValueID toValueID(JNIEnv *env, jobject object);
private:
    jclass clazz;
    jmethodID constructor;
    jfieldID nativePtr;
};

ValueIdWrapper::ValueIdWrapper(JNIEnv *env) {
    clazz = reinterpret_cast<jclass> (env->NewGlobalRef(env->FindClass("org/kyrillos/jozw/ValueId")));
    constructor = env->GetMethodID(clazz, "<init>", "(JSISSSI)V");
    nativePtr = env->GetFieldID(clazz, "nativePtr", "J");
}

void ValueIdWrapper::destroy(JNIEnv *env) {
    env->DeleteGlobalRef(clazz);
    constructor = nullptr;
}

jobject ValueIdWrapper::toJava(JNIEnv *env, OpenZWave::ValueID const *valueID) {
    if(nullptr == valueID){
        return nullptr;
    }
    return env->NewObject(
            clazz,
            constructor,
            valueID->GetHomeId(),
            valueID->GetNodeId(),
            valueID->GetGenre(),
            valueID->GetCommandClassId(),
            valueID->GetInstance(),
            valueID->GetIndex(),
            valueID->GetType()
    );
}

OpenZWave::ValueID ValueIdWrapper::toValueID(JNIEnv *env, jobject object){
    jlong ptr;
    OpenZWave::ValueID* valueIDptr;
    ptr = env->GetLongField(object, nativePtr);
    valueIDptr = reinterpret_cast<OpenZWave::ValueID*>(ptr);
    return *valueIDptr;
}

class NotificationWrapper {
public:
    explicit NotificationWrapper(JNIEnv *env);
    void destroy(JNIEnv *env);
    jobject toJava(JNIEnv *env, const OpenZWave::Notification *notification);
private:
    jclass clazz;
    jmethodID constructor;
    ValueIdWrapper *valueIdWrapper;
};

NotificationWrapper::NotificationWrapper(JNIEnv *env) {
    jclass _temp;

    _temp = (*env).FindClass("org/kyrillos/jozw/Notification");
    clazz = reinterpret_cast<jclass> (env->NewGlobalRef(_temp));
    constructor = env->GetMethodID(clazz, "<init>", "(IJSLorg/kyrillos/jozw/ValueId;SSSSSS)V");
    valueIdWrapper = new ValueIdWrapper(env);
}

void NotificationWrapper::destroy(JNIEnv *env) {
    env->DeleteGlobalRef(clazz);
    valueIdWrapper->destroy(env);
    constructor = nullptr;
}

jobject NotificationWrapper::toJava(JNIEnv *env, OpenZWave::Notification const *notification) {
    if (nullptr == notification){
        return nullptr;
    }
    notification->GetByte();
    return env->NewObject(
            clazz,
            constructor,
            notification->GetType(),
            notification->GetHomeId(),
            notification->GetNodeId(),
            valueIdWrapper->toJava(env, &notification->GetValueID()),
            notification->GetGroupIdx(),
            notification->GetEvent(),
            notification->GetButtonId(),
            notification->GetSceneId(),
            notification->GetNotification(),
            notification->GetByte()
    );
}

class OzwMapper {
public:
    OzwMapper(JNIEnv* env, jobject  instance);
    void destroy(JNIEnv* env);
    void onNotification (OpenZWave::Notification const* _pNotification);
private:
    jobject ozwInst;
    jclass ozwClass;
    jmethodID ozwNotifMethod;
    NotificationWrapper *notificationWrapper;
};

OzwMapper::OzwMapper(JNIEnv* env, jobject  instance){
    ozwInst = env->NewGlobalRef(instance);
    ozwClass = reinterpret_cast<jclass> (env->NewGlobalRef(env->GetObjectClass(instance)));
    ozwNotifMethod = env->GetMethodID(ozwClass, "onNotification", "(Lorg/kyrillos/jozw/Notification;)V");
    notificationWrapper = new NotificationWrapper(env);
}

void OzwMapper::destroy(JNIEnv *env) {
    env->DeleteGlobalRef(ozwInst);
    env->DeleteGlobalRef(ozwClass);

    notificationWrapper->destroy(env);
    ozwNotifMethod = nullptr;
}

void OzwMapper::onNotification(OpenZWave::Notification const *_pNotification) {
    JNIEnv *env;
    jint res;
    jobject notification;
    jthrowable ex;

    res = cachedJVM->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION);
    if(res == JNI_EDETACHED){
        if (cachedJVM->AttachCurrentThread(reinterpret_cast<void**>(&env), nullptr) != 0){
            std::cout << "Failed to attacher Thread" << std::endl;
            return;
        }
    }

    notification = notificationWrapper->toJava(env, _pNotification);
    env->CallVoidMethod(ozwInst, ozwNotifMethod, notification);
    ex = env->ExceptionOccurred();
    if (ex){
        env->Throw(ex);
    }
}

ValueIdWrapper *valueIdWrapper;
OzwMapper *ozwMapper;

static void handleNotification(OpenZWave::Notification const *_pNotification, void *_context){
    OzwMapper *mapper;
    mapper = static_cast<OzwMapper *>(_context);
    if (mapper != nullptr){
        mapper->onNotification(_pNotification);
    }
}

/**************************************************************
 * JNI methods
 **************************************************************/

JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getVersionAsString
    (JNIEnv *env, jclass inst){
    std::string str;

    str = OpenZWave::Manager::getVersionAsString();
    return env->NewStringUTF(str.c_str());
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    doCreate
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT jlong JNICALL Java_org_kyrillos_jozw_Ozw_doCreate
    (JNIEnv *env, jobject instance, jstring configPath, jstring userPath, jstring arguments) {
    std::string *_configPath;
    std::string *_userPath;
    std::string *_arguments;

    OpenZWave::Options* options;
    OpenZWave::Manager* manager;

    _configPath = new std::string(env->GetStringUTFChars(configPath, nullptr));
    _userPath = new std::string(env->GetStringUTFChars(userPath, nullptr));
    _arguments= new std::string(env->GetStringUTFChars(arguments, nullptr));

    options= OpenZWave::Options::Create(*_configPath, *_userPath, *_arguments);
    options->Lock();

    manager= OpenZWave::Manager::Create();
    ozwMapper = new OzwMapper(env, instance);
    manager->AddWatcher(handleNotification, ozwMapper);

    valueIdWrapper = new ValueIdWrapper(env);

    return (jlong) manager;
}

JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_doAddDriver
    (JNIEnv *env, jobject instance, jstring path, jint interfaceType){
    bool res;
    const char *_path;

    _path = env->GetStringUTFChars(path, nullptr);
    switch (interfaceType){
        case 1:
            res = OpenZWave::Manager::Get()->AddDriver(_path, OpenZWave::Driver::ControllerInterface_Serial);
            break;
        case 2:
            res = OpenZWave::Manager::Get()->AddDriver(_path, OpenZWave::Driver::ControllerInterface_Hid);
            break;
        default:
            res = OpenZWave::Manager::Get()->AddDriver(_path, OpenZWave::Driver::ControllerInterface_Unknown);
            break;
    }
    env->ReleaseStringUTFChars(path, _path);
    return static_cast<jboolean>(res);
}


/*
 * Class:     my_project_openzwave_Manager
 * Method:    destroy
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_destroy
        (JNIEnv *env, jclass clazz){
    OpenZWave::Manager::Destroy();
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    writeConfig
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_writeConfig
        (JNIEnv *env, jobject instance, jlong homeId){
    
    OpenZWave::Manager::Get()->WriteConfig(static_cast<const uint32>(homeId));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    removeDriver
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_removeDriver
        (JNIEnv *env, jobject instance, jstring driverPath){
    const char *_driverPath;

    _driverPath = env->GetStringUTFChars(driverPath, nullptr);
    return jboolean (OpenZWave::Manager::Get()->RemoveDriver(_driverPath));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getControllerNodeId
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getControllerNodeId
        (JNIEnv *env, jobject instance, jlong homeId){
    return jint (OpenZWave::Manager::Get()->GetControllerNodeId(static_cast<const uint32>(homeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getSUCNodeId
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getSUCNodeId
        (JNIEnv *env, jobject instance,  jlong homeId){
    return jint (OpenZWave::Manager::Get()->GetSUCNodeId(static_cast<const uint32>(homeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    isPrimaryController
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isPrimaryController
        (JNIEnv *env, jobject instance,  jlong homeId){
    return jboolean (OpenZWave::Manager::Get()->IsPrimaryController(static_cast<const uint32>(homeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    isStaticUpdateController
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isStaticUpdateController
        (JNIEnv *env, jobject instance,  jlong homeId){
    return jboolean (OpenZWave::Manager::Get()->IsStaticUpdateController(static_cast<const uint32>(homeId)));
}
/*
 * Class:     my_project_openzwave_Manager
 * Method:    isBridgeController
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isBridgeController
        (JNIEnv *env, jobject instance,  jlong homeId){
    return jboolean (OpenZWave::Manager::Get()->IsBridgeController(static_cast<const uint32>(homeId)));
}
/*
 * Class:     my_project_openzwave_Manager
 * Method:    getLibraryVersion
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getLibraryVersion
        (JNIEnv *env, jobject instance,  jlong homeId){
    std::string _version;
    _version = OpenZWave::Manager::Get()->GetLibraryVersion(static_cast<const uint32>(homeId));
    return env->NewStringUTF(_version.data());
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getLibraryTypeName
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getLibraryTypeName
        (JNIEnv *env, jobject instance,  jlong homeId){
    std::string _type;
    _type = OpenZWave::Manager::Get()->GetLibraryTypeName(static_cast<const uint32>(homeId));
    return env->NewStringUTF(_type.data());
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getSendQueueCount
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL Java_org_kyrillos_jozw_Ozw_getSendQueueCount
        (JNIEnv *env, jobject instance,  jlong homeId){
    int32 _count;
    _count = OpenZWave::Manager::Get()->GetSendQueueCount(static_cast<const uint32>(homeId));
    return jlong (_count);
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    logDriverStatistics
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_logDriverStatistics
        (JNIEnv *env, jobject instance,  jlong homeId){
    OpenZWave::Manager::Get()->LogDriverStatistics(static_cast<const uint32>(homeId));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getControllerInterfaceType
 * Signature: (I)Lmy/project/openzwave/ControllerInterface;
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getControllerInterfaceType
        (JNIEnv *env, jobject instance, jlong homeId){
    OpenZWave::Driver::ControllerInterface type = OpenZWave::Manager::Get()->GetControllerInterfaceType(static_cast<const uint32>(homeId));
    switch (type){
        case OpenZWave::Driver::ControllerInterface_Serial:
            return 1;
        case OpenZWave::Driver::ControllerInterface_Hid:
            return 2;
        default:
            return 3;
    }
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getControllerPath
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getControllerPath
        (JNIEnv *env, jobject instance,  jlong homeId){
    std::string _path;
    _path = OpenZWave::Manager::Get()->GetControllerPath(static_cast<const uint32>(homeId));
    if(&_path != nullptr){
        return env->NewStringUTF(_path.data());
    }
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getPollInterval
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_org_kyrillos_jozw_Ozw_getPollInterval
        (JNIEnv *env, jobject instance){
    return jlong (OpenZWave::Manager::Get()->GetPollInterval());
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    setPollInterval
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setPollInterval
        (JNIEnv *env, jobject instance, jlong interval){
    OpenZWave::Manager::Get()->SetPollInterval((int32) interval, true);
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    enablePoll
 * Signature: (Lmy/project/openzwave/ValueID;I)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_enablePoll
        (JNIEnv *env, jobject instance, jobject valueId, jint itensity){
    return jboolean (OpenZWave::Manager::Get()->EnablePoll(valueIdWrapper->toValueID(env, valueId), static_cast<uint8>(itensity)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    disablePoll
 * Signature: (Lmy/project/openzwave/ValueID;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_disablePoll
        (JNIEnv *env, jobject instance, jobject valueId){
    return jboolean (OpenZWave::Manager::Get()->DisablePoll(valueIdWrapper->toValueID(env, valueId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    isPolled
 * Signature: (Lmy/project/openzwave/ValueID;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isPolled
        (JNIEnv *env, jobject instance, jobject valueId){
    return jboolean (OpenZWave::Manager::Get()->IsValuePolled(valueIdWrapper->toValueID(env, valueId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    setPollIntensity
 * Signature: (Lmy/project/openzwave/ValueID;I)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setPollIntensity
        (JNIEnv *env, jobject instance, jobject valueId, jint itensity){
    OpenZWave::Manager::Get()->SetPollIntensity(valueIdWrapper->toValueID(env, valueId), static_cast<uint8>(itensity));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getPollIntensity
 * Signature: (Lmy/project/openzwave/ValueID;)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getPollIntensity
        (JNIEnv *env, jobject instance, jobject valueId){
    return jint (OpenZWave::Manager::Get()->GetPollIntensity(valueIdWrapper->toValueID(env, valueId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    refreshNodeInfo
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_refreshNodeInfo
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    return jboolean (OpenZWave::Manager::Get()->RefreshNodeInfo(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    requestNodeState
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_requestNodeState
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    return jboolean (OpenZWave::Manager::Get()->RequestNodeState(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    requestNodeDynamic
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_requestNodeDynamic
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    jboolean (OpenZWave::Manager::Get()->RequestNodeDynamic(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    isNodeListeningDevice
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isNodeListeningDevice
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    return jboolean (OpenZWave::Manager::Get()->IsNodeListeningDevice(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    isNodeFrequentListeningDevice
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isNodeFrequentListeningDevice
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    return jboolean (OpenZWave::Manager::Get()->IsNodeFrequentListeningDevice(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    isNodeBeamingDevice
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isNodeBeamingDevice
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    return jboolean (OpenZWave::Manager::Get()->IsNodeBeamingDevice(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    isNodeRoutingDevice
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isNodeRoutingDevice
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    return jboolean (OpenZWave::Manager::Get()->IsNodeRoutingDevice(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    isNodeSecurityDevice
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isNodeSecurityDevice
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    return jboolean (OpenZWave::Manager::Get()->IsNodeSecurityDevice(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeMaxBaudRate
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getNodeMaxBaudRate
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    return jint (OpenZWave::Manager::Get()->GetNodeMaxBaudRate(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeVersion
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getNodeVersion
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    return jboolean (OpenZWave::Manager::Get()->GetNodeVersion(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeSecurity
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getNodeSecurity
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    return jint (OpenZWave::Manager::Get()->GetNodeSecurity(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    isNodeZWavePlus
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isNodeZWavePlus
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    return jboolean (OpenZWave::Manager::Get()->IsNodeZWavePlus(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeBasic
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getNodeBasic
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    return jint (OpenZWave::Manager::Get()->GetNodeBasic(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeGeneric
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getNodeGeneric
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    return jint (OpenZWave::Manager::Get()->GetNodeGeneric(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeSpecific
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getNodeSpecific
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    return jint (OpenZWave::Manager::Get()->GetNodeSpecific(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeType
 * Signature: (II)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getNodeType
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    std::string _type;
    _type = OpenZWave::Manager::Get()->GetNodeType(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId));
    if (&_type != nullptr){
        return env->NewStringUTF(_type.data());
    }
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeNeighbors
 * Signature: (II)[I
 */
JNIEXPORT jintArray JNICALL Java_org_kyrillos_jozw_Ozw_getNodeNeighbors
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    uint8 *neighbors;
    int numNeighbors;
    numNeighbors = OpenZWave::Manager::Get()->GetNodeNeighbors(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId), &neighbors);
    if(&neighbors == nullptr){
        jint fill[numNeighbors];
        jintArray _intArray;
        for (int nr = 0; nr < numNeighbors; nr++) {
            fill[nr] = static_cast<jint>(neighbors[nr]);
        }
        _intArray = env->NewIntArray(numNeighbors);
        env->SetIntArrayRegion(_intArray, 0, numNeighbors, fill);

        delete neighbors;

        return _intArray;
    }
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeManufacturerName
 * Signature: (II)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getNodeManufacturerName
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    std::string _manufacturer;
    _manufacturer = OpenZWave::Manager::Get()->GetNodeManufacturerName(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId));
    if (&_manufacturer != nullptr){
        return env->NewStringUTF(_manufacturer.data());
    }
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeProductName
 * Signature: (II)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getNodeProductName
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    std::string _productName;
    _productName = OpenZWave::Manager::Get()->GetNodeProductName(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId));
    if (&_productName != nullptr){
        return env->NewStringUTF(_productName.data());
    }
}
/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeName
 * Signature: (II)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getNodeName
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    std::string _nodeName;
    _nodeName = OpenZWave::Manager::Get()->GetNodeName(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId));
    if (&_nodeName != nullptr){
        return env->NewStringUTF(_nodeName.data());
    }
}
/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeLocation
 * Signature: (II)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getNodeLocation
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    std::string _nodeLocation;
    _nodeLocation = OpenZWave::Manager::Get()->GetNodeLocation(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId));
    if (&_nodeLocation != nullptr){
        return env->NewStringUTF(_nodeLocation.data());
    }
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeManufacturerId
 * Signature: (II)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getNodeManufacturerId
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    std::string _nodeManufacturer;
    _nodeManufacturer = OpenZWave::Manager::Get()->GetNodeManufacturerId(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId));
    if (&_nodeManufacturer != nullptr){
        return env->NewStringUTF(_nodeManufacturer.data());
    }
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeProductType
 * Signature: (II)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getNodeProductType
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    std::string _nodeProductType;
    _nodeProductType = OpenZWave::Manager::Get()->GetNodeProductType(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId));
    if (&_nodeProductType != nullptr){
        return env->NewStringUTF(_nodeProductType.data());
    }
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeProductId
 * Signature: (II)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getNodeProductId
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    std::string _nodeProductId;
    _nodeProductId = OpenZWave::Manager::Get()->GetNodeProductId(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId));
    if (&_nodeProductId != nullptr){
        return env->NewStringUTF(_nodeProductId.data());
    }
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    setNodeManufacturerName
 * Signature: (IILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setNodeManufacturerName
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId, jstring name){
    if(&name != nullptr){
        const char *_name = env->GetStringUTFChars(name, nullptr);
        OpenZWave::Manager::Get()->SetNodeManufacturerName(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId), _name);
    }
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    setNodeProductName
 * Signature: (IILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setNodeProductName
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId, jstring productName){
    if(productName != nullptr){
        const char *_productName = env->GetStringUTFChars(productName, nullptr);
        OpenZWave::Manager::Get()->SetNodeManufacturerName(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId), _productName);
    }
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    setNodeName
 * Signature: (IILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setNodeName
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId, jstring nodeName){
    if(nodeName != nullptr){
        const char *_nodeName = env->GetStringUTFChars(nodeName, nullptr);
        OpenZWave::Manager::Get()->SetNodeManufacturerName(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId), _nodeName);
    }
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    setNodeLocation
 * Signature: (IILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setNodeLocation
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId, jstring location){
    if(location != nullptr){
        const char *_nodeLocation = env->GetStringUTFChars(location, nullptr);
        OpenZWave::Manager::Get()->SetNodeManufacturerName(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId), _nodeLocation);
    }
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    setNodeOn
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setNodeOn
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    OpenZWave::Manager::Get()->SetNodeOn(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    setNodeOff
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setNodeOff
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    OpenZWave::Manager::Get()->SetNodeOff(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId));
}
/*
 * Class:     my_project_openzwave_Manager
 * Method:    setNodeLevel
 * Signature: (III)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setNodeLevel
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId, jint level){
    OpenZWave::Manager::Get()->SetNodeLevel(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId), static_cast<uint8>(level));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    isNodeInfoReceived
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isNodeInfoReceived
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    return jboolean (OpenZWave::Manager::Get()->IsNodeInfoReceived(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeClassInformation
 * Signature: (IIILjava/lang/String;I)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_getNodeClassInformation
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId, jint commandClassId, jstring className, jobject classVersion){
    std::string _className;
    uint8 _version;
    if (className != nullptr) {
        std::string _temp = env->GetStringUTFChars(className, nullptr);
    }

    if (classVersion != nullptr){
        jmethodID getVal;
        getVal = env->GetMethodID(env->GetObjectClass(classVersion), "intValue", "()I");
        _version = static_cast<uint8>(env->CallIntMethod(classVersion, getVal));
    }
    return jboolean (OpenZWave::Manager::Get()->GetNodeClassInformation(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId), static_cast<uint8>(commandClassId), &_className, &_version));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    isNodeAwake
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isNodeAwake
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    return jboolean (OpenZWave::Manager::Get()->IsNodeAwake(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    isNodeFailed
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isNodeFailed
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    return jboolean (OpenZWave::Manager::Get()->IsNodeFailed(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}
/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeQueryStage
 * Signature: (II)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getNodeQueryStage
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId) {
    std::string _queryStage;
    _queryStage = OpenZWave::Manager::Get()->GetNodeQueryStage(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId));
    if(&_queryStage != nullptr){
        return env->NewStringUTF(_queryStage.data());
    }
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeDeviceType
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getNodeDeviceType
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId) {
    return jint(OpenZWave::Manager::Get()->GetNodeDeviceType(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}
/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeDeviceTypeString
 * Signature: (II)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getNodeDeviceTypeString
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId) {
    std::string _typeString;
    _typeString = OpenZWave::Manager::Get()->GetNodeDeviceTypeString(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId));
    if(&_typeString != nullptr){
        return env->NewStringUTF(_typeString.data());
    }
}
/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeRole
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getNodeRole
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId) {
    return jint(OpenZWave::Manager::Get()->GetNodeRole(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}
/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeRoleString
 * Signature: (II)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getNodeRoleString
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId) {
    std::string _role;
    _role = OpenZWave::Manager::Get()->GetNodeRoleString(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId));
    if(&_role != nullptr){
        return env->NewStringUTF(_role.data());
    }
}
/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodePlusType
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getNodePlusType
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId) {
    return jint(OpenZWave::Manager::Get()->GetNodePlusType(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}
/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodePlusTypeString
 * Signature: (II)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getNodePlusTypeString
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId) {
    std::string _plusType;
    _plusType = OpenZWave::Manager::Get()->GetNodePlusTypeString(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId));
    if(&_plusType != nullptr){
        return env->NewStringUTF(_plusType.data());
    }
}
/*
 * Class:     my_project_openzwave_Manager
 * Method:    getValueLabel
 * Signature: (Lmy/project/openzwave/ValueID;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getValueLabel
        (JNIEnv *env, jobject instance, jobject valueId){
    std::string _label;
    _label = OpenZWave::Manager::Get()->GetValueLabel(valueIdWrapper->toValueID(env, valueId));
    if (&_label != nullptr){
        return env->NewStringUTF(_label.data());
    }
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    setValueLabel
 * Signature: (Lmy/project/openzwave/ValueID;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setValueLabel
        (JNIEnv *env, jobject instance, jobject valueId, jstring label){
    if(label != nullptr){
        const char *_label = env->GetStringUTFChars(label, nullptr);
        OpenZWave::Manager::Get()->SetValueLabel(valueIdWrapper->toValueID(env, valueId), _label);
    }
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getValueUnits
 * Signature: (Lmy/project/openzwave/ValueID;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getValueUnits
        (JNIEnv *env, jobject instance, jobject valueId){
    std::string _valueUnit;
    _valueUnit = OpenZWave::Manager::Get()->GetValueLabel(valueIdWrapper->toValueID(env, valueId));
    if (&_valueUnit != nullptr){
        return env->NewStringUTF(_valueUnit.data());
    }
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    setValueUnits
 * Signature: (Lmy/project/openzwave/ValueID;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setValueUnits
        (JNIEnv *env, jobject instance, jobject valueId, jstring unit){
    if(unit != nullptr){
        const char *_unit = env->GetStringUTFChars(unit, nullptr);
        OpenZWave::Manager::Get()->SetValueLabel(valueIdWrapper->toValueID(env, valueId), _unit);
    }
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getValueHelp
 * Signature: (Lmy/project/openzwave/ValueID;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getValueHelp
        (JNIEnv *env, jobject instance, jobject valueId){
    std::string _valueHelp;
    _valueHelp = OpenZWave::Manager::Get()->GetValueHelp(valueIdWrapper->toValueID(env, valueId));
    if (&_valueHelp != nullptr){
        return env->NewStringUTF(_valueHelp.data());
    }
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    setValueHelp
 * Signature: (Lmy/project/openzwave/ValueID;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setValueHelp
        (JNIEnv *env, jobject instance, jobject valueId, jstring help){
    if(help != nullptr){
        const char *_help = env->GetStringUTFChars(help, nullptr);
        OpenZWave::Manager::Get()->SetValueHelp(valueIdWrapper->toValueID(env, valueId), _help);
    }
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getValueMin
 * Signature: (Lmy/project/openzwave/ValueID;)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getValueMin
        (JNIEnv *env, jobject instance, jobject valueId){
    return jint (OpenZWave::Manager::Get()->GetValueMin(valueIdWrapper->toValueID(env, valueId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getValueMax
 * Signature: (Lmy/project/openzwave/ValueID;)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getValueMax
        (JNIEnv *env, jobject instance, jobject valueId){
    return jint (OpenZWave::Manager::Get()->GetValueMax(valueIdWrapper->toValueID(env, valueId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    isValueReadOnly
 * Signature: (Lmy/project/openzwave/ValueID;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isValueReadOnly
        (JNIEnv *env, jobject instance, jobject valueId){
    return jboolean (OpenZWave::Manager::Get()->IsValueReadOnly(valueIdWrapper->toValueID(env, valueId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    isValueWriteOnly
 * Signature: (Lmy/project/openzwave/ValueID;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isValueWriteOnly
        (JNIEnv *env, jobject instance, jobject valueId){
    return jboolean (OpenZWave::Manager::Get()->IsValueWriteOnly(valueIdWrapper->toValueID(env, valueId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    isValueSet
 * Signature: (Lmy/project/openzwave/ValueID;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isValueSet
        (JNIEnv *env, jobject instance, jobject valueId){
    return jboolean (OpenZWave::Manager::Get()->IsValueSet(valueIdWrapper->toValueID(env, valueId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    isValuePolled
 * Signature: (Lmy/project/openzwave/ValueID;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isValuePolled
        (JNIEnv *env, jobject instance, jobject valueId){
    return jboolean (OpenZWave::Manager::Get()->IsValuePolled(valueIdWrapper->toValueID(env, valueId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getValueAsBool
 * Signature: (Lmy/project/openzwave/ValueID;)Lmy/project/openzwave/Value;
 */
JNIEXPORT jobject JNICALL Java_org_kyrillos_jozw_Ozw_getValueAsBool
        (JNIEnv *env, jobject instance, jobject valueId){
    //TODO
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getValueListItemsAsString
 * Signature: (Lmy/project/openzwave/ValueID;)Ljava/util/List;
 */
JNIEXPORT jobject JNICALL Java_org_kyrillos_jozw_Ozw_getValueListItemsAsString
        (JNIEnv *env, jobject instance, jobject valueId){
    //TODO
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getValueListValuesAsInt
 * Signature: (Lmy/project/openzwave/ValueID;)Ljava/util/List;
 */
JNIEXPORT jobject JNICALL Java_org_kyrillos_jozw_Ozw_getValueListValuesAsInt
        (JNIEnv *env, jobject instance, jobject valueId){
    //TODO
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getValueFloatPrecision
 * Signature: (Lmy/project/openzwave/ValueID;)Ljava/lang/Float;
 */
JNIEXPORT jobject JNICALL Java_org_kyrillos_jozw_Ozw_getValueFloatPrecision
        (JNIEnv *env, jobject instance, jobject valueId){
    //TODO
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    setValue
 * Signature: (Lmy/project/openzwave/ValueID;Lmy/project/openzwave/Value;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_setValue
        (JNIEnv *env, jobject instance, jobject valueId, jobject);

/*
 * Class:     my_project_openzwave_Manager
 * Method:    refreshValue
 * Signature: (Lmy/project/openzwave/ValueID;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_refreshValue
        (JNIEnv *env, jobject instance, jobject valueId){
    return jboolean (OpenZWave::Manager::Get()->RefreshValue(valueIdWrapper->toValueID(env, valueId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    setChangeVerified
 * Signature: (Lmy/project/openzwave/ValueID;Z)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setChangeVerified
        (JNIEnv *env, jobject instance, jobject valueId, jboolean verify){
    OpenZWave::Manager::Get()->SetChangeVerified(valueIdWrapper->toValueID(env, valueId), verify);
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getChangeVerified
 * Signature: (Lmy/project/openzwave/ValueID;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_getChangeVerified
        (JNIEnv *env, jobject instance, jobject valueId){
    return jboolean (OpenZWave::Manager::Get()->GetChangeVerified(valueIdWrapper->toValueID(env, valueId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    pressButton
 * Signature: (Lmy/project/openzwave/ValueID;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_pressButton
        (JNIEnv *env, jobject instance, jobject valueId){
    return jboolean (OpenZWave::Manager::Get()->PressButton(valueIdWrapper->toValueID(env, valueId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    releaseButton
 * Signature: (Lmy/project/openzwave/ValueID;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_releaseButton
        (JNIEnv *env, jobject instance, jobject valueId){
    return jboolean (OpenZWave::Manager::Get()->ReleaseButton(valueIdWrapper->toValueID(env, valueId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNumSwitchPoints
 * Signature: (Lmy/project/openzwave/ValueID;)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getNumSwitchPoints
        (JNIEnv *env, jobject instance, jobject valueId){
    return jint (OpenZWave::Manager::Get()->GetNumSwitchPoints(valueIdWrapper->toValueID(env, valueId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    setSwitchPoint
 * Signature: (Lmy/project/openzwave/ValueID;III)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_setSwitchPoint
        (JNIEnv *env, jobject instance, jobject valueId, jint hours, jint minutes, jint setback){
    return jboolean (OpenZWave::Manager::Get()->SetSwitchPoint(valueIdWrapper->toValueID(env, valueId), static_cast<uint8>(hours), static_cast<uint8>(minutes), static_cast<uint8>(setback)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    RemoveSwitchPoint
 * Signature: (Lmy/project/openzwave/ValueID;II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_RemoveSwitchPoint
        (JNIEnv *env, jobject instance, jobject valueId, jint hours, jint minutes){
    return jboolean (OpenZWave::Manager::Get()->RemoveSwitchPoint(valueIdWrapper->toValueID(env, valueId), static_cast<uint8>(hours), static_cast<uint8>(minutes)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    ClearSwitchPoints
 * Signature: (Lmy/project/openzwave/ValueID;)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_ClearSwitchPoints
        (JNIEnv *env, jobject instance, jobject valueId){
    OpenZWave::Manager::Get()->ClearSwitchPoints(valueIdWrapper->toValueID(env, valueId));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getSwitchPoint
 * Signature: (Lmy/project/openzwave/ValueID;IIII)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_getSwitchPoint
        (JNIEnv *env, jobject instance, jobject valueId, jint idx, jint hours, jint minutes, jint setback){
    uint8 _idx;
    uint8 _hours;
    uint8 _minutes;
    int8 _setBack;

    _idx = static_cast<uint8>(idx);
    _hours = static_cast<uint8>(hours);
    _minutes = static_cast<uint8>(minutes);
    _setBack= static_cast<int8>(setback);
    OpenZWave::Manager::Get()->GetSwitchPoint(valueIdWrapper->toValueID(env, valueId), _idx, &_hours, &_minutes, &_setBack);
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    SwitchAllOn
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_SwitchAllOn
        (JNIEnv *env, jobject instance, jlong homeId){
    OpenZWave::Manager::Get()->SwitchAllOn(static_cast<const uint32>(homeId));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    SwitchAllOff
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_SwitchAllOff
        (JNIEnv *env, jobject instance, jlong homeId){
    OpenZWave::Manager::Get()->SwitchAllOff(static_cast<const uint32>(homeId));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    setConfigParam
 * Signature: (IIIII)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_setConfigParam
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId, jint paramId, jint paramValue, jint size){
    jboolean (OpenZWave::Manager::Get()->SetConfigParam(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId), static_cast<uint8>(paramId), static_cast<const uint32>(paramValue), static_cast<uint8>(size)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    requestConfigParam
 * Signature: (III)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_requestConfigParam
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId, jint paramId){
    OpenZWave::Manager::Get()->RequestConfigParam(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId), static_cast<uint8>(paramId));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    requestAllConfigParams
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_requestAllConfigParams
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    OpenZWave::Manager::Get()->RequestAllConfigParams(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNumGroups
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getNumGroups
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    return jint (OpenZWave::Manager::Get()->GetNumGroups(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getAssociations
 * Signature: (III)[Lmy/project/openzwave/InstanceAssociation;
 */
JNIEXPORT jobjectArray JNICALL Java_org_kyrillos_jozw_Ozw_getAssociations
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId, jint groupId){
    OpenZWave::InstanceAssociation *associations;
    int associatioCount;
    associatioCount = OpenZWave::Manager::Get()->GetAssociations(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId), static_cast<uint8>(groupId), &associations);
    if (&associations != nullptr){
        jclass classId;
        jmethodID contructorId;
        jobject _temp;
        jobjectArray _objArray;

        classId = env->FindClass("org.kyrillos.jowz.InstanceAssociation");
        if (classId == nullptr){
            throwIllegalException(env, "Couldn't find class org.kyrillos.jowz.InstanceAssociation");
        }

        contructorId = env->GetMethodID(classId, "<init>", "(JJ)V");
        if(contructorId == nullptr){
            throwIllegalException(env, "Couldn't find constructor for InstanceAssociation");
        }

        _objArray = env->NewObjectArray(associatioCount, classId, nullptr);
        for (int i = 0; i < associatioCount; ++i) {
            OpenZWave::InstanceAssociation asso = associations[i];
            _temp = env->NewObject(classId, contructorId, static_cast<jint>(asso.m_nodeId), static_cast<jint>(asso.m_instance));
            env->SetObjectArrayElement(_objArray, i, _temp);
        }

        delete associations;

        return _objArray;
    }
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getMaxAssociations
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getMaxAssociations
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId, jint groupId){
    return jint (OpenZWave::Manager::Get()->GetMaxAssociations(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId), static_cast<uint8>(groupId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getGroupLabel
 * Signature: (III)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getGroupLabel
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId, jint groupId){
    std::string _label;
    _label = OpenZWave::Manager::Get()->GetGroupLabel(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId), static_cast<uint8>(groupId));
    if (&_label != nullptr){
        return env->NewStringUTF(_label.data());
    }
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    addAssociation
 * Signature: (IIIII)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_addAssociation
        (JNIEnv *env, jobject instance, jlong homeId, jint, jint, jint, jint){
    //TODO
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    removeAssociation
 * Signature: (IIIII)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_removeAssociation
        (JNIEnv *env, jobject instance, jlong homeId, jint, jint, jint, jint){
    //TODO
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    resetController
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_resetController
        (JNIEnv *env, jobject instance, jlong homeId){
    OpenZWave::Manager::Get()->ResetController(static_cast<const uint32>(homeId));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    softReset
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_softReset
        (JNIEnv *env, jobject instance, jlong homeId){
    OpenZWave::Manager::Get()->SoftReset(static_cast<const uint32>(homeId));
}
/*
 * Class:     my_project_openzwave_Manager
 * Method:    cancelControllerCommand
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_cancelControllerCommand
        (JNIEnv *env, jobject instance, jlong homeId){
    return jboolean (OpenZWave::Manager::Get()->CancelControllerCommand(static_cast<const uint32>(homeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    testNetworkNode
 * Signature: (III)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_testNetworkNode
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId, jint count){
    OpenZWave::Manager::Get()->TestNetworkNode(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId), static_cast<const uint32>(count));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    testNetwork
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_testNetwork
        (JNIEnv *env, jobject instance, jlong homeId, jint count){
    OpenZWave::Manager::Get()->TestNetwork(static_cast<const uint32>(homeId), static_cast<const uint32>(count));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    healNetworkNode
 * Signature: (IIZ)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_healNetworkNode
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId, jboolean doRR){
    OpenZWave::Manager::Get()->HealNetworkNode(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId), doRR);
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    healNetwork
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_healNetwork
        (JNIEnv *env, jobject instance, jlong homeId, jboolean doRR){
    OpenZWave::Manager::Get()->HealNetwork(static_cast<const uint32>(homeId), doRR);
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    addNode
 * Signature: (IZ)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_addNode
        (JNIEnv *env, jobject instance, jlong homeId, jboolean doSecurity){
    return jboolean (OpenZWave::Manager::Get()->AddNode(static_cast<const uint32>(homeId), doSecurity));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    removeNode
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_removeNode
        (JNIEnv *env, jobject instance, jlong homeId){
    return jboolean (OpenZWave::Manager::Get()->RemoveNode(static_cast<const uint32>(homeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    removeFailedNode
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_removeFailedNode
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    return jboolean (OpenZWave::Manager::Get()->RemoveFailedNode(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}
/*
 * Class:     my_project_openzwave_Manager
 * Method:    hasNodeFailed
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_hasNodeFailed
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    return jboolean (OpenZWave::Manager::Get()->HasNodeFailed(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    requestNodeNeighborUpdate
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_requestNodeNeighborUpdate
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    return jboolean (OpenZWave::Manager::Get()->RequestNodeNeighborUpdate(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    assignReturnRoute
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_assignReturnRoute
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    return jboolean (OpenZWave::Manager::Get()->AssignReturnRoute(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    deleteAllReturnRoutes
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_deleteAllReturnRoutes
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    return jboolean (OpenZWave::Manager::Get()->DeleteAllReturnRoutes(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    sendNodeInformation
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_sendNodeInformation
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    return jboolean (OpenZWave::Manager::Get()->SendNodeInformation(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    createNewPrimary
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_createNewPrimary
        (JNIEnv *env, jobject instance, jlong homeId){
    return jboolean (OpenZWave::Manager::Get()->CreateNewPrimary(static_cast<const uint32>(homeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    receiveConfiguration
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_receiveConfiguration
        (JNIEnv *env, jobject instance, jlong homeId){
    return jboolean (OpenZWave::Manager::Get()->ReceiveConfiguration(static_cast<const uint32>(homeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    replaceFailedNode
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_replaceFailedNode
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    return jboolean (OpenZWave::Manager::Get()->ReplaceFailedNode(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    transferPrimaryRole
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_transferPrimaryRole
        (JNIEnv *env, jobject instance, jlong homeId){
    return jboolean (OpenZWave::Manager::Get()->TransferPrimaryRole(static_cast<const uint32>(homeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    requestNetworkUpdate
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_requestNetworkUpdate
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    return jboolean (OpenZWave::Manager::Get()->RequestNetworkUpdate(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    replicationSend
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_replicationSend
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId){
    return jboolean (OpenZWave::Manager::Get()->ReplicationSend(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    createButton
 * Signature: (III)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_createButton
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId, jint buttonId){
    return jboolean (OpenZWave::Manager::Get()->CreateButton(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId), static_cast<uint8>(buttonId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    deleteButton
 * Signature: (III)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_deleteButton
        (JNIEnv *env, jobject instance, jlong homeId, jint nodeId, jint buttonId){
    return jboolean (OpenZWave::Manager::Get()->DeleteButton(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId), static_cast<uint8>(buttonId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNumScenes
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getNumScenes
        (JNIEnv *env, jobject instance){
    return jint (OpenZWave::Manager::Get()->GetNumScenes());
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getAllScenes
 * Signature: ()[I
 */
JNIEXPORT jintArray JNICALL Java_org_kyrillos_jozw_Ozw_getAllScenes
        (JNIEnv *env, jobject instance){
    uint8* scenes;
    uint8 scenesCount;

    scenesCount = OpenZWave::Manager::Get()->GetAllScenes(&scenes);
    if (scenes != nullptr){
        jint fill[scenesCount];
        jintArray _jarray;
        uint8 n;

        for (int nr = 0; nr < scenesCount; nr++) {
            n = scenes[nr];
            fill[nr] = static_cast<jint>(n);
        }
        _jarray = env->NewIntArray(scenesCount);
        env->SetIntArrayRegion(_jarray, 0, scenesCount, fill);
        return _jarray;
    }
    return nullptr;
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    removeAllScenes
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_removeAllScenes
        (JNIEnv *env, jobject instance, jlong homeId){
    OpenZWave::Manager::Get()->RemoveAllScenes(static_cast<const uint32>(homeId));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    createScene
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_createScene
        (JNIEnv *env, jobject instance){
    return jint (OpenZWave::Manager::Get()->CreateScene());
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    removeScene
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_removeScene
        (JNIEnv *env, jobject instance, jint sceneId){
    return jboolean (OpenZWave::Manager::Get()->RemoveScene(static_cast<uint8>(sceneId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    addSceneValue
 * Signature: (ILmy/project/openzwave/ValueID;Lmy/project/openzwave/Value;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_addSceneValue
        (JNIEnv *env, jobject instance, jint, jobject, jobject){
    //TODO
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    removeSceneValue
 * Signature: (ILmy/project/openzwave/ValueID;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_removeSceneValue
        (JNIEnv *env, jobject instance, jint, jobject){
    //TODO
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    sceneGetValues
 * Signature: (ILjava/util/List;)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_sceneGetValues
        (JNIEnv *env, jobject instance, jint, jobject){
    //TODO
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    sceneGetValue
 * Signature: (ILmy/project/openzwave/Value;)Lmy/project/openzwave/Value;
 */
JNIEXPORT jobject JNICALL Java_org_kyrillos_jozw_Ozw_sceneGetValue
        (JNIEnv *env, jobject instance, jint, jobject){
    //TODO
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    setSceneValue
 * Signature: (ILmy/project/openzwave/ValueID;Lmy/project/openzwave/Value;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_setSceneValue
        (JNIEnv *env, jobject instance, jint, jobject, jobject){
    //TODO
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getSceneLabel
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getSceneLabel
        (JNIEnv *env, jobject instance, jint sceneId){
    std::string label;
    label = OpenZWave::Manager::Get()->GetSceneLabel(static_cast<uint8>(sceneId));
    return env->NewStringUTF(label.data());
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    setSceneLabel
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setSceneLabel
        (JNIEnv *env, jobject instance, jint sceneId, jstring label){
    OpenZWave::Manager::Get()->SetSceneLabel(static_cast<uint8>(sceneId), env->GetStringUTFChars(label, nullptr));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    sceneExists
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_sceneExists
        (JNIEnv *env, jobject instance, jint sceneId){
    return jboolean (OpenZWave::Manager::Get()->SceneExists(static_cast<uint8>(sceneId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    activateScene
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_activateScene
        (JNIEnv *env, jobject instance, jint sceneId){
    return jboolean (OpenZWave::Manager::Get()->ActivateScene(static_cast<uint8>(sceneId)));
}
