#pragma once

#include <filesystem>

#include <fmt/core.h>
#include <pugixml.hpp>

#include "texture.h"

struct Tile : Texture {
    Tile() : Texture() {}
    Tile(Texture* parent, int _id, std::string _name, int _x, int _y, int _width, int _height) :
        Texture(parent, _id, _name, _x, _y, _width, _height) {}
    //Data members
};

class Tileset {
public:
    int Load(const std::filesystem::path& _path) {
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_file(_path.c_str());
        if (!result)
            return -1;
        
        pugi::xml_node tsNode = doc.child("tileset");
        name_ = tsNode.attribute("name").as_string();
        tilewidth_ = tsNode.attribute("tilewidth").as_int();
        tileheight_ = tsNode.attribute("tileheight").as_int();
        tilecount_ = tsNode.attribute("tilecount").as_int();
        columns_ = tsNode.attribute("columns").as_int();
        margin_ = tsNode.attribute("margin").as_int();
        spacing_ = tsNode.attribute("spacing").as_int();

        //<image source="tiles.png" width="1536" height="1664"/>
        pugi::xml_node imNode = tsNode.child("image");
        const char* imPath = imNode.attribute("source").as_string();
        int imWidth = imNode.attribute("width").as_int();
        int imHeight = imNode.attribute("height").as_int();

        rows_ = imHeight / tileheight_;

        Texture* texture = new Texture();
        texture->Load(_path.parent_path() / imPath);

        for (pugi::xml_node node: tsNode.children("tile"))
        {
            std::string name = node.attribute("id").as_string();
            int id = node.attribute("id").as_int();
            int row = id / columns_;
            int col = id % columns_;
            int x = col * tilewidth_;
            int y = row * tileheight_;
            int width = tilewidth_;
            int height = tileheight_;
            Tile tile(texture, id, name, x, y, width, height);
            //textures_[name] = subTex;
            tiles_.push_back(tile);
        }
        return 0;
    }
    /*Texture* GetTexture(const std::string& _name) {
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
        return std::distance(textures_.begin(), textures_.find(_name));
    }*/
    std::vector<const char*> GetNames() {
        std::vector<const char*> names;
        for (Tile& tile : tiles_) {
            names.push_back(tile.name.c_str());
        }
        return names;
    }
    Tile* GetTile(int id) {
        return &tiles_[id];
    }
    // Data members
    std::vector<Tile> tiles_;
    //std::map<std::string, Texture*> textures_;
    std::string name_;
    int tilewidth_;
    int tileheight_;
    int tilecount_;
    int columns_;
    int rows_;
    int margin_;
    int spacing_;

};
