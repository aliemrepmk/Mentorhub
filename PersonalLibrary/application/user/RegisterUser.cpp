#include "RegisterUser.hpp"
#include "user/User.hpp"

RegisterUser::RegisterUser(std::shared_ptr<IUserRepository> r, std::shared_ptr<PasswordHasher> h)
    : repo(std::move(r)), hasher(std::move(h)) {}

bool RegisterUser::execute(const std::string& first, const std::string& last, const std::string& email, const std::string& password) {
    if (repo->findByEmail(email)) return false;

    std::string hash = hasher->hash(password);
    repo->save(User(0, first, last, email, hash));
    return true;
}