package com.yilz.ffmpegdemo.jni;

public class FFmpegUtils {

    // Used to load the 'ffmpeg_uilts' library on application startup.
    static {
        System.loadLibrary("ffmpeg_uilts");
    }

    public static native byte[] h264IFrameToNv21(byte[] frame);

    public static native int createMp4File(String fileName, int width, int height, int time);

    public static native void closeMp4File();

    public static native int writerAudioFrame(byte[] frame, int len);

    public static native int writerVideoFrame(byte[] frame);

}
