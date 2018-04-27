package com.ndk.myndk.exception;

/**
 * Created by j.guzikowski on 4/16/18.
 */

public class StoreFullException extends RuntimeException {
    public StoreFullException(String pString){
        super(pString);
    }
}
