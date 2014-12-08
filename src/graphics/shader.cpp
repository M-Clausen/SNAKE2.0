/******************************************
 * COPYRIGHT (C) 2014 MADS HAUPT CLAUSEN
 ******************************************/

#include "shader.h"

namespace graphics
{
    char Shader::compile()
    {
        // Create the shaders
        GLuint vertShaderID = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);

        // Read the Vertex Shader code from the file
        std::string vertShaderCode;
        std::ifstream vertShaderStream(_vsPath, std::ios::in);
        if(vertShaderStream.is_open())
        {
            std::string Line = "";
            while(getline(vertShaderStream, Line))
                vertShaderCode += "\n" + Line;
            vertShaderStream.close();
        }

        // Read the Fragment Shader code from the file
        std::string fragShaderCode;
        std::ifstream fragShaderStream(_fsPath, std::ios::in);
        if(fragShaderStream.is_open()){
            std::string Line = "";
            while(getline(fragShaderStream, Line))
                fragShaderCode += "\n" + Line;
            fragShaderStream.close();
        }

        GLint Result = GL_FALSE;
        int infoLogLen;

        char const *vertSourcePointer = vertShaderCode.c_str();
        GLint shader_len = vertShaderCode.length();
        glShaderSource(vertShaderID, 1, &vertSourcePointer, &shader_len);
        glCompileShader(vertShaderID);

        // Check Vertex Shader
        glGetShaderiv(vertShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(vertShaderID, GL_INFO_LOG_LENGTH, &infoLogLen);
        std::vector<char> vertShaderErrMsgs(infoLogLen);
        glGetShaderInfoLog(vertShaderID, infoLogLen, NULL, &vertShaderErrMsgs[0]);
        // fprintf(stdout, "%s\n", &vertShaderErrMsgs[0]);

        // Compile Fragment Shader
        char const *fragSourcePointer = fragShaderCode.c_str();
        shader_len = fragShaderCode.length();
        glShaderSource(fragShaderID, 1, &fragSourcePointer, &shader_len);
        glCompileShader(fragShaderID);

        // Check Fragment Shader
        glGetShaderiv(fragShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(fragShaderID, GL_INFO_LOG_LENGTH, &infoLogLen);
        std::vector<char> fragShaderErrMsgs(infoLogLen);
        glGetShaderInfoLog(fragShaderID, infoLogLen, NULL, &fragShaderErrMsgs[0]);
        // fprintf(stdout, "%s\n", &fragShaderErrMsgs[0]);

        // Link the program
        glAttachShader(_id, vertShaderID);
        glAttachShader(_id, fragShaderID);
        glLinkProgram(_id);

        // Check the program
        glGetProgramiv(_id, GL_LINK_STATUS, &Result);
        glGetProgramiv(_id, GL_INFO_LOG_LENGTH, &infoLogLen);
        std::vector<char> progErrMsgs(std::max(infoLogLen, int(1)));

        glGetProgramInfoLog(_id, infoLogLen, NULL, &progErrMsgs[0]);
        fprintf(stdout, "%s\n", &progErrMsgs[0]);

        glDeleteShader(vertShaderID);
        glDeleteShader(fragShaderID);

        _id = _id;


        if(progErrMsgs.size() > 1)
        {
            return -1;
        }

        compiled = 1;
        return 0;
    }

    char Shader::compile(const char *vertSourcePointer, const char *fragSourcePointer)
    {
        // Create the shaders
        GLuint vertShaderID = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);

        GLint Result = GL_FALSE;
        int infoLogLen;

        // Compile Vertex Shader
        glShaderSource(vertShaderID, 1, &vertSourcePointer, NULL);
        glCompileShader(vertShaderID);

        // Check Vertex Shader
        glGetShaderiv(vertShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(vertShaderID, GL_INFO_LOG_LENGTH, &infoLogLen);
        std::vector<char> vertShaderErrMsgs(infoLogLen);
        glGetShaderInfoLog(vertShaderID, infoLogLen, NULL, &vertShaderErrMsgs[0]);
        fprintf(stdout, "%s\n", &vertShaderErrMsgs[0]);

        // Compile Fragment Shader
        glShaderSource(fragShaderID, 1, &fragSourcePointer, NULL);
        glCompileShader(fragShaderID);

        // Check Fragment Shader
        glGetShaderiv(fragShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(fragShaderID, GL_INFO_LOG_LENGTH, &infoLogLen);
        std::vector<char> fragShaderErrMsgs(infoLogLen);
        glGetShaderInfoLog(fragShaderID, infoLogLen, NULL, &fragShaderErrMsgs[0]);
        fprintf(stdout, "%s\n", &fragShaderErrMsgs[0]);

        // Link the program
        glAttachShader(_id, vertShaderID);
        glAttachShader(_id, fragShaderID);
        glLinkProgram(_id);

        // Check the program
        glGetProgramiv(_id, GL_LINK_STATUS, &Result);
        glGetProgramiv(_id, GL_INFO_LOG_LENGTH, &infoLogLen);
        std::vector<char> progErrMsgs(std::max(infoLogLen, int(1)));
        glGetProgramInfoLog(_id, infoLogLen, NULL, &progErrMsgs[0]);
        fprintf(stdout, "%s\n", &progErrMsgs[0]);

        glDeleteShader(vertShaderID);
        glDeleteShader(fragShaderID);

        _id = _id;


        if(progErrMsgs.size() > 1)
        {
            return -1;
        }

        compiled = 1;
        return 0;
    }

    char Shader::addGeometryShader(const char *geoPath)
    {
        if(_id == 0)
        {
            return -1;
        }

        std::string geoShaderCode;
        std::ifstream geoShaderStream(geoPath, std::ios::in);
        if(geoShaderStream.is_open()){
            std::string Line = "";
            while(getline(geoShaderStream, Line))
                geoShaderCode += "\n" + Line;
            geoShaderStream.close();
        }

        GLint Result = GL_FALSE;
        int infoLogLen;

        char const *geoSourcePointer = geoShaderCode.c_str();
        GLuint geoShaderID = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geoShaderID, 1, &geoSourcePointer, NULL);
        glCompileShader(geoShaderID);

        // Check Vertex Shader
        glGetShaderiv(geoShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(geoShaderID, GL_INFO_LOG_LENGTH, &infoLogLen);
        std::vector<char> geoShaderErrMsgs(infoLogLen);
        glGetShaderInfoLog(geoShaderID, infoLogLen, NULL, &geoShaderErrMsgs[0]);
        fprintf(stdout, "%s\n", &geoShaderErrMsgs[0]);

        glAttachShader(_id, geoShaderID);

        if(geoShaderErrMsgs.size() > 1)
        {
            return -1;
        }
        return 0;
    }

    GLuint Shader::getUniformLocation(const char *uniformName)
    {
        return glGetUniformLocation(this->_id, uniformName);
    }
}
