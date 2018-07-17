package org.mog2d;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.KeyEvent;

public final class MogActivityEvent {

    public interface EventListener extends OnCreateListener, OnDestroyListener,
            OnStartListener, OnStopListener, OnPauseListener, OnResumeListener, OnActivityResultListener {
    }

    public interface Listener {
    }

    public interface OnCreateListener extends Listener {
        public void onCreate(Activity activity, Bundle savedInstanceState);
    }

    public interface OnDestroyListener extends Listener  {
        public void onDestroy(Activity activity);
    }

    public interface OnStartListener extends Listener  {
        public void onStart(Activity activity);
    }

    public interface OnStopListener extends Listener  {
        public void onStop(Activity activity);
    }

    public interface OnPauseListener extends Listener  {
        public void onPause(Activity activity);
    }

    public interface OnResumeListener extends Listener  {
        public void onResume(Activity activity);
    }

    public interface OnActivityResultListener extends Listener  {
        public void onActivityResult(Activity activity, int requestCode, int resultCode, Intent data);
    }

    public interface DispatchKeyEventListener extends Listener {
        public boolean dispatchKeyEvent(Activity activity, KeyEvent event);
    }
}
