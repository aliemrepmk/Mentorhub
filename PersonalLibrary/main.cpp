#include <QApplication>
#include "ui/login/LoginWindow.hpp"

#include "infrastructure/persistence/PostgreSQLUserRepository.hpp"
#include "infrastructure/crypto/SodiumPasswordHasher.hpp"
#include "infrastructure/db/PostgreDBManager.hpp"
#include "infrastructure/config/EnvLoader.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    try {
        // Load environment config
        auto env = EnvLoader::load("../.env");
        std::string connStr = EnvLoader::buildConnectionString(env);

        // Initialize PostgreSQL connection and tables
        PostgreDBManager::initialize(connStr);
        PostgreDBManager::getInstance().initializeTables();

        // Create infrastructure and service layer components
        auto userRepo = std::make_shared<PostgreSQLUserRepository>();
        auto passwordHasher = std::make_shared<SodiumPasswordHasher>();
        AuthenticateUser loginUseCase(userRepo, passwordHasher);

        // Show login window
        LoginWindow window(loginUseCase);
        window.show();

        return app.exec();

    } catch (const std::exception& ex) {
        qCritical("Fatal error: %s", ex.what());
        return 1;
    }
}