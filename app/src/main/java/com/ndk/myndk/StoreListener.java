package com.ndk.myndk;

/**
 * Created by j.guzikowski on 4/16/18.
 */

public interface StoreListener {
    void onSuccess(int pValue);
    void onSuccess(String pValue);
    void onSuccess(Color pValue);
}
