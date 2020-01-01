//
// Created by 弋磊钊 on 2019-07-14.
//

#ifndef H264_PCM_TO_MP4
#define H264_PCM_TO_MP4
int write_audio_frame(void *byte, int len);
int write_video_frame(void *byte, int len);
int createMp4File(const char *fileName, int width, int height,int time);
void closeMp4File();
void initFormat();
#endif //H264_PCM_TO_MP4
