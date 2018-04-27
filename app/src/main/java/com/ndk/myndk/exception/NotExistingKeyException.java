package com.ndk.myndk.exception;

/**
 * Created by j.guzikowski on 4/16/18.
 */

public class NotExistingKeyException extends Exception {
    public NotExistingKeyException(String pDetailMessage){
        super(pDetailMessage);
    }
}
