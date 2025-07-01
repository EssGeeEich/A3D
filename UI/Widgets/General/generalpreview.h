#ifndef GENERALPREVIEW_H
#define GENERALPREVIEW_H

#include <QWidget>
#include <QListWidget>

namespace Ui {
class GeneralPreview;
}

class GeneralPreview : public QWidget {
	Q_OBJECT

public:
    explicit GeneralPreview(QWidget* parent = nullptr);
	~GeneralPreview();
    QListWidget* previewWidget() const;
signals:
    void removedItem(size_t index);
public slots:
    void onItemSelectionChanged();
private slots:
    void onRemoveButtonClicked();

    void onAddButtonClickedProxy();
    void onItemDoubleClickedProxy(QModelIndex const&);
    void onEditIndicatorsClickedProxy();

    virtual void onAddButtonClicked()                    = 0;
    virtual void onItemDoubleClicked(QModelIndex const&) = 0;
    virtual void onEditIndicatorsClicked()               = 0;

private:
    virtual bool isAddEnabled() const       = 0;
    virtual bool isEditEnabled() const      = 0;
    virtual bool isMultiEditEnabled() const = 0;
    virtual bool isRemoveEnabled() const    = 0;

    Ui::GeneralPreview* ui;
};

#endif // GENERALPREVIEW_H
