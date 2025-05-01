#include "DeleteUser.hpp"

DeleteUser::DeleteUser(std::shared_ptr<IUserRepository> r) : repo(std::move(r)) {}

bool DeleteUser::execute(const std::string& email) {
    repo->removeByEmail(email);
    return true;
}