# `std::variant` vs inheritance?

I originally used `std::variant` to mimic Rust's algebraic data types / a `match`-like interface,
but it turns out the `std::variant` interface is exceedingly un-ergonomic. Let's see if we can
maintain the request/response API, while converting from `std::variant`s to inheritance.

# [DONE] Structural Changes

So, I realized that these network helpers could be combined into one ClientConn and ServerConn
struct, regardless of Shardkv or Shardmaster:

- Migrate message definitions from `network_helpers.hpp` -> `network_messages.hpp`
- Migrate `{Kv, Sm}ClientConn` and `{Kv, Sm}ServerConn` -> `network_conn.hpp`
- Within `network_conn.hpp`:
  - Create a `Request` and `Response` union type
    - Also investigate if I really need `Sm`/`Kv` variants of each
  - Migrate from `accept_{server, shardmaster}_client(int sfd)` to generic `accept_client(int sfd)`
  - Create generic `connect_to_server(const std::string &addr)`
    - Should just be a couple line changes, since `connect_to_address` already handles most of it
