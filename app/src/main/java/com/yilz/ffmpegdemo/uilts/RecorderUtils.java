package com.yilz.ffmpegdemo.uilts;

import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;

import static android.media.AudioFormat.CHANNEL_IN_MONO;

public class RecorderUtils {

    static RecorderUtils instance;

    public static RecorderUtils getInstance() {
        if (instance == null) {
            instance = new RecorderUtils();
        }
        return instance;
    }


    private pcmCallback pcmCallback;

    public void setPcmCallback(RecorderUtils.pcmCallback pcmCallback) {
        this.pcmCallback = pcmCallback;
    }

    public interface pcmCallback {
        void pcmDatas(byte[] datas, int len);
    }

    private AudioRecord audioRecord = null;  // 声明 AudioRecord 对象
    private int recordBufSize = 0; // 声明recoordBufffer的大小字段
    byte data[];
    private boolean isRecording;
    private Thread recordingThread = new Thread() {
        @Override
        public void run() {
            super.run();
            while (isRecording) {
                int len = audioRecord.read(data, 0, recordBufSize);
                if (pcmCallback != null) {
                    pcmCallback.pcmDatas(data, len);
                }
            }
        }
    };

    public void createAudioRecord() {
        recordBufSize = AudioRecord.getMinBufferSize(44100, CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT);  //audioRecord能接受的最小的buffer大小
        audioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC, 44100, CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT, recordBufSize);
        data = new byte[recordBufSize];
    }

    /**
     * 开始录音 使用PCM格式
     * 录音文件
     *
     * @return
     */
    public void startRecord() {
        createAudioRecord();
        audioRecord.startRecording();
        isRecording = true;
        recordingThread.start();
    }

    /**
     * 停止录音
     */
    public void stopRecord() {
        if (null != audioRecord) {
            recordingThread.interrupt();
            isRecording = false;
            audioRecord.stop();
            audioRecord.release();
            audioRecord = null;
        }
    }


}
