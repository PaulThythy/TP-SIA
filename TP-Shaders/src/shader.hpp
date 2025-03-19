#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL/glew.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

#include <stdlib.h>
#include <string.h>

inline GLuint LoadShaders(const char *vertex_file_path, const char *fragment_file_path)
{

    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if (VertexShaderStream.is_open())
    {
        std::string Line = "";
        while (getline(VertexShaderStream, Line))
            VertexShaderCode += "\n" + Line;
        VertexShaderStream.close();
    }
    else
    {
        printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
        getchar();
        return 0;
    }

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if (FragmentShaderStream.is_open())
    {
        std::string Line = "";
        while (getline(FragmentShaderStream, Line))
            FragmentShaderCode += "\n" + Line;
        FragmentShaderStream.close();
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    printf("Compiling shader : %s\n", vertex_file_path);
    char const *VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0)
    {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }

    // Compile Fragment Shader
    printf("Compiling shader : %s\n", fragment_file_path);
    char const *FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0)
    {
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        printf("%s\n", &FragmentShaderErrorMessage[0]);
    }

    // Link the program
    printf("Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0)
    {
        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}

enum class ShaderID
{
    PHONG,
    PHONG_TEX,
    TOON,
    TOON_OUTLINES,
    TOON_OUTLINES_MULTIPASS,
    SHADOW_MAP,
    GOOCH,
    GOOCH_OUTLINES_MULTIPASS,
    ENV_MAP
};

struct ShaderPassPaths {
    std::string vs; // vertex shader path
    std::string fs; // fragment shader path
};

struct ShaderDefinition {
    ShaderID m_id;
    bool m_multipass;
    ShaderPassPaths m_pass1Paths;
    ShaderPassPaths m_pass2Paths;
    std::string m_shaderName;

    GLuint programID1 = 0;
    GLuint programID2 = 0;

    ShaderDefinition(ShaderID _id,
        const ShaderPassPaths _pass1Paths,
        const std::string _shaderName
    ) : m_id(_id), m_multipass(false), m_pass1Paths(_pass1Paths), m_shaderName(_shaderName) {}

    ShaderDefinition(ShaderID _id,
        const ShaderPassPaths _pass1Paths,
        const ShaderPassPaths _pass2Paths,
        const std::string _shaderName
    ) : m_id(_id), m_multipass(true), m_pass1Paths(_pass1Paths), m_pass2Paths(_pass2Paths), m_shaderName(_shaderName) {}
};

static std::vector<ShaderDefinition> g_allShaders = {
    ShaderDefinition(
        ShaderID::PHONG,
        { "shaders/phong/vertex.vert", "shaders/phong/fragment.frag" },
        "Phong shader"
    ),

    ShaderDefinition(
        ShaderID::PHONG_TEX,
        { "shaders/phongWithTexture/vertex.vert", "shaders/phongWithTexture/fragment.frag" },
        "Phong shader with a texture"
    ),

    ShaderDefinition(
        ShaderID::TOON,
        { "shaders/toon/vertex.vert", "shaders/toon/fragment.frag" },
        "Toon shader"
    ),

    ShaderDefinition(
        ShaderID::TOON_OUTLINES,
        { "shaders/toonOutlines/vertex.vert", "shaders/toonOutlines/fragment.frag" },
        "Toon shader with outlines (single pass)"
    ),

    ShaderDefinition(
        ShaderID::TOON_OUTLINES_MULTIPASS,
        { "shaders/toonOutlinesMultipass/vertex_pass1.vert", "shaders/toonOutlinesMultipass/fragment_pass1.frag" },
        { "shaders/toonOutlinesMultipass/vertex_pass2.vert", "shaders/toonOutlinesMultipass/fragment_pass2.frag" },
        "Toon shader with outlines (multipass)"
    ),

    ShaderDefinition(
        ShaderID::SHADOW_MAP,
        { "shaders/shadowMap/vertex_pass1.vert", "shaders/shadowMap/fragment_pass1.frag" },
        {"shaders/shadowMap/vertex_pass2.vert", "shaders/shadowMap/fragment_pass2.frag"},
        "Shadow Map shader (multipass)"
    ),

    ShaderDefinition(
        ShaderID::GOOCH,
        { "shaders/gooch/vertex.vert", "shaders/gooch/fragment.frag" },
        "Gooch shader (single pass)"
    ),

    ShaderDefinition(
        ShaderID::GOOCH_OUTLINES_MULTIPASS,
        { "shaders/goochOutlinesMultipass/vertex_pass1.vert", "shaders/goochOutlinesMultipass/fragment_pass1.frag" },
        { "shaders/goochOutlinesMultipass/vertex_pass2.vert", "shaders/goochOutlinesMultipass/fragment_pass2.frag" },
        "Gooch shader (multipass)"
    ),

    ShaderDefinition(
        ShaderID::ENV_MAP,
        { "shaders/envMap/vertex.vert", "shaders/envMap/fragment.frag" }, 
        "Environment map (single pass)"
    )
};

#endif
