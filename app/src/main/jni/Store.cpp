//
// Created by j.guzikowski on 4/4/18.
//

#include <string.h>
#include "Store.h"
#include <android/log.h>
#include <cstdint>
#include <cstdlib>
#include <stdint.h>
#include <unistd.h>
static Store gStore;
static jobject gLock;


static jclass StringClass;
static jclass ColorClass;

static jmethodID MethodOnSuccessInt;
static jmethodID MethodOnSuccessString;
static jmethodID MethodOnSuccessColor;

void releaseEntryValue(JNIEnv *pEnv, StoreEntry *entry);

void abort(){
    __android_log_write(ANDROID_LOG_ERROR, "Abort","szukaj");//Or ANDROID_LOG_INFO, ..

}
StoreWatcher* startWatcher(JavaVM* pJavaVM, Store* pStore, jobject pLock);
void stopWatcher(StoreWatcher* pWatcher);
void* runWatcher(void* pArgs);
void processEntry(StoreEntry* pEntry);


extern "C"
JNIEXPORT jint JNI_OnLoad(JavaVM* pVM, void* reserved){
    JNIEnv *env;

    if(pVM->GetEnv((void**) &env,JNI_VERSION_1_6)!=JNI_OK){
        abort();
    }
    jclass ObjectClass = env -> FindClass("java/lang/Object");
    if(ObjectClass == NULL) abort();
    jmethodID ObjectConstructor = env -> GetMethodID(ObjectClass, "<init>","()V");
    if(ObjectConstructor == NULL) abort();
    jobject lockTmp = env -> NewObject(ObjectClass, ObjectConstructor);
    env->DeleteLocalRef(ObjectClass);
    gLock = env-> NewGlobalRef(lockTmp);
    env->DeleteLocalRef(lockTmp);

    jclass StoreThreadSafeClass = env -> FindClass("com/ndk/myndk/StoreThradeSafe");
    if(StoreThreadSafeClass == NULL) abort();
    jfieldID lockField = env -> GetStaticFieldID(StoreThreadSafeClass, "LOCK", "Ljava/lang/Object;");
    if(lockField == NULL) abort();
    env -> SetStaticObjectField(StoreThreadSafeClass,lockField,gLock);
    env-> DeleteLocalRef(StoreThreadSafeClass);

    jclass StoreClass = env -> FindClass("com/ndk/myndk/Store");
    if(StoreClass == NULL) abort();

    MethodOnSuccessInt = env -> GetMethodID(StoreClass,"onSuccess","(I)V");
    if(MethodOnSuccessInt == NULL) abort();

    MethodOnSuccessString = env -> GetMethodID(StoreClass,"onSuccess","(Ljava/lang/String;)V");
    if(MethodOnSuccessString == NULL) abort();

    MethodOnSuccessColor = env -> GetMethodID(StoreClass,"onSuccess","(Lcom/ndk/myndk/Color;)V");
    if(MethodOnSuccessColor == NULL) abort();

    env->DeleteLocalRef(StoreClass);

    jclass StringClassTmp = env -> FindClass("java/lang/String");
    if(StringClassTmp == NULL) abort();
    StringClass = (jclass) env -> NewGlobalRef(StringClassTmp);
    env -> DeleteLocalRef(StringClassTmp);
    jclass ColorClassTmp = env -> FindClass("com/ndk/myndk/Color");
    if(ColorClassTmp == NULL) abort();
    ColorClass = (jclass) env -> NewGlobalRef(ColorClassTmp);
    env->DeleteLocalRef(ColorClassTmp);
    gStore.mLength=0;
    return JNI_VERSION_1_6;
}

extern "C"
JNIEXPORT jint JNICALL Java_com_ndk_myndk_Store_getCount(JNIEnv* pEnv, jobject pObject){
    return gStore.mLength;
}


void throwNotExistingKeyException(JNIEnv* pEnv){
    jclass clazz = pEnv -> FindClass("com/ndk/myndk/exception/NotExistingKeyException");
    if(clazz != NULL){
        pEnv->ThrowNew(clazz,"Key does not exist");
    }
    pEnv->DeleteLocalRef(clazz);
}

void  throwInvalidTypeException(JNIEnv* pEnv){
    jclass clazz = pEnv -> FindClass("com/ndk/myndk/exception/InvalidTypeException");
    if(clazz != NULL){
        pEnv->ThrowNew(clazz,"Invalid Type Exception");
    }
    pEnv->DeleteLocalRef(clazz);
}

void throwStoreFullException(JNIEnv* pEnv){
    jclass clazz = pEnv -> FindClass("com/ndk/myndk/exception/StoreFullException");
    if(clazz != NULL){
        pEnv->ThrowNew(clazz,"Store Full Exception");
    }
    pEnv->DeleteLocalRef(clazz);
}


