#pragma once
#include "Common.hpp"

class HighScores
{
public:
    std::vector<int> load() const
    {
        std::ifstream file("point.txt");

        std::vector<int> scores;
        int value;

        while (file >> value)
        {
            if (value >= 0)
                scores.push_back(value);
        }

        std::sort(
            scores.rbegin(),
            scores.rend()
        );

        if (scores.size() > 10)
            scores.resize(10);

        return scores;
    }

    void save(int score) const
    {
        auto scores = load();

        scores.push_back(score);

        std::sort(
            scores.rbegin(),
            scores.rend()
        );

        if (scores.size() > 10)
            scores.resize(10);

        std::ofstream file("point.txt");

        for (int value : scores)
            file << value << '\n';
    }
};
