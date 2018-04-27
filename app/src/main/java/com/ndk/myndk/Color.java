package com.ndk.myndk;

import android.text.TextUtils;

/**
 * Created by j.guzikowski on 4/16/18.
 */

public class Color{
    private int mColor;

    public Color(String pColor){
        if(TextUtils.isEmpty(pColor)){
            throw new IllegalArgumentException();
        }
        mColor = android.graphics.Color.parseColor(pColor);
    }

    @Override
    public String toString() {
        return String.format("#%06X",mColor);
    }

}
