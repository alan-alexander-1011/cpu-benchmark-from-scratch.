#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <filesystem> // For C++17 or later
#include <vector>
#include <cmath>
#include <thread>
#include <mutex>

std::mutex mtx;
const __uint128_t threadCount = 8;

__uint128_t power128(__uint128_t a, __uint128_t b) {
    if (b == 0) return 1;
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
    if (num == 2 || num == 3) return true;
    if (num % 2 == 0 || num % 3 == 0) return false;
    for (__uint128_t j = 5; j * j <= num; j += 6) {
        if (num % j == 0 || num % (j + 2) == 0) return false;
    }
    return true;
}

std::vector<__uint128_t> chunk_integer(__uint128_t exponent, __uint128_t num_chunks) {
    std::vector<__uint128_t> chunks;
    __uint128_t quotient = exponent / num_chunks;
    __uint128_t remainder = exponent % num_chunks;
    for (__uint128_t i = 0; i < num_chunks; ++i) {
        __uint128_t chunk_value = quotient + (i < remainder ? 1 : 0);
        chunks.push_back(chunk_value);
    }
    return chunks;
}

// Function to be executed by each thread
// Function to be executed by each thread
void thread_function(__uint128_t exponent, __uint128_t& result) {
    __uint128_t partial_result = 1;
    __uint128_t a = power128(2, exponent);
    std::lock_guard<std::mutex> lock(mtx);
    result *= a;
    // std::cout << "thread:" + to_string128(a) + " 2^" << to_string128(exponent-1) << std::endl;
}

int main() {
    __uint128_t start = 2;
    __uint128_t end = 61;

    // Create directory if it doesn't exist
    std::filesystem::create_directory("./calculated");

    for (__uint128_t p = start; p <= end; p++) {
        __uint128_t mersenne_prime = power128(2, p) - 1;
        if (isPrime(mersenne_prime)) {
            __uint128_t perfect_number = 1;
            // this piece of shit code (but important) here, i forgot to add -1 so that i still think why the fuck i can't calculate.
            std::vector<__uint128_t> exponents = chunk_integer(p-1, threadCount); 
            std::vector<std::thread> threads;

            auto starttime = std::chrono::high_resolution_clock::now();
            for (const auto& exponent : exponents) {
                // std::cout << to_string128(exponent) << std::endl;
                if (exponent != 0){
                    threads.emplace_back(thread_function, exponent, std::ref(perfect_number));
                }
            }

            for (auto& thread : threads) {
                thread.join();
            }

            // std::cout << "intermediate perfect number: " << to_string128(perfect_number) << std::endl;
            perfect_number *= mersenne_prime;

            auto endtime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = endtime - starttime;

            std::string filename = "./calculated/perfect-number-" + to_string128(p) + ".txt";
            std::ofstream outfile(filename);
            if (outfile.is_open()) {
                outfile << to_string128(perfect_number) << std::endl;
                outfile.close();
                std::cout << "Perfect number " << to_string128(perfect_number) << " saved to " << filename << std::endl;
            }

            std::cout << to_string128(p) + ": done in " << elapsed.count() << " seconds" << std::endl;
        }
    }

    return 0;
}
