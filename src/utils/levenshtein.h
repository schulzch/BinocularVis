#ifndef LEVENSHTEIN_H
#define LEVENSHTEIN_H

#include <vector>
#include <functional>

template <typename T>
using PVector = std::vector<std::vector<std::pair<float, T> > >;

template <typename T>
int levenshtein(const std::vector<T>& a, std::vector<T>& b)
{
    // Allocate matrix.
    size_t sizeI = a.size() + 1;
    size_t sizeJ = b.size() + 1;
    int* matrix = new int[sizeI * sizeJ];
    auto D = [&](size_t i, size_t j) -> int& { return matrix[i + j * sizeI]; };

    // Compute matrix using dynamic programming.
    D(0, 0) = 0;
    for (size_t i = 1; i < sizeI; ++i) {
        D(i, 0) = static_cast<int>(i);
    }
    for (size_t j = 1; j < sizeJ; ++j) {
        D(0, j) = static_cast<int>(j);
    }
    for (size_t i = 1; i < sizeI; ++i) {
        for (size_t j = 1; j < sizeJ; ++j) {
            int costReplace = (a[i - 1] == b[j - 1]) ? 0 : 1;
            int opReplace = D(i - 1, j - 1) + costReplace;
            int opInsert = D(i, j - 1) + 1;
            int opDelete = D(i - 1, j) + 1;
            D(i, j) = std::min(opReplace, std::min(opInsert, opDelete));
        }
    }

    // Store result and free matrix.
    int distance = D(sizeI - 1, sizeJ - 1);
    delete[] matrix;

    return distance;
}

template <typename T>
float levenshteinP1(const PVector<T>& a, PVector<T>& b)
{
    auto printable = [](std::vector<T> s) -> std::string {
        std::stringstream ss;
        for (size_t i = 0; i < s.size(); ++i) {
            ss << s[i];
        }
        return ss.str();
    };
    auto expand = [&](const PVector<T>& pv, size_t alphabetSize) {
        // Expand tree of probabilties.
        float pTotal = 0;
        std::vector<std::pair<float, std::vector<T> > > pve;
        for (size_t i = 0; i < pow(alphabetSize, pv.size()); ++i) {
            std::vector<T> s;
            float p = 1.0;
            for (size_t j = 0; j < pv.size(); ++j) {
                size_t idx = (i / static_cast<size_t>(pow(alphabetSize, j))) % alphabetSize;
                p *= pv[j][idx].first;
                s.push_back(pv[j][idx].second);
            }
            pTotal += p;
            pve.push_back({ p, s });
            //std::cout << printable(s) << ";" << p << std::endl;
        }
        //std::cout << pTotal << std::endl;

        return pve;
    };

    size_t alphabetSize = a[0].size();
    auto pveA = expand(a, alphabetSize);
    auto pveB = expand(b, alphabetSize);

    // Brute force levenshtein distances.
    float pTotal = 0;
    float lTotal = 0;
    for (size_t i = 0; i < pveA.size(); ++i) {
        float pA = pveA[i].first;
        std::vector<T> sA = pveA[i].second;
        for (size_t j = 0; j < pveB.size(); ++j) {
            float pB = pveB[j].first;
            std::vector<T> sB = pveB[j].second;
            float p = pA * pB;
            float l = levenshtein(sA, sB);
            lTotal += p * l;
            pTotal += p;
            //std::cout << printable(sA) << " " << printable(sB) << " " << l << " " << p << std::endl;
        }
    }

    return lTotal;
}

template <typename T>
float levenshteinP1_2(const PVector<T>& a, PVector<T>& b)
{
    auto printable = [](PVector<T> pv) -> std::string {
        std::stringstream ss;
        for (size_t i = 0; i < pv.size(); ++i) {
            ss << "(";
            for (size_t j = 0; j < pv[i].size(); ++j) {
                if (j > 0) {
                    ss << "; ";
                }
                ss << pv[i][j].first << " " << pv[i][j].second;
            }
            ss << ")";
        }
        return ss.str();
    };

    PVector<T> c;
    for (size_t i = 0; i < a.size(); ++i) {
        std::vector<std::pair<float, T> > x;
        for (size_t j = 0; j < a[i].size(); ++j) {
            // assuming same size
            float p = (a[i][j].first + b[i][j].first) / 2;
            // assuming same order
            std::pair<float, T> pair(p, a[i][j].second);
            x.push_back(pair);
        }
        c.push_back(x);
    }

    std::cout << printable(a) << std::endl
              << printable(b) << std::endl
              << printable(c) << std::endl;

    return levenshteinP1(c, c);
}

