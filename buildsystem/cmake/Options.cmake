cmake_dependent_option(AVALANCHE_ENABLE_DX12 "Enable DX12 features for RHI" ON "WIN32" OFF)
option(AVALANCHE_ENABLE_VULKAN "Enable Vulkan features for RHI" ON)

# render_driver_vulkan
option(AVALANCHE_ENABLE_VULKAN_DRIVER_TESTS "Enable vulkan driver tests" OFF)
