#include "filesystem/system_filesystem.h"
#include "container/exception.hpp"

#include <container/string.hpp>
#include <filesystem>
#include <fstream>

#include "cppfs/FileHandle.h"
#include "cppfs/fs.h"


namespace avalanche {

    class SystemFileImpl : public ISystemFile {
    public:
        explicit SystemFileImpl(path_t path)
            : m_path_(path)
            , m_file_handle_(cppfs::fs::open(std::string(path))) {
        }

        AVALANCHE_NO_DISCARD bool is_read_only() const override {
            return false;
        }

        AVALANCHE_NO_DISCARD bool is_valid() const override {
            return m_file_handle_.isFile();
        }

        AVALANCHE_NO_DISCARD string read(FileOpenMode open_mode) override {
            if (!is_valid()) {
                return "";
            }
            auto stream = m_file_handle_.createInputStream(static_cast<std::ios::openmode>(open_mode));
            return { m_file_handle_.readFile().data() };
        }

        AVALANCHE_NO_DISCARD FileSystemResult write(std::string_view content, FileOpenMode open_mode) override {
            if (!is_valid()) {
                return FileSystemResult::FileNotExist;
            }
            if (m_file_handle_.writeFile(std::string(content))) {
                return FileSystemResult::Success;
            }
            return FileSystemResult::InternalError;
        }

        string sha1() override {
            return { m_file_handle_.sha1().data() };
        }

    private:
        path_t m_path_{};
        cppfs::FileHandle m_file_handle_;
    };

    FileSystemResult SystemFilesystem::initialize() {
        return FileSystemResult::Success;
    }

    bool SystemFilesystem::is_initialized() const {
        return true;
    }

    bool SystemFilesystem::is_read_only() const { return false; }

    file_t SystemFilesystem::open_file(path_t file_path) {
        if (get_path_type(file_path) == FileType::File) {
            return file_t { new SystemFileImpl(file_path) };
        }
        return file_t{nullptr};
    }

    FileType SystemFilesystem::get_path_type(path_t file_path) {
        auto handle = cppfs::fs::open(std::string(file_path));
        if (!handle.exists()) {
            return FileType::NoneExist;
        } else if (handle.isFile()) {
            return FileType::File;
        } else if (handle.isDirectory()) {
            return FileType::Directory;
        }
        return FileType::Unknown;
    }

    vector<string> SystemFilesystem::list_directory(path_t file_path) {
        auto handle = cppfs::fs::open(std::string(file_path));
        auto files = handle.listFiles();
        vector<string> res{files.size()};
        for (const auto& s : files) {
            res.add_item({ std::string_view(s) });
        }
        return res;
    }

} // namespace avalanche
