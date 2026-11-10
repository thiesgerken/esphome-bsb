#pragma once

#include "esphome/core/defines.h"
#ifdef USE_BSB_PARAMETER_DUMP

  #include "bsbDumpChannel.h"

  #include "esphome/components/web_server_base/web_server_base.h"
  #include "esphome/core/component.h"

namespace esphome {
  namespace bsb {

    // Serves `GET /JB` like BSB-LAN: streams the parameter dump as chunked JSON while the
    // component loop queries the parameters.
    class BsbDumpRequestHandler
        : public AsyncWebHandler
        , public Component {
    public:
      BsbDumpRequestHandler( web_server_base::WebServerBase* base, DumpChannel* channel ) : base_( base ), channel_( channel ) {}

      void  setup() override;
      float get_setup_priority() const override { return setup_priority::WIFI - 1.0f; }

      bool canHandle( AsyncWebServerRequest* request ) const override;
      void handleRequest( AsyncWebServerRequest* request ) override;

    private:
      void stream_dump( httpd_req_t* request );
      void cancel_after_disconnect();

      web_server_base::WebServerBase* base_;
      DumpChannel*                    channel_;
    };

  } // namespace bsb
} // namespace esphome

#endif
