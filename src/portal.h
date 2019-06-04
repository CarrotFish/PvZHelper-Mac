#ifndef PORTAL_H
#define PORTAL_H

#include <QWidget>

namespace Ui {
    class PortalWindow;
}

class MainWindow;

class Portal : public QWidget {
Q_OBJECT
signals:
    
    void SetBlackPortal(int row_1, int column_1, int row_2, int column_2);
    
    void SetWhitePortal(int row_1, int column_1, int row_2, int column_2);
    
    void ActivePortal(bool on);
    
    void LockPortal(bool on);

public:
    explicit Portal(MainWindow *mainWindow);
    void RestoreChanges();
private:
    Ui::PortalWindow *PortalUi;
    MainWindow *mainWindow;
protected:
    void closeEvent(QCloseEvent *event) override;
};


#endif //PORTAL_H
