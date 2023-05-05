#include "gdpr_deletecommand.hpp"

void GDPRDeleteCommand::handle(const std::string& s) {
  std::vector<std::string> tokens = split(s);
  if (tokens.size() != 1) {
    cerr_color(RED, "Missing key. ", usage());
    return;
  }

  auto res = this->client->GDPRDelete(tokens[0]);
  if (!res) {
    return;
  }

  std::cout << "Success" << '\n';
}

std::string GDPRDeleteCommand::name() const {
  return "gdprdelete";
}

std::string GDPRDeleteCommand::params() const {
  return "<user>";
}

std::string GDPRDeleteCommand::description() const {
  return "Deletes <user>'s data according to GDPR requirements";
}
