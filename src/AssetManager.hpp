#pragma once
#include "logging.hpp"

#include <filesystem>
#include <fstream>


class ImageAsset {
public:
    ImageAsset(const char* filename, const char* assetName);
    ~ImageAsset();

    unsigned char* get() { return buf; }
    unsigned int get_width() { return width; }
    unsigned int get_height() { return height; }

private:
    void read_png(const std::ifstream& file);

    const char* assetName;
    unsigned int width;
    unsigned int height;

    unsigned char* buf = nullptr;

    Logger logger = Logger("assets");
};

class ShaderAsset {
public:
    ShaderAsset(const char* filename, const char* assetName);
    ~ShaderAsset();

    char* get() const { return buf; }
    size_t get_size() const { return bufSize; }

private:
    const char* assetName;
    size_t bufSize = 0;
    char* buf = nullptr;

    Logger logger = Logger("assets");
};


class AssetManager {
public:
    AssetManager(const char* baseDir = nullptr);
    ~AssetManager() {}

    ImageAsset get_image(const char* name) const;
    ShaderAsset get_shader(const char* name) const;

private:
    std::filesystem::path proc_dir();

    std::filesystem::path dir;
    Logger logger = Logger("assets");
};
