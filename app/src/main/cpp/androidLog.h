//
// Created by 弋磊钊 on 2019-07-14.
//

#ifndef H264TOJPG_ANDROIDLOG_H
#define H264TOJPG_ANDROIDLOG_H
#include<android/log.h>//包含Android log打印   需要再make文件中添加  LOCAL_LDLIBS := -llog
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, "ndk_yilz", __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO  , "ndk_yilz", __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN  , "ndk_yilz", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , "ndk_yilz", __VA_ARGS__)
#endif //H264TOJPG_ANDROIDLOG_H
