#include <iostream>
#include <vector>
#include <unordered_map>
#include <sstream>

class CardGrid {
    int columns, rows, maxCardsPerColumn, maxCardsPerRow;
    std::vector<int> columnCardCounts;
    std::unordered_map<std::string, long long> memoCache;

    std::string generateMemoKey() const {
        std::vector<int> deficit(maxCardsPerColumn + 1, 0);
        for (int cnt : columnCardCounts) {
            ++deficit[maxCardsPerColumn - cnt];
        }

        std::stringstream sstream;
        sstream << rows << "#";
        for (int i = 0; i < deficit.size(); i++) {
            if (deficit[i] > 0) {
                sstream << i << "=" << deficit[i] << ";";
            }
        }
        return sstream.str();
    }

public:
    CardGrid(int cols, int rows, int maxPerCol, int maxPerRow)
        : columns(cols), rows(rows), maxCardsPerRow(maxPerRow), maxCardsPerColumn(maxPerCol), columnCardCounts(cols, 0) {}

    long long calculateAssignments(int currentRow = 0) {
        if (currentRow == rows) {
            for (int count : columnCardCounts)
                if (count != maxCardsPerColumn) return 0;
            return 1;
        }

        std::string key = generateMemoKey();
        if (memoCache.count(key))
            return memoCache[key];

        long long totalAssignments = 0;
        exploreAssignments(currentRow, 0, maxCardsPerRow, totalAssignments);

        memoCache[key] = totalAssignments;
        return totalAssignments;
    }

    void exploreAssignments(int currentRow, int startCol, int remainingCards, long long& assignments) {
        if (remainingCards == 0) {
            assignments += calculateAssignments(currentRow + 1);
            return;
        }

        if (startCol >= columns || remainingCards > maxCardsPerRow) return;

        for (int col = startCol; col <= columns - remainingCards; ++col) {
            if (columnCardCounts[col] < maxCardsPerColumn) {
                columnCardCounts[col]++;
                exploreAssignments(currentRow, col + 1, remainingCards - 1, assignments);
                columnCardCounts[col]--;
            }
        }
    }
};

int main() {
    int testCases, columns, rows, maxCardsPerRow, maxCardsPerColumn;
    std::cin >> testCases;
    while (testCases--) {
        std::cin >> columns >> rows >> maxCardsPerRow >> maxCardsPerColumn;
        CardGrid grid(rows, columns, maxCardsPerColumn, maxCardsPerRow);
        std::cout << grid.calculateAssignments() << std::endl;
    }
    return 0;
}