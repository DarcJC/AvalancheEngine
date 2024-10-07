#pragma once
#include <container/vector.hpp>
#include <container/shared_ptr.hpp>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <manager/server_manager.h>
#include <string_view>


namespace avalanche {

    using path_t = std::string_view;

    enum class FileSystemResult {
        Success,
    };

    /// @brief File open mode
    enum class FileOpenMode {
        /// @brief Open for reading.
        Read = std::ios::in,
        /// @brief Open for writing.
        Write = std::ios::out,
        /// @brief Seek to the end of stream before each write.
        Append = std::ios::app,
        /// @brief Open in binary mode.
        /// @note If this flag isn't provided, Windows would convert '\\n' to '\\r\\n'.
        Binary = std::ios::binary,
        /// @brief Discard the contents of the stream when opening.
        Truncate = std::ios::trunc,
        /// @brief Seek to the end of stream immediately after open.
        Ate = std::ios::ate,
        /// @brief Open in exclusive mode
        Exclusive = std::ios::noreplace,
        /// @brief Open for reading and writing.
        ReadWrite = Read | Write,
        /// @brief Open for writing at then end of file
        AppendWrite = Write | Append,
        /// @brief Open for truncate and writing.
        TruncateWrite = Write | Truncate,
    };

    struct FileInfo final {
    public:
        explicit FileInfo(path_t path);
    };

    class IFile {
    public:
        virtual ~IFile() = default;

        /// @brief Check if readonly file
        AVALANCHE_NO_DISCARD virtual bool is_read_only() const = 0;

    };

    using file_t = shared_ptr<IFile>;

    /// @brief Filesystem interface to unify file operations
    class IFilesystem {
    public:
        virtual ~IFilesystem() = default;

        /// @brief Perform filesystem initialization, like loading file entries from an archive file.
        virtual FileSystemResult initialize() = 0;

        /// @brief Check does filesystem server initialized
        AVALANCHE_NO_DISCARD virtual bool is_initialized() const = 0;

        /// @brief Check if readonly filesystem
        AVALANCHE_NO_DISCARD virtual bool is_read_only() const = 0;

        /// @brief Open a file by path
        virtual file_t open_file(path_t file_path) = 0;
    };

} // namespace avalanche

/**
 * The filesystem module references the design of https://github.com/nextgeniuspro/vfspp .
 * Thanks nextgeniuspro.
 */

#include "filesystem.generated.h"
