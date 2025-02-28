#include "pch.h"
#include "Shader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "Platforms/OpenGL/OpenGLTexture.h"

Shader::Shader(const std::string& vertexSrc, const std::string& fragmentSrc,  std::string shaderName)
{
	m_ShaderName = shaderName;

	// Create empty vertex shader handle
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	// Send the shaders source code to GL
	const GLchar* source = vertexSrc.c_str();
	glShaderSource(vertexShader, 1, &source, 0);

	// Compile the shader code
	glCompileShader(vertexShader);

	GLint isCompiled = 0;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

		// We don't need the shader anymore.
		glDeleteShader(vertexShader);

		ERROR("{0}", infoLog.data());
		ASSERT(false, "Vertex Shader compilation failure.");
		return;
	}

	// Create an empty fragment shader handle
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// Send the fragment shader source code to GL
	// Note that std::string's .c_str is NULL character terminated.
	source = fragmentSrc.c_str();
	glShaderSource(fragmentShader, 1, &source, 0);

	// Compile the fragment shader
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

		// We don't need the shader anymore.
		glDeleteShader(fragmentShader);
		// Either of them. Don't leak shaders.
		glDeleteShader(vertexShader);

		// Use the infoLog as you see fit.
		ERROR("{0}", infoLog.data());
		TRACE_LOG(infoLog.data());
		ASSERT(false, "Fragment Shader compilation failure.");

		// In this simple program, we'll just leave
		return;
	}

	// Vertex and fragment shaders are successfully compiled.
	// Now time to link them together into a program.
	// Get a program object. - copy to our id
	m_RendererID = glCreateProgram();
	GLuint program = m_RendererID;

	// Attach our shaders to our program
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	// Link our program
	glLinkProgram(program);

	// Note the different functions here: glGetProgram* instead of glGetShader*.
	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

		// We don't need the program anymore.
		glDeleteProgram(program);
		// Don't leak shaders either.
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		// Use the infoLog as you see fit.
		ERROR("{0}", infoLog.data());
		ASSERT(false, "Shader link failure.");
		// In this simple program, we'll just leave
		return;
	}

	// Always detach shaders after a successful link.
	glDetachShader(program, vertexShader);
	glDetachShader(program, fragmentShader);

}

Shader::~Shader()
{
	glDeleteProgram(m_RendererID);
}

void Shader::Bind() const
{
	glUseProgram(m_RendererID);
}

void Shader::Unbind() const
{
	glUseProgram(0);
}

void Shader::UploadUniformFloat(const std::string& name, float value)
{
	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location == -1)
	{
		ERROR_LOG("Uniform {0} not found in shader.", name);
		return;
	}

	glUniform1f(location, value);  
}

void Shader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
{
	// Upload mat4 into shader
	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location == -1)
	{
		ERROR_LOG("Uniform {0} not found in shader.", name);
		return;
	}

	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::UploadUniformFloat3(const std::string& name, const glm::vec3& vec)
{
	// Get the uniform location in the shader
	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location == -1)
	{
		ERROR_LOG("Uniform {0} not found in shader.", name);
		return;
	}

	// Upload the float vector to the shader
	glUniform3fv(location, 1, glm::value_ptr(vec));
}

void Shader::UploadUniformMat4Array(const std::string& name, const std::vector<glm::mat4>& matrices)
{
	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location == -1)
	{
		ERROR_LOG("Uniform {0} not found in shader.", name);
		return;
	}
	glUniformMatrix4fv(location, matrices.size(), GL_FALSE, glm::value_ptr(matrices[0]));

}




uint32_t Shader::GetUniformLocation(const std::string& name) const
{
	return glGetUniformLocation(m_RendererID, name.c_str());
}

std::shared_ptr<Shader> Shader::CreateFromFile(const std::string& vertexPath, const std::string& fragmentPath, std::string shaderName)
{
	// Read file function for creating
	auto readFile = [](const char* fileLocation) -> std::string {
		std::string content;
		std::ifstream fileStream(fileLocation, std::ios::in);

		if (!fileStream.is_open())
		{
			ERROR_LOG("Failed to read {0}! File doesn't exist.", fileLocation);
			return "";
		}

		std::string line = "";
		while (!fileStream.eof()) // Whilst not the end of file
		{
			std::getline(fileStream, line);
			content.append(line + "\n");
		}

		fileStream.close();
		return content;

		};

	// File Strings
	std::string vertexString = readFile(vertexPath.c_str());

	std::string fragmentString = readFile(fragmentPath.c_str());
	//std::string geometryString = ""; Currently not supported
	//if (gShaderFile != nullptr) geometryString = readFile(gShaderFile);

	


	return  std::make_shared<Shader>(vertexString, fragmentString, shaderName);
}
