#include "filesystem/system_filesystem.h"
#include "container/exception.hpp"

#include <fstream>
#include <filesystem>


namespace avalanche {

    FileSystemResult SystemFilesystem::initialize() {
        return FileSystemResult::Success;
    }

    bool SystemFilesystem::is_initialized() const {
        return true;
    }

    bool SystemFilesystem::is_read_only() const { return false; }

    file_t SystemFilesystem::open_file(path_t file_path) {
        return file_t { nullptr };
    }

} // namespace avalanche
