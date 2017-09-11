package org.mog2d;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Color;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.HashMap;

public class MogActivity extends Activity {

    static {
        System.loadLibrary("native-lib");
    }

    private static final String TAG = "MOG";

    private ViewGroup layout;
    private GLSurfaceView glSurfaceView;
    private float density;

    private class TouchAction {
        public static final int NONE = 0;
        public static final int DOWN = 1;
        public static final int UP = 2;
        public static final int MOVE = 3;
    }

    private HashMap<String, Object> pluginMap = new HashMap<>();
    private ArrayList<MogActivityEvent.OnCreateListener> onCreateListeners = new ArrayList<>();
    private ArrayList<MogActivityEvent.OnDestroyListener> onDestroyListeners = new ArrayList<>();
    private ArrayList<MogActivityEvent.OnStartListener> onStartListeners = new ArrayList<>();
    private ArrayList<MogActivityEvent.OnStopListener> onStopListeners = new ArrayList<>();
    private ArrayList<MogActivityEvent.OnPauseListener> onPauseListeners = new ArrayList<>();
    private ArrayList<MogActivityEvent.OnResumeListener> onResumeListeners = new ArrayList<>();
    private ArrayList<MogActivityEvent.OnActivityResultListener> onActivityResultListeners = new ArrayList<>();
    private ArrayList<MogActivityEvent.DispatchKeyEventListener> dispatchKeyEventListeners = new ArrayList<>();


    public void registerActivityEventListener(MogActivityEvent.Listener listener) {
        if (listener instanceof MogActivityEvent.OnCreateListener) {
            this.onCreateListeners.add((MogActivityEvent.OnCreateListener)listener);
        }
        if (listener instanceof MogActivityEvent.OnDestroyListener) {
            this.onDestroyListeners.add((MogActivityEvent.OnDestroyListener)listener);
        }
        if (listener instanceof MogActivityEvent.OnStartListener) {
            this.onStartListeners.add((MogActivityEvent.OnStartListener)listener);
        }
        if (listener instanceof MogActivityEvent.OnStopListener) {
            this.onStopListeners.add((MogActivityEvent.OnStopListener)listener);
        }
        if (listener instanceof MogActivityEvent.OnPauseListener) {
            this.onPauseListeners.add((MogActivityEvent.OnPauseListener)listener);
        }
        if (listener instanceof MogActivityEvent.OnResumeListener) {
            this.onResumeListeners.add((MogActivityEvent.OnResumeListener)listener);
        }
        if (listener instanceof MogActivityEvent.OnActivityResultListener) {
            this.onActivityResultListeners.add((MogActivityEvent.OnActivityResultListener)listener);
        }
        if (listener instanceof MogActivityEvent.DispatchKeyEventListener) {
            this.dispatchKeyEventListeners.add((MogActivityEvent.DispatchKeyEventListener)listener);
        }
    }

    public void removeActivityEventListener(MogActivityEvent.Listener listener) {
        if (listener instanceof MogActivityEvent.OnCreateListener) {
            this.onCreateListeners.remove(listener);
        }
        if (listener instanceof MogActivityEvent.OnDestroyListener) {
            this.onDestroyListeners.remove(listener);
        }
        if (listener instanceof MogActivityEvent.OnStartListener) {
            this.onStartListeners.remove(listener);
        }
        if (listener instanceof MogActivityEvent.OnStopListener) {
            this.onStopListeners.remove(listener);
        }
        if (listener instanceof MogActivityEvent.OnPauseListener) {
            this.onPauseListeners.remove(listener);
        }
        if (listener instanceof MogActivityEvent.OnResumeListener) {
            this.onResumeListeners.remove(listener);
        }
        if (listener instanceof MogActivityEvent.OnActivityResultListener) {
            this.onActivityResultListeners.remove(listener);
        }
        if (listener instanceof MogActivityEvent.DispatchKeyEventListener) {
            this.dispatchKeyEventListeners.remove(listener);
        }
    }

    public ViewGroup getLayout() {
        return this.layout;
    }

    public GLSurfaceView getGlSurfaceView() {
        return this.glSurfaceView;
    }

    public Object getPluginObject(String className) {
        return this.pluginMap.get(className);
    }

