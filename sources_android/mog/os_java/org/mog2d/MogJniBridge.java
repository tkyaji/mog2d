package org.mog2d;

import android.app.Activity;
import android.content.res.AssetManager;

public class MogJniBridge {

    public static native void onSurfaceCreated();
    public static native void onSurfaceChanged(int w,int h, int vw, int vh);
    public static native void onDrawFrame();
    public static native int getDefaultFps();

    public static native void onCreate(Activity activity, AssetManager assetManager, float scaleFactor);
    public static native void onDestroy();
    public static native void onPause();
    public static native void onResume();
    public static native void onStart();
    public static native void onStop();
    public static native void onTouchEvent(int pointerId, int touchAction, float x, float y);

    public static native void runCallback(long functionPtr, Object ... params);
    public static native void releaseNativeFunction(long functionPtr);
}
