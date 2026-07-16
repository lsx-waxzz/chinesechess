#include "UciEngine.h"
#include "GameEngine.h"
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

UciEngine::UciEngine(QObject* parent) : QObject(parent) {}

UciEngine::~UciEngine() { stop(); }

bool UciEngine::start(const QString& enginePath) {
    if (process_ && process_->state() != QProcess::NotRunning) {
        qDebug() << "引擎已在运行";
        return false;
    }

    enginePath_ = enginePath;
    QFileInfo fi(enginePath_);
    if (!fi.exists()) {
        emit error(QString("引擎文件不存在: %1").arg(enginePath_));
        return false;
    }

    delete process_;
    process_ = new QProcess(this);
    process_->setWorkingDirectory(fi.absolutePath());
    process_->setProcessChannelMode(QProcess::SeparateChannels);

    QObject::connect(process_, &QProcess::readyReadStandardOutput,
                     this, &UciEngine::onReadyRead);
    QObject::connect(process_, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                     this, &UciEngine::onFinished);

    ready_ = false;
    searching_ = false;
    waitingForBestmove_ = false;

    process_->start(enginePath_, QStringList(), QIODevice::ReadWrite);
    if (!process_->waitForStarted(3000)) {
        emit error(QString("启动引擎失败: %1").arg(process_->errorString()));
        return false;
    }

    // UCI 初始化序列
    sendCommand("uci");

    return true;
}

void UciEngine::stop() {
    if (process_ && process_->state() != QProcess::NotRunning) {
        sendCommand("quit");
        if (!process_->waitForFinished(1000)) {
            process_->kill();
        }
    }
    delete process_;
    process_ = nullptr;
    ready_ = false;
    searching_ = false;
}

void UciEngine::search(const QString& fen, int timeMs) {
    if (!ready_) {
        qDebug() << "引擎未就绪，无法搜索";
        return;
    }
    if (searching_) {
        stopSearch();
    }

    sendCommand(QString("position fen %1").arg(fen));
    searching_ = true;
    waitingForBestmove_ = true;
    sendCommand(QString("go movetime %1").arg(timeMs));
}

void UciEngine::stopSearch() {
    if (searching_) {
        sendCommand("stop");
        searching_ = false;
    }
}

void UciEngine::sendCommand(const QString& cmd) {
    if (process_ && process_->state() == QProcess::Running) {
        QByteArray data = (cmd + "\n").toUtf8();
        process_->write(data);
        qDebug() << ">> " << cmd;
    }
}

void UciEngine::onReadyRead() {
    while (process_->canReadLine()) {
        QByteArray line = process_->readLine().trimmed();
        if (line.isEmpty()) continue;
        handleLine(QString::fromUtf8(line));
    }
}

void UciEngine::handleLine(const QString& line) {
    qDebug() << "<< " << line;

    if (line == "uciok") {
        sendCommand("setoption name UCI_Variant value xiangqi");
        sendCommand("isready");
        return;
    }

    if (line == "readyok") {
        ready_ = true;
        emit started();
        return;
    }

    if (line.startsWith("bestmove")) {
        waitingForBestmove_ = false;
        searching_ = false;
        QStringList parts = line.split(' ', Qt::SkipEmptyParts);
        qDebug() << "  parsing bestmove, parts:" << parts;
        if (parts.size() >= 2) {
            QString move = parts[1];
            qDebug() << "  extracted move:" << move;
            if (move != "(none)" && move != "0000") {
                qDebug() << "  emitting bestMove signal with:" << move;
                emit bestMove(move);
            }
        }
        return;
    }
}

void UciEngine::onFinished(int exitCode) {
    qDebug() << "引擎退出，exitCode:" << exitCode;
    ready_ = false;
    searching_ = false;
    if (exitCode != 0 && !enginePath_.isEmpty()) {
        emit error(QString("引擎异常退出，exitCode: %1").arg(exitCode));
    }
}

// ── 静态工具方法 ──

QString UciEngine::boardToFen(const GameEngine& game) {
    const ChessBoard& board = game.board();
    QString fen;

    auto pieceToFenChar = [](const Piece& p) -> QChar {
        QChar c;
        switch (p.type) {
        case PieceType::King:    c = 'k'; break;
        case PieceType::Advisor:  c = 'a'; break;
        case PieceType::Elephant: c = 'b'; break;
        case PieceType::Knight:   c = 'n'; break;
        case PieceType::Rook:     c = 'r'; break;
        case PieceType::Cannon:   c = 'c'; break;
        case PieceType::Pawn:     c = 'p'; break;
        default:                  c = '?'; break;
        }
        return (p.color == PieceColor::Red) ? c.toUpper() : c.toLower();
    };

    for (int r = 0; r < 10; ++r) {
        int emptyCount = 0;
        for (int c = 0; c < 9; ++c) {
            const auto& piece = board.pieceAt(r, c);
            if (piece.has_value()) {
                if (emptyCount > 0) {
                    fen += QString::number(emptyCount);
                    emptyCount = 0;
                }
                fen += pieceToFenChar(*piece);
            } else {
                ++emptyCount;
            }
        }
        if (emptyCount > 0) fen += QString::number(emptyCount);
        if (r < 9) fen += '/';
    }

    fen += (game.currentPlayer() == PieceColor::Red) ? " w" : " b";
    fen += " - - 0 1";

    return fen;
}

Move UciEngine::uciToMove(const QString& uci) {
    if (uci.length() < 4) return { -1, -1, -1, -1 };

    int fromCol = uci[0].toLatin1() - 'a';
    int fromRow = 9 - uci[1].digitValue();   // Pikafish row → 内部 row
    int toCol   = uci[2].toLatin1() - 'a';
    int toRow   = 9 - uci[3].digitValue();

    if (fromCol < 0 || fromCol > 8 || toCol < 0 || toCol > 8 ||
        fromRow < 0 || fromRow > 9 || toRow   < 0 || toRow   > 9)
        return { -1, -1, -1, -1 };

    return Move{ fromRow, fromCol, toRow, toCol };
}