//c++ methods
bool isEntryValid(JNIEnv* pEnv, StoreEntry* pEntry, StoreType pType){

    if(pEntry == NULL){
        throwNotExistingKeyException(pEnv);
    }else if(pEntry->mType != pType){
        throwInvalidTypeException(pEnv);
    }

    return !pEnv -> ExceptionCheck();


    //return ((pEntry !=NULL) && (pEntry->mType == pType) );
}

StoreEntry* findEntry(JNIEnv* pEnv, Store* pStore, jstring pKey){

    StoreEntry* entry = pStore -> mEntry;
    StoreEntry* entryEnd = entry + pStore->mLength;

    const char* tmpKey = pEnv -> GetStringUTFChars(pKey,NULL);
//    __android_log_write(ANDROID_LOG_ERROR, "findEntry", "Error here");//Or ANDROID_LOG_INFO, ..
    while ((entry<entryEnd) && (strcmp(entry->mKey,tmpKey)!= 0 )  ){
        ++entry;
    }

    pEnv -> ReleaseStringUTFChars(pKey,tmpKey);

    return (entry == entryEnd) ? NULL : entry;

}


StoreEntry* allocateEntry(JNIEnv* pEnv, Store* pStore, jstring pKey){
    StoreEntry* entry = findEntry(pEnv,pStore,pKey);
    if(entry != NULL){
        releaseEntryValue(pEnv,entry);
    }else{
        if(pStore->mLength >= STORE_MAX_CAPACITY){
            throwStoreFullException(pEnv);
            return NULL;
        }

        entry = pStore ->mEntry + pStore ->mLength;
        const char* tmpKey = pEnv -> GetStringUTFChars(pKey,NULL);
        entry->mKey = new char[strlen(tmpKey)+1];
        strcpy(entry->mKey,tmpKey);
        pEnv -> ReleaseStringUTFChars(pKey, tmpKey);
        ++pStore->mLength;

    }
    return entry;

}

void releaseEntryValue(JNIEnv *pEnv, StoreEntry *pEntry) {
    switch (pEntry->mType){
        case StoreType_String:
            delete pEntry->mValue.mString;
            break;
        case StoreType_Color:
            pEnv->DeleteGlobalRef(pEntry->mValue.mColor);
            break;
        case StoreType_IntegerArray:
            delete[] pEntry -> mValue.mIntegerArray;
            break;
        case StoreType_StringArray:
            for(int32_t i=0; i<pEntry->mLength;++i){
                delete pEntry -> mValue.mStringArray[i];
            }
            delete[] pEntry -> mValue.mStringArray;
            break;
        case StoreType_ColorArray:
            for(int32_t i=0; i<pEntry->mLength;++i){
                pEnv->DeleteGlobalRef(pEntry->mValue.mColorArray[i]);
            }
            delete[] pEntry->mValue.mColorArray;
            break;
    }
}

JNIEXPORT jstring JNICALL Java_com_ndk_myndk_Store_getString(JNIEnv* pEnv, jobject pThis, jstring key){

    StoreEntry* entry = findEntry(pEnv,&gStore,key);
    if(isEntryValid(pEnv,entry,StoreType_String)){
        return pEnv->NewStringUTF(entry->mValue.mString);
    }else{
        return NULL;
    }

}

JNIEXPORT void JNICALL Java_com_ndk_myndk_Store_setString(JNIEnv* pEnv, jobject pThis, jstring pKey,jstring pString){
    StoreEntry* entry = allocateEntry(pEnv, &gStore, pKey);

    if(entry != NULL){
        entry -> mType = StoreType_String;
        jsize stringLength = pEnv -> GetStringUTFLength(pString);
        entry ->mValue.mString = new char[stringLength +1];
        pEnv->GetStringUTFRegion(pString,0,stringLength,entry->mValue.mString);
        entry->mValue.mString[stringLength] = '\0';

        pEnv -> CallVoidMethod(pThis, MethodOnSuccessString, (jstring) pEnv->NewStringUTF(entry->mValue.mString));
    }
}

JNIEXPORT jint JNICALL Java_com_ndk_myndk_Store_getInteger(JNIEnv* pEnv, jobject pThis, jstring pKey){
    StoreEntry* entry = findEntry(pEnv, &gStore, pKey);
    if(isEntryValid(pEnv,entry,StoreType_Integer)){
        return entry -> mValue.mInteger;
    }else{
        return 0;
    }
}

