#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include "../common/Commands.h"
#include "../common/Snapshots.h"

class ChessView;

class MainWindow : public QWidget {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

    // ===== 属性绑定入口（App 层调用） =====
    void setBoardSnapshotProvider(BoardSnapshotProvider provider);
    void setGameSnapshotProvider(GameSnapshotProvider provider);

    // ===== 命令绑定入口（App 层调用） =====
    void setCellClickCommand(CellClickCommand command);
    void setResetCommand(SimpleCommand command);
    void setUndoCommand(SimpleCommand command);
    void setBackCommand(SimpleCommand command);

public slots:
    void refreshBoard();
    void refreshStatus();
    void showMessage(const QString& msg);

private:
    void setupUI();

    ChessView* chessView_ = nullptr;
    QLabel* statusLabel_ = nullptr;
    QPushButton* resetBtn_ = nullptr;
    QPushButton* undoBtn_ = nullptr;
    QPushButton* backBtn_ = nullptr;

    // 快照提供者（由 App 绑定）
    BoardSnapshotProvider boardProvider_;
    GameSnapshotProvider gameProvider_;

    // 命令（由 App 绑定）
    CellClickCommand cellClickCommand_;
    SimpleCommand resetCommand_;
    SimpleCommand undoCommand_;
    SimpleCommand backCommand_;
};
