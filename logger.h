#pragma once

#include <cstdio>
#include <string>
#include <vector>
#include <GL/glew.h>

class Logger
{
	FILE *file;
	std::string indent;
	bool compile_log_created; //so that fragment log won't overwrite vertex log
    Logger(const Logger &a) = delete;
    Logger &operator= (const Logger &a) = delete;
public:
	Logger(void);
	~Logger(void);
	void Write(const char *format, ...);
	void GoDown() { indent += "\t"; };
	void GoUp()   { indent.pop_back(); };

	void LogStartup(int argc, char *argv[]);
	void LogSystemInfo();
	void LogShaderInfo(GLuint object, const char *prefix = "");
	void LogShutdown();

	void LogOpenGLError(GLenum error);

	void LogVector(std::vector<GLuint> &v);
	void LogVector(std::vector<GLfloat> &v);
};

extern Logger logger;
