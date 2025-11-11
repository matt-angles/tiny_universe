#include "AssetManager.hpp"
#include "Application.hpp"

#include <png.h>
#include <cstddef>

#if defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
#   include <unistd.h>
#endif
#if defined(PLATFORM_WINDOWS)
#   include <windows.h>
#endif


constexpr size_t maxPathLength = 175;


static void pngcall_io_read(png_structp pngFile, 
                            png_bytep buf, png_size_t size)
{
    auto file = (std::ifstream*) (png_get_io_ptr(pngFile));
    try
    {
        file->read((char*) buf, size);
        if (!file->good())
            png_error(pngFile, "error reading PNG data");
    } 
    catch (const std::exception& e)
    {
        png_error(pngFile, e.what());
    }
}

static void pngcall_debug_warn(png_structp pngFile,
                               png_const_charp warnMsg)
{
    (void) pngFile;
    spdlog::warn("lpng: {}", warnMsg);
}

static void pngcall_debug_error(png_structp pngFile,
                                png_const_charp errorMsg)
{
    (void) pngFile;
    throw std::runtime_error("lpng: " + std::string(errorMsg));
}

std::filesystem::path AssetManager::proc_dir()
{
#   if defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
        char pathBuf[maxPathLength+1];
        ssize_t copied = readlink("/proc/self/exe", pathBuf, maxPathLength+1);
        if (0 < copied && copied < (ssize_t) maxPathLength+1)
        {
            pathBuf[copied+1] = '\0';  // readlink doesn't null-terminate the string
            return std::filesystem::path(pathBuf).parent_path();
        }

        if (copied == maxPathLength)
            logger.error("could not retrieve the process directory - path exceeds maximum length");
        else
            logger.error("could not retrieve the process directory - unknown error {}", copied);

#   elif defined(PLATFORM_WINDOWS)
        wchar_t pathBuf[maxPathLength+1];
        size_t copied = GetModuleFileNameW(nullptr, pathBuf, maxPathLength+1);
        if (copied < maxPathLength)
            return std::filesystem::path(pathBuf).parent_path();

        if (copied == maxPathLength)
            logger.error("could not retrieve the process directory - path exceeds maximum length");
        else
            logger.error("could not retrieve the process directory - unknown error {}", copied);

#   endif
        logger.warn("using user-set current working directory");
        return std::filesystem::current_path();
}

AssetManager::AssetManager(const char* baseDir)
{
    if (!baseDir)
        dir = proc_dir();
    else
    {
        auto pathBaseDir = std::filesystem::path(baseDir);
        if (pathBaseDir.is_relative())
            pathBaseDir = proc_dir() / pathBaseDir;
        if (std::filesystem::exists(pathBaseDir) && std::filesystem::is_directory(pathBaseDir))
        {
            dir = std::filesystem::canonical(pathBaseDir);
        }
        else
        {
            logger.warn("ignoring invalid directory \"{}\"", baseDir);
            dir = proc_dir();
        }
    }
    logger.debug("setting up in path {}", dir.string());
}

ImageAsset AssetManager::get_image(const char* name) const
{
    auto path = dir / "assets" / "images" / name;
    if (!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path))
        throw std::runtime_error("assets: invalid image file \"" + path.string() + "\"");

    return ImageAsset(reinterpret_cast<const char*>(path.u8string().c_str()), name);
}

ShaderAsset AssetManager::get_shader(const char* name) const
{
    auto fullName = name + std::string(".spv");
    auto path = dir / "shaderc" / fullName;
    if (!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path))
        throw std::runtime_error("assets: invalid shader file \"" + path.string() + "\"");

    return ShaderAsset(reinterpret_cast<const char*>(path.u8string().c_str()), name);
}


ImageAsset::ImageAsset(const char* filename, const char* assetName)
  : assetName(assetName)
{
    auto file = std::ifstream(filename, std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("failed to open file \"" + std::string(filename) + "\"");
    logger.trace("opened file {}", filename);

    unsigned char header[8];
    file.read((char*) header, 8);
    if (png_sig_cmp(header, 0, 8) == 0)
        read_png(file);
    else
    {
        throw std::runtime_error("assets: unsupported image format");
    }
}

ImageAsset::~ImageAsset()
{
    logger.trace("unloaded asset {}", assetName);
    if (buf) delete[] buf;
}

void ImageAsset::read_png(const std::ifstream& file)
{
    png_structp pngFile = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, pngcall_debug_error, pngcall_debug_warn);
    if (!pngFile)
        throw std::runtime_error("assets: failed to read PNG file (" + std::string(assetName) + ")");
    png_set_read_fn(pngFile, (void*) &file, pngcall_io_read);
    png_infop pngInfo = png_create_info_struct(pngFile);
    if (!pngInfo)
    {
        png_destroy_read_struct(&pngFile, nullptr, nullptr);
        throw std::runtime_error("assets: failed to read PNG info (" + std::string(assetName) + ")");
    }
    png_infop pngEnd = png_create_info_struct(pngFile);
    if (!pngEnd)
    {
        png_destroy_read_struct(&pngFile, &pngInfo, nullptr);
        throw std::runtime_error("assets: failed to read PNG info (" + std::string(assetName) + ")");
    }

    png_set_sig_bytes(pngFile, 8);
    png_read_info(pngFile, pngInfo);
    png_byte pngBitDepth = png_get_bit_depth(pngFile, pngInfo);
    png_byte pngColorType = png_get_color_type(pngFile, pngInfo);

    // Transform into an R8G8B8A8_SRGB image
    png_set_expand(pngFile);
    if (pngBitDepth == 16)
        png_set_strip_16(pngFile);
    if (pngColorType == PNG_COLOR_TYPE_GRAY || 
        pngColorType == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(pngFile);
    if (pngColorType == PNG_COLOR_TYPE_GRAY ||
        pngColorType == PNG_COLOR_TYPE_RGB)
        png_set_add_alpha(pngFile, 0xFF, PNG_FILLER_AFTER);
    png_set_alpha_mode(pngFile, PNG_ALPHA_STANDARD, PNG_GAMMA_sRGB);
    png_read_update_info(pngFile, pngInfo);

    width = png_get_image_width(pngFile, pngInfo);
    height = png_get_image_height(pngFile, pngInfo);
    size_t rowbytes = png_get_rowbytes(pngFile, pngInfo);
    buf = new unsigned char[rowbytes*height];
    auto pngBuf = new png_bytep[height];    // pointer into buf lines - needed by libpng
    for (size_t i=0; i < height; i++)
        pngBuf[i] = buf + i*rowbytes;
    png_read_image(pngFile, pngBuf);

    png_read_end(pngFile, pngEnd);
    png_destroy_read_struct(&pngFile, &pngInfo, &pngEnd);
    logger.trace("loaded asset {}", assetName);
}


ShaderAsset::ShaderAsset(const char* filename, const char* assetName)
  : assetName(assetName)
{
    auto file = std::ifstream(filename, std::ios::ate|std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("failed to open file \"" + std::string(filename) + "\"");
    logger.trace("opened file {}", filename);

    bufSize = file.tellg(); file.seekg(0);
    buf = new char[bufSize];

    file.read(buf, bufSize);
    if (!file.good())
        throw std::runtime_error("failed to load asset " + std::string(assetName));
    logger.trace("loaded asset " + std::string(assetName));
}

ShaderAsset::~ShaderAsset()
{
    logger.trace("unloading asset " + std::string(assetName));
    delete[] buf;
}
