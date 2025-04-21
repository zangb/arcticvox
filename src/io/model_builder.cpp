#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <glm/vec3.hpp>
#include <spdlog/spdlog.h>

#include "arcticvox/io/filesystem.hpp"
#include "arcticvox/io/model_builder.hpp"

namespace arcticvox::io {
bool model_builder::load_model(const std::filesystem::path& path) {
    std::filesystem::path working_path;
    if(path.is_absolute())
        working_path = path;
    else
        working_path = get_current_exe_path() / path;

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path.string(), 0U);

    if(!scene || !scene->HasMeshes()) {
        spdlog::warn("Scene {} has no meshes", path.string());
        return false;
    }

    for(std::size_t mesh_i = 0U; mesh_i < scene->mNumMeshes; ++mesh_i) {
        const aiMesh* mesh = scene->mMeshes[mesh_i];
        for(std::size_t vtx_i = 0U; vtx_i < mesh->mNumVertices; ++vtx_i) {
            components::vertex vtx {};
            if(mesh->HasPositions()) {
                vtx.position.pos = glm::vec3 {
                    mesh->mVertices[vtx_i].x, mesh->mVertices[vtx_i].y, mesh->mVertices[vtx_i].z};
            }

            if(mesh->HasNormals()) {
                vtx.normal = glm::vec3 {
                    mesh->mNormals[vtx_i].x, mesh->mNormals[vtx_i].y, mesh->mNormals[vtx_i].z};
            }

            if(mesh->HasVertexColors(0)) {
                vtx.colour.col = glm::vec4 {mesh->mColors[0][vtx_i].r,
                                            mesh->mColors[0][vtx_i].g,
                                            mesh->mColors[0][vtx_i].b,
                                            1.0};
            }

            if(mesh->HasTextureCoords(0U)) {
                vtx.uv.uv = glm::vec3 {mesh->mTextureCoords[0][vtx_i].x,
                                       mesh->mTextureCoords[0][vtx_i].y,
                                       mesh->mTextureCoords[0][vtx_i].z};
            }
            vertices.push_back(vtx);
        }
    }

    return true;
}
}
