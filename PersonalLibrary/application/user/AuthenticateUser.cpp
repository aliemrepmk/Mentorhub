#include "AuthenticateUser.hpp"

AuthenticateUser::AuthenticateUser(std::shared_ptr<IUserRepository> r, std::shared_ptr<PasswordHasher> h)
    : repo(std::move(r)), hasher(std::move(h)) {}

bool AuthenticateUser::execute(const std::string& email, const std::string& password) {
    auto user = repo->findByEmail(email);
    return user && hasher->verify(user->getPasswordHash(), password);
}