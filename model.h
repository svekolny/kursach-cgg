#pragma once

#define _USE_MATH_DEFINES

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <vector>

// ======================
// Структуры данных
// ======================

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture {
    unsigned int id;
    std::string type;   // "texture_diffuse", "texture_specular"
    std::string path;
};

// ======================
// Mesh
// ======================

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;

    Mesh(
        const std::vector<Vertex>& v,
        const std::vector<unsigned int>& i,
        const std::vector<Texture>& t
    );

    void Draw(unsigned int shaderID);

private:
    void setupMesh();
};

// ======================
// Model
// ======================

class Model {
public:
    std::vector<Mesh> meshes;
    std::string directory;

    explicit Model(const std::string& path);

    void Draw(unsigned int shaderID);

private:
    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

    std::vector<Texture> loadMaterialTextures(
        aiMaterial* mat,
        aiTextureType type,
        const std::string& typeName
    );
};

// ======================
// Загрузка текстур
// ======================

unsigned int TextureFromFile(
    const char* path,
    const std::string& directory
);
