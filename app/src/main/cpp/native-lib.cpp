#include <jni.h>
#include <string>
#include "../jni/Store.h"
#include <android/log.h>
#include <jni.h>
#include <string.h>

extern "C"
JNIEXPORT jstring

JNICALL
Java_com_ndk_myndk_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_ndk_myndk_Store_getCount(JNIEnv *env, jobject instance) {
    return 0;

    // TODO

}

static Store gStore;


void releaseEntryValue(JNIEnv *pEnv, StoreEntry *entry);

JNIEXPORT jint JNI_OnLoad(JavaVM* pVM, void* reserved){
    gStore.mLength =0;
    return JNI_VERSION_1_6;
}



StoreEntry* findEntry(JNIEnv* pEnv, Store* pStore, jstring pKey){
    __android_log_write(ANDROID_LOG_ERROR, "Tag", "Error here");//Or ANDROID_LOG_INFO, ..
    StoreEntry* entry = pStore -> mEntry;
    StoreEntry* entryEnd = entry + pStore->mLength;
    const char* tmpKey = pEnv -> GetStringUTFChars(pKey,NULL);
    while ((entry<entryEnd) && ((entry->mKey,tmpKey)!= 0 )  ){
        ++entry;
    }

    pEnv -> ReleaseStringUTFChars(pKey,tmpKey);
    return (entry == entryEnd) ? NULL : entry;

}

//c++ methods
bool isEntryValid(JNIEnv* pEnv, StoreEntry* pEntry, StoreType pType){
    return ((pEntry !=NULL) && (pEntry->mType == pType) );
}



StoreEntry* allocateEntry(JNIEnv* pEnv, Store* pStore, jstring pKey){
    StoreEntry* entry = findEntry(pEnv,pStore,pKey);
    if(entry != NULL){
        releaseEntryValue(pEnv,entry);
    }else{
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
    }
}

JNIEXPORT jstring JNICALL Java_com_ndk_myndk_Store_getString(JNIEnv* pEnv, jobject pObject, jstring key){
    __android_log_write(ANDROID_LOG_ERROR, "Tag", "Error here");//Or ANDROID_LOG_INFO, ...
    StoreEntry* entry = findEntry(pEnv,&gStore,key);
    if(isEntryValid(pEnv,entry,StoreType_String)){
        return pEnv->NewStringUTF(entry->mValue.mString);
    }else{
        return NULL;
    }

}

JNIEXPORT jstring JNICALL Java_com_ndk_myndk_Store_setString(JNIEnv* pEnv, jobject pThi, jstring pKey,jstring pString){
    StoreEntry* entry = allocateEntry(pEnv, &gStore, pKey);
    if(entry != NULL){
        entry -> mType = StoreType_String;
        jsize stringLength = pEnv -> GetStringUTFLength(pString);
        entry ->mValue.mString = new char[stringLength +1];
        pEnv->GetStringUTFRegion(pString,0,stringLength,entry->mValue.mString);
        entry->mValue.mString[stringLength] = '\0';
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_com_ndk_myndk_MainActivity_setString(JNIEnv *env, jobject instance, jstring s_) {
    const char *s = env->GetStringUTFChars(s_, 0);

    // TODO

    env->ReleaseStringUTFChars(s_, s);
}