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

    Logger logger;
};

class ShaderAsset {
public:
    ShaderAsset(const char* filename, const char* assetName);
    ~ShaderAsset();

    char* get() { return buf; }

private:
    const char* assetName;
    char* buf = nullptr;

    Logger logger;
};


class AssetManager {
public:
    AssetManager(const char* baseDir = nullptr);
    ~AssetManager() {}

    ImageAsset get_image(const char* name);
    ShaderAsset get_shader(const char* name);

private:
    std::filesystem::path proc_dir();

    std::filesystem::path dir;
    Logger logger;
};
