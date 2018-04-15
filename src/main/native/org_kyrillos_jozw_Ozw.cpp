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
    jclass clazz = env->FindClass("java.lang.IllegalStateException");
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
    jmethodID nativePtr;
};

ValueIdWrapper *valueIdWrapper;

ValueIdWrapper::ValueIdWrapper(JNIEnv *env) {
    clazz = reinterpret_cast<jclass> (env->NewGlobalRef(env->FindClass("org/kyrillos/jozw/ValueId")));
    constructor = env->GetMethodID(clazz, "<init>", "(JSISSSI)V");
    nativePtr = env->GetFieldID(env, clazz, "nativePtr", "J");
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

OpenZWave::ValueID* toValueID(JNIEnv *env, jobject object){
    jlong ptr = env->GetLongField(env, object, nativePtr);
    return reinterpret_cast<OpenZWave::ValueID*>(ptr)
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

// public Notification(int type, long homeId, short nodeId, ValueId valueId, short groupIdx, short event, short buttonId, short sceneId, short notification, short bytes){

NotificationWrapper::NotificationWrapper(JNIEnv *env) {
    std::cout<<"Notification wapper env: "<<  (*env).GetVersion() << std::endl;
    jclass c = (*env).FindClass("org/kyrillos/jozw/Notification");
    std::cout<<"Notification class: " << c << std::endl;
    clazz = reinterpret_cast<jclass> (env->NewGlobalRef(c));
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

OzwMapper *ozwMapper;

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
    jint res = cachedJVM->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION);
    if(res == JNI_EDETACHED){
        if (cachedJVM->AttachCurrentThread(reinterpret_cast<void**>(&env), nullptr) != 0){
            std::cout << "Failed to attacher Thread" << std::endl;
            return;
        }
    }

    jobject notification = notificationWrapper->toJava(env, _pNotification);
    env->CallVoidMethod(ozwInst, ozwNotifMethod, notification);
    jthrowable ex = env->ExceptionOccurred();
    if (ex){
        env->Throw(ex);
    }
}

void handleNotification(OpenZWave::Notification const *_pNotification, void *_context){
    std::cout << "notification " << _pNotification->GetAsString()<< std::endl;
    auto *mapper = static_cast<OzwMapper *>(_context);
    if (mapper == nullptr){
        std::cout << "Invalid watcher parameter" << std::endl;
        return;
    }
    mapper->onNotification(_pNotification);
}

/**************************************************************
 * JNI methods
 **************************************************************/

JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getVersionAsString
    (JNIEnv *env, jclass inst){
    std::string str = OpenZWave::Manager::getVersionAsString();
    return env->NewStringUTF(str.c_str());
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    doCreate
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT jlong JNICALL Java_org_kyrillos_jozw_Ozw_doCreate
    (JNIEnv *env, jobject instance, jstring configPath, jstring userPath, jstring arguments) {

    std::string *_configPath = new std::string(env->GetStringUTFChars(configPath, nullptr));
    std::string *_userPath = new std::string(env->GetStringUTFChars(userPath, nullptr));
    std::string *_arguments = new std::string(env->GetStringUTFChars(arguments, nullptr));

    OpenZWave::Options* options = OpenZWave::Options::Create(*_configPath, *_userPath, *_arguments);
    options->Lock();

    OpenZWave::Manager* manager = OpenZWave::Manager::Create();
    ozwMapper = new OzwMapper(env, instance);
    manager->AddWatcher(handleNotification, ozwMapper);

    valueIdWrapper = new ValueIdWrapper(env);

    return (jlong) manager;
}

JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_addDriver
    (JNIEnv *env, jobject instance, jstring path, jint interfaceType){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    bool res;
    const char *s = env->GetStringUTFChars(path, nullptr);
    switch (interfaceType){
        case 1:
            res = manager->AddDriver(s, OpenZWave::Driver::ControllerInterface_Serial);
            break;
        case 2:
            res = manager->AddDriver(s, OpenZWave::Driver::ControllerInterface_Hid);
            break;
        default:
            res = manager->AddDriver(s, OpenZWave::Driver::ControllerInterface_Unknown);
            break;
    }
    env->ReleaseStringUTFChars(path, s);
    return static_cast<jboolean>(res);
}


/*
 * Class:     my_project_openzwave_Manager
 * Method:    destroy
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_destroy
        (JNIEnv *env, jclass clazz){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    manager->Destroy();
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    writeConfig
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_writeConfig
        (JNIEnv *env, jobject instance, jint homeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    manager->WriteConfig(static_cast<const uint32>(homeId));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    removeDriver
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_removeDriver
        (JNIEnv *env, jobject instance, jstring driverPath){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    const char *_driverPath = env->GetStringUTFChars(driverPath, nullptr);
    return jboolean (manager->RemoveDriver(_driverPath));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getControllerNodeId
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getControllerNodeId
        (JNIEnv *env, jobject instance, jint homeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jint (manager->GetControllerNodeId(static_cast<const uint32>(homeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getSUCNodeId
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getSUCNodeId
        (JNIEnv *env, jobject instance,  jint homeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jint (manager->GetSUCNodeId(static_cast<const uint32>(homeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    isPrimaryController
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isPrimaryController
        (JNIEnv *env, jobject instance,  jint homeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->IsPrimaryController(static_cast<const uint32>(homeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    isStaticUpdateController
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isStaticUpdateController
        (JNIEnv *env, jobject instance,  jint homeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->IsStaticUpdateController(static_cast<const uint32>(homeId)));
}
/*
 * Class:     my_project_openzwave_Manager
 * Method:    isBridgeController
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isBridgeController
        (JNIEnv *env, jobject instance,  jint homeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->IsBridgeController(static_cast<const uint32>(homeId)));
}
/*
 * Class:     my_project_openzwave_Manager
 * Method:    getLibraryVersion
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getLibraryVersion
        (JNIEnv *env, jobject instance,  jint homeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    const std::string &_version = manager->GetLibraryVersion(static_cast<const uint32>(homeId));
    return env->NewStringUTF(_version.data());
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getLibraryTypeName
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getLibraryTypeName
        (JNIEnv *env, jobject instance,  jint homeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    const std::string &_type = manager->GetLibraryTypeName(static_cast<const uint32>(homeId));
    return env->NewStringUTF(_type.data());
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getSendQueueCount
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL Java_org_kyrillos_jozw_Ozw_getSendQueueCount
        (JNIEnv *env, jobject instance,  jint homeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    int32 _count = manager->GetSendQueueCount(static_cast<const uint32>(homeId));
    return jlong (_count);
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    logDriverStatistics
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_logDriverStatistics
        (JNIEnv *env, jobject instance,  jint homeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    manager->LogDriverStatistics(static_cast<const uint32>(homeId));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getControllerInterfaceType
 * Signature: (I)Lmy/project/openzwave/ControllerInterface;
 */
JNIEXPORT jobject JNICALL Java_org_kyrillos_jozw_Ozw_getControllerInterfaceType
        (JNIEnv *env, jobject instance, jint homeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    OpenZWave::Driver::ControllerInterface type = manager->GetControllerInterfaceType(static_cast<const uint32>(homeId));

}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getControllerPath
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getControllerPath
        (JNIEnv *env, jobject instance,  jint homeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    const std::string &_path = manager->GetControllerPath(static_cast<const uint32>(homeId));
    if(_path != nullptr){
        return env->NewStringUTF(_path.data());
    }
    return nullptr;
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getPollInterval
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_org_kyrillos_jozw_Ozw_getPollInterval
        (JNIEnv *env, jobject instance){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jlong (manager->GetPollInterval());
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    setPollInterval
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setPollInterval
        (JNIEnv *env, jobject instance, jlong interval){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    manager->SetPollInterval((int32) interval, true);
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    enablePoll
 * Signature: (Lmy/project/openzwave/ValueID;I)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_enablePoll
        (JNIEnv *env, jobject instance, jobject valueId, jint itensity){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->EnablePoll(valueIdWrapper->toValueID(env, valueId), static_cast<uint8>(itensity)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    disablePoll
 * Signature: (Lmy/project/openzwave/ValueID;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_disablePoll
        (JNIEnv *env, jobject instance, jobject valueId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->DisablePoll(valueIdWrapper->toValueID(env, valueId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    isPolled
 * Signature: (Lmy/project/openzwave/ValueID;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isPolled
        (JNIEnv *env, jobject instance, jobject valueId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->IsValuePolled(valueIdWrapper->toValueID(env, valueId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    setPollIntensity
 * Signature: (Lmy/project/openzwave/ValueID;I)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setPollIntensity
        (JNIEnv *env, jobject instance, jobject valueId, jint itensity){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    manager->SetPollIntensity(valueIdWrapper->toValueID(env, valueId), static_cast<uint8>(itensity));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getPollIntensity
 * Signature: (Lmy/project/openzwave/ValueID;)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getPollIntensity
        (JNIEnv *env, jobject instance, jobject valueId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jint (manager->GetPollIntensity(valueIdWrapper->toValueID(env, valueId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    refreshNodeInfo
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_refreshNodeInfo
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->RefreshNodeInfo(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    requestNodeState
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_requestNodeState
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->RequestNodeState(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    requestNodeDynamic
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_requestNodeDynamic
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    jboolean (manager->RequestNodeDynamic(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    isNodeListeningDevice
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isNodeListeningDevice
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->IsNodeListeningDevice(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    isNodeFrequentListeningDevice
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isNodeFrequentListeningDevice
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->IsNodeFrequentListeningDevice(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    isNodeBeamingDevice
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isNodeBeamingDevice
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->IsNodeBeamingDevice(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    isNodeRoutingDevice
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isNodeRoutingDevice
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->IsNodeRoutingDevice(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    isNodeSecurityDevice
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isNodeSecurityDevice
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->IsNodeSecurityDevice(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeMaxBaudRate
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getNodeMaxBaudRate
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jint (manager->GetNodeMaxBaudRate(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeVersion
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getNodeVersion
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->GetNodeVersion(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeSecurity
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getNodeSecurity
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jint (manager->GetNodeSecurity(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    isNodeZWavePlus
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isNodeZWavePlus
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->IsNodeZWavePlus(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeBasic
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getNodeBasic
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jint (manager->GetNodeBasic(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeGeneric
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getNodeGeneric
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jint (manager->GetNodeGeneric(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeSpecific
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getNodeSpecific
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jint (manager->GetNodeSpecific(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeType
 * Signature: (II)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getNodeType
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    const std::string &_type = manager->GetNodeType(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId));
    if (_type != nullptr){
        return env->NewStringUTF(_type.data());
    }
    return nullptr;
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeNeighbors
 * Signature: (II)[I
 */
JNIEXPORT jintArray JNICALL Java_org_kyrillos_jozw_Ozw_getNodeNeighbors
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    uint8 **neighbors = nullptr;
    int numNeighbors = manager->GetNodeNeighbors(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId), neighbors);
    if(neighbors == nullptr){
        return nullptr;
    }

    jint fill[numNeighbors];
    for (int nr = 0; nr < numNeighbors; nr++) {
        uint8 val = reinterpret_cast<uint8>(neighbors[nr]);
        fill[nr] = val;

    }
    jintArray _jarray = env->NewIntArray(numNeighbors);
    env->SetIntArrayRegion(_jarray, 0, numNeighbors, fill);
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeManufacturerName
 * Signature: (II)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getNodeManufacturerName
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    const std::string &_manufacturer = manager->GetNodeManufacturerName(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId));
    if (_manufacturer != nullptr){
        return env->NewStringUTF(_manufacturer.data());
    }
    return nullptr;
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeProductName
 * Signature: (II)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getNodeProductName
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    const std::string &_productName = manager->GetNodeProductName(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId));
    if (_productName != nullptr){
        return env->NewStringUTF(_productName.data());
    }
    return nullptr;
}
/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeName
 * Signature: (II)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getNodeName
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    const std::string &_nodeName = manager->GetNodeName(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId));
    if (_nodeName != nullptr){
        return env->NewStringUTF(_nodeName.data());
    }
    return nullptr;
}
/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeLocation
 * Signature: (II)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getNodeLocation
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    const std::string &_nodeLocation = manager->GetNodeLocation(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId));
    if (_nodeLocation != nullptr){
        return env->NewStringUTF(_nodeLocation.data());
    }
    return nullptr;
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeManufacturerId
 * Signature: (II)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getNodeManufacturerId
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    const std::string &_nodeManufacturer = manager->GetNodeManufacturerId(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId));
    if (_nodeManufacturer != nullptr){
        return env->NewStringUTF(_nodeManufacturer.data());
    }
    return nullptr;
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeProductType
 * Signature: (II)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getNodeProductType
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    const std::string &_nodeProductType = manager->GetNodeProductType(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId));
    if (_nodeProductType != nullptr){
        return env->NewStringUTF(_nodeProductType.data());
    }
    return nullptr;
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeProductId
 * Signature: (II)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getNodeProductId
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    const std::string &_nodeProductId = manager->GetNodeProductId(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId));
    if (_nodeProductId != nullptr){
        return env->NewStringUTF(_nodeProductId.data());
    }
    return nullptr;
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    setNodeManufacturerName
 * Signature: (IILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setNodeManufacturerName
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId, jstring name){
    if(name != nullptr){
        OpenZWave::Manager *manager = OpenZWave::Manager::Get();
        const char *_name = env->GetStringUTFChars(name, nullptr);
        manager->SetNodeManufacturerName(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId), _name);
    }
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    setNodeProductName
 * Signature: (IILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setNodeProductName
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId, jstring productName){
    if(productName != nullptr){
        OpenZWave::Manager *manager = OpenZWave::Manager::Get();
        const char *_productName = env->GetStringUTFChars(productName, nullptr);
        manager->SetNodeManufacturerName(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId), _productName);
    }
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    setNodeName
 * Signature: (IILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setNodeName
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId, jstring nodeName){
    if(nodeName != nullptr){
        OpenZWave::Manager *manager = OpenZWave::Manager::Get();
        const char *_nodeName = env->GetStringUTFChars(nodeName, nullptr);
        manager->SetNodeManufacturerName(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId), _nodeName);
    }
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    setNodeLocation
 * Signature: (IILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setNodeLocation
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId, jstring location){
    if(location != nullptr){
        OpenZWave::Manager *manager = OpenZWave::Manager::Get();
        const char *_nodeLocation = env->GetStringUTFChars(location, nullptr);
        manager->SetNodeManufacturerName(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId), _nodeLocation);
    }
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    setNodeOn
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setNodeOn
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    manager->SetNodeOn(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    setNodeOff
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setNodeOff
        (JNIEnv *env, jobject instance,jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    manager->SetNodeOff(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId));
}
/*
 * Class:     my_project_openzwave_Manager
 * Method:    setNodeLevel
 * Signature: (III)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setNodeLevel
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId, jint level){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    manager->SetNodeLevel(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId), static_cast<uint8>(level));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    isNodeInfoReceived
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isNodeInfoReceived
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->IsNodeInfoReceived(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeClassInformation
 * Signature: (IIILjava/lang/String;I)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_getNodeClassInformation
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId, jint commandClassId, jstring className, jint classVersion){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    std::string _className = nullptr;
    if (className != nullptr) {
        std::string _temp = env->GetStringUTFChars(className, nullptr);
    }
    uint8 _version = static_cast<uint8>(classVersion);
    return jboolean (manager->GetNodeClassInformation(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId), static_cast<uint8>(commandClassId), &_className, &_version));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    isNodeAwake
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isNodeAwake
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->IsNodeAwake(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    isNodeFailed
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isNodeFailed
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->IsNodeFailed(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}
/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeQueryStage
 * Signature: (II)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getNodeQueryStage
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId) {
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    const std::string &_queryStage = manager->GetNodeQueryStage(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId));
    if(_queryStage != nullptr){
        return env->NewStringUTF(_queryStage.data());
    }
    return nullptr;
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeDeviceType
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getNodeDeviceType
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId) {
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jint(manager->GetNodeDeviceType(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}
/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeDeviceTypeString
 * Signature: (II)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getNodeDeviceTypeString
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId) {
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    const std::string &_typeString = manager->GetNodeDeviceTypeString(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId));
    if(_typeString != nullptr){
        return env->NewStringUTF(_typeString.data());
    }
    return nullptr;
}
/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeRole
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getNodeRole
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId) {
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jint(manager->GetNodeRole(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}
/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodeRoleString
 * Signature: (II)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getNodeRoleString
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId) {
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    const std::string &_role = manager->GetNodeRoleString(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId));
    if(_role != nullptr){
        return env->NewStringUTF(_role.data());
    }
    return nullptr;
}
/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodePlusType
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getNodePlusType
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId) {
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jint(manager->GetNodePlusType(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}
/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNodePlusTypeString
 * Signature: (II)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getNodePlusTypeString
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId) {
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    const std::string &_plusType = manager->GetNodePlusTypeString(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId));
    if(_plusType != nullptr){
        return env->NewStringUTF(_plusType.data());
    }
    return nullptr;
}
/*
 * Class:     my_project_openzwave_Manager
 * Method:    getValueLabel
 * Signature: (Lmy/project/openzwave/ValueID;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getValueLabel
        (JNIEnv *env, jobject instance, jobject valueId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    const std::string &_label = manager->GetValueLabel(valueIdWrapper->toValueID(env, valueId));
    if (_label != nullptr){
        return env->NewStringUTF(_label.data());
    }
    return nullptr;
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    setValueLabel
 * Signature: (Lmy/project/openzwave/ValueID;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setValueLabel
        (JNIEnv *env, jobject instance, jobject valueId, jstring label){
    if(label != nullptr){
        OpenZWave::Manager *manager = OpenZWave::Manager::Get();
        const char *_label = env->GetStringUTFChars(label, nullptr);
        manager->SetValueLabel(valueIdWrapper->toValueID(env, valueId), _label);
    }
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getValueUnits
 * Signature: (Lmy/project/openzwave/ValueID;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getValueUnits
        (JNIEnv *env, jobject instance, jobject valueId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    const std::string &_valueUnit = manager->GetValueLabel(valueIdWrapper->toValueID(env, valueId));
    if (_valueUnit != nullptr){
        return env->NewStringUTF(_valueUnit.data());
    }
    return nullptr;
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    setValueUnits
 * Signature: (Lmy/project/openzwave/ValueID;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setValueUnits
        (JNIEnv *env, jobject instance, jobject valueId, jstring unit){
    if(unit != nullptr){
        OpenZWave::Manager *manager = OpenZWave::Manager::Get();
        const char *_unit = env->GetStringUTFChars(unit, nullptr);
        manager->SetValueLabel(valueIdWrapper->toValueID(env, valueId), _unit);
    }
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getValueHelp
 * Signature: (Lmy/project/openzwave/ValueID;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getValueHelp
        (JNIEnv *env, jobject instance, jobject valueId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    const std::string &_valueHelp = manager->GetValueHelp(valueIdWrapper->toValueID(env, valueId));
    if (_valueHelp != nullptr){
        return env->NewStringUTF(_valueHelp.data());
    }
    return nullptr;
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    setValueHelp
 * Signature: (Lmy/project/openzwave/ValueID;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setValueHelp
        (JNIEnv *env, jobject instance, jobject valueId, jstring help){
    if(help != nullptr){
        OpenZWave::Manager *manager = OpenZWave::Manager::Get();
        const char *_help = env->GetStringUTFChars(help, nullptr);
        manager->SetValueHelp(valueIdWrapper->toValueID(env, valueId), _help);
    }
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getValueMin
 * Signature: (Lmy/project/openzwave/ValueID;)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getValueMin
        (JNIEnv *env, jobject instance, jobject valueId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jint (manager->GetValueMin(valueIdWrapper->toValueID(env, valueId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getValueMax
 * Signature: (Lmy/project/openzwave/ValueID;)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getValueMax
        (JNIEnv *env, jobject instance, jobject valueId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jint (manager->GetValueMax(valueIdWrapper->toValueID(env, valueId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    isValueReadOnly
 * Signature: (Lmy/project/openzwave/ValueID;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isValueReadOnly
        (JNIEnv *env, jobject instance, jobject valueId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->IsValueReadOnly(valueIdWrapper->toValueID(env, valueId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    isValueWriteOnly
 * Signature: (Lmy/project/openzwave/ValueID;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isValueWriteOnly
        (JNIEnv *env, jobject instance, jobject valueId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->IsValueWriteOnly(valueIdWrapper->toValueID(env, valueId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    isValueSet
 * Signature: (Lmy/project/openzwave/ValueID;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isValueSet
        (JNIEnv *env, jobject instance, jobject valueId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->IsValueSet(valueIdWrapper->toValueID(env, valueId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    isValuePolled
 * Signature: (Lmy/project/openzwave/ValueID;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_isValuePolled
        (JNIEnv *env, jobject instance, jobject valueId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->IsValuePolled(valueIdWrapper->toValueID(env, valueId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getValueAsBool
 * Signature: (Lmy/project/openzwave/ValueID;)Lmy/project/openzwave/Value;
 */
JNIEXPORT jobject JNICALL Java_org_kyrillos_jozw_Ozw_getValueAsBool
        (JNIEnv *env, jobject instance, jobject valueId);

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getValueListItemsAsString
 * Signature: (Lmy/project/openzwave/ValueID;)Ljava/util/List;
 */
JNIEXPORT jobject JNICALL Java_org_kyrillos_jozw_Ozw_getValueListItemsAsString
        (JNIEnv *env, jobject instance, jobject valueId);

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getValueListValuesAsInt
 * Signature: (Lmy/project/openzwave/ValueID;)Ljava/util/List;
 */
JNIEXPORT jobject JNICALL Java_org_kyrillos_jozw_Ozw_getValueListValuesAsInt
        (JNIEnv *env, jobject instance, jobject valueId);

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getValueFloatPrecision
 * Signature: (Lmy/project/openzwave/ValueID;)Ljava/lang/Float;
 */
JNIEXPORT jobject JNICALL Java_org_kyrillos_jozw_Ozw_getValueFloatPrecision
        (JNIEnv *env, jobject instance, jobject valueId);

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
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->RefreshValue(valueIdWrapper->toValueID(env, valueId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    setChangeVerified
 * Signature: (Lmy/project/openzwave/ValueID;Z)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setChangeVerified
        (JNIEnv *env, jobject instance, jobject valueId, jboolean verify){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    manager->SetChangeVerified(valueIdWrapper->toValueID(env, valueId), verify);
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getChangeVerified
 * Signature: (Lmy/project/openzwave/ValueID;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_getChangeVerified
        (JNIEnv *env, jobject instance, jobject valueId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->GetChangeVerified(valueIdWrapper->toValueID(env, valueId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    pressButton
 * Signature: (Lmy/project/openzwave/ValueID;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_pressButton
        (JNIEnv *env, jobject instance, jobject valueId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->PressButton(valueIdWrapper->toValueID(env, valueId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    releaseButton
 * Signature: (Lmy/project/openzwave/ValueID;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_releaseButton
        (JNIEnv *env, jobject instance, jobject valueId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->ReleaseButton(valueIdWrapper->toValueID(env, valueId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNumSwitchPoints
 * Signature: (Lmy/project/openzwave/ValueID;)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getNumSwitchPoints
        (JNIEnv *env, jobject instance, jobject valueId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jint (manager->GetNumSwitchPoints(valueIdWrapper->toValueID(env, valueId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    setSwitchPoint
 * Signature: (Lmy/project/openzwave/ValueID;III)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_setSwitchPoint
        (JNIEnv *env, jobject instance, jobject valueId, jint hours, jint minutes, jint setback){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->SetSwitchPoint(valueIdWrapper->toValueID(env, valueId), static_cast<uint8>(hours), static_cast<uint8>(minutes), static_cast<uint8>(setback)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    RemoveSwitchPoint
 * Signature: (Lmy/project/openzwave/ValueID;II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_RemoveSwitchPoint
        (JNIEnv *env, jobject instance, jobject valueId, jint hours, jint minutes){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->RemoveSwitchPoint(valueIdWrapper->toValueID(env, valueId), static_cast<uint8>(hours), static_cast<uint8>(minutes)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    ClearSwitchPoints
 * Signature: (Lmy/project/openzwave/ValueID;)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_ClearSwitchPoints
        (JNIEnv *env, jobject instance, jobject valueId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    manager->ClearSwitchPoints(valueIdWrapper->toValueID(env, valueId));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getSwitchPoint
 * Signature: (Lmy/project/openzwave/ValueID;IIII)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_getSwitchPoint
        (JNIEnv *env, jobject instance, jobject valueId, jint idx, jint hours, jint minutes, jint setback){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    uint8 _idx = static_cast<uint8>(idx);
    uint8 _hours = static_cast<uint8>(hours);
    uint8 _minutes = static_cast<uint8>(minutes);
    uint8 _setBack = static_cast<uint8>(setback);
    manager->GetSwitchPoint(valueIdWrapper->toValueID(env, valueId), _idx, &_hours, &_minutes, &_setBack);
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    SwitchAllOn
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_SwitchAllOn
        (JNIEnv *env, jobject instance, jint homeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    manager->SwitchAllOn(static_cast<const uint32>(homeId));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    SwitchAllOff
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_SwitchAllOff
        (JNIEnv *env, jobject instance, jint homeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    manager->SwitchAllOff(static_cast<const uint32>(homeId));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    setConfigParam
 * Signature: (IIIII)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_setConfigParam
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId, jint paramId, jint paramValue, jint size){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    jboolean (manager->SetConfigParam(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId), static_cast<uint8>(paramId), toUint32(env, paramValue), static_cast<uint8>(size)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    requestConfigParam
 * Signature: (III)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_requestConfigParam
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId, jint paramId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    manager->RequestConfigParam(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId), static_cast<uint8>(paramId));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    requestAllConfigParams
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_requestAllConfigParams
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    manager->RequestAllConfigParams(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNumGroups
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getNumGroups
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jint (manager->GetNumGroups(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getAssociations
 * Signature: (III)[Lmy/project/openzwave/InstanceAssociation;
 */
JNIEXPORT jobjectArray JNICALL Java_org_kyrillos_jozw_Ozw_getAssociations
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId, jint groupId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    OpenZWave::InstanceAssociation **associations = nullptr;
    int associatioCount = manager->GetAssociations(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId), static_cast<uint8>(groupId), associations);
    if (associations != nullptr){
        jclass classId = env->FindClass("");
        for (int i = 0; i < associatioCount; ++i) {
            OpenZWave::InstanceAssociation *asso = associations[i];
            jobject temp[associatioCount];
            env->NewObject()
        }
    }
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getMaxAssociations
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getMaxAssociations
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId, jint groupId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jint (manager->GetMaxAssociations(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId), static_cast<uint8>(groupId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getGroupLabel
 * Signature: (III)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getGroupLabel
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId, jint groupId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    const std::string &_label = manager->GetGroupLabel(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId), static_cast<uint8>(groupId));
    if (_label != nullptr){
        return env->NewStringUTF(_label.data());
    }
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    addAssociation
 * Signature: (IIIII)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_addAssociation
        (JNIEnv *env, jobject instance, jint, jint, jint, jint, jint);

/*
 * Class:     my_project_openzwave_Manager
 * Method:    removeAssociation
 * Signature: (IIIII)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_removeAssociation
        (JNIEnv *env, jobject instance, jint, jint, jint, jint, jint);

/*
 * Class:     my_project_openzwave_Manager
 * Method:    resetController
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_resetController
        (JNIEnv *env, jobject instance, jint homeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    manager->ResetController(static_cast<const uint32>(homeId));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    softReset
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_softReset
        (JNIEnv *env, jobject instance, jint homeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    manager->SoftReset(static_cast<const uint32>(homeId));
}
/*
 * Class:     my_project_openzwave_Manager
 * Method:    cancelControllerCommand
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_cancelControllerCommand
        (JNIEnv *env, jobject instance, jint homeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->CancelControllerCommand(static_cast<const uint32>(homeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    testNetworkNode
 * Signature: (III)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_testNetworkNode
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId, jint count){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    manager->TestNetworkNode(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId), toUint32(env, count));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    testNetwork
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_testNetwork
        (JNIEnv *env, jobject instance, jint homeId, jint count){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    manager->TestNetwork(static_cast<const uint32>(homeId), toUint32(env, count));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    healNetworkNode
 * Signature: (IIZ)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_healNetworkNode
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId, jboolean doRR){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    manager->HealNetworkNode(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId), doRR);
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    healNetwork
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_healNetwork
        (JNIEnv *env, jobject instance, jint homeId, jboolean doRR){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    manager->HealNetwork(static_cast<const uint32>(homeId), doRR);
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    addNode
 * Signature: (IZ)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_addNode
        (JNIEnv *env, jobject instance, jint homeId, jboolean doSecurity){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->AddNode(static_cast<const uint32>(homeId), doSecurity));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    removeNode
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_removeNode
        (JNIEnv *env, jobject instance, jint homeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->RemoveNode(static_cast<const uint32>(homeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    removeFailedNode
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_removeFailedNode
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->RemoveFailedNode(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}
/*
 * Class:     my_project_openzwave_Manager
 * Method:    hasNodeFailed
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_hasNodeFailed
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->HasNodeFailed(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    requestNodeNeighborUpdate
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_requestNodeNeighborUpdate
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->RequestNodeNeighborUpdate(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    assignReturnRoute
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_assignReturnRoute
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->AssignReturnRoute(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    deleteAllReturnRoutes
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_deleteAllReturnRoutes
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->DeleteAllReturnRoutes(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    sendNodeInformation
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_sendNodeInformation
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->SendNodeInformation(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    createNewPrimary
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_createNewPrimary
        (JNIEnv *env, jobject instance, jint homeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->CreateNewPrimary(static_cast<const uint32>(homeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    receiveConfiguration
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_receiveConfiguration
        (JNIEnv *env, jobject instance, jint homeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->ReceiveConfiguration(static_cast<const uint32>(homeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    replaceFailedNode
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_replaceFailedNode
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->ReplaceFailedNode(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    transferPrimaryRole
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_transferPrimaryRole
        (JNIEnv *env, jobject instance, jint homeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->TransferPrimaryRole(static_cast<const uint32>(homeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    requestNetworkUpdate
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_requestNetworkUpdate
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->RequestNetworkUpdate(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    replicationSend
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_replicationSend
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->ReplicationSend(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    createButton
 * Signature: (III)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_createButton
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId, jint buttonId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->CreateButton(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId), static_cast<uint8>(buttonId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    deleteButton
 * Signature: (III)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_deleteButton
        (JNIEnv *env, jobject instance, jint homeId, jint nodeId, jint buttonId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->DeleteButton(static_cast<const uint32>(homeId), static_cast<uint8>(nodeId), static_cast<uint8>(buttonId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getNumScenes
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_getNumScenes
        (JNIEnv *, jobject){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jint (manager->GetNumScenes());
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getAllScenes
 * Signature: ()[I
 */
JNIEXPORT jintArray JNICALL Java_org_kyrillos_jozw_Ozw_getAllScenes
        (JNIEnv *, jobject);

/*
 * Class:     my_project_openzwave_Manager
 * Method:    removeAllScenes
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_removeAllScenes
        (JNIEnv *env, jobject instance, jint homeId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    manager->RemoveAllScenes(static_cast<const uint32>(homeId));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    createScene
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_createScene
        (JNIEnv *env, jobject instance){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jint (manager->CreateScene());
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    removeScene
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_removeScene
        (JNIEnv *env, jobject instance, jint sceneId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->RemoveScene(static_cast<uint8>(sceneId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    addSceneValue
 * Signature: (ILmy/project/openzwave/ValueID;Lmy/project/openzwave/Value;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_addSceneValue
        (JNIEnv *env, jobject instance, jint, jobject, jobject);

/*
 * Class:     my_project_openzwave_Manager
 * Method:    removeSceneValue
 * Signature: (ILmy/project/openzwave/ValueID;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_removeSceneValue
        (JNIEnv *env, jobject instance, jint, jobject);

/*
 * Class:     my_project_openzwave_Manager
 * Method:    sceneGetValues
 * Signature: (ILjava/util/List;)I
 */
JNIEXPORT jint JNICALL Java_org_kyrillos_jozw_Ozw_sceneGetValues
        (JNIEnv *env, jobject instance, jint, jobject);

/*
 * Class:     my_project_openzwave_Manager
 * Method:    sceneGetValue
 * Signature: (ILmy/project/openzwave/Value;)Lmy/project/openzwave/Value;
 */
JNIEXPORT jobject JNICALL Java_org_kyrillos_jozw_Ozw_sceneGetValue
        (JNIEnv *env, jobject instance, jint, jobject);

/*
 * Class:     my_project_openzwave_Manager
 * Method:    setSceneValue
 * Signature: (ILmy/project/openzwave/ValueID;Lmy/project/openzwave/Value;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_setSceneValue
        (JNIEnv *env, jobject instance, jint, jobject, jobject);

/*
 * Class:     my_project_openzwave_Manager
 * Method:    getSceneLabel
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_kyrillos_jozw_Ozw_getSceneLabel
        (JNIEnv *env, jobject instance, jint sceneId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    const std::string &label = manager->GetSceneLabel(static_cast<uint8>(sceneId));
    return env->NewStringUTF(label.data());
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    setSceneLabel
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_kyrillos_jozw_Ozw_setSceneLabel
        (JNIEnv *env, jobject instance, jint sceneId, jstring label){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    manager->SetSceneLabel(static_cast<uint8>(sceneId), env->GetStringUTFChars(label, nullptr));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    sceneExists
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_sceneExists
        (JNIEnv *env, jobject instance, jint sceneId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->SceneExists(static_cast<uint8>(sceneId)));
}

/*
 * Class:     my_project_openzwave_Manager
 * Method:    activateScene
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_org_kyrillos_jozw_Ozw_activateScene
        (JNIEnv *env, jobject instance, jint sceneId){
    OpenZWave::Manager *manager = OpenZWave::Manager::Get();
    return jboolean (manager->ActivateScene(static_cast<uint8>(sceneId)));
}
