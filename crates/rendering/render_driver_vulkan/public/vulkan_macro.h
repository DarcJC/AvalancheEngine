#pragma once

#include "logger.h"

#ifndef AVALANCHE_CHECK_VK_SUCCESS
#   define AVALANCHE_CHECK_VK_SUCCESS(res) AVALANCHE_CHECK_RUNTIME(res == VK_SUCCESS, "There is an error while invoking Vulkan API")
#endif
