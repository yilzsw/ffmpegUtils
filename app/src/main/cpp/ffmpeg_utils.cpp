#include "ffmpeg_utils.h"
#include <stdio.h>

extern "C"
{
#include "h264_pcm_to_mp4.h"
#include "include/libavcodec/avcodec.h"
#include "include/libavutil/frame.h"
#include <libavcodec/jni.h>
}

static const char *classFile = "com/yilz/ffmpegdemo/jni/FFmpegUtils";

jint jniCrateMp4File(JNIEnv *env, jobject thiz, jstring name, jint width, jint height, jint time) {
    const char *names = env->GetStringUTFChars(name, 0);
    int result = createMp4File(names, width, height, time);
    return result;
}

int jni_write_audio_frame(JNIEnv *env, jobject thiz, jbyteArray byteArray, jint len) {
    jbyte *bytes = env->GetByteArrayElements(byteArray, 0);
    LOGW("data len = %d", len);
    write_audio_frame(bytes, len);
    env->ReleaseByteArrayElements(byteArray, bytes, 0);
    return 0;
}

int jni_write_video_frame(JNIEnv *env, jobject thiz, jbyteArray byteArray) {
    jbyte *bytes = env->GetByteArrayElements(byteArray, 0);
    int chars_len = env->GetArrayLength(byteArray);
    LOGW("data len = %d", chars_len);
    write_video_frame(bytes, chars_len);
    env->ReleaseByteArrayElements(byteArray, bytes, 0);
    return 1;
}

void jni_CloseMp4File(JNIEnv *env, jobject thiz) {
    closeMp4File();
}

jbyteArray h264IFrameToNv21(JNIEnv *env, jobject thiz, jbyteArray byteArray) {

    jbyte *bytes = env->GetByteArrayElements(byteArray, 0);
    int chars_len = env->GetArrayLength(byteArray);
    LOGW("data len = %d", chars_len);
//    AVCodecParserContext *parser;
    AVCodecContext *ctx = NULL;
    AVCodec *codec = NULL;
    AVPacket *avpkt = NULL;
    AVFrame *frame = NULL;
    jbyteArray jbyteArray = NULL;
    int height = 0;
    int width = 0;
    int result = 0;
    //1.从注册的解码器里找到H264解码器
    codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec) {
        LOGE("Codec not found\\\n");
        goto cleanup;
    }
//2. 初始化解码的上下文，上下文很关键，包含了解码所需要的信息
    ctx = avcodec_alloc_context3(codec);
    if (!ctx) {
        LOGE("Could not allocate video codec context\n");
        goto cleanup;
    }
    if (avcodec_open2(ctx, codec, NULL) < 0) {
        LOGE("Could not open codec\n");
        goto cleanup;
    }
//3. 准备一个容器用来装需要解码的原始H264数据
    avpkt = av_packet_alloc();
//4. 准备一个容器用来装解码后的数据，AVFrame既可以表示视频数据，也可以表示音频数据
    frame = av_frame_alloc();
    if (!avpkt) {
        LOGE("Could not allocate video frame avpkt");
        goto cleanup;
    }

    if (!frame) {
        LOGE("Could not allocate video frame frame");
        goto cleanup;
    }
    LOGE("frame size = %d  avpkt = %d", frame->pkt_size, avpkt->size);

//5. 初始化avpkt,并将H264数据放进去
    av_init_packet(avpkt);
    avpkt->data = (uint8_t *) bytes;
    avpkt->size = chars_len;
//6. 解码 - 发送需要解码的数据给上下文
    avcodec_send_packet(ctx, avpkt);
//7. 解码 - 从上下文中获取解码后的frame，解码完成
    result = avcodec_receive_frame(ctx, frame);
    LOGW("frame size = %d  width = %d  height = %d result  = %d format = %d",
         frame->linesize[0],
         frame->width,
         frame->height, result,
         frame->format);
    height = frame->height;
    width = frame->width;
    if (frame->format == AV_PIX_FMT_YUV420P) {
        int frameSize = width * height;
        int qFrameSize = frameSize / 4;
        int yuvSize = frameSize + qFrameSize + qFrameSize;
        unsigned char *yuvbuff = new unsigned char[yuvSize];
        //数据转化为NV21
        memcpy(yuvbuff, frame->data[0],
               frameSize);//Y
        for (int i = 0; i < qFrameSize; i++) {
            yuvbuff[frameSize + i * 2] = frame->data[2][i]; // Cr (V)
            yuvbuff[frameSize + i * 2 + 1] = frame->data[1][i]; // Cb (U)
        }
        jbyteArray = env->NewByteArray(yuvSize);
        env->SetByteArrayRegion(jbyteArray, 0, yuvSize,
                                (jbyte *) yuvbuff);
        delete (yuvbuff);
    }

    cleanup:
    if (ctx)
        avcodec_free_context(&ctx);
    if (frame)
        av_frame_free(&frame);
    if (avpkt)
        av_packet_free(&avpkt);
    env->ReleaseByteArrayElements(byteArray, bytes, 0);
    return jbyteArray;
}

//注册Java端的方法  以及本地相对应的方法
JNINativeMethod method[] = {{"h264IFrameToNv21", "([B)[B",                   (void *) h264IFrameToNv21},
                            {"createMp4File",    "(Ljava/lang/String;III)I", (void *) jniCrateMp4File},
                            {"closeMp4File",     "()V",                      (void *) jni_CloseMp4File},
                            {"writerAudioFrame", "([BI)I",                   (void *) jni_write_audio_frame},
                            {"writerVideoFrame", "([B)I",                    (void *) jni_write_video_frame}};

//注册相应的类以及方法
jint registerNativeMethod(JNIEnv *env) {
    jclass cl = env->FindClass(classFile);
    if ((env->RegisterNatives(cl, method, sizeof(method) / sizeof(method[0]))) < 0) {
        return -1;
    }
    return 0;
}

//实现jni_onload 动态注册方法
jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    if (registerNativeMethod(env) != JNI_OK) {//注册方法
        return -1;
    }
    LOGI("%d", env->GetVersion());
    initFormat();
    av_jni_set_java_vm(vm, NULL);
    return JNI_VERSION_1_6;//必须返回这个值
}

void JNI_OnUnload(JavaVM *vm, void *reserved) {
    LOGE("JNI_OnUnload");
}
