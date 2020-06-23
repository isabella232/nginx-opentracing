#include "discover_span_context_keys.h"
#include "load_tracer.h"

#include <opentracing/ext/tags.h>
#include <opentracing/propagation.h>
#include <datadog/opentracing.h>
#include <algorithm>
#include <iostream>
#include <new>

namespace ngx_opentracing {
//------------------------------------------------------------------------------
// HeaderKeyWriter
//------------------------------------------------------------------------------
namespace {
class HeaderKeyWriter : public opentracing::HTTPHeadersWriter {
 public:
  HeaderKeyWriter(ngx_pool_t* pool, std::vector<opentracing::string_view>& keys)
      : pool_{pool}, keys_(keys) {}

  opentracing::expected<void> Set(
      opentracing::string_view key,
      opentracing::string_view value) const override {
    auto data = static_cast<char*>(ngx_palloc(pool_, key.size()));
    if (data == nullptr) {
      throw std::bad_alloc{};
    }
    std::copy_n(key.data(), key.size(), data);

    keys_.emplace_back(data, key.size());

    return {};
  }

 private:
  ngx_pool_t* pool_;
  std::vector<opentracing::string_view>& keys_;
};
}  // namespace

//------------------------------------------------------------------------------
// discover_span_context_keys
//------------------------------------------------------------------------------
// Loads the vendor tracing library and creates a dummy span so as to obtain
// a list of the keys used for span context propagation. This is necessary to
// make context propagation work for requests proxied upstream.
//
// See propagate_opentracing_context, set_tracer.
//
// Note: Any keys that a tracer might use for propagation that aren't discovered
// here will get dropped during propagation.
ngx_array_t* discover_span_context_keys(ngx_pool_t* pool, ngx_log_t* log) {
  ngx_log_error(NGX_LOG_ERR, log, 0, "discover_span_context_keys entrypoint");
  datadog::opentracing::TracerOptions options;
  options.service = "traced-nginx";
  auto tracer = datadog::opentracing::makeTracer(options);
  auto span = tracer->StartSpan("dummySpan");
  span->SetTag(opentracing::ext::sampling_priority, 0);
  std::vector<opentracing::string_view> keys;
  HeaderKeyWriter carrier_writer{pool, keys};
  auto was_successful = tracer->Inject(span->context(), carrier_writer);
  if (!was_successful) {
    ngx_log_error(NGX_LOG_ERR, log, 0,
                  "failed to discover span context tags: %s",
                  was_successful.error().message().c_str());
    return nullptr;
  }
  if (keys.empty()) {
    ngx_log_error(NGX_LOG_ERR, log, 0, "discover_span_context_keys has empty result");
  } else {
    ngx_log_error(NGX_LOG_ERR, log, 0, "discover_span_context_keys result: %d", keys.size());
    for (auto& key : keys) {
      ngx_log_error(NGX_LOG_ERR, log, 0, "  %s", key);
    }
  }

  ngx_array_t* result =
      ngx_array_create(pool, keys.size(), sizeof(opentracing::string_view));
  if (result == nullptr) {
    throw std::bad_alloc{};
  }

  for (auto key : keys) {
    auto element =
        static_cast<opentracing::string_view*>(ngx_array_push(result));
    *element = key;
  }
  return result;
}
}  // namespace ngx_opentracing
