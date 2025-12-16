#include <iostream>
#include <cstdlib>
#include <ctime>

// Объявления тестовых функций
void run_clientdb_tests();       // tests/test_clientdb.cpp
void run_vectorprocessor_tests(); // tests/test_vectorprocessor.cpp
void run_logger_tests();         // tests/test_logger.cpp
void run_config_tests();         // tests/test_config.cpp
void run_protocol_tests();       // tests/test_protocol.cpp
void run_server_tests();         // tests/test_server.cpp
void run_filesystem_tests();     // tests/test_filesystem.cpp

int main() {
    // Инициализация генератора случайных чисел
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    
    std::cout << "========================================\n";
    std::cout << "    vcalc_server COMPLETE Unit Tests\n";
    std::cout << "========================================\n\n";
    
    struct TestModule {
        const char* name;
        void (*func)();
        bool run;
    };
    
    TestModule modules[] = {
        {"ClientDB", run_clientdb_tests, true},
        {"VectorProcessor", run_vectorprocessor_tests, true},
        {"Logger", run_logger_tests, true},
        {"Config", run_config_tests, true},
        {"Protocol", run_protocol_tests, true},
        {"Server", run_server_tests, true},
        {"Filesystem", run_filesystem_tests, true},
    };
    
    int totalModules = sizeof(modules) / sizeof(modules[0]);
    int passed = 0;
    int failed = 0;
    
    for (int i = 0; i < totalModules; i++) {
        if (modules[i].run) {
            std::cout << "\n[" << (i+1) << "/" << totalModules << "] ";
            std::cout << "Running " << modules[i].name << " tests...\n";
            std::cout << "========================================\n";
            
            try {
                modules[i].func();
                passed++;
            } catch (const std::exception& e) {
                std::cerr << "\n❌ " << modules[i].name << " tests FAILED: " << e.what() << "\n";
                failed++;
            } catch (...) {
                std::cerr << "\n❌ " << modules[i].name << " tests FAILED with unknown error\n";
                failed++;
            }
        }
    }
    
    std::cout << "\n========================================\n";
    std::cout << "          FINAL TEST SUMMARY\n";
    std::cout << "========================================\n";
    std::cout << "Total modules: " << totalModules << "\n";
    std::cout << "Passed: " << passed << "\n";
    std::cout << "Failed: " << failed << "\n";
    
    if (failed > 0) {
        std::cout << "\n❌ SOME TESTS FAILED\n";
        return 1;
    } else {
        std::cout << "\n✅ ALL TESTS PASSED!\n";
        return 0;
    }
}
