#include "logger.h"

#include "defines.h"

#include <stdexcept>
#include <cstdio>
#include <cstdarg>

// apparently this is an outdated version
Logger logger;

Logger::Logger(void) : file(NULL), indent(), compile_log_created(false)
{
	file = fopen(FILE_LOG, "w");
	if (file == NULL) {
        throw std::runtime_error("Failed to create log file");
    }
	time_t timer = time(NULL);
	fprintf(file, "%s\nLogger initialized\n", ctime(&timer));
	compile_log_created = false;
}

Logger::~Logger(void)
{
	if (file) fclose(file);
}

void Logger::Write(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(file, format, args);
	va_end(args);
	fflush(file);
}

void Logger::LogStartup(int argc, char *argv[])
{
	logger.Write("Program launched");
	if (argc > 0) {
		logger.Write(" with %d argument%s:", argc, argc == 1 ? "" : "s");
		for (int i = 0; i < argc; i++)
			logger.Write(" %s", argv[i]);
	}
	logger.Write("\n");
}

void Logger::LogSystemInfo()
{
	logger.Write("OpenGL render context information:\n"
		"  Renderer       : %s\n"
		"  Vendor         : %s\n"
		"  Version        : %s\n"
		"  GLSL version   : %s\n",
		(char*)glGetString(GL_RENDERER),
		(char*)glGetString(GL_VENDOR),
		(char*)glGetString(GL_VERSION),
		(char*)glGetString(GL_SHADING_LANGUAGE_VERSION)
	);
	GLint vert_num, ind_num;
	glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &vert_num);
	glGetIntegerv(GL_MAX_ELEMENTS_INDICES,  &ind_num);
	logger.Write("Max number of vertex array vertices: %d\n"
		         "Max number of vertex array indices : %d\n",
		         vert_num,
		         ind_num);
}

void Logger::LogShaderInfo(GLuint object, const char *prefix)
{
	GLint log_length = 0;
	if (glIsShader(object))
		glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
	else if (glIsProgram(object))
		glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
	else {
		logger.Write("LogShaderInfo: argument is not a shader or a program.\n");
		return;
	}

	char* log = (char*)malloc(log_length);

	if (glIsShader(object))
		glGetShaderInfoLog(object, log_length, NULL, log);
	else if (glIsProgram(object))
		glGetProgramInfoLog(object, log_length, NULL, log);
	logger.Write("%s\n", log);

	FILE *fcomp;
	if (compile_log_created) {
		fcomp = fopen(FILE_COMPILE_ERRORS, "a");
		fprintf(fcomp, "\n");
	} 
	else {
		fcomp = fopen(FILE_COMPILE_ERRORS, "w");
		compile_log_created = true;
	}
	fprintf(fcomp, "%s\n%s\n", prefix, log);

	free(log);
}

void Logger::LogShutdown()
{
	logger.Write("Program has terminated.\n");
}

void Logger::LogOpenGLError(GLenum error)
{
    logger.Write("OpenGL error %d occured (%s)\n", error, gluErrorString(error));
}

void Logger::LogVector(std::vector<GLuint> &v)
{
	logger.Write("size = %d\n", v.size());
    for (unsigned i = 0; i < v.size() / 3; i++)
        logger.Write("%d %d %d\n", v[3 * i], v[3 * i + 1], v[3 * i + 2]);
}

void Logger::LogVector(std::vector<GLfloat> &v)
{
	logger.Write("size = %d\n", v.size());
    for (unsigned i = 0; i < v.size() / 3; i++)
        logger.Write("%.2f %.2f %.2f\n", v[3 * i], v[3 * i + 1], v[3 * i + 2]);
}
