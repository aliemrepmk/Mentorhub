#include "UpdateUserName.hpp"

UpdateUserName::UpdateUserName(std::shared_ptr<IUserRepository> r) : repo(std::move(r)) {}

bool UpdateUserName::execute(const std::string& email, const std::string& newFirstName) {
    repo->updateFirstName(email, newFirstName);
    return true;
}