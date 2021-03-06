/******************************************
 * COPYRIGHT (C) 2014 MADS HAUPT CLAUSEN
 ******************************************/

/**
 * @file Contains definition of the Shader class.
 * @author Mads Clausen
 */

#ifndef SHADER_INCLUDED
#define SHADER_INCLUDED

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#include <GL/glew.h>

namespace graphics
{
    class Shader
    {
        private:
            char *_vsPath, *_fsPath, *_gsPath, compiled;
            GLuint _id;

        public:
            /************************
             * Default constructor.
             ************************/
            Shader()
            {
                _vsPath = (char*) "";
                _fsPath = (char*) "";
                _id = compiled = 0;
            }

            /****************************************************
             * Constructor.
             *
             * @param v The filepath to the vertex shader.
             * @param f The filepath to the fragment shader.
             ****************************************************/
            Shader(const char *v, const char *f)
            {
                _vsPath = (char *) v;
                _fsPath = (char *) f;
                _id = compiled = 0;
            }

            void prepare()
            {
                _id = glCreateProgram();
            }


            /***********************************************************
             * Compile the shader from source code located in files.
             ***********************************************************/
            char compile();


            /**********************************************************
             * Compile the shader from source code in strings.
             *
             * @param vertSourcePointer The vertex shader source.
             * @param fragSourcePointer The fragment shader source.
             **********************************************************/
            char compile(const char *vertSourcePointer, const char *fragSourcePointer);

            char addGeometryShader(const char *geoSourcePointer);


            /************************************************
             * Returns the path to the vertex shader.
             *
             * @return The filepath to the vertex shader.
             ************************************************/
            const char *getVertexPath()
            {
                return (const char*) _vsPath;
            }


            /************************************************
             * Returns the path to the fragment shader.
             *
             * @return The filepath to the fragment shader.
             ************************************************/
            const char *getFragmentPath()
            {
                return (const char *) _fsPath;
            }


            /******************************************
             * Returns the ID of the OpenGL shader.
             *
             * @return The ID.
             ******************************************/
            const GLuint getID()
            {
                return (const GLuint) _id;
            }

            GLuint getUniformLocation(const char *uniformName);


            /*********************
             * Uses the shader.
             *********************/
            void bind()
            {
                if(_id != 0)
                    glUseProgram(_id);
            }


            /**************************
             * "Unbinds" the shader.
             **************************/
            void unbind()
            {
                glUseProgram(0);
            }

            char getIsCompiled()
            {
                return compiled;
            }
    };
}

#endif // SHADER_INCLUDED
