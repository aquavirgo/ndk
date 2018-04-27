package com.ndk.myndk;

import com.ndk.myndk.exception.InvalidTypeException;
import com.ndk.myndk.exception.NotExistingKeyException;

/**
 * Created by j.guzikowski on 4/4/18.
 */

public class Store implements StoreListener{
    private StoreListener mListener;
    static {
        System.loadLibrary("Store");
    }

    public Store(StoreListener pListener){
        mListener = pListener;
    }

    public native int getCount();
    public native String getString(String pKey)
            throws NotExistingKeyException, InvalidTypeException;
    public native void setString(String pKey, String pString);

    public native int getInteger(String pKey)
            throws NotExistingKeyException, InvalidTypeException;
    public native void setInteger(String pKey, int pInt);

    public native boolean getBoolean(String pKey);
    public native void setBoolean(String pKey, boolean pBoolean);

    public native Color getColor(String pKey)
            throws NotExistingKeyException, InvalidTypeException;
    public native void setColor(String pKey, Color color);

    public native int[] getIntegerArray(String pKey);
    public native void setIntegerArray(String pKey, int[] pIntArray);

    public native String[] getStringArray(String pKey);
    public native void setStringArray(String pKey, String[] pStringArray);

    public native Color[] getColorArray(String pKey);
    public native void setColorArray(String pKey, Color[] pColorArray);

    public native long startWatcher();
    public native void stopWatcher(long pPointer);

    @Override
    public void onSuccess(int pValue) {
        mListener.onSuccess(pValue);
    }

    @Override
    public void onSuccess(String pValue) {
        mListener.onSuccess(pValue);
    }

    @Override
    public void onSuccess(Color pValue) {
        mListener.onSuccess(pValue);
    }


}
