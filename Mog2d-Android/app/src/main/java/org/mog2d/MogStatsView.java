package org.mog2d;

import android.content.Context;
import android.graphics.Color;
import android.graphics.Typeface;
import android.view.Gravity;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import android.widget.TextView;

public class MogStatsView extends FrameLayout {

    public enum Alignment {
        TOP_LEFT,
        TOP_CENTER,
        TOP_RIGHT,
        MIDDLE_LEFT,
        MIDDLE_CENTER,
        MIDDLE_RIGHT,
        BOTTOM_LEFT,
        BOTTOM_CENTER,
        BOTTOM_RIGHT,
    }

    private TextView textView;
    private FrameLayout.LayoutParams layoutParams;

    public MogStatsView(Context context) {
        super(context);

        float d = this.getResources().getDisplayMetrics().density;
        this.textView = new TextView(context);
        this.textView.setBackgroundColor(Color.argb(128, 0, 0, 0));
        this.textView.setPadding((int)Math.ceil(10 * d), (int)Math.ceil(8 * d), (int)Math.ceil(10 * d), (int)Math.ceil(8 * d));
        this.textView.setTypeface(Typeface.MONOSPACE);
        this.layoutParams = new FrameLayout.LayoutParams((int)Math.ceil(185.0f * d), (int)Math.ceil(70.0f * d));
        this.textView.setLayoutParams(this.layoutParams);
        this.addView(this.textView);
        this.setAlignment(Alignment.BOTTOM_LEFT);
    }

    public void setStats(float delta, int drawCall, int instants) {
        String text = String.format(" %.3f / %3f\n DRAW CALL : %d\n INSTANTS  : %d", 1/delta, delta, drawCall, instants);
        this.textView.setText(text);
    }

    public void setAlignment(Alignment alignment) {
        switch (alignment) {
            case TOP_LEFT:
                this.layoutParams.gravity = Gravity.TOP | Gravity.LEFT;
                break;
            case TOP_CENTER:
                this.layoutParams.gravity = Gravity.TOP | Gravity.CENTER_HORIZONTAL;
                break;
            case TOP_RIGHT:
                this.layoutParams.gravity = Gravity.TOP | Gravity.RIGHT;
                break;
            case MIDDLE_LEFT:
                this.layoutParams.gravity = Gravity.CENTER_VERTICAL | Gravity.LEFT;
                break;
            case MIDDLE_CENTER:
                this.layoutParams.gravity = Gravity.CENTER;
                break;
            case MIDDLE_RIGHT:
                this.layoutParams.gravity = Gravity.CENTER_VERTICAL | Gravity.RIGHT;
                break;
            case BOTTOM_LEFT:
                this.layoutParams.gravity = Gravity.BOTTOM | Gravity.LEFT;
                break;
            case BOTTOM_CENTER:
                this.layoutParams.gravity = Gravity.BOTTOM | Gravity.CENTER_HORIZONTAL;
                break;
            case BOTTOM_RIGHT:
                this.layoutParams.gravity = Gravity.BOTTOM | Gravity.RIGHT;
                break;
        }
    }
}
