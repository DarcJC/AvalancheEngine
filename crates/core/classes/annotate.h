#pragma once

#pragma warning(disable: 5030)  // warning C5030: attribute is not recognized

#define ACLASS(...) [[avalanche::class_decl(__VA_ARGS__)]]
#define ASTRUCT(...) [[avalanche::struct_decl(__VA_ARGS__)]]
#define AENUM(...) [[avalanche::enum_decl(__VA_ARGS__)]]
