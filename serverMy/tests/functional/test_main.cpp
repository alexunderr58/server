#include <iostream>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <csignal>
#include <memory>

// Объявления тестовых функций
void test_full_auth_flow();
void test_vector_processing_flow();

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    
    std::cout << "========================================\n";
    std::cout << "  VCALC_SERVER Functional Tests\n";
    std::cout << "========================================\n\n";
    
    try {
        std::cout << "1. Testing authentication flow...\n";
        test_full_auth_flow();
        
        std::cout << "\n2. Testing vector processing flow...\n";
        test_vector_processing_flow();
        
        std::cout << "\n✅ ALL FUNCTIONAL TESTS PASSED!\n";
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Functional test FAILED: " << e.what() << "\n";
        return 1;
    }
}
