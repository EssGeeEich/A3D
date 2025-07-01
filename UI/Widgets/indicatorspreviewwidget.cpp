#include "indicatorspreviewwidget.h"
#include "chartaxisgeneralsettings.h"
#include <QPainter>
#include "customframe.h"
#include "chartaxissettings.h"
#include "incompatibilitydialog.h"

IndicatorsPreviewWidget::IndicatorsPreviewWidget(QWidget* parent)
    : GeneralPreview(parent) {
    GeneralPreview::onItemSelectionChanged();

    connect(&m_addDialog, &QDialog::accepted, this, &IndicatorsPreviewWidget::onAddDialogAccepted);
    connect(&m_editDialog, &QDialog::rejected, this, &IndicatorsPreviewWidget::onEditDialogFinished);
    connect(&m_editDialog, &QDialog::accepted, this, &IndicatorsPreviewWidget::onEditDialogFinished);
    connect(this, &GeneralPreview::removedItem, this, &IndicatorsPreviewWidget::onRemoveItem);

    previewWidget()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget* parentWidget = parent;
    while(parentWidget) {
        ChartAxisSettings* settings = qobject_cast<ChartAxisSettings*>(parentWidget);
        if(settings) {
            m_settings = settings;
            break;
        }
        parentWidget = parentWidget->parentWidget();
    }
    previewWidget()->setFlow(QListWidget::LeftToRight);
}

void IndicatorsPreviewWidget::onRemoveItem(size_t index) {
    if(index >= m_indicators.size())
        return;
    m_indicators.erase(m_indicators.begin() + index);
}

void IndicatorsPreviewWidget::onAddButtonClicked() {
    A3D::ChartAxisIndicatorStyle style;
    if(m_settings)
        style = m_settings->style();
    m_addDialog.setStyle(style);
    m_addDialog.open();
}

void IndicatorsPreviewWidget::onAddDialogAccepted() {
    addIndicators(m_addDialog.indicators());
}

void IndicatorsPreviewWidget::onItemDoubleClicked(QModelIndex const& index) {
    // The idea is to remove the widget and then re add it modified
    int row = index.row();

    A3D::ChartAxisIndicator indicator = m_indicators.at(row);

    delete previewWidget()->takeItem(row);
    m_indicators.erase(m_indicators.begin() + row);

    editIndicators({ indicator });
}

void IndicatorsPreviewWidget::onEditIndicatorsClicked() {

    std::vector<A3D::ChartAxisIndicator> indicators;
    QList<QListWidgetItem*> selectedItems = previewWidget()->selectedItems();

    for(int i = previewWidget()->count() - 1; i >= 0; --i) {
        QListWidgetItem* item = previewWidget()->item(i);
        if(!selectedItems.contains(item))
            continue;
        // Qt documentation says that this is the correct way
        indicators.push_back(m_indicators.at(i));
        delete previewWidget()->takeItem(i);
        m_indicators.erase(m_indicators.begin() + i);
    }
    editIndicators(indicators);
}

void IndicatorsPreviewWidget::onEditDialogFinished() {
    addIndicators(m_editDialog.indicators());
}

void IndicatorsPreviewWidget::editIndicators(std::vector<A3D::ChartAxisIndicator> const& indicators) {
    std::vector<IndicatorInfo> infos;
    infos.clear();

    m_editDialog.reset();

    infos.emplace_back(indicators[0]);

    for(size_t i = 0; i < indicators.size() - 1; i++) {
        IndicatorInfo current = IndicatorInfo(indicators[i]);
        IndicatorInfo next    = IndicatorInfo(indicators[i + 1]);

        if(current != next && std::find(infos.begin(), infos.end(), next) == infos.end())
            infos.push_back(next);
    }
    size_t count = infos.size();

    if(count == 1) {
        // Ho un solo tipo di indicatore quindi li posso pushare in edit

        IndicatorInfo info = infos[0];
        m_editDialog.setStyle(info.m_style);
        m_editDialog.setChartIndicatorsType(info.m_type);
        m_editDialog.editIndicators(indicators);
        return;
    }

    // Insert here dialog
    IncompatibilityDialog* dialog = new IncompatibilityDialog(this, infos);
    dialog->open();

    connect(dialog, &IncompatibilityDialog::finished, this, [this, dialog, indicators]() {
        switch(dialog->getUserChoice()) {
        case IncompatibilityDialog::UC_CHOOSE:
            {
                IndicatorInfo info = dialog->getInfo();
                m_editDialog.setStyle(info.m_style);
                m_editDialog.setChartIndicatorsType(info.m_type);
            }
            break;
        case IncompatibilityDialog::UC_CONTINUE:
            break;
        default:
        case IncompatibilityDialog::UC_DISCARD:
            // Total rollback
            addIndicators(indicators);
            return;
        }

        m_editDialog.editIndicators(indicators);
    });
}

void IndicatorsPreviewWidget::clear() {
    m_indicators.clear();
    for(int i = 0; i < previewWidget()->count(); i++)
        delete previewWidget()->takeItem(i);
}

void IndicatorsPreviewWidget::addIndicators(std::vector<A3D::ChartAxisIndicator> indicators) {
    for(A3D::ChartAxisIndicator const& it: indicators) {

        float value = it.m_value;

        if(std::find_if(
               m_indicators.begin(), m_indicators.end(),
               [value](A3D::ChartAxisIndicator const& it) -> bool {
                   return std::abs(it.m_value - value) < std::numeric_limits<float>::min();
               }
           )
           != m_indicators.end())
            continue;

        m_indicators.push_back(it);
    }

    std::sort(m_indicators.begin(), m_indicators.end(), [this](A3D::ChartAxisIndicator const& a, A3D::ChartAxisIndicator const& b) -> bool {
        return a.m_value < b.m_value;
    });

    previewWidget()->clear();

    for(A3D::ChartAxisIndicator const& it: m_indicators) {

        QString text = it.m_label;

        QListWidgetItem* item = new QListWidgetItem(previewWidget());

        CustomFrame* frame = new CustomFrame(previewWidget(), it.m_style.m_indicatorColor, it.m_type);

        frame->setText(text);
        frame->setFormats(it.m_style.m_labelColor, it.m_style.m_labelFont);

        // Aumento artificialmente l'altezza per starci dentro
        item->setSizeHint(frame->sizeHint());

        size_t height = item->sizeHint().height();
        if(height + 20 >= previewWidget()->minimumHeight())
            previewWidget()->setMinimumHeight(height + 20);

        previewWidget()->addItem(item);
        previewWidget()->setItemWidget(item, frame);
    }
}

std::vector<A3D::ChartAxisIndicator> IndicatorsPreviewWidget::indicators() const {
    return m_indicators;
}

bool IndicatorsPreviewWidget::isAddEnabled() const {
    return true;
}

bool IndicatorsPreviewWidget::isEditEnabled() const {
    return true;
}

bool IndicatorsPreviewWidget::isMultiEditEnabled() const {
    return true;
}

bool IndicatorsPreviewWidget::isRemoveEnabled() const {
    return true;
}