template <typename T>
float levenshteinP2(const PVector<T>& a, PVector<T>& b)
{
    // Allocate matrix.
    size_t sizeI = a.size() + 1;
    size_t sizeJ = b.size() + 1;
    float* matrix = new float[sizeI * sizeJ];
    auto D = [&](size_t i, size_t j) -> float& { return matrix[i + j * sizeI]; };
    auto pReplace = [&](size_t i, size_t j) -> float {
        float p = 0;
        for (size_t k = 0; k < a[i].size(); ++k) {
            for (size_t l = 0; l < b[j].size(); ++l) {
                if (a[i][k].second != b[j][l].second) {
                    p += a[i][k].first * b[j][l].first;
                }
            }
        }
        return p;
    };

    // Compute matrix using dynamic programming.
    D(0, 0) = 0;
    for (size_t i = 1; i < sizeI; ++i) {
        D(i, 0) = static_cast<float>(i);
    }
    for (size_t j = 1; j < sizeJ; ++j) {
        D(0, j) = static_cast<float>(j);
    }
    for (size_t i = 1; i < sizeI; ++i) {
        for (size_t j = 1; j < sizeJ; ++j) {
            float p = pReplace(i - 1, j - 1);
            float opReplace = D(i - 1, j - 1) + p;
            float opInsert = D(i, j - 1) + 1;
            float opDelete = D(i - 1, j) + 1;

            D(i, j) = std::min(opReplace, std::min(opInsert, opDelete));
        }
    }

    // Store result and free matrix.
    float distance = D(sizeI - 1, sizeJ - 1);
    delete[] matrix;

    return distance;
}

inline void testLevenshtein()
{
    typedef std::pair<float, char> pchar;

#define MODE 2
#if MODE == 2
    PVector<char> pvA = {
        { { 0.5f, 'a' }, { 0.5f, 'b' } },
        { { 0.1f, 'a' }, { 0.9f, 'b' } },
        { { 0.8f, 'a' }, { 0.2f, 'b' } },
    };
    PVector<char> pvB = {
        { { 0.3f, 'a' }, { 0.7f, 'b' } },
        { { 0.1f, 'a' }, { 0.9f, 'b' } },
        { { 0.2f, 'a' }, { 0.8f, 'b' } },
    };
#elif MODE == 3
    PVector<char> pvA = {
        { { 0.5f, 'a' }, { 0.5f, 'b' } },
        { { 1.0f, 'a' }, { 0.0f, 'b' } },
        { { 1.0f, 'a' }, { 0.0f, 'b' } },
        { { 1.0f, 'a' }, { 0.0f, 'b' } },
        { { 1.0f, 'a' }, { 0.0f, 'b' } },
    };
    PVector<char> pvB = pvA;
#elif MODE == 4
    PVector<char> pvA = {
        { { 0.3f, 'a' }, { 0.3f, 'b' }, { 0.4f, 'c' } },
        { { 0.3f, 'a' }, { 0.3f, 'b' }, { 0.4f, 'c' } },
        { { 0.3f, 'a' }, { 0.3f, 'b' }, { 0.4f, 'c' } },
        { { 0.3f, 'a' }, { 0.3f, 'b' }, { 0.4f, 'c' } },
        { { 0.3f, 'a' }, { 0.3f, 'b' }, { 0.4f, 'c' } },
        { { 0.3f, 'a' }, { 0.3f, 'b' }, { 0.4f, 'c' } },
    };
    PVector<char> pvB = pvA;
#endif

    auto filter = [](const PVector<char>& pv) {
        std::vector<char> result;
        for (const auto& pc : pv) {
            auto maxP = pc[0].first;
            auto maxC = pc[0].second;
            for (const auto& pp : pc) {
                if (pp.first > maxP) {
                    maxP = pp.first;
                    maxC = pp.second;
                }
            }
            result.push_back(maxC);
        }
        return result;
    };

    auto s1 = filter(pvA);
    auto s2 = filter(pvB);
    std::cout << "N  " << levenshtein(s1, s2) << std::endl
              << "P1 " << levenshteinP1(pvA, pvB) << std::endl
              << "Px " << levenshteinP1_2(pvA, pvB) << std::endl
              << "P2 " << levenshteinP2(pvB, pvB) << std::endl;
}

#endif
