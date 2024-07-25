#include <iostream>
#include <vector>
#include <cmath>

const double DAMPING_FACTOR = 0.85;
// we will compare this variable with difference of newRank and rank
const double EPSILON = 1e-6; // Convergence threshold

// function to printing pageRanks
void printRanks(const std::vector<double>& ranks) {
    for (size_t i = 0; i < ranks.size(); ++i) {
        std::cout << "Page " << i << " Rank: " << ranks[i] << std::endl;
    }
}

// function to calculate pageranks
std::vector<double> pageRank(const std::vector<std::vector<int>>& links, int numPages) {
    // ranks that we will update and compare with newRanks
    std::vector<double> ranks(numPages, 1.0 / numPages);
    // newRanks that will change depends on how many iterations we have
    std::vector<double> newRanks(numPages, 0.0);

    // Precompute outbound links for each page
    std::vector<int> outboundLinks(numPages, 0);
    for (int i = 0; i < numPages; ++i) {
        for (int j = 0; j < numPages; ++j) {
            if (links[i][j] == 1) {
                // increment the number of outbounded links
                outboundLinks[i]++;
            }
        }
    }

    bool converged = false;
    while (!converged) {
        // Calculate new PageRank values
        for (int i = 0; i < numPages; ++i) {
            // variable to calculate sum
            double inboundSum = 0.0;
            for (int j = 0; j < numPages; ++j) {
                if (links[j][i] == 1) {
                    if (outboundLinks[j] > 0) {
                        inboundSum += ranks[j] / outboundLinks[j];
                    }
                }
            }
            // formula of finding newRank
            newRanks[i] = (1.0 - DAMPING_FACTOR) / numPages + DAMPING_FACTOR * inboundSum;
        }

        // Check for convergence
        // we are checking it to stop iterating when the newRank are so similar as rank
        converged = true;
        for (int i = 0; i < numPages; ++i) {
            // if the difference between newRand and previous rank is less than EPSILON, we are stopping the iterations
            if (std::fabs(newRanks[i] - ranks[i]) > EPSILON) {
                converged = false;
                break;
            }
        }

        // Update ranks for next iteration
        ranks = newRanks;
    }
    // returning ranks to main function to put into printRanks function to print them
    return ranks;
}

int main2() {
    // Example graph with 3 pages (A, B, C)
    // Adjacency matrix representation of links
    // 1 means there is a link from page i to page j, 0 means no link
    std::vector<std::vector<int>> links = {
        {1, 1, 1}, // A links to B and C
        {1, 0, 1}, // B links to A
        {0, 1, 1}, // C links to A
    };

    // getting size of the vector
    int numPages = links.size();
    // calling pageRand function to calculate pagerank
    std::vector<double> ranks = pageRank(links, numPages);

    // calling printRanks to print pageranks (the largest one is the most important)
    printRanks(ranks);

    return 0;
}