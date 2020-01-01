package com.yilz.ffmpegdemo;

import android.graphics.ImageFormat;
import android.graphics.Rect;
import android.graphics.YuvImage;
import android.os.Environment;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import com.yilz.ffmpegdemo.jni.FFmpegUtils;
import com.yilz.ffmpegdemo.uilts.RecorderUtils;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.concurrent.BlockingDeque;
import java.util.concurrent.LinkedBlockingDeque;

import static com.yilz.ffmpegdemo.jni.FFmpegUtils.closeMp4File;
import static com.yilz.ffmpegdemo.jni.FFmpegUtils.createMp4File;
import static com.yilz.ffmpegdemo.jni.FFmpegUtils.writerAudioFrame;
import static com.yilz.ffmpegdemo.jni.FFmpegUtils.writerVideoFrame;

public class MainActivity extends AppCompatActivity {


    private byte[] frame;
    private Object lock = new Object();
    private Handler mHandler = new Handler();
    private BlockingDeque<byte[]> blockingDeque;
    boolean isRecorder = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        blockingDeque = new LinkedBlockingDeque<>();
        setContentView(R.layout.activity_main);
        try {
            InputStream inputStream = getResources().getAssets().open("photoh264");
            int size = inputStream.available();
            frame = new byte[size];
            inputStream.read(frame);
        } catch (IOException e) {
            e.printStackTrace();
        }
        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);
        tv.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                byte[] data = FFmpegUtils.h264IFrameToNv21(frame);
                Log.d("yilz", "data = " + data.length);
                YuvImage image = new YuvImage(data, ImageFormat.NV21, 1920, 1080, null);
                FileOutputStream fileOutputStream = null;
                try {
                    fileOutputStream = new FileOutputStream(new File(Environment.getExternalStorageDirectory().toString() + "/phone1.jpg"));
                    image.compressToJpeg(new Rect(0, 0, 1920, 1080), 80, fileOutputStream);
                    fileOutputStream.flush();
                    fileOutputStream.close();
                } catch (FileNotFoundException e) {
                    e.printStackTrace();
                } catch (IOException e) {
                    e.printStackTrace();
                }


            }
        });
    }

}
