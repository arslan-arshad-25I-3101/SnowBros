// TileMap.h
#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

using namespace std;
using namespace sf;

class TileMap : public Drawable, public Transformable
{
public:
    TileMap();
    ~TileMap();

    // Load tileset image. tileWidth/tileHeight default to 32.
    bool LoadTileset(const string& tilesetPath, unsigned int tileWidth = 32, unsigned int tileHeight = 32);

    // Load map from CSV file. Each row is comma-separated tile indices (0-based).
    bool LoadFromCSV(const string& csvPath);

    unsigned int GetWidth() const;
    unsigned int GetHeight() const;
    unsigned int GetTileWidth() const;
    unsigned int GetTileHeight() const;

    int GetTile(unsigned int x, unsigned int y) const; // returns -1 if out of range

    void Clear();

private:
    virtual void draw(RenderTarget& target, RenderStates states) const override;

    unsigned int width = 0;
    unsigned int height = 0;
    unsigned int tileWidth = 32;
    unsigned int tileHeight = 32;
    vector<int> tiles; // row-major indices

    Texture tileset;
    VertexArray vertices; // quads for drawing
};