    public void runOnUiThread(final MogFunction function) {
        this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                function.invoke();
            }
        });
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.d(TAG, "onCreate");
        super.onCreate(savedInstanceState);

        int resId = getResources().getIdentifier("plugin_classes", "array", getPackageName());
        String[] pluginClasses = this.getResources().getStringArray(resId);
        for (String pluginClass : pluginClasses) {
            try {
                Object o = Class.forName(pluginClass).newInstance();
                pluginMap.put(pluginClass, o);
                if (o instanceof MogActivityEvent.Listener) {
                    this.registerActivityEventListener((MogActivityEvent.Listener)o);
                }
            } catch (Exception e) {
                Log.e("MOG", e.getLocalizedMessage(), e);
            }
        }

        this.density = this.getResources().getDisplayMetrics().density;
        MogJniBridge.onCreate(this, this.getAssets(), this.density);

        this.layout = new FrameLayout(this);
        this.glSurfaceView = new GLSurfaceView(this);
//        this.glSurfaceView.setEGLContextClientVersion(2);
        this.glSurfaceView.setRenderer(new MogRenderer());
        this.glSurfaceView.setPreserveEGLContextOnPause(true);
        layout.addView(this.glSurfaceView);
        this.setContentView(layout);

        this.hideNavigationBar();

        for (MogActivityEvent.OnCreateListener listener : this.onCreateListeners) {
            listener.onCreate(this, savedInstanceState);
        }
    }

    @Override
    public void onDestroy() {
        Log.d(TAG, "onDestroy");
        for (MogActivityEvent.OnDestroyListener listener : this.onDestroyListeners) {
            listener.onDestroy(this);
        }
        super.onDestroy();
    }

    @Override
    protected void onPause() {
        Log.d(TAG, "onPause");

        for (MogActivityEvent.OnPauseListener listener : this.onPauseListeners) {
            listener.onPause(this);
        }
        MogJniBridge.onPause();
        this.glSurfaceView.onPause();
        super.onPause();
    }

    @Override
    protected void onResume() {
        Log.d(TAG, "onResume");
        super.onResume();
        this.hideNavigationBar();
        this.glSurfaceView.onResume();
        for (MogActivityEvent.OnResumeListener listener : this.onResumeListeners) {
            listener.onResume(this);
        }
    }

    @Override
    protected void onStart() {
        Log.d(TAG, "onStart");
        super.onStart();
        MogJniBridge.onStart();
        for (MogActivityEvent.OnStartListener listener : this.onStartListeners) {
            listener.onStart(this);
        }
    }

    @Override
    protected void onStop() {
        Log.d(TAG, "onStop");
        for (MogActivityEvent.OnStopListener listener : this.onStopListeners) {
            listener.onStop(this);
        }
        MogJniBridge.onStop();
        super.onStop();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        Log.d(TAG, "onActivityResult requestCode=" + requestCode + ", resultCode=" + resultCode);

        for (MogActivityEvent.OnActivityResultListener listener : this.onActivityResultListeners) {
            listener.onActivityResult(this, requestCode, resultCode, data);
        }
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        final int touchAction = this.toTouchAction(event.getActionMasked());

        if (touchAction != TouchAction.NONE) {
            for (int i = 0; i < event.getPointerCount(); i++) {
                final int pId = event.getPointerId(i);
                final float x = event.getX(i) / this.density;
                final float y = event.getY(i) / this.density;
                this.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        MogJniBridge.onTouchEvent(pId, touchAction, x, y);
                    }
                });
            }
        }
        return super.onTouchEvent(event);
    }

    @Override
    public boolean dispatchKeyEvent(KeyEvent event) {
        for (MogActivityEvent.DispatchKeyEventListener listener : this.dispatchKeyEventListeners) {
            if (listener.dispatchKeyEvent(this, event)) {
                return true;
            }
        }
        return super.dispatchKeyEvent(event);
    }

    private int toTouchAction(int actionMasked) {
        switch (actionMasked) {
            case MotionEvent.ACTION_DOWN:
            case MotionEvent.ACTION_POINTER_DOWN:
                return TouchAction.DOWN;

            case MotionEvent.ACTION_MOVE:
                return TouchAction.MOVE;

            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_OUTSIDE:
            case MotionEvent.ACTION_CANCEL:
            case MotionEvent.ACTION_POINTER_UP:
                return TouchAction.UP;

            default:
                return TouchAction.NONE;
        }
    }

    public void hideNavigationBar() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
            this.glSurfaceView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_FULLSCREEN | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
        } else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN) {
            this.glSurfaceView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_FULLSCREEN | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION);
        } else {
            this.glSurfaceView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_HIDE_NAVIGATION);
        }
    }
}
