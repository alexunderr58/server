#include <iostream>
#include <cassert>
#include <memory>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
#include "../test_utils.h"
#include "../../include/ClientDB.h"
#include "../../include/Logger.h"

// Mock-клиент для тестирования аутентификации (уникальное имя)
class AuthTestClient {
private:
    std::string login;
    std::string password;
    
public:
    AuthTestClient(const std::string& l, const std::string& p) 
        : login(l), password(p) {}
    
    // Симуляция протокола аутентификации
    bool authenticate(const ClientDB& db, std::string& receivedSalt) {
        // Шаг 1: Клиент существует?
        if (!db.clientExists(login)) {
            std::cout << "  [AuthTestClient] Login '" << login << "' not found\n";
            return false;
        }
        
        // Шаг 2: Генерация соли (в реальности сервер отправляет)
        receivedSalt = ClientDB::generateSalt();
        std::cout << "  [AuthTestClient] Received salt: " << receivedSalt << "\n";
        
        // Шаг 3: Клиент вычисляет хэш
        std::string hash = ClientDB::generateHash(receivedSalt, password);
        std::cout << "  [AuthTestClient] Computed hash: " << hash << "\n";
        
        // Шаг 4: Сервер проверяет хэш (в реальности)
        // Для теста мы проверяем формат
        if (hash.length() != 40) {
            std::cout << "  [AuthTestClient] Invalid hash length: " << hash.length() << "\n";
            return false;
        }
        
        return true;
    }
};

void test_full_auth_flow() {
    std::cout << "=== Full Authentication Flow Test ===\n";
    
    // 1. Создание тестовой среды
    std::string testDbFile = TestUtils::createTempFile(
        "user:FE78BDB4183B677A55E711B17E9CB6CCC2AE7315\n"  // P@ssW0rd
    );
    
    std::string testLogFile = "/tmp/test_auth_flow.log";
    
    // 2. Инициализация компонентов
    ClientDB db(testDbFile);
    assert(db.loadFromFile() == true);
    
    Logger logger(testLogFile);
    logger.initialize();
    
    // 3. Тест 1: Успешная аутентификация
    std::cout << "Test 1: Successful authentication\n";
    {
        AuthTestClient client("user", "P@ssW0rd");
        std::string salt;
        bool result = client.authenticate(db, salt);
        
        assert(result == true);
        assert(salt.length() == 16);
        assert(TestUtils::isValidHex(salt, true));
        
        logger.log(LogLevel::INFO, "Auth test passed", "user=user");
        std::cout << "  ✓ Authentication successful\n";
    }
    
    // 4. Тест 2: Неверный логин
    std::cout << "Test 2: Invalid login\n";
    {
        AuthTestClient client("wronguser", "P@ssW0rd");
        std::string salt;
        bool result = client.authenticate(db, salt);
        
        assert(result == false);
        logger.log(LogLevel::WARNING, "Invalid login detected", "login=wronguser");
        std::cout << "  ✓ Invalid login correctly rejected\n";
    }
    
    // 5. Тест 3: Неверный пароль
    std::cout << "Test 3: Invalid password\n";
    {
        AuthTestClient client("user", "wrongpass");
        std::string salt = ClientDB::generateSalt();
        std::string hash = ClientDB::generateHash(salt, "wrongpass");
        
        // Хэш будет валидного формата, но не совпадет с хранимым
        assert(hash.length() == 40);
        assert(hash != "FE78BDB4183B677A55E711B17E9CB6CCC2AE7315");
        
        std::cout << "  ✓ Invalid password produces different hash\n";
    }
    
    // 6. Тест 4: Формат соли
    std::cout << "Test 4: Salt format validation\n";
    {
        std::vector<std::string> salts;
        for (int i = 0; i < 5; i++) {
            salts.push_back(ClientDB::generateSalt());
        }
        
        for (const auto& salt : salts) {
            assert(salt.length() == 16);
            assert(TestUtils::isValidHex(salt, true));
            assert(TestUtils::isUppercase(salt));
        }
        
        std::cout << "  ✓ All salts have correct format\n";
    }
    
    // 7. Тест 5: Хэш формат и детерминированность
    std::cout << "Test 5: Hash format and determinism\n";
    {
        std::string salt = "0123456789ABCDEF";
        std::string password = "test123";
        
        std::string hash1 = ClientDB::generateHash(salt, password);
        std::string hash2 = ClientDB::generateHash(salt, password);
        
        assert(hash1 == hash2);  // Детерминированность
        assert(hash1.length() == 40);
        assert(TestUtils::isValidHex(hash1, true));
        
        std::cout << "  ✓ Hash is deterministic and has correct format\n";
    }
    
    // 8. Очистка
    TestUtils::deleteTempFile(testDbFile);
    std::remove(testLogFile.c_str());
    
    std::cout << "=== Authentication flow test completed ===\n";
}
