#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <glm/gtc/quaternion.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


class Helpers
{
public:
	static inline glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from)
	{
		return glm::transpose(glm::make_mat4(&from.a1));
	}

	
	static inline glm::vec3 GetGLMVec(const aiVector3D& vec)
	{
		return glm::vec3(vec.x, vec.y, vec.z);
	}


	static inline glm::quat GetGLMQuat(const aiQuaternion& pOrientation)
	{
		return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
	}


	static void LogMatrix(const glm::mat4& matrix, const std::string& matrixName = "Matrix") {
		// Log matrix name
		INFO_LOG("Logging matrix: {}", matrixName);

		// Log each row of the matrix
		for (int row = 0; row < 4; ++row) {
			std::ostringstream rowStream;
			rowStream << "[ ";
			for (int col = 0; col < 4; ++col) {
				rowStream << matrix[row][col];
				if (col < 3) {
					rowStream << ", ";
				}
			}
			rowStream << " ]";
			INFO_LOG("{} - Row {}: {}", matrixName, row, rowStream.str());
		}
	}
};
