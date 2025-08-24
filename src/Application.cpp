#include "Application.hpp"
#include "AssetManager.hpp"

#include <exception>
#include <iostream>
#include <cstdlib>


class Application {
public:
    Application();
    void run() {}
    ~Application() {}

private:
    AssetManager assets;
};

Application::Application()
  : assets(app::workDir)
{
    log_init(app::logMode, app::logLevel);
}


int main()
{
    try
    {
        Application app = Application();
        app.run();
    }
    catch (const std::exception& exception)
    {
        std::cerr << exception.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (...)
    {
        std::cerr << "### An unknown error has occured!! ###" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
