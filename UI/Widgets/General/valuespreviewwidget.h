#ifndef VALUESPREVIEWWIDGET_H
#define VALUESPREVIEWWIDGET_H

#include "../../A3D/chart.h"
#include "generalpreview.h"

class ValuesPreviewWidget : public GeneralPreview {
	Q_OBJECT

public:
    explicit ValuesPreviewWidget(QWidget* parent = nullptr);

    void addValues(std::vector<float>);

    std::vector<float> getValues() const;

	void addValue(float);

    void clear();

public slots:
    void setStringPrecision(size_t);
private slots:
    virtual void onAddButtonClicked() override;
    inline virtual void onItemDoubleClicked(QModelIndex const&) override {}
    inline virtual void onEditIndicatorsClicked() override {}

private:
    virtual bool isAddEnabled() const override;
    virtual bool isEditEnabled() const override;
    virtual bool isMultiEditEnabled() const override;
    virtual bool isRemoveEnabled() const override;
};

#endif // VALUESPREVIEWWIDGET_H