JNIEXPORT void JNICALL Java_com_ndk_myndk_Store_setInteger(JNIEnv* pEnv, jobject pThis, jstring pKey,jint pInteger){
    StoreEntry* entry = allocateEntry(pEnv, &gStore, pKey);
    if(entry != NULL){
        entry->mType = StoreType_Integer;
        entry->mValue.mInteger = pInteger;

        pEnv -> CallVoidMethod(pThis, MethodOnSuccessInt, (jint) entry->mValue.mInteger);
    }
}

JNIEXPORT jboolean JNICALL Java_com_ndk_myndk_Store_getBoolean(JNIEnv* pEnv, jobject pThis, jstring pKey){
    StoreEntry* entry = findEntry(pEnv, &gStore, pKey);
    if(isEntryValid(pEnv,entry,StoreType_Boolean)){
        return entry -> mValue.mBoolean;
    }else{
        return 0;
    }
}

JNIEXPORT void JNICALL Java_com_ndk_myndk_Store_setBoolean(JNIEnv* pEnv, jobject pThis, jstring pKey,jboolean pBoolean){
    StoreEntry* entry = allocateEntry(pEnv, &gStore, pKey);
    if(entry != NULL){
        entry->mType = StoreType_Boolean;
        entry->mValue.mBoolean = pBoolean;
    }
}

JNIEXPORT void JNICALL Java_com_ndk_myndk_Store_setColor(JNIEnv* pEnv, jobject pThis, jstring pKey,jobject pColor){
    StoreEntry* entry = allocateEntry(pEnv, &gStore,pKey);

    if(entry != NULL){
        entry->mType = StoreType_Color;
        entry->mValue.mColor = pEnv->NewGlobalRef(pColor);
        pEnv -> CallVoidMethod(pThis, MethodOnSuccessColor, (jstring) entry->mValue.mColor);
    }
}

JNIEXPORT jobject JNICALL Java_com_ndk_myndk_Store_getColor(JNIEnv *pEnv, jobject instance, jstring pKey){
    StoreEntry* entry = findEntry(pEnv, &gStore, pKey );

    if(isEntryValid(pEnv,entry,StoreType_Color)){
        return entry -> mValue.mColor;
    }else{
        return NULL;
    }
}

JNIEXPORT jintArray JNICALL Java_com_ndk_myndk_Store_getIntegerArray(JNIEnv *pEnv, jobject instance, jstring pKey){
    StoreEntry* entry = findEntry(pEnv, &gStore,pKey);
    if(isEntryValid(pEnv,entry,StoreType_IntegerArray)){
        jintArray javaArray = pEnv -> NewIntArray(entry->mLength);
        pEnv->SetIntArrayRegion(javaArray,0,entry->mLength,entry->mValue.mIntegerArray);
        return javaArray;
    }else{
        return NULL;
    }
}

JNIEXPORT void JNICALL Java_com_ndk_myndk_Store_setIntegerArray(JNIEnv *pEnv, jobject instance, jstring pKey, jintArray pIntegerArray){
    StoreEntry* entry = allocateEntry(pEnv, &gStore,pKey);
    if(entry != NULL){
        jsize length = pEnv -> GetArrayLength(pIntegerArray);
        int32_t* array = new int32_t[length];
        pEnv -> GetIntArrayRegion(pIntegerArray,0,length,array);
        entry -> mType = StoreType_IntegerArray;
        entry -> mLength = length;
        entry -> mValue.mIntegerArray = array;
    }
}

JNIEXPORT jobjectArray JNICALL Java_com_ndk_myndk_Store_getStringArray(JNIEnv *pEnv, jobject instance, jstring pKey){
    StoreEntry* entry = findEntry(pEnv, &gStore,pKey);
    if(isEntryValid(pEnv,entry,StoreType_StringArray)){
        jobjectArray javaArray = pEnv -> NewObjectArray(entry->mLength,StringClass, NULL);
        for(int32_t i =0;i<entry->mLength;++i){
            jstring string = pEnv -> NewStringUTF(entry->mValue.mStringArray[i]);
            pEnv->SetObjectArrayElement(javaArray,i,string);
            pEnv->DeleteLocalRef(string);
        }
        return javaArray;
    }else{
        return NULL;
    }
}

JNIEXPORT void JNICALL Java_com_ndk_myndk_Store_setStringArray(JNIEnv *pEnv, jobject instance, jstring pKey, jobjectArray pStringArray){
    StoreEntry* entry = allocateEntry(pEnv, &gStore,pKey);
    if(entry != NULL){
        jsize length = pEnv -> GetArrayLength(pStringArray);
        char** array = new char*[length];

        for(int32_t i=0;i<length;++i){
            jstring string = (jstring) pEnv -> GetObjectArrayElement(pStringArray,i);
            jsize stringLength = pEnv -> GetStringUTFLength(string);
            array[i] = new char[stringLength +1];
            pEnv -> GetStringUTFRegion(string,0,stringLength,array[i]);
            array[i][stringLength] = '\0';
            pEnv -> DeleteLocalRef(string);
        }
        entry -> mType = StoreType_StringArray;
        entry->mLength = length;
        entry->mValue.mStringArray =array;
    }
}


