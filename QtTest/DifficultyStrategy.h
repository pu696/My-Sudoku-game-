#ifndef DIFFICULTYSTRATEGY_H
#define DIFFICULTYSTRATEGY_H

// 难度策略接口
class DifficultyStrategy {
public:
    virtual ~DifficultyStrategy() {}
    virtual int getRemovalCount() = 0;
};

// 简单难度策略
class EasyStrategy : public DifficultyStrategy {
public:
    int getRemovalCount() override { return 35; }
};

// 中等难度策略
class MediumStrategy : public DifficultyStrategy {
public:
    int getRemovalCount() override { return 45; }
};

// 困难难度策略
class HardStrategy : public DifficultyStrategy {
public:
    int getRemovalCount() override { return 55; }
};

#endif // DIFFICULTYSTRATEGY_H
