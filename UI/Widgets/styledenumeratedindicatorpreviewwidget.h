#ifndef LISTINDICATORSPREVIEWWIDGET_H
#define LISTINDICATORSPREVIEWWIDGET_H

#include "../../A3D/chart.h"
#include "../Dialogs/editenumdialog.h"
#include "General/generalpreview.h"

class ChartAxisSettings;

class StyledEnumeratedIndicatorsPreviewWidget : public GeneralPreview {
	Q_OBJECT

public:
    explicit StyledEnumeratedIndicatorsPreviewWidget(QWidget* parent = nullptr);

    void addIndicators(std::vector<A3D::ChartAxisIndicator> const&);

    struct StyledIndicator {
        inline StyledIndicator(A3D::ChartAxisIndicator indicator)
            : m_type(indicator.m_type) {
            m_data.m_label = indicator.m_label;
            m_data.m_style = indicator.m_style;
        }

        A3D::ChartAxisIndicatorType m_type;
        A3D::ChartAxisStyledEnumeratedIndicatorData m_data;
    };

    std::vector<StyledIndicator> enumeratedIndicators() const;

    std::vector<A3D::ChartAxisIndicator> indicators() const;

private slots:

    virtual void onAddButtonClicked() override;

    virtual void onEditIndicatorsClicked() override;
    virtual void onItemDoubleClicked(QModelIndex const&) override;
    void onEditAccepted();
    void onRemoveItem(size_t);

private:
    virtual bool isAddEnabled() const override;
    virtual bool isEditEnabled() const override;
    virtual bool isMultiEditEnabled() const override;
    virtual bool isRemoveEnabled() const override;
    std::vector<A3D::ChartAxisIndicator> m_indicators;
    ChartAxisSettings* m_settings;
    EditEnumDialog m_dialog;
};

#endif // LISTINDICATORSPREVIEWWIDGET_H
