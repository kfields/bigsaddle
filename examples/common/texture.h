#pragma once

#include <filesystem>

#include <pugixml.hpp>

struct TexCoord {
    TexCoord(float _u, float _v) : u(_u), v(_v) {}
    float u;
    float v;
};

struct Texture {
    Texture() {}
    Texture(Texture* parent, int _id, std::string _name, int _x, int _y, int _width, int _height) :
        id(_id), name(_name), texture(parent->texture), x(_x), y(_y), width(_width), height(_height) {
        float pWidth = float(parent->width);
        float pHeight = float(parent->height);
        coords[0] = TexCoord(x / pWidth, (y + height) / pHeight);
        coords[1] = TexCoord((x + width) / pWidth, (y + height) / pHeight);
        coords[2] = TexCoord(x / pWidth, y / pHeight);
        coords[3] = TexCoord((x + width) / pWidth, y / pHeight);
    }
    Texture Mirror() {
        Texture texture(*this);
        texture.coords[0].u = coords[1].u;
        texture.coords[1].u = coords[0].u;
        texture.coords[2].u = coords[3].u;
        texture.coords[3].u = coords[2].u;
        return texture;
    }
    void Load(const std::filesystem::path& _path) {
        bgfx::TextureInfo info = bgfx::TextureInfo();

        texture = loadTexture(_path.string().c_str(), BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT, 0, &info);
        if (!bgfx::isValid(texture)) {
            //TODO: throw
        }

        width = info.width;
        height = info.height;
        name = _path.string();
    }
    int id;
    std::string name;
    bgfx::TextureHandle texture;
    int x;
    int y;
    int width;
    int height;
    TexCoord coords[4] = {
        {0.0f,  1.0f},
        {1.0f,  1.0f},
        {0.0f, 0.0f},
        {1.0f, 0.0f}
    };

};

class TextureManager {
public:
    int LoadSpriteSheet(const std::filesystem::path& _path) {
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_file(_path.c_str());
        if (!result)
            return -1;
        
        pugi::xml_node atlas = doc.child("TextureAtlas");
        const char* imagePath = atlas.attribute("imagePath").as_string();

        Texture* texture = new Texture();
        texture->Load(_path.parent_path() / imagePath);

        for (pugi::xml_node node: atlas.children("SubTexture"))
        {
            //<SubTexture name="beam0.png" x="143" y="377" width="43" height="31"/>
            std::string name = node.attribute("name").as_string();
            int x = node.attribute("x").as_int();
            int y = node.attribute("y").as_int();
            int width = node.attribute("width").as_int();
            int height = node.attribute("height").as_int();
            //std::cout << name << x << y << width << height << "\n";
            Texture* subTex = new Texture(texture, int(textures_.size()), name, x, y, width, height);
            textures_[name] = subTex;
        }
        return 0;
    }
    Texture* GetTexture(const std::string& _name) {
        return textures_[_name];
    }
    std::vector<const char*> GetNames() {
        std::vector<const char*> names;
        for (std::map<std::string, Texture*>::iterator it = textures_.begin(); it != textures_.end(); ++it) {
            names.push_back(it->first.c_str());
        }
        return names;
    }
    int GetNameIndex(const std::string& _name) {
        return int(std::distance(textures_.begin(), textures_.find(_name)));
    }
    // Data members
    std::map<std::string, Texture*> textures_;
};
