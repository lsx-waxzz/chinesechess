#pragma once
#include <QObject>
#include <QProcess>
#include <QString>
#include "../common/Types.h"

class GameEngine;

/// UCI 象棋引擎通信层
/// 通过 QProcess 启动引擎子进程，使用 UCI 协议通信
class UciEngine : public QObject {
    Q_OBJECT
public:
    explicit UciEngine(QObject* parent = nullptr);
    ~UciEngine();

    // ── 静态工具方法 ──
    /// 将游戏局面转为中国象棋 FEN 串
    static QString boardToFen(const GameEngine& game);

    /// 将 UCI 走法（如 "b2e2"）转为内部 Move
    static Move uciToMove(const QString& uci);

    /// 启动引擎
    /// @param enginePath 引擎可执行文件路径
    /// @return 是否成功启动
    bool start(const QString& enginePath);

    /// 停止引擎
    void stop();

    /// 是否已就绪
    bool isReady() const { return ready_; }

    /// 设置棋盘局面（FEN 格式），开始搜索最佳走法
    /// @param fen  中国象棋 FEN 串
    /// @param timeMs 思考时间（毫秒）
    void search(const QString& fen, int timeMs);

    /// 强制停止搜索
    void stopSearch();

signals:
    /// 引擎启动成功
    void started();
    /// 引擎遇到错误
    void error(const QString& message);
    /// 搜索完成，返回最佳走法（UCI 格式，如 "b2e2"）
    void bestMove(const QString& uciMove);

private slots:
    void onReadyRead();
    void onFinished(int exitCode);

private:
    void sendCommand(const QString& cmd);
    void handleLine(const QString& line);

    QProcess* process_ = nullptr;
    bool ready_ = false;
    bool searching_ = false;
    bool waitingForBestmove_ = false;
    QString enginePath_;
};
