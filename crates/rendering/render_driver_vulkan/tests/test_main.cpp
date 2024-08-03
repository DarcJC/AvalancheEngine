//
// Created by DarcJC on 8/3/2024.
//
#include "render_device.h"
#include "vulkan_context.h"


int main(int argc, char* argv[]) {

    using namespace avalanche::rendering;

    DeviceSettings settings{};

    vulkan::Context context(settings);

    return 0;
}
