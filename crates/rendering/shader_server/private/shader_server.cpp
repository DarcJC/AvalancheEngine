#include "shader_server.h"

#include <include/slang.h>
#include <include/slang-com-ptr.h>


namespace avalanche {

    class ShaderServer : public IShaderServer {
    public:
        ShaderServer() {
        }
    };

    IShaderServer& IShaderServer::get() {
        static ShaderServer server;
        return server;
    }

} // namespace avalanche
