#pragma once
#include <container/vector.hpp>
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
    };

    /// @brief Filesystem interface to unify file operations
    class IFilesystem : public core::ServerCRTPBase<IFilesystem> {
    public:
        /// @brief Check does filesystem server initialized
        [[nodiscard]] virtual bool is_initialized() const = 0;

        /// @brief Check if readonly filesystem
        [[nodiscard]] virtual bool is_read_only() const = 0;
    };

} // namespace avalanche

/**
 * The filesystem module references the design of https://github.com/nextgeniuspro/vfspp .
 * Thanks nextgeniuspro.
 */