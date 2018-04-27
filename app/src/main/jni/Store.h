//
// Created by j.guzikowski on 4/4/18.
//

#ifndef MYNDK_STORE_H
#define MYNDK_STORE_H

#include <cstdint>
#include <pthread.h>
#include <jni.h>
#define STORE_MAX_CAPACITY 16

typedef enum {
    StoreType_String,
    StoreType_Integer,
    StoreType_Boolean,
    StoreType_Color,
    StoreType_IntegerArray,
    StoreType_StringArray,
    StoreType_ColorArray
} StoreType;

typedef union{
    int32_t* mIntegerArray;
    char** mStringArray;
    jobject* mColorArray;
    uint8_t mBoolean;
    int32_t mInteger;
    char* mString;
    jobject mColor;
} StoreValue;

typedef struct {
    char* mKey;
    StoreType mType;
    StoreValue mValue;
    int32_t mLength;
} StoreEntry;

typedef struct {
    StoreEntry mEntry[STORE_MAX_CAPACITY];
    int32_t mLength;
}Store;

typedef struct {
    Store* mStore;
    JavaVM* mJavaVM;
    jobject mLock;
    pthread_t mThread;
    int32_t mRunning;
}StoreWatcher;



bool isEntryValid(JNIEnv* pEnv, StoreEntry* pEntry, StoreType pType);
StoreEntry* allocateEntry(JNIEnv* pEnv, Store* pStore, jstring pKey);
StoreEntry* findEntry(JNIEnv* pEnv, Store* pStore,jstring pKey);
void releaseEntryValue(JNIEnv* pEnv, StoreEntry* pEntry);

extern "C"{
JNIEXPORT void JNICALL Java_com_ndk_myndk_Store_setString(JNIEnv* pEnv, jobject pThis, jstring pKey,jstring pString);
JNIEXPORT jstring JNICALL Java_com_ndk_myndk_Store_getString(JNIEnv* pEnv, jobject pThis, jstring key);

JNIEXPORT void JNICALL Java_com_ndk_myndk_Store_setInteger(JNIEnv* pEnv, jobject pThis, jstring pKey,jint pInteger);
JNIEXPORT jint JNICALL Java_com_ndk_myndk_Store_getInteger(JNIEnv *env, jobject instance, jstring pKey);

JNIEXPORT void JNICALL Java_com_ndk_myndk_Store_setBoolean(JNIEnv* pEnv, jobject pThis, jstring pKey,jboolean pBoolean);
JNIEXPORT jboolean JNICALL Java_com_ndk_myndk_Store_getBoolean(JNIEnv *env, jobject instance, jstring pKey);

JNIEXPORT void JNICALL Java_com_ndk_myndk_Store_setColor(JNIEnv* pEnv, jobject pThis, jstring pKey,jobject pColor);
JNIEXPORT jobject JNICALL Java_com_ndk_myndk_Store_getColor(JNIEnv *env, jobject instance, jstring pKey);

JNIEXPORT jintArray JNICALL Java_com_ndk_myndk_Store_getIntegerArray(JNIEnv *env, jobject instance, jstring pKey);
JNIEXPORT void JNICALL Java_com_ndk_myndk_Store_setIntegerArray(JNIEnv *env, jobject instance, jstring pKey, jintArray pIntArray);

JNIEXPORT jobjectArray JNICALL Java_com_ndk_myndk_Store_getStringArray(JNIEnv *env, jobject instance, jstring pKey);
JNIEXPORT void JNICALL Java_com_ndk_myndk_Store_setStringArray(JNIEnv *env, jobject instance, jstring pKey,jobjectArray pStringArray);

JNIEXPORT jobjectArray JNICALL Java_com_ndk_myndk_Store_getColorArray(JNIEnv *env, jobject instance, jstring pKey);
JNIEXPORT void JNICALL Java_com_ndk_myndk_Store_setColorArray(JNIEnv *env, jobject instance, jstring pKey, jobjectArray pColorArray);

JNIEXPORT jlong JNICALL Java_com_ndk_myndk_Store_startWatcher(JNIEnv *pEnv, jobject pThis);
JNIEXPORT void JNICALL Java_com_ndk_myndk_Store_stopWatcher(JNIEnv *pEnv, jobject pThis, jlong pWatcher);
}


void throwInvalidTypeException(JNIEnv* pEnv);
void throwNotExistingKeyException(JNIEnv* pEnv);
void throwStoreFullException(JNIEnv* pEnv);


StoreWatcher* startWatcher(JavaVM* pJavaVM, Store* pStore, jobject pLock);
void stopWatcher(StoreWatcher* pWatcher);
void* runWatcher(void* pArgs);
void processEntry(StoreEntry* pEntry);


#endif //MYNDK_STORE_H

