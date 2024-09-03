#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>

// 相手の戦略に基づいてペアを取る関数
int opponentStrategy(std::vector<int>& state, std::unordered_map<int, int>& revealedCards) {
    int pairs = 0;
    for (const auto& card : revealedCards) {
        int firstIndex = card.second;
        if (state[firstIndex] == -1) continue;

        // ペアがあればそれを取る
        for (int i = 0; i < state.size(); i++) {
            if (i == firstIndex || state[i] == -1 || state[i] != card.first) continue;

            int secondIndex = i;
            state[firstIndex] = state[secondIndex] = -1;
            revealedCards.erase(card.first);
            pairs++;
            break;
        }
    }
    return pairs;
}

// 自分のターンに最適な選択肢を評価する関数
int evaluateBestMove(std::vector<int>& state, std::unordered_map<int, int>& revealedCards, bool isPlayerTurn) {
    int n = state.size();
    int maxPairs = 0;
    
    // 一枚目を未公開カードから引く戦略
    for (int i = 0; i < n; i++) {
        if (state[i] == -1 || revealedCards.count(state[i])) continue;
        
        int firstCard = state[i];
        state[i] = -1;
        
        // ペアを取ることができるかチェック
        if (revealedCards.count(firstCard)) {
            int secondIndex = revealedCards[firstCard];
            state[secondIndex] = -1;
            revealedCards.erase(firstCard);
            maxPairs = std::max(maxPairs, 1 + opponentStrategy(state, revealedCards));
            state[secondIndex] = firstCard;
            revealedCards[firstCard] = secondIndex;
        }
        
        // 二枚目を未公開カードから引く戦略
        for (int j = i + 1; j < n; j++) {
            if (state[j] == -1 || revealedCards.count(state[j])) continue;
            
            int secondCard = state[j];
            state[j] = -1;
            revealedCards[firstCard] = i;
            revealedCards[secondCard] = j;
            
            int newPairs = (firstCard == secondCard) ? 1 : 0;
            maxPairs = std::max(maxPairs, newPairs + opponentStrategy(state, revealedCards));
            
            state[j] = secondCard;
            revealedCards.erase(secondCard);
        }
        
        state[i] = firstCard;
        revealedCards.erase(firstCard);
    }
    
    return maxPairs;
}

// 再帰的にすべてのパターンをシミュレーションし、勝敗を記録する
void simulateAllPatterns(std::vector<int>& state, std::unordered_map<int, int>& revealedCards, int playerPairs, int opponentPairs, int& playerWins, int& opponentWins, bool isPlayerTurn) {
    bool allMatched = true;
    for (int i = 0; i < state.size(); i++) {
        if (state[i] != -1) {
            allMatched = false;
            break;
        }
    }

    // 全てのペアが見つかった場合
    if (allMatched) {
        if (playerPairs > opponentPairs) playerWins++;
        else if (playerPairs < opponentPairs) opponentWins++;
        return;
    }

    // プレイヤーのターン
    if (isPlayerTurn) {
        // すでに分かっているペアがあれば取る
        playerPairs += opponentStrategy(state, revealedCards);

        // 最適な選択肢を評価
        int optimalMove = evaluateBestMove(state, revealedCards, true);

        // 最適な戦略を実行
        if (optimalMove > 0) {
            playerPairs += optimalMove;
            simulateAllPatterns(state, revealedCards, playerPairs, opponentPairs, playerWins, opponentWins, !isPlayerTurn);
        } else {
            simulateAllPatterns(state, revealedCards, playerPairs, opponentPairs, playerWins, opponentWins, !isPlayerTurn);
        }
    } else {
        // 相手のターン
        opponentPairs += opponentStrategy(state, revealedCards);

        // 次のパターンをシミュレート
        simulateAllPatterns(state, revealedCards, playerPairs, opponentPairs, playerWins, opponentWins, !isPlayerTurn);
    }
}

int main() {
    // カードデッキの初期化（例：4ペアのカード）
    int num;
    std::cin >> num;
    std::vector<int> deck(num * 4);
    for (int i = 0;i < num;i++) {
        for (int j = 0;j < 4;j++) deck[i +  num * (j + 1)] , std::cout << deck[i + num * (j + 1)];
    }
    
    std::unordered_map<int, int> revealedCards; // 既に公開されたカードの位置を記憶

    int playerWins = 0;
    int opponentWins = 0;

    // すべてのパターンをシミュレート
    simulateAllPatterns(deck, revealedCards, 0, 0, playerWins, opponentWins, true);

    std::cout << "Player wins: " << playerWins << " times, Opponent wins: " << opponentWins << " times" << std::endl;

    return 0;
}