#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


class Helpers
{
public:
	static inline glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from)
	{
		glm::mat4 to;
		//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
		to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
		to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
		to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
		to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
		return to;
	}

	static inline glm::vec3 GetGLMVector(const aiVector3D& vec) {
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
