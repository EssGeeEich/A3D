#ifndef INDICATORSPREVIEWWIDGET_H
#define INDICATORSPREVIEWWIDGET_H

#include "../../A3D/chart.h"
#include "../Dialogs/addindicatorsdialog.h"
#include "../Dialogs/editindicatorsdialog.h"
#include "generalpreview.h"

class ChartAxisSettings;

class IndicatorsPreviewWidget : public GeneralPreview {
	Q_OBJECT

public:
    explicit IndicatorsPreviewWidget(QWidget* parent = nullptr);

    void addIndicators(std::vector<A3D::ChartAxisIndicator>);
    std::vector<A3D::ChartAxisIndicator> indicators() const;

    void clear();

private slots:
    // Add
    virtual void onAddButtonClicked() override;
    void onAddDialogAccepted();

    // Edit
    virtual void onItemDoubleClicked(QModelIndex const&) override;
    virtual void onEditIndicatorsClicked() override;
    void onEditDialogFinished();
    void onRemoveItem(size_t);

private:
    virtual bool isAddEnabled() const override;
    virtual bool isEditEnabled() const override;
    virtual bool isMultiEditEnabled() const override;
    virtual bool isRemoveEnabled() const override;
    void editIndicators(std::vector<A3D::ChartAxisIndicator> const&);
    ChartAxisSettings* m_settings;
    AddIndicatorsDialog m_addDialog;
    EditIndicatorsDialog m_editDialog;
    std::vector<A3D::ChartAxisIndicator> m_indicators;
};

#endif // INDICATORSPREVIEWWIDGET_H
