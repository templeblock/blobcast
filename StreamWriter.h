#pragma once
#include <GL/glew.h>
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
}

class StreamWriter
{
	public:
		StreamWriter(int viewportWidth, int viewportHeight);
		~StreamWriter();
		void WriteFrame();
		void Close();
		bool IsOpen() const;

	private:
		AVFrame *avframe;
		AVCodecContext *avctx;
		AVFormatContext *avfmt;
		SwsContext *swctx;
		GLuint pbo;
		int width;
		int height;
		bool open;
};