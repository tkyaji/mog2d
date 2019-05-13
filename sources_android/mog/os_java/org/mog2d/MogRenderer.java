package org.mog2d;

import android.opengl.GLSurfaceView;
import android.view.SurfaceView;
import android.view.View;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class MogRenderer implements GLSurfaceView.Renderer {

    public int displayWidth;
    public int displayHeight;
    public View view;

    private int fps = 0;
    private long lastRendererTickInNanoSec = 0;
    private float density;

    public MogRenderer(View view, float density) {
        this.view = view;
        this.density = density;
    }

    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
        this.fps = MogJniBridge.getDefaultFps();
        MogJniBridge.onSurfaceCreated();
    }

    @Override
    public void onSurfaceChanged(GL10 gl10, int w, int h) {
        this.displayWidth = w;
        this.displayHeight = h;
        MogJniBridge.onSurfaceChanged(w, h, this.view.getWidth(), this.view.getHeight(), this.density);
    }

    @Override
    public void onDrawFrame(GL10 gl10) {
        final long now = System.nanoTime();
        final long interval = now - this.lastRendererTickInNanoSec;
        final long diff = (long)((1.0 / this.fps * 1000000000L) - interval);
        if (diff > 0) {
            try {
                Thread.sleep(diff / 1000000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        MogJniBridge.onDrawFrame();
        this.lastRendererTickInNanoSec = now;
    }
}
