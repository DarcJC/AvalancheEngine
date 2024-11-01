#pragma once

#include "filesystem/filesystem.h"


namespace avalanche {
    class SystemFilesystem : public IFilesystem {
    public:
        FileSystemResult initialize() override;

        AVALANCHE_NO_DISCARD bool is_initialized() const override;

        AVALANCHE_NO_DISCARD bool is_read_only() const override;

        file_t open_file(path_t file_path) override;

        FileType get_path_type(path_t file_path) override;

        vector<string> list_directory(path_t file_path) override;
    };

    class ISystemFile : public IFile {
    };
} // namespace avalanche
