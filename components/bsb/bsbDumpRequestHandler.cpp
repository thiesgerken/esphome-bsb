#include "bsbDumpRequestHandler.h"
#ifdef USE_BSB_PARAMETER_DUMP

  #include "bsbDumpFormat.h"

  #include "esphome/core/log.h"

  #include <cstring>

namespace esphome {
  namespace bsb {

    static const char* const TAG = "bsb.dump";

    namespace {
      bool send_chunk( httpd_req_t* request, const char* data, size_t length ) {
        return httpd_resp_send_chunk( request, data, length ) == ESP_OK;
      }

      bool send_chunk( httpd_req_t* request, const char* text ) { return send_chunk( request, text, strlen( text ) ); }
    } // namespace

    void BsbDumpRequestHandler::setup() {
      base_->init();
      base_->add_handler( this );
    }

    bool BsbDumpRequestHandler::canHandle( AsyncWebServerRequest* request ) const {
      char url[AsyncWebServerRequest::URL_BUF_SIZE];
      return request->method() == HTTP_GET && request->url_to( url ) == "/JB";
    }

    void BsbDumpRequestHandler::handleRequest( AsyncWebServerRequest* request ) {
      // ESPHome's web server handles one request at a time, so a second request usually waits
      // for the running dump instead of getting here while it runs.
      if( !channel_->request_dump() ) {
        request->send( 409, "text/plain", "A parameter dump is already running" );
        return;
      }
      request->beginResponse( 200, "application/json; charset=utf-8" );
      httpd_resp_set_hdr( *request, "Cache-Control", "no-cache, no-store, must-revalidate" );
      stream_dump( *request );
    }

    // Receives every line until the end of the dump, even after the client is gone: the loop
    // may still be sending, and the channel is only free again after the end.
    void BsbDumpRequestHandler::stream_dump( httpd_req_t* request ) {
      bool connected = send_chunk( request, DumpHeader );
      if( !connected ) {
        cancel_after_disconnect();
      }
      bool        first = true;
      std::string line;
      while( channel_->receive_line( line ) ) {
        if( !connected ) {
          continue;
        }
        connected = ( first || send_chunk( request, DumpLineSeparator ) ) && send_chunk( request, line.data(), line.size() );
        first     = false;
        if( !connected ) {
          cancel_after_disconnect();
        }
      }
      if( connected ) {
        send_chunk( request, DumpFooter );
        send_chunk( request, nullptr, 0 );
      }
    }

    void BsbDumpRequestHandler::cancel_after_disconnect() {
      ESP_LOGW( TAG, "Client disconnected, cancelling the parameter dump" );
      channel_->cancel();
    }

  } // namespace bsb
} // namespace esphome

#endif
