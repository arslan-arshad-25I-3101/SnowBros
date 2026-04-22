#include "TileMap.h"
#include <fstream>
#include <sstream>

using namespace std;
using namespace sf;

TileMap::TileMap()
{
    vertices.setPrimitiveType(Quads);
}

TileMap::~TileMap()
{
}

bool TileMap::LoadTileset(const string& tilesetPath, unsigned int tWidth, unsigned int tHeight)
{
    if (!tileset.loadFromFile(tilesetPath))
        return false;

    tileWidth = tWidth;
    tileHeight = tHeight;
    return true;
}

bool TileMap::LoadFromCSV(const string& csvPath)
{
    ifstream file(csvPath);
    if (!file.is_open())
        return false;

    vector<vector<int>> rows;
    string line;
    unsigned int cols = 0;

    while (std::getline(file, line)) {
        if (line.empty())
            continue;
        vector<int> row;
        stringstream ss(line);
        string cell;
        while (std::getline(ss, cell, ',')) {
            int idx = 0;
            try { idx = stoi(cell); } catch (...) { idx = -1; }
            row.push_back(idx);
        }
        if (row.size() > cols) cols = (unsigned int)row.size();
        rows.push_back(row);
    }

    height = (unsigned int)rows.size();
    width = cols;
    tiles.assign(width * height, -1);

    for (unsigned int y = 0; y < height; ++y) {
        for (unsigned int x = 0; x < width; ++x) {
            if (x < rows[y].size())
                tiles[y * width + x] = rows[y][x];
            else
                tiles[y * width + x] = -1;
        }
    }

    // Build vertex array
    unsigned int tu = tileset.getSize().x / tileWidth;
    unsigned int tv = tileset.getSize().y / tileHeight;

    vertices.clear();
    vertices.resize(width * height * 4);

    for (unsigned int y = 0; y < height; ++y) {
        for (unsigned int x = 0; x < width; ++x) {
            int tileNumber = tiles[y * width + x];

            Vertex* quad = &vertices[(x + y * width) * 4];

            quad[0].position = Vector2f(x * tileWidth, y * tileHeight);
            quad[1].position = Vector2f((x + 1) * tileWidth, y * tileHeight);
            quad[2].position = Vector2f((x + 1) * tileWidth, (y + 1) * tileHeight);
            quad[3].position = Vector2f(x * tileWidth, (y + 1) * tileHeight);

            if (tileNumber < 0) {
                // transparent quad
                quad[0].texCoords = Vector2f(0, 0);
                quad[1].texCoords = Vector2f(0, 0);
                quad[2].texCoords = Vector2f(0, 0);
                quad[3].texCoords = Vector2f(0, 0);
            } else {
                unsigned int tuIndex = (unsigned int)tileNumber % tu;
                unsigned int tvIndex = (unsigned int)tileNumber / tu;

                quad[0].texCoords = Vector2f(tuIndex * tileWidth, tvIndex * tileHeight);
                quad[1].texCoords = Vector2f((tuIndex + 1) * tileWidth, tvIndex * tileHeight);
                quad[2].texCoords = Vector2f((tuIndex + 1) * tileWidth, (tvIndex + 1) * tileHeight);
                quad[3].texCoords = Vector2f(tuIndex * tileWidth, (tvIndex + 1) * tileHeight);
            }
        }
    }

    return true;
}

unsigned int TileMap::GetWidth() const { return width; }
unsigned int TileMap::GetHeight() const { return height; }
unsigned int TileMap::GetTileWidth() const { return tileWidth; }
unsigned int TileMap::GetTileHeight() const { return tileHeight; }

int TileMap::GetTile(unsigned int x, unsigned int y) const
{
    if (x >= width || y >= height) return -1;
    return tiles[y * width + x];
}

void TileMap::Clear()
{
    width = height = 0;
    tiles.clear();
    vertices.clear();
}

void TileMap::draw(RenderTarget& target, RenderStates states) const
{
    if (tileset.getSize().x == 0 || tileset.getSize().y == 0)
        return; // nothing to draw

    states.transform *= getTransform();
    states.texture = &tileset;
    target.draw(vertices, states);
}
