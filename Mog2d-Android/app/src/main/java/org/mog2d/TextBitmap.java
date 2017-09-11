package org.mog2d;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Typeface;
import android.util.Log;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.nio.Buffer;
import java.nio.ByteBuffer;

public class TextBitmap {

    private static final String TAG = "MOG";

    public static class Result {
        public byte[] bytes;
        public int width;
        public int height;

        public Result(byte[] bytes, int width, int height) {
            this.bytes = bytes;
            this.width = width;
            this.height = height;
        }
    }

    public static Result createFontTexture(Activity activity, String text, float fontSize, String fontFace, boolean isBold, boolean isItalic, int height) {
        Paint forePaint = getFontPaint(activity, fontSize, fontFace, isBold, isItalic);
        Paint.FontMetrics metrics = forePaint.getFontMetrics();

        Paint backPaint = new Paint();
        backPaint.setColor(Color.TRANSPARENT);
        backPaint.setStyle(Paint.Style.FILL);

        String[] lines = text.split("\n");
        int textWidth = 0;
        for (String line : lines) {
            int width = (int)Math.ceil(forePaint.measureText(line));
            textWidth = Math.max(textWidth, width);
        }
        float lineHeight = metrics.bottom - metrics.top;
        int textHeight = (int)Math.ceil(lineHeight * lines.length);
        if (height > 0) {
            textHeight = height;
        }

        Bitmap bitmap = Bitmap.createBitmap(textWidth, textHeight, Bitmap.Config.ARGB_8888);
        Canvas canvas = new Canvas(bitmap);
        canvas.drawRect(0, 0, textWidth, textHeight, backPaint);

        for (int i = 0; i < lines.length; i++) {
            String line = lines[i];
            canvas.drawText(line, 0, (lineHeight * i) + (lineHeight - metrics.descent), forePaint);
        }

        ByteBuffer buffer = ByteBuffer.allocate(textHeight * textWidth * 4);
        bitmap.copyPixelsToBuffer(buffer);

        return new Result(buffer.array(), textWidth, textHeight);
    }

    private static Paint getFontPaint(Activity activity, float fontSize, String fontFace, boolean isBold, boolean isItalic) {
        Paint paint = new Paint();
        Typeface typeface = getTypeFace(activity, fontFace);
        if (isBold && isItalic) {
            paint.setTypeface(Typeface.create(typeface, Typeface.BOLD_ITALIC));
        } else if (isBold) {
            paint.setTypeface(Typeface.create(typeface, Typeface.BOLD));
        } else {
            paint.setTypeface(typeface);
        }
        paint.setColor(Color.WHITE);
        paint.setAntiAlias(true);
        if (fontSize > 0) {
            paint.setTextSize(fontSize);
        }
        return paint;
    }

    private static Typeface getTypeFace(Activity activity, String fontFace) {
        Typeface typeface = Typeface.DEFAULT;
        if (fontFace == null || fontFace.length() == 0) {
            return typeface;
        }

        String lowerCase = fontFace.toLowerCase();
        if (lowerCase.equals("serif")) {
            return Typeface.SERIF;
        } else if (lowerCase.equals("sans_serif")) {
            return Typeface.SANS_SERIF;
        } else if (lowerCase.equals("monospace")) {
            return Typeface.MONOSPACE;
        } else if (fontFace.contains(".")) {
            try {
                activity.getAssets().open(fontFace);
                return Typeface.createFromAsset(activity.getAssets(), fontFace);
            } catch (IOException e) {
                Log.e(TAG, "Font file is not found. font=" + fontFace);
            }
        } else {
            try {
                activity.getAssets().open(fontFace + ".ttf");
                return Typeface.createFromAsset(activity.getAssets(), fontFace + ".ttf");
            } catch (IOException e) {
                try {
                    activity.getAssets().open(fontFace + ".otf");
                    return Typeface.createFromAsset(activity.getAssets(), fontFace + ".otf");
                } catch (IOException ee) {
                    Log.e(TAG, "Font file is not found. font=" + fontFace);
                }
            }
        }
        return Typeface.DEFAULT;
    }
}
