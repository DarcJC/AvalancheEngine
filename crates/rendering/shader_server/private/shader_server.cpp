#include "shader_server.h"

#include <slang.h>
#include <slang-com-ptr.h>


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
