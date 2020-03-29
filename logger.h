#pragma once

#include <GL/glew.h>

#include <cstdio>
#include <string>
#include <vector>

class Logger {
    FILE *file;
    bool duplicate_to_stderr;
    std::string indent;
    Logger(const Logger &a) = delete;
    Logger &operator=(const Logger &a) = delete;
    static void WriteToFile(FILE *fout, const char *format, va_list args);

public:
    Logger(void);
    ~Logger(void);
    void Write(const char *format, ...);
    void GoDown() { indent += "\t"; };
    void GoUp() { indent.pop_back(); };

    void LogStartup(int argc, char *argv[]);
    void LogSystemInfo();
    void LogShaderInfo(GLuint object, const char *prefix = "");
    void LogShutdown();

    void LogOpenGLError(GLenum error);

    void LogVector(std::vector<GLuint> &v);
    void LogVector(std::vector<GLfloat> &v);
};

extern Logger logger;
