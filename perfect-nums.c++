#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <filesystem> // For C++17 or later

__uint128_t power128(__uint128_t a, __uint128_t b) {
    __uint128_t res = 1;
    while (b > 0) {
        if (b & 1)
            res = res * a;
        a = a * a;
        b >>= 1;
    }
    return res;
}

std::string to_string128(__uint128_t num) {
    if (num == 0) return "0";
    std::string str;
    while (num != 0) {
        str = char(num % 10 + '0') + str;
        num /= 10;
    }
    return str;
}

bool isPrime(__uint128_t num) {
    if (num < 2) return false;
    if (num == 2) return true;
    if (num % 2 == 0) return false;
    for (__uint128_t j = 3; j * j <= num; j += 2) {
        if (num % j == 0) return false;
    }
    return true;
}

int main() {
    __uint128_t start_range = 0;
    __uint128_t end_range = 1000000;

    // Create directory if it doesn't exist
    std::filesystem::create_directory("./calculated");

    for (__uint128_t i = start_range; i <= end_range; i++) {
        __uint128_t mersenne_prime = power128(2, i) - 1;
        if (isPrime(mersenne_prime)) {
            auto start = std::chrono::high_resolution_clock::now();
            __uint128_t perfect_number = power128(2, i - 1) * mersenne_prime;
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = end - start;

            std::string filename = "./calculated/perfect-number-" + to_string128(i) + ".txt";
            std::ofstream outfile(filename);
            if (outfile.is_open()) {
                outfile << to_string128(perfect_number) << std::endl;
                outfile.close();
                std::cout << "Perfect number " << to_string128(i) << " saved to " << filename << std::endl;
            } else {
                std::cerr << "Unable to open file " << filename << " for writing" << std::endl;
                // You can add more specific error handling here if needed
            }

            std::cout << to_string128(i) + ": done in " << elapsed.count() << " seconds" << std::endl;
        }
    }
    return 0;
}