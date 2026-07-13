#pragma once
#include <functional>
#include "Snapshots.h"

/// 命令：点击棋盘格子
using CellClickCommand = std::function<void(int row, int col)>;

/// 命令：无参简单操作（重新开始、悔棋、返回菜单等）
using SimpleCommand = std::function<void()>;

/// 命令：设置游戏模式
///   mode: 0=双人, 1=人机
///   difficulty: 0=简单, 1=普通, 2=困难
using SetGameModeCommand = std::function<void(int mode, int difficulty)>;

/// 属性提供者：View 通过这些函数读取只读快照
using BoardSnapshotProvider = std::function<const BoardSnapshot&()>;
using GameSnapshotProvider = std::function<const GameSnapshot&()>;
