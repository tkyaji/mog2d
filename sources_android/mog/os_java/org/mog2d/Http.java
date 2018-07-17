package org.mog2d;

import android.app.Activity;

import java.io.ByteArrayOutputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.Map;
import java.util.logging.Handler;

public class Http {

    private enum Method {
        GET,
        POST,
    }

    public static void request(final Activity activity, final String urlStr, final int methodIdx, final int timeout, Map<String, Object> params, final MogFunction callback) {
        final Method method = Method.values()[methodIdx];

        new Thread(new Runnable() {
            @Override
            public void run() {
                HttpURLConnection conn = null;
                try {
                    URL url = new URL(urlStr);
                    conn = (HttpURLConnection)url.openConnection();
                    conn.setRequestMethod(method.toString());
                    conn.setInstanceFollowRedirects(true);
                    conn.setDoInput(true);
                    if (method == Method.POST) {
                        conn.setDoOutput(true);
                    }
                    conn.setChunkedStreamingMode(0);
                    conn.connect();

                    ByteArrayOutputStream byteOutStream = new ByteArrayOutputStream();
                    byte[] buffer = new byte[1024000];
                    int len = 0;
                    while((len = conn.getInputStream().read(buffer)) > 0) {
                        byteOutStream.write(buffer, 0, len);
                    }
                    callback.invoke(conn.getResponseCode(), "", byteOutStream.toByteArray());

                } catch (Exception ex) {
                    int responseCode = 0;
                    try {
                        responseCode = conn.getResponseCode();
                    } catch (Exception ex2) {
                    }
                    callback.invoke(responseCode, ex.getMessage(), null);

                } finally {
                    if (conn != null) conn.disconnect();
                }
            }
        }).run();
    }
}