JNIEXPORT jobjectArray JNICALL Java_com_ndk_myndk_Store_getColorArray(JNIEnv *pEnv, jobject instance, jstring pKey){

    StoreEntry* entry = findEntry(pEnv, &gStore,pKey);
    if(isEntryValid(pEnv,entry,StoreType_ColorArray)){
        jobjectArray javaArray = pEnv -> NewObjectArray(entry->mLength,ColorClass, NULL);
        for(int32_t i =0;i<entry->mLength;++i){
            pEnv->SetObjectArrayElement(javaArray,i,entry->mValue.mColorArray[i]);
        }
        return javaArray;
    }else{
        return NULL;
    }
}

JNIEXPORT void JNICALL Java_com_ndk_myndk_Store_setColorArray(JNIEnv *pEnv, jobject instance, jstring pKey, jobjectArray pColorArray) {
    __android_log_write(ANDROID_LOG_ERROR, "setString %s", "colorarray");//Or ANDROID_LOG_INFO, ..
    StoreEntry *entry = allocateEntry(pEnv, &gStore, pKey);

    if (entry != NULL) {
        jsize length = pEnv->GetArrayLength(pColorArray);
        jobject *array = new jobject[length];

        for (int32_t i = 0; i < length; ++i) {
            jobject localColor = pEnv->GetObjectArrayElement(pColorArray, i);
            array[i] = pEnv->NewGlobalRef(localColor);
            pEnv->DeleteLocalRef(localColor);
        }
        entry->mType = StoreType_ColorArray;
        entry->mLength = length;
        entry->mValue.mColorArray = array;

    }

}

JNIEXPORT jlong JNICALL Java_com_ndk_myndk_Store_startWatcher(JNIEnv *pEnv, jobject pThis){
    JavaVM* javaVM;
    if(pEnv -> GetJavaVM(&javaVM) != JNI_OK) abort();

    StoreWatcher* watcher = startWatcher(javaVM,&gStore,gLock);
    return (jlong) watcher;
}

JNIEXPORT void JNICALL Java_com_ndk_myndk_Store_stopWatcher(JNIEnv *pEnv, jobject pThis, jlong pWatcher){
    stopWatcher((StoreWatcher*) pWatcher);
}

StoreWatcher* startWatcher(JavaVM* pJavaVM, Store* pStore, jobject pLock){
    StoreWatcher* watcher = new StoreWatcher();
    watcher -> mJavaVM = pJavaVM;
    watcher -> mStore =pStore;
    watcher -> mLock = pLock;
    watcher -> mRunning = true;

    pthread_attr_t lAttributes;
    if(pthread_attr_init(&lAttributes)) abort();
    if(pthread_create(&watcher->mThread,&lAttributes,runWatcher,watcher)) abort();
    return watcher;
}
void stopWatcher(StoreWatcher* pWatcher){
    pWatcher->mRunning = false;
}

void* runWatcher(void* pArgs){
    StoreWatcher* watcher = (StoreWatcher*) pArgs;
    Store* store = watcher->mStore;

    JavaVM* javaVM = watcher->mJavaVM;
    JavaVMAttachArgs javaVMAttachArgs;
    javaVMAttachArgs.version = JNI_VERSION_1_6;
    javaVMAttachArgs.name="NativeThread";
    javaVMAttachArgs.group = NULL;
    JNIEnv* env;

    if(javaVM  -> AttachCurrentThreadAsDaemon(&env, &javaVMAttachArgs) != JNI_OK) abort();
    while (true){
        sleep(5);
        env -> MonitorEnter(watcher->mLock);
        if(!watcher->mRunning) break;
        StoreEntry* entry = watcher->mStore->mEntry;
        StoreEntry* entryEnd = entry+ watcher->mStore->mLength;
        while (entry < entryEnd){
            processEntry(entry);
            ++entry;
        }
        env->MonitorExit(watcher->mLock);
    }

    javaVM->DetachCurrentThread();
    delete watcher;
    pthread_exit(NULL);

}



void processEntry(StoreEntry* pEntry){
    switch (pEntry->mType){
        case StoreType_Integer:
            if(pEntry->mValue.mInteger >100000){
                pEntry->mValue.mInteger = 100000;
            }else if(pEntry->mValue.mInteger < -100000){
                pEntry->mValue.mInteger = -100000;
            }
            break;
    }
}










