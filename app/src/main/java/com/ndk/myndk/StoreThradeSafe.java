package com.ndk.myndk;

import com.ndk.myndk.exception.InvalidTypeException;
import com.ndk.myndk.exception.NotExistingKeyException;

/**
 * Created by j.guzikowski on 4/17/18.
 */

public class StoreThradeSafe extends Store {
    protected static Object LOCK;

    public StoreThradeSafe(StoreListener pListener) {
        super(pListener);
    }


    @Override
    public int getInteger(String pKey) throws NotExistingKeyException, InvalidTypeException {
        synchronized (LOCK) {
            return super.getInteger(pKey);
        }
    }

    @Override
    public void setInteger(String pKey, int pInt){
        synchronized (LOCK){
            super.setInteger(pKey,pInt);
        }
    }

    @Override
    public String getString(String pKey) throws NotExistingKeyException, InvalidTypeException {
        synchronized (LOCK) {
            return super.getString(pKey);
        }
    }

    @Override
    public void setString(String pKey, String pString){
        synchronized (LOCK){
            super.setString(pKey,pString);
        }
    }

    public Color getColor(String pKey) throws NotExistingKeyException, InvalidTypeException {
        synchronized (LOCK) {
            return super.getColor(pKey);
        }
    }

    @Override
    public void setColor(String pKey, Color pColor){
        synchronized (LOCK){
            super.setColor(pKey,pColor);
        }
    }

    @Override
    public void stopWatcher(long pPointer){
        synchronized (LOCK){
            super.stopWatcher(pPointer);
        }
    }

}

