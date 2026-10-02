#include <httplib.h>
#define CPPHTTPLIB_OPENSSL_SUPPORT


int main(void) {
// HTTP
httplib::Server svr;

// HTTPS
//httplib::SSLServer svr;

svr.Get("/hi", [](const httplib::Request &, httplib::Response &res) {
  res.set_content("Hello my World!", "text/plain");
});

svr.listen("0.0.0.0", 8080);
return 0;
}
