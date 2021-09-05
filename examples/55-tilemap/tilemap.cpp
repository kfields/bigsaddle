#include <iostream>
#include <map>
#include <vector>
#include <filesystem>
#include <sstream>

#include <pugixml.hpp>

#include <imgui/imgui.h>
#include <bx/math.h>
#include <bgfx/utils/utils.h>

#include <examples/common.h>

class TileLayer;

class TileMap {
public:
    int Load(const std::filesystem::path& _path);
    void Draw(uint8_t _view);
    // Data members
    std::string version_;
    std::string tiledversion_;
    std::string orientation_;
    std::string renderorder_;
    int width_;
    int height_;
    int tilewidth_;
    int tileheight_;
    bool infinite_;
    int nextlayerid_;
    int nextobjectid_;
    //
    std::vector<Tile> tiles_;
    std::vector<Tileset*> tilesets_;
    std::vector<TileLayer*> layers_;
};

class TileLayer {
public:
    TileLayer(TileMap* map) : map_(map) {}
    void Load(const pugi::xml_node& layerNode) {
        //<layer id="1" name="ground" width="100" height="10">
        id_ = layerNode.attribute("id").as_int();
        name_ = layerNode.attribute("name").as_string();
        width_ = layerNode.attribute("width").as_int();
        height_ = layerNode.attribute("height").as_int();
        //<data encoding="csv">
        pugi::xml_node dataNode = layerNode.child("data");
        std::string encoding = layerNode.attribute("encoding").as_string();
        std::string dataText = dataNode.text().as_string();
        std::istringstream iss(dataText);

        for (auto& csvRow : CSVRange(iss))
        {
            if (csvRow.m_line.size() == 0)
                continue;
            std::vector<int> row(width_);
            for (int i = 0; i < width_; ++i) {
                row[i] = std::stoi(csvRow[i].data());
            }
            data_.push_back(row);
        }
        int tilewidth = map_->tilewidth_;
        int tileheight = map_->tileheight_;
        for (int j = 0; j < height_; ++j) {
            for (int i = 0; i < width_; ++i) {
                int gid = data_[j][i];
                if (gid == 0)
                    continue;
                sprites_.push_back(Sprite::Produce(i * tilewidth, j * tileheight, map_->tiles_[gid]));
            }
        }
    }
    void Draw(uint8_t _view) {
        for (auto sprite : sprites_) {
            sprite->Draw(_view);
        }
    }
    // Data members
    TileMap* map_;
    int id_;
    std::string name_;
    int width_;
    int height_;

    std::vector< std::vector<int> > data_;
    std::vector<Sprite*> sprites_;
};

int TileMap::Load(const std::filesystem::path& _path) {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(_path.c_str());
    if (!result)
        return -1;
    //<map version="1.5" tiledversion="1.7.1" orientation="orthogonal" renderorder="right-down" width="100" height="10" tilewidth="128" tileheight="128" infinite="0" nextlayerid="13" nextobjectid="36">
    pugi::xml_node mapNode = doc.child("map");
    version_ = mapNode.attribute("version").as_string();
    tiledversion_ = mapNode.attribute("tiledversion").as_string();
    orientation_ = mapNode.attribute("orientation").as_string();
    renderorder_ = mapNode.attribute("renderorder").as_string();
    width_ = mapNode.attribute("width").as_int();
    height_ = mapNode.attribute("height").as_int();
    tilewidth_ = mapNode.attribute("tilewidth").as_int();
    tileheight_ = mapNode.attribute("tileheight").as_int();
    infinite_ = mapNode.attribute("tileheight").as_int() == 1;
    nextlayerid_ = mapNode.attribute("nextlayerid").as_int();
    nextobjectid_ = mapNode.attribute("nextobjectid").as_int();

    tiles_.push_back(Tile()); //tiles_[0] is blank
    //<tileset firstgid="1" source="tiles.tsx"/>
    for (pugi::xml_node tsNode : mapNode.children("tileset"))
    {
        int firstGid = tsNode.attribute("firstgid").as_int();
        const char* tsxPath = tsNode.attribute("source").as_string();

        Tileset* tileset = new Tileset();
        tileset->Load(_path.parent_path() / tsxPath);
        tilesets_.push_back(tileset);
        for (auto tile : tileset->tiles_) {
            tiles_.push_back(tile);
        }
    }
    for (pugi::xml_node layerNode : mapNode.children("layer"))
    {
        TileLayer* layer = new TileLayer(this);
        layer->Load(layerNode);
        layers_.push_back(layer);
    }
    return 0;
}

void TileMap::Draw(uint8_t _view) {
    for (auto layer : layers_) {
        layer->Draw(_view);
    }
}

using namespace bigsaddle;

class ExampleTileMap : public ExampleApp {
public:
    EXAMPLE_CTOR(ExampleTileMap)

    virtual void Create() override {
        ExampleApp::Create();

        tilemap_ = new TileMap();
        tilemap_->Load("platformer/ground.tmx");
    }

    virtual void Draw() override {
        ExampleApp::Draw();
        ShowExampleDialog();

        ImGui::SetNextWindowPos(
            ImVec2(width() - width() / 5.0f - 10.0f, 10.0f)
            , ImGuiCond_FirstUseEver
        );
        ImGui::SetNextWindowSize(
            ImVec2(width() / 5.0f, height() / 7.0f)
            , ImGuiCond_FirstUseEver
        );
        /*ImGui::Begin("Settings", NULL, 0);

        ImGui::SliderFloat("Scale", &sprite_->scale_, 0.25f, 4.0f);
        ImGui::SliderFloat("Angle", &sprite_->angle_, 0.0f, 360.0f);
        float color[4] = {
            sprite_->color_.r / 255.0f,
            sprite_->color_.g / 255.0f,
            sprite_->color_.b / 255.0f,
            sprite_->color_.a / 255.0f
        };
        if (ImGui::ColorEdit4("Color", color))
            sprite_->color_ = ColorRgba(color);

        std::vector<const char*> names = tileset_->GetNames();
        static int item_current = tileset_->GetNameIndex(sprite_->texture_.name);
        if (ImGui::ListBox("Textures", &item_current, names.data(), names.size(), 4)) {
            sprite_->SetTexture(tileset_->GetTexture(names[item_current]));
        }

        ImGui::End();*/

        /*
        float ortho[16];
        const bgfx::Caps* caps = bgfx::getCaps();
        bx::mtxOrtho(ortho, 0, width(), height(), 0, 0.0f, 1000.0f, 0.0f, caps->homogeneousDepth);
        bgfx::setViewTransform(viewId_, NULL, ortho);
        */
        float ortho[16];
        const bgfx::Caps* caps = bgfx::getCaps();
        bx::mtxOrtho(ortho, 0, width()*2, height()*2, 0, 0.0f, 1000.0f, 0.0f, caps->homogeneousDepth);
        bgfx::setViewTransform(viewId_, NULL, ortho);


        tilemap_->Draw(viewId_);
    }
    //Data members
    TileMap* tilemap_ = nullptr;
};

EXAMPLE_MAIN(
    ExampleTileMap
    , "55-tilemap"
    , "Render a TileMap."
    , "https://bkaradzic.github.io/bgfx/examples.html#helloworld"
);

