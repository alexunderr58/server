#include <UnitTest++/UnitTest++.h>
#include <iostream>

// Включаем все тестовые файлы (реализации)
// Но только для того чтобы зарегистрировать тесты
// Фактически тесты регистрируются через макросы в каждом файле

int main() {
    std::cout << "========================================\n";
    std::cout << "   VCALC_SERVER Unit Tests (UnitTest++)\n";
    std::cout << "========================================\n\n";
    
    return UnitTest::RunAllTests();
}
