package org.mog2d;

public class MogFunction {

    private long functionPtr;

    public MogFunction(long functionPtr) {
        this.functionPtr = functionPtr;
    }

    @Override
    protected void finalize() throws Throwable {
        try {
            super.finalize();
        } finally {
            MogJniBridge.releaseNativeFunction(this.functionPtr);
        }
    }

    public void invoke(Object ... params) {
        MogJniBridge.runCallback(this.functionPtr, params);
    }
}
