#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>
#include <cstdlib>
#include <ctime>
#include <algorithm>

const double DAMPING_FACTOR = 0.85;
const double EPSILON = 1e-6;
#define NUM_THREADS 8

// Function to print PageRanks
void printRanksParallel(const std::vector<double>& ranks) {
    for (size_t i = 0; i < ranks.size(); ++i) {
        std::cout << "Page " << i << " Rank: " << ranks[i] << std::endl;
    }
}

// Function to generate a random page link matrix
std::vector<std::vector<int>> generateLinkMatrix(int numPages, int linksPerPage) {
    std::vector<std::vector<int>> links(numPages, std::vector<int>(numPages, 0));
    std::srand(std::time(nullptr)); // Seed the random number generator

    for (int i = 0; i < numPages; ++i) {
        std::vector<int> potentialLinks(numPages);
        for (int j = 0; j < numPages; ++j) {
            potentialLinks[j] = j;
        }
        std::random_shuffle(potentialLinks.begin(), potentialLinks.end());

        // Ensure page i links to exactly linksPerPage other pages
        for (int j = 0; j < linksPerPage; ++j) {
            if (potentialLinks[j] != i) {
                links[i][potentialLinks[j]] = 1;
            }
        }
    }

    return links;
}

// Function to calculate PageRank in parallel
std::vector<double> pageRankParallel(const std::vector<std::vector<int>>& links, int numPages) {
    std::vector<double> ranks(numPages, 1.0 / numPages);
    std::vector<double> newRanks(numPages, 0.0);
    omp_set_num_threads(NUM_THREADS);

    std::vector<int> outboundLinks(numPages, 0);
    for (int i = 0; i < numPages; ++i) {
        for (int j = 0; j < numPages; ++j) {
            if (links[i][j] == 1) {
                outboundLinks[i]++;
            }
        }
    }

    bool converged = false;
    while (!converged) {
#pragma omp parallel for
        for (int i = 0; i < numPages; ++i) {
            double inboundSum = 0.0;
            for (int j = 0; j < numPages; ++j) {
                if (links[j][i] == 1) {
                    if (outboundLinks[j] > 0) {
                        inboundSum += ranks[j] / outboundLinks[j];
                    }
                }
            }
            newRanks[i] = (1.0 - DAMPING_FACTOR) / numPages + DAMPING_FACTOR * inboundSum;
        }

        converged = true;
#pragma omp parallel for
        for (int i = 0; i < numPages; ++i) {
            if (std::fabs(newRanks[i] - ranks[i]) > EPSILON) {
#pragma omp atomic write
                converged = false;
            }
        }

        ranks = newRanks;
    }

    return ranks;
}

int main() {
    const int numPages = 10000;  // Number of pages
    const int linksPerPage = 200; // Each page will link to exactly 20 other pages

    // Generate the link matrix
    std::vector<std::vector<int>> links = generateLinkMatrix(numPages, linksPerPage);

    // Start timer
    double start_time = omp_get_wtime();

    // Calculate PageRank
    std::vector<double> ranks = pageRankParallel(links, numPages);

    // End timer
    double end_time = omp_get_wtime();
    double total_time = end_time - start_time;

    // Print the PageRanks
    printRanksParallel(ranks);

    std::cout << "Time taken: " << total_time << " seconds." << std::endl;

    return 0;
}
